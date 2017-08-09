#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/init.h>
#include <linux/fs.h>

static struct kobject *study_kobject;
static int val;

static ssize_t val_show(struct kobject *kobj, struct kobj_attribute *attr,
                        char *buf)
{
	return sprintf(buf, "%d\n", val);
}

static ssize_t val_store(struct kobject *kobj, struct kobj_attribute *attr,
                         const char *buf, size_t count)
{
	sscanf(buf, "%du", &val);
	return count;
}
static struct kobj_attribute val_attribute =__ATTR(val, 0660, val_show, val_store);

static int __init mod_init (void)
{
	int ret = 0;

	study_kobject = kobject_create_and_add("study_kobject", kernel_kobj);
	if(!study_kobject) {
		ret = -ENOMEM;
		goto out;
	}

	ret = sysfs_create_file(study_kobject, &val_attribute.attr);
	if (ret)
		printk(KERN_ERR
		 "failed to create file in /sys/kernel/study_kobject \n");

out:
	return ret;
}

static void __exit mod_exit (void)
{
	kobject_put(study_kobject);
}

module_init(mod_init);
module_exit(mod_exit);
MODULE_LICENSE("GPL");

