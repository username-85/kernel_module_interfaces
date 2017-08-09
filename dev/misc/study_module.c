#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>

#define DEV_BUFSIZE 256
#define DEV_NAME "study_dev"
#define DEV_INIT_MSG "module init msg\n"

static ssize_t dev_read(struct file *file, char *buf,
                        size_t len, loff_t *offset);
static ssize_t dev_write(struct file *file, const char *buf,
                         size_t len, loff_t *offset);

static char dev_buf[DEV_BUFSIZE];

static const struct file_operations dev_fops = {
	.owner = THIS_MODULE,
	.read  = dev_read,
	.write = dev_write,
};

static struct miscdevice study_dev = {
	MISC_DYNAMIC_MINOR, DEV_NAME, &dev_fops
};

static ssize_t dev_read(struct file *file, char *buf,
                        size_t len, loff_t *offset)
{
	if (*offset >= DEV_BUFSIZE)
		return 0;

	if (*offset + len >= DEV_BUFSIZE)
		len = DEV_BUFSIZE - *offset;

	if (copy_to_user(buf, dev_buf, len))
		return -EINVAL;

	*offset += len;
	return len;
}

static ssize_t dev_write(struct file *file, const char *buf,
                         size_t len, loff_t *offset)
{
	memset(dev_buf, 0, DEV_BUFSIZE);
	if (len > DEV_BUFSIZE - 1)
		len = DEV_BUFSIZE - 1;
	strncpy(dev_buf, buf, len);
	*offset += len;
	return len;
}

static int __init dev_init( void )
{
	int ret = misc_register(&study_dev);
	if (ret)
		printk(KERN_ERR "unable to register %s\n", DEV_NAME);

	strncpy(dev_buf, DEV_INIT_MSG, DEV_BUFSIZE - 1);

	return ret;
}

static void __exit dev_exit( void )
{
	misc_deregister(&study_dev);
}

MODULE_LICENSE("GPL");
module_init(dev_init);
module_exit(dev_exit);

