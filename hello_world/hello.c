#include <linux/init.h>
#include <linux/module.h>

static int  hello_init(void){
    printk(KERN_ALERT "Hello world, starting Captain");
    return 0;
}

static void  hello_exit(void){
    printk(KERN_ALERT "Hello world,  ending Captain");
}

module_init(hello_init);
module_exit(hello_exit);