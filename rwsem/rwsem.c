#include <linux/module.h> 
#include <linux/sched.h> 
#include <linux/delay.h> 
#include <linux/kthread.h>
#include <linux/types.h>
#include <linux/atomic.h>
#include <linux/semaphore.h>
#include <linux/rwsem.h>
#include <linux/sched.h>
#include <linux/membarrier.h>

static struct rw_semaphore rwsem;

static struct task_struct *kthread1;
static struct task_struct *kthread2;
static struct task_struct *kthread3;
static struct task_struct *kthread4;
static struct task_struct *kthread5;
static struct task_struct *kthread6;
static struct task_struct *kthread7;

static int counter = 0;

static int thread_write(void * data) 
{ 
   printk("thread_write started PID: [%05d]\n", current->pid); 
   printk("waiting for semaphore to write");
     
   down_write(&rwsem);

   printk("write semaphore obtained");
   
   counter += 50;
   
   up_write(&rwsem);

   printk("write semaphore returned");
   printk("thread_write ended [%05d]\n", current->pid); 
 
   return 0; 
} 

static int thread_read(void * data) 
{ 
   printk("thread_read started PID: [%05d]\n", current->pid); 
      
   down_read(&rwsem);

   printk("read semaphore obtained");
   printk("Counter value is: %d", counter);
   ssleep(10);
    
   up_read(&rwsem);
 
   printk("thread_read ended [%05d]\n", current->pid);
  
   return 0; 
} 

int test_thread(void) 
{
   int n = 0;
   init_rwsem(&rwsem);   

   kthread1 = kthread_run(thread_write, NULL, "potok%d", n); 
   n++;
   kthread2 = kthread_run(thread_read, NULL, "potok%d", n);
   n++;
   kthread3 = kthread_run(thread_read, NULL, "potok%d", n);
   n++;   
   kthread4 = kthread_run(thread_read, NULL, "potok%d", n);
   n++;
   kthread5 = kthread_run(thread_read, NULL, "potok%d", n);
   n++;
   kthread6 = kthread_run(thread_write, NULL, "potok%d", n);
   n++;
   kthread7 = kthread_run(thread_read, NULL, "potok%d", n);
   rmb();

   kthread_stop(kthread1);
   kthread_stop(kthread2);
   kthread_stop(kthread3);
   kthread_stop(kthread4);
   kthread_stop(kthread5);
   kthread_stop(kthread6);
   kthread_stop(kthread7);


   printk( KERN_INFO "counter is: %i \n", counter);
   printk("*********************");
        
   return -1; 
} 
   
module_init(test_thread); 
MODULE_LICENSE( "GPL" );
