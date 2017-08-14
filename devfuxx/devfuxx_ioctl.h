#ifndef _DEVFUXX_IOCTL_H
#define _DEVFUXX_IOCTL_H

#include <linux/ioctl.h>

struct brain_index {
	int	pos;
	char	data;
};

#define IOC_MAGIC 'd'
#define IOCTL_INC_PTR	_IO(IOC_MAGIC, 1)
#define IOCTL_DEC_PTR	_IO(IOC_MAGIC, 2)
#define IOCTL_INC_DATA	_IO(IOC_MAGIC, 3)
#define IOCTL_DEC_DATA	_IO(IOC_MAGIC, 4)
#define IOCTL_READDATA	_IOR(IOC_MAGIC, 5, struct brain_index)
#define IOCTL_WRITEDATA	_IOWR(IOC_MAGIC, 6, struct brain_index)

#endif
