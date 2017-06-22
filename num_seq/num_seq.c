#include <linux/init.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/module.h>
#include <linux/seq_file.h>

MODULE_AUTHOR("Vladimir Petrigo");
MODULE_DESCRIPTION("Print number sequence to /proc/num_seq");
MODULE_LICENSE("Dual BSD/GPL");

struct num_seq_dev {
	struct proc_dir_entry *pdir;
};

static int num_seq_proc_open(struct inode *, struct file *);
static void *num_seq_op_start(struct seq_file *, loff_t *);
static void num_seq_op_stop(struct seq_file *, void *);
static void *num_seq_op_next(struct seq_file *, void *, loff_t *);
static int num_seq_op_show(struct seq_file *, void *);

static const struct file_operations num_seq_proc_ops = {
	.owner = THIS_MODULE,
	.open = num_seq_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release
};

static const struct seq_operations num_seq_iter = {
	.start = num_seq_op_start,
	.stop = num_seq_op_stop,
	.next = num_seq_op_next,
	.show = num_seq_op_show 
};

static struct num_seq_dev num_dev;

static void num_seq_create_proc(void)
{
	num_dev.pdir = proc_create("num_seq", 0, NULL, &num_seq_proc_ops);
}

static void *num_seq_op_start(struct seq_file *s_file, loff_t *pos)
{
	return NULL;
}

static void num_seq_op_stop(struct seq_file *s_file, void *v)
{
}

static void *num_seq_op_next(struct seq_file *s_file, void *v, loff_t *pos)
{
	return NULL;
}

static int num_seq_op_show(struct seq_file *s_file, void *v)
{
	return 0;
}

static int num_seq_proc_open(struct inode *inode, struct file *filp)
{
	pr_debug("num_seq: open /proc file\n");

	return seq_open(filp, &num_seq_iter);
}

static int __init num_seq_init(void)
{
	num_seq_create_proc();
	return 0;
}

static void __exit num_seq_exit(void)
{
	proc_remove(num_dev.pdir);
}

module_init(num_seq_init);
module_exit(num_seq_exit);
