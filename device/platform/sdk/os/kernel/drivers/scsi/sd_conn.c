/*-------------------------------------------------------------------------
** include
**-----------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <asm/io.h>

/*-------------------------------------------------------------------------
** define
**-----------------------------------------------------------------------*/
#define SD_CONN_MAJOR    241
#define SD_CONN_NAME     "sd_conn"

/*-------------------------------------------------------------------------
** global variable
**-----------------------------------------------------------------------*/
atomic_t conn_status;
int flag_wakeup;
DECLARE_WAIT_QUEUE_HEAD(sd_conn_wait);

/*-------------------------------------------------------------------------
** static variable
**-----------------------------------------------------------------------*/
static int ext_status;

/*-------------------------------------------------------------------------
** functions
**-----------------------------------------------------------------------*/
static ssize_t sd_conn_read (struct file *file, char __user *buf, size_t count, loff_t *ppos) 
{
	int rc;

	if (count < sizeof(int)) {
		return -EFAULT;
	}

check:	
	if(flag_wakeup) {
		flag_wakeup = 0;
		ext_status = atomic_read(&conn_status);
		rc = copy_to_user(buf, &ext_status, sizeof(int));
		if (rc < 0)
			return rc;	
		
		return sizeof(int);
	}

	interruptible_sleep_on(&sd_conn_wait);
	if (flag_wakeup) {
		goto check;
	}

	return -1;
}

static ssize_t sd_conn_write (struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	wake_up_interruptible(&sd_conn_wait);
	
	return count;
}


struct file_operations sd_conn_fops = {
	.owner = THIS_MODULE,
	.read =	sd_conn_read,
	.write = sd_conn_write,
};

static int __init sd_conn_init_module (void)
{
	return register_chrdev(SD_CONN_MAJOR, SD_CONN_NAME, &sd_conn_fops);
}

static void __exit sd_conn_cleanup_module (void)
{
	unregister_chrdev(SD_CONN_MAJOR, SD_CONN_NAME);
}

module_init(sd_conn_init_module);
module_exit(sd_conn_cleanup_module);
MODULE_AUTHOR("www.humaxdigital.com");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Linux Device Driver for get scsi disk connection status");
