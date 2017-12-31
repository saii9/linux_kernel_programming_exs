//4. passing arguments to  the kernel module
#include<linux/init.h>
#include<linux/module.h>
//step 1
#include<linux/moduleparam.h>

///step 2 : create variable
int param_var = 0;
//step 3 register macro
//module_param(name_var, type, permissions)
/*
	S_IRUSR,S_IWUSR,S_IXUSR,S_IRGRP,S_IWGRP
	S_IRUSR | S_IWUSR
*/
module_param(param_var, int, S_IRUSR | S_IWUSR);

void display_var(void){
	printk(KERN_ALERT "passed val : %d", param_var);	
}

static int hello_init(void){
	display_var();
	printk(KERN_ALERT "Hello World from 4");
	return 0;
}

static void hello_exit(void){
	display_var();
	printk(KERN_ALERT "Good bye World from 4");
}

module_init(hello_init);
module_exit(hello_exit);
