#include "simple.h"

#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/uaccess.h>

MODULE_AUTHOR("Vladimir Petrigo");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Simple char device driver");

static int simple_major_no = 0;
static int simple_minor_no = 0;
static int simple_num_dev = 1;
static char *greet_msg = "Hello";
module_param(greet_msg, charp, S_IRUGO);

static struct simple_dev *s_dev = NULL;

static int simple_mod_open(struct inode *, struct file *);
static int simple_mod_release(struct inode *, struct file *);
static ssize_t simple_mod_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t simple_mod_write(struct file *, const char __user *, size_t, loff_t *);
static void simple_mod_cleanup(void);
static void simple_buf_clean(struct simple_dev *dev);
static int __init simple_init(void);
static void simple_exit(void);

static struct file_operations simple_ops = {
	.owner = THIS_MODULE,
	.open = simple_mod_open,
	.release = simple_mod_release,
	.read = simple_mod_read,
	.write = simple_mod_write
};

static int simple_mod_open(struct inode *inode, struct file *filp)
{
	struct simple_dev *dev = container_of(inode->i_cdev, struct simple_dev, cdev);

	filp->private_data = dev;
	pr_debug("simple: open simple file\n");

	if (filp->f_mode & FMODE_WRITE) {
		simple_buf_clean(dev);
	}

	return 0;
}

static int simple_mod_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t simple_mod_read(struct file *filp, char __user *buf, 
		size_t count, loff_t *off)
{
	int retval = 0;
	struct simple_dev *dev = filp->private_data;

	if (*off >= count) {
		goto out;
	}

	if (*off + count > dev->size) {
		count = dev->size - *off;
	}

	pr_debug("simple: read count %zu\n", count);

	if (copy_to_user(buf, &dev->buffer[*off], count) != 0) {
		retval = -EFAULT;
		goto out;
	}

	*off += count;
	retval = count;
out:
	pr_debug("simple: read retval %d\n", retval);
	return retval;
}

static ssize_t simple_mod_write(struct file *filp, const char __user *buf, 
		size_t count, loff_t *off)
{
	int retval = -ENOMEM;
	struct simple_dev *dev = filp->private_data;

	if (*off >= SIMPLE_BUFFER_SIZE) {
		goto out;
	}

	if (*off + count > SIMPLE_BUFFER_SIZE) {
		count = SIMPLE_BUFFER_SIZE - *off;
	}

	pr_debug("simple: write count: %zu\n", count);

	if (copy_from_user(&dev->buffer[*off], buf, count) != 0) {
		retval = -EFAULT;
		goto out;
	}

	*off += count;
	retval = count;

	if (dev->size < *off) {
		dev->size = *off;
	}
out:
	pr_debug("simple: write retval %d\n", retval);
	return retval;
}

static void simple_buf_clean(struct simple_dev *dev)
{
	memset(dev->buffer, 0, SIMPLE_BUFFER_SIZE);
	dev->size = 0;
}

static void simple_mod_cleanup(void)
{
	dev_t dev_no = MKDEV(simple_major_no, simple_minor_no);	

	cdev_del(&s_dev->cdev);
	kfree(s_dev);
	unregister_chrdev_region(dev_no, simple_num_dev);
}

static int __init simple_init(void)
{
	dev_t dev_no;
	int res = alloc_chrdev_region(&dev_no, simple_minor_no, 
			simple_num_dev, "simple");

	pr_debug("simple: %s\n", greet_msg);

	if (res < 0) {
		pr_warn("simple: can not get major number %d\n", simple_major_no);
		goto fail;
	}

	s_dev = kmalloc(sizeof(struct simple_dev), GFP_KERNEL);

	if (s_dev == NULL) {
		res = -EPERM;
		goto fail;
	}

	cdev_init(&s_dev->cdev, &simple_ops);
	res = cdev_add(&s_dev->cdev, dev_no, 1);

	if (res < 0) {
		pr_warn("simple: can not add cdev structure\n");
		goto fail;
	}

	simple_major_no = MAJOR(dev_no);
	pr_debug("simple: allocate major number %d\n", simple_major_no);
	return 0;
fail:
	simple_mod_cleanup();
	return res;
}

static void __exit simple_exit(void)
{
	simple_mod_cleanup();
	pr_debug("simple: Bye\n");
}

module_init(simple_init);
module_exit(simple_exit);
