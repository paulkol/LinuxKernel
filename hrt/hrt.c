#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <asm/types.h>

MODULE_LICENSE("GPL");

#define MS_TO_NS(x)     (x * 1000000L)
#define WR_BUF_SIZE 1024

static struct hrtimer hr_timer;

static int restart = 5;
static unsigned long delay_in_ms = 200L;

char wr_buf[WR_BUF_SIZE];

struct proc_dir_entry *proc;
int temp;
char *msg;



enum hrtimer_restart my_hrtimer_callback( struct hrtimer *timer)
{
	pr_info( "my_hrtimer_callback called (%lld).\n", ktime_to_ms(timer->base->get_time()));
	if (restart--) {
		hrtimer_forward_now(timer, ns_to_ktime(MS_TO_NS(delay_in_ms)));
		return HRTIMER_RESTART;
	}

	return HRTIMER_NORESTART;
}


//ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
ssize_t write_proc(struct file *filp, const char __user *buf, size_t count, loff_t *offp)
{
	char *data;
	data = PDE_DATA(file_inode(filp));
	if(!(data)){
		printk(KERN_INFO "Null data");
		return 0;
	}

	if(count > temp)
	{
		count = temp;
	}
	temp = temp - count;

	memset(data, 0, WR_BUF_SIZE);
	if (copy_from_user(data, buf, count)) {
		pr_err("Data copy error\n");
		return -EFAULT;
	}
	
	if(count == 0)
		temp = WR_BUF_SIZE;

	sscanf(data, "%i %li", &restart, &delay_in_ms);
	sprintf(msg, "%i %li", restart, delay_in_ms);

	ktime_t ktime;
	ktime = ktime_set(0, MS_TO_NS(delay_in_ms));

	hrtimer_start( &hr_timer, ktime, HRTIMER_MODE_REL );
		
	return count;
}

ssize_t read_proc(struct file *filp,char *buf,size_t count,loff_t *offp )
{
	char *data;
	data = PDE_DATA(file_inode(filp));
	if(!(data)){
		printk(KERN_INFO "Null data");
		return 0;
	}

	if(count > temp)
	{
		count = temp;
	}
	temp = temp - count;

	if (copy_to_user(buf, data, count)) {
		pr_err("Data copy error\n");
		return -EFAULT;
	}

	if(count == 0)
		temp = WR_BUF_SIZE;

	return count;
}

struct file_operations proc_fops = {
	.read = read_proc,
	.write = write_proc,
};

int init_module(void)
{
	msg = "Hello World\n";

	memcpy(wr_buf, msg, strlen(msg));
	msg = wr_buf;	

	proc = proc_create_data("parameters", 0, NULL, &proc_fops, msg);	
	
	temp = WR_BUF_SIZE;

	ktime_t ktime;

	pr_info("HR Timer module installing\n");

	ktime = ktime_set(0, MS_TO_NS(delay_in_ms));

	hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);

	hr_timer.function = &my_hrtimer_callback;

	pr_info( "Starting timer to fire in %lld ms (%ld)\n", ktime_to_ms(hr_timer.base->get_time()) + delay_in_ms, jiffies);

	hrtimer_start( &hr_timer, ktime, HRTIMER_MODE_REL );

	return 0;
}

void cleanup_module( void )
{
	remove_proc_entry("parameters",NULL);	
	int ret;

	ret = hrtimer_cancel( &hr_timer );
	if (ret)
		pr_info("The timer was still in use...\n");

	pr_info("HR Timer module uninstalling\n");

	return;
}
