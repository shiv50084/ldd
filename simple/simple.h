#ifndef __SIMPLE_MODULE_H__
#define __SIMPLE_MODULE_H__

#include <linux/cdev.h>
#include <linux/types.h>

#define SIMPLE_BUFFER_SIZE 1024

struct simple_dev {
	char buffer[SIMPLE_BUFFER_SIZE];
	size_t size;	
	struct cdev cdev;
};

#endif // __SIMPLE_MODULE_H__
