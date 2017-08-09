#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include "ioctl.h"

static long dev_ioctl(struct file *f, unsigned int cmd, unsigned long arg);

#define MOD_MAJOR_NUM 200
#define MOD_NAME "study_module"
#define DEV_INIT_MSG "module init msg\n"
#define DEV_BUFSIZE 256

static char dev_buf[DEV_BUFSIZE];

static const struct file_operations dev_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = dev_ioctl,
};

static long dev_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	if((_IOC_TYPE(cmd) != IOC_MAGIC))
		return -ENOTTY;
	switch( cmd ) {
	case IOCTL_GET:
		if(copy_to_user((void*)arg, dev_buf, DEV_BUFSIZE))
			return -EFAULT;
		break;
	case IOCTL_SET:
		//memset(dev_buf, 0, DEV_BUFSIZE);
		if (copy_from_user(dev_buf, (void*)arg, DEV_BUFSIZE))
			return -EFAULT;
		break;
	default:
		return -ENOTTY;
	}
	return 0;
}

static int __init dev_init( void )
{
	int ret = register_chrdev(MOD_MAJOR_NUM, MOD_NAME, &dev_fops);
	if(ret < 0) {
		printk(KERN_ERR "Can not register char device\n" );
		goto out;
	}

	strncpy(dev_buf, DEV_INIT_MSG, DEV_BUFSIZE - 1);
out:
	return ret;
}

static void __exit dev_exit( void )
{
	unregister_chrdev( MOD_MAJOR_NUM, MOD_NAME );
}

MODULE_LICENSE("GPL");
module_init(dev_init);
module_exit(dev_exit);

