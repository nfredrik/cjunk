
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>    

#include <linux/miscdevice.h>
#include <asm/uaccess.h>


MODULE_DESCRIPTION("My kernel module");
MODULE_AUTHOR("Fredrik Svärd (fredrik@dronten)");
MODULE_LICENSE("$LICENSE$");
MODULE_VERSION("0.0.1");

/* A couple of parameters that could be passed in: how many times and to whom we say hello */

static char *whom = "world";
module_param(whom, charp, 0);

static int how_many = 1;
module_param(how_many, int, 0);


/*
 * hello_read is the function called when a process calls read() on
 * /dev/hello.  It writes "Hello, world!" to the buffer passed in the
 * read() call.
 */

static ssize_t hello_read(struct file * file, char * buf, 
			  size_t count, loff_t *ppos)
{
	char *hello_str = "Hello, world!\n";
	int len = strlen(hello_str); /* Don't include the null byte. */
	/*
	 * We only support reading the whole string at once.
	 */
	if (count < len)
		return -EINVAL;
	/*
	 * If file position is non-zero, then assume the string has
	 * been read and indicate there is no more data to be read.
	 */
	if (*ppos != 0)
		return 0;
	/*
	 * Besides copying the string to the user provided buffer,
	 * this function also checks that the user has permission to
	 * write to the buffer, that it is mapped, etc.
	 */
	if (copy_to_user(buf, hello_str, len))
		return -EINVAL;
	/*
	 * Tell the user how much data we wrote.
	 */
	*ppos = len;

	return len;
}

/*
 * The only file operation we care about is read.
 */

static const struct file_operations hello_fops = {
	.owner		= THIS_MODULE,
	.read		= hello_read,
};

static struct miscdevice hello_dev = {
	/*
	 * We don't care what minor number we end up with, so tell the
	 * kernel to just pick one.
	 */
	MISC_DYNAMIC_MINOR,
	/*
	 * Name ourselves /dev/hello.
	 */
	"hello",
	/*
	 * What functions to call when a program performs file
	 * operations on the device.
	 */
	&hello_fops
};




/*
 * Write "Hello, world!" to the buffer passed through the /proc file.
 */

static int
hello_read_proc(char *buffer, char **start, off_t offset, int size, int *eof,
		void *data)
{
	char *hello_str = "Hello, world!\n";
	int len = strlen(hello_str); /* Don't include the null byte. */
	/*
	 * We only support reading the whole string at once.
	 */
	if (size < len)
		return -EINVAL;
	/*
	 * If file position is non-zero, then assume the string has
	 * been read and indicate there is no more data to be read.
	 */
	if (offset != 0)
		return 0;
	/*
	 * We know the buffer is big enough to hold the string.
	 */
	strcpy(buffer, hello_str);
	/*
	 * Signal EOF.
	 */
	*eof = 1;

	return len;

}


static int tets_init_module(void)
{
   int i, ret = 0;
	for (i=0;i<how_many;i++)
		printk( KERN_DEBUG "(%d) Hello, %s\n", i, whom);


	/*
	 * Create an entry in /proc named "hello_world" that calls
	 * hello_read_proc() when the file is read.
	 */
	if (create_proc_read_entry("hello_world", 0, NULL, hello_read_proc,
				    NULL) == 0) {
		printk(KERN_ERR
		       "Unable to register \"Hello, world!\" proc file\n");
		return -ENOMEM;
	}


	/*
	 * Create the "hello" device in the /sys/class/misc directory.
	 * Udev will automatically create the /dev/hello device using
	 * the default rules.
	 */
	ret = misc_register(&hello_dev);
	if (ret)
		printk(KERN_ERR
		       "Unable to register \"Hello, world!\" misc device\n");


	return ret;

}

static void tets_exit_module(void)
{
	remove_proc_entry("hello_world", NULL);

	misc_deregister(&hello_dev);
	printk( KERN_DEBUG "Godbye cruel %s\n", whom);
}

module_init(tets_init_module);
module_exit(tets_exit_module);
