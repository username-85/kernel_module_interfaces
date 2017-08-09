#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/kernel.h>


#define DEV_BUFSIZE 256
#define DEV_NAME "study_dev"
#define DEV_CLASS_NAME "study"
#define DEV_INIT_MSG "module init msg\n"

static ssize_t dev_read(struct file *file, char *buf,
                        size_t len, loff_t *offset);
static ssize_t dev_write(struct file *file, const char *buf,
                         size_t len, loff_t *offset);


static char dev_buf[DEV_BUFSIZE];
static int mod_major_num;
static struct class*  mod_class = NULL;
static struct device* mod_device = NULL;

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


static int __init dev_init(void)
{
	mod_major_num = register_chrdev(0, DEV_NAME, &dev_fops);
	if (mod_major_num < 0) {
		printk(KERN_ALERT "register_chrdev failed\n");
		return mod_major_num;
	}

	mod_class = class_create(THIS_MODULE, DEV_CLASS_NAME);
	if (IS_ERR(mod_class)) {
		unregister_chrdev(mod_major_num, DEV_NAME);
		printk(KERN_ALERT "failed to register class\n");
		return PTR_ERR(mod_class);
	}

	mod_device = device_create(mod_class, NULL, MKDEV(mod_major_num, 0), NULL, DEV_NAME);
	if (IS_ERR(mod_device)) {
		class_destroy(mod_class);
		unregister_chrdev(mod_major_num, DEV_NAME);
		printk(KERN_ALERT "failed to create the device\n");
		return PTR_ERR(mod_device);
	}

	strncpy(dev_buf, DEV_INIT_MSG, DEV_BUFSIZE - 1);
	return 0;
}

static void __exit dev_exit( void )
{
	device_destroy(mod_class, MKDEV(mod_major_num, 0));
	class_unregister(mod_class);
	class_destroy(mod_class);
	unregister_chrdev(mod_major_num, DEV_NAME);
}

MODULE_LICENSE("GPL");
module_init(dev_init);
module_exit(dev_exit);

