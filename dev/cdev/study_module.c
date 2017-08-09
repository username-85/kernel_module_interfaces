#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <asm/uaccess.h>

#define DEV_BUFSIZE 256
#define DEV_NAME "study_dev"
#define DEV_INIT_MSG "module init msg\n"
#define DEV_FIRST  0
#define DEV_COUNT  1
#define MOD_NAME "study_dev"

static ssize_t dev_read(struct file *file, char *buf,
                        size_t len, loff_t *offset);
static ssize_t dev_write(struct file *file, const char *buf,
                         size_t len, loff_t *offset);

static char dev_buf[DEV_BUFSIZE];
static int mod_major_num;
static struct cdev hcdev;

static const struct file_operations dev_fops = {
	.owner = THIS_MODULE,
	.read  = dev_read,
	.write = dev_write,
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
	int ret;
	dev_t dev;
	ret = alloc_chrdev_region(&dev, DEV_FIRST, DEV_COUNT, MOD_NAME);
	mod_major_num = MAJOR(dev);

	if(ret < 0) {
		printk( KERN_ERR "can not register char device region\n" );
		goto out;
	}

	cdev_init(&hcdev, &dev_fops);
	hcdev.owner = THIS_MODULE;
	hcdev.ops = &dev_fops;

	ret = cdev_add(&hcdev, dev, DEV_COUNT);
	if(ret < 0) {
		unregister_chrdev_region(MKDEV(mod_major_num, DEV_FIRST),
		                         DEV_COUNT);
		printk(KERN_ERR "can not add char device\n" );
		goto out;
	}

	strncpy(dev_buf, DEV_INIT_MSG, DEV_BUFSIZE - 1);

out:
	return ret;
}

static void __exit dev_exit( void )
{
	cdev_del( &hcdev );
	unregister_chrdev_region(MKDEV(mod_major_num, DEV_FIRST), DEV_COUNT );
}

MODULE_LICENSE("GPL");
module_init(dev_init);
module_exit(dev_exit);

