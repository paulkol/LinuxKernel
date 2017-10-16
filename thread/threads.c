#include <linux/module.h> 
#include <linux/sched.h> 
#include <linux/delay.h> 
#include <linux/kthread.h>
#include <linux/types.h>
#include <linux/atomic.h>

static struct task_struct *kthread1;
static struct task_struct *kthread2;
static int counter = 0;

int oldone = 0;
int newone = 1;

int i = 0;
int* ptr = &i;

void lock (int* m)
{
   while(cmpxchg(m, oldone, newone));
}

void unlock(int* m)
{
   *m = 0;
}

static int thread1(void * data) 
{ 
   
   printk(KERN_INFO "thread1 counter started\n"); 
   
   int i;
   //lock(ptr);
   for (i = 0; i < 1000000; i++)
   {
      counter++;
      ndelay(1);
   }      
   //unlock(ptr);
   return 0; 
} 

static int thread2(void * data) 
{ 
   printk(KERN_INFO "thread2 counter started\n"); 
   
   int i;
   //lock(ptr);
   for (i = 0; i < 1000000; i++)
   {
      counter++;
      ndelay(1);
   }    
   //unlock(ptr);
   return 0; 
} 

int test_thread(void) 
{
     
   kthread1 = kthread_run(thread1, NULL, "potok1"); 
   kthread2 = kthread_run(thread2, NULL, "potok2");
   
   kthread_stop(kthread1);
   kthread_stop(kthread2);

   printk( KERN_INFO "counter is: %i \n", counter);
        
   return -1; 
} 
   
module_init(test_thread); 
MODULE_LICENSE( "GPL" );
