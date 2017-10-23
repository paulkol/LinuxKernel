#include <linux/module.h> 
#include <linux/slab.h> 
#include <linux/version.h> 

#define SLABNAME "my_cache"

static int size = 10;  
static int number = 1;
    
static void* *line = NULL;
          
static int cons_called;

static void constructor(void* p) 
{
   cons_called++;
} 

struct kmem_cache *cache = NULL;
    
static int __init ins( void ) 
{
   int i;
   
   line = kmalloc(sizeof(void*) *number, GFP_KERNEL);
  
   for(i = 0; i < number; i++)
      line[i] = NULL;

   cache = kmem_cache_create(SLABNAME, size, 0, SLAB_HWCACHE_ALIGN, constructor);

   for(i = 0; i < number; i++)
      line[i] = kmem_cache_alloc(cache, GFP_KERNEL);
        
   printk(KERN_INFO "allocate %d objects into slab: %s\n", number, SLABNAME);
   printk(KERN_INFO "object size %d bytes, full size %ld bytes\n", size, (long)size * number);
   printk(KERN_INFO "constructor called %d times\n", cons_called);
   
   return 0;

}

static void __exit rem(void) 
{
   int i;
   for(i = 0; i < number; i++)
      kmem_cache_free(cache, line[i]);
   kmem_cache_destroy(cache);
   kfree(line);
}

module_init(ins);
module_exit(rem);
MODULE_LICENSE("GPL");
