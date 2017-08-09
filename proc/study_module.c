#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>

#define LEN_MSG 256
#define NAME_NODE "study"
#define INIT_MSG "module init msg\n"

static int proc_open(struct inode *inode, struct  file *file);

static ssize_t proc_write(struct file *file, const char *buf,
                          size_t count, loff_t *ppos);

static char buf_msg[LEN_MSG + 1];

static const struct file_operations proc_fops = {
	.owner = THIS_MODULE,
	.open = proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
	.write = proc_write,
};

static int proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, buf_msg);
	return 0;
}

static int proc_open(struct inode *inode, struct  file *file)
{
	return single_open(file, proc_show, NULL);
}

static ssize_t proc_write( struct file *file, const char *buf,
                           size_t count, loff_t *ppos )
{
	int res, len = count < LEN_MSG ? count : LEN_MSG;
	res = copy_from_user( &buf_msg, (void*)buf, len );
	if( '\n' == buf_msg[ len -1 ] ) buf_msg[ len -1 ] = '\0';
	else buf_msg[ len ] = '\0';
	return len;
}

static int __init proc_init( void )
{
	int ret = 0;
	struct proc_dir_entry *own_proc_node;
	own_proc_node = proc_create( NAME_NODE, S_IFREG | S_IRUGO | S_IWUGO,
	                             NULL, &proc_fops);
	if (NULL == own_proc_node) {
		ret = -ENOMEM;
		goto out;
	}
	strncpy(buf_msg, INIT_MSG, LEN_MSG);

out:
	return ret;
}

static void __exit proc_exit( void )
{
	remove_proc_entry(NAME_NODE, NULL );
}

MODULE_LICENSE("GPL");
module_init(proc_init);
module_exit(proc_exit);

