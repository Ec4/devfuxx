#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#define STACKSIZE	256
#define TAPESIZE	32

/* IOCTL COMMAND */
#define	INC_PTR		5
#define	DEC_PTR		6
#define	INC_DATA	7
#define	DEC_DATA	8
#define READDATA	9

MODULE_AUTHOR("Ec4");
MODULE_LICENSE("Dual MIT/GPL");

typedef enum {
	HEAD, VALID, TAIL, INVALID
} Position;


typedef struct {
	int pos;
	char tape[TAPESIZE];
} tape_t;

Position pos_where(int pos)
{
	if (pos < 0 || TAPESIZE <= pos)	return INVALID;
	else if (pos == 0)		return HEAD;
	else if (pos == TAPESIZE-1)	return TAIL;
	else				return VALID;
}

static DEFINE_MUTEX(devfuxx_mutex);


/**********************OPEN*************************/
static int devfuxx_open(struct inode *inode, struct file *filp)
{
	tape_t *p;

	printk(KERN_INFO "1devfuxx(%s): major %d minor %d (pid %d)\n"
			, __func__
			, imajor(inode)
			, iminor(inode)
			, current->pid);

	if ((p = kmalloc(sizeof(tape_t), GFP_KERNEL)) == NULL)
	{
		printk(KERN_ERR "%s: No memory\n", __func__);
		return -ENOMEM;
	}

	printk(KERN_INFO "2devfuxx(%s): tape[%d] = %d", __func__, p->pos, p->tape[p->pos]);

	p->pos = 0;
	memset(&(p->tape), 0, sizeof(p->tape));

	printk(KERN_INFO "3devfuxx(%s): tape[%d] = %d", __func__, p->pos, p->tape[p->pos]);

	filp->private_data = p;

	printk(KERN_INFO "4devfuxx(%s): tape[%d] = %d", __func__, p->pos, p->tape[p->pos]);

	return 0;
}

/*********************RELEASE***********************/
static int devfuxx_release(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "devfuxx(%s): major %d minor %d (pid %d)\n"
			, __func__
			, imajor(inode)
			, iminor(inode)
			, current->pid);

	kfree(filp->private_data);

	return 0;
}

/**********************READ*************************/
static ssize_t devfuxx_read(struct file *filp, char __user *buf,
			size_t count, loff_t *f_pos)
{
	tape_t *p = filp->private_data;

	mutex_lock(&devfuxx_mutex);
	if (copy_to_user(buf, &(p->tape[p->pos]), 1))
		return -EFAULT;
	mutex_unlock(&devfuxx_mutex);

	printk(KERN_INFO "devfuxx(%s): READ!!!", __func__);
	*f_pos += 1;

	return 1;
}


/**********************WRITE************************/
static ssize_t devfuxx_write(struct file *filp, const char __user *buf,
				size_t count, loff_t *f_pos)
{
	tape_t *p = filp->private_data;

	mutex_lock(&devfuxx_mutex);
	if (copy_from_user(&(p->tape[p->pos]), buf, 1))
		return -EFAULT;
	mutex_unlock(&devfuxx_mutex);

	return 1;
}


/**********************IOCTL************************/
static long devfuxx_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	tape_t *p = filp->private_data;

	printk(KERN_INFO "1devfuxx(%s): tape[%d] = %d", __func__, p->pos, p->tape[p->pos]);

	Position pos_area = pos_where(p->pos);
	char data = p->tape[p->pos];
	int *arg_p = (int *) arg;

	printk(KERN_INFO "2devfuxx(%s): tape[%d] = %d", __func__, p->pos, p->tape[p->pos]);

	switch (cmd) {
		case INC_PTR:		// >
			if (pos_area == VALID || pos_area == HEAD) {
				p->pos++;
				return 0;
			} else {
				printk(KERN_ERR "invalid area\n");
				return -EFAULT;
			}
		case DEC_PTR:		// <
			if (pos_area == VALID || pos_area == TAIL) {
				p->pos--;
				return 0;
			} else {
				printk(KERN_ERR "invalid area\n");
				return -EFAULT;
			}
		case INC_DATA:		// +
			p->tape[p->pos] = (data == 127) ? 0 : (data+1);
			return 0;
		case DEC_DATA:		// -
			p->tape[p->pos] = (data == 0) ? 127 : (data-1);
			return 0;
		case READDATA:		// debug print tape
			 if (copy_to_user(arg_p, p->tape, TAPESIZE*sizeof(char)))
				 return -EFAULT;
			 return TAPESIZE*sizeof(char);
	}

	return -EFAULT;
}


/****************Handler Register******************/
static const struct file_operations devfuxx_f_op = {
	.owner		= THIS_MODULE,
	.llseek		= no_seek_end_llseek,
	.open		= devfuxx_open,
	.release	= devfuxx_release,
	.read		= devfuxx_read,
	.write		= devfuxx_write,
	.unlocked_ioctl	= devfuxx_ioctl,
};

/*****************Device Register******************/
static struct miscdevice devfuxx_miscdev = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= "devfuxx",
	.mode	= 0666,
	.fops	= &devfuxx_f_op,
};

/*********************INIT*************************/
static int __init devfuxx_init(void)
{
	int err;
	if ((err = misc_register(&devfuxx_miscdev)) != 0) {
		printk(KERN_ALERT "register error\n");
		return err;
	}

	return 0;
}

/*********************EXIT*************************/
static void __exit devfuxx_exit(void)
{
	misc_deregister(&devfuxx_miscdev);
}

module_init(devfuxx_init);
module_exit(devfuxx_exit);
