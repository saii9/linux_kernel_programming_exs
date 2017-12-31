#include<linux/module.h>
#include<linux/kernel.h>
//file ops structure which will allow open/ close read/write to device
#include<linux/fs.h>
//make cdev available for a char driver
#include<linux/cdev.h>
//used to access semaphores, synchronization devices
#include<linux/semaphore.h>
//copy_to_user and copy_from_user
#include<asm/uaccess.h>
#define DEVICE_NAME "mytestdevice" // as appers in /proc/devices

//(1) create a fake device
struct fake_device{
	char data[100];
	struct semaphore sem;
} virtual_device;

//(2) to register out device we need a cdev object and some other vars
struct cdev *mcdev; // stands for my char device
//will store major number- extracted from dev_t using macro -
//mknod /dir/file c major minor
int major_number;
//to hold the return values of functions. this is because the kernal stack is 
//very small . declaring the variables all over will eat up the stack
int ret;
dev_t dev_num; //will hold the major number kernel gives us

//(7) called on device file open
//	inode refs to file on disk and contains info about that file
//	struct file represents an abstract open file
int device_open(struct inode *inode, struct file *filp){
	//only allow one process to open this device
	//using semaphore as mutual exclusive lock - mutex
	if(down_interruptible(&virtual_device.sem) != 0){
		printk(KERN_ALERT "could not lock device during open");
		return -1;
	}
	printk(KERN_INFO "char device: opened device");
	return 0;
}

//(8) called when user wants to take information from the device
ssize_t device_read(struct file* filp, char* bufStoreData, size_t bufCount, 
	loff_t* curOffset){
	//take data from kernel space to user space(process)
	//copy to user(destination, source, size_to_transfer)
	printk(KERN_INFO "reding from the device");
	ret = copy_to_user(bufStoreData, virtual_device.data, bufCount);
	return ret;
}

//(9) called when user wants to write info the device
ssize_t device_write(struct file* filp, const char* bufStoreData, 
	size_t bufCount, loff_t* curOffset){
	//send info to kernel space from user space(process)
	//copy_from_user(destination, source, size_to_transfer)
	printk(KERN_INFO "writing to the device");
	ret = copy_from_user(virtual_device.data, bufStoreData, bufCount);
	return ret;
}

//(10) called up on close
int device_close(struct inode *inode, struct file *filp){
	// up => opposite of down. releases the mutex
	//this will allow other process to use the device now
	up(&virtual_device.sem);
	printk(KERN_INFO "char_driver: closed device");
	return 0;
}

//(6) give fucntion call backs to kernal for actions on device files
struct  file_operations fops = {
	.owner = THIS_MODULE, 	//prevent unloading of module when ops in use
	.open = device_open,  	// to call when opening the device file
	.release = device_close,// closing
	.write = device_write,  // writing 
	.read = device_read	//reading
};

static int driver_entry(void) {
	//(3) register device with teh system is a 2 step process
	//1=> use dynamic allocation to assign our device a major number 
	//	  --alloc_chrdev_region(dev_t* , uint fminor, uint count, char* name)
	// use the dev num which is gng to hold major and minor number
	//start with a minor number of 0 and allocate up to 1 minor number
	ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	if (ret < 0){
		printk(KERN_ALERT "char driver - failed to allocate a major number");
		return ret;
	}
	major_number = MAJOR(dev_num);
	printk(KERN_INFO "char device: %s major number is %d", DEVICE_NAME, 
		major_number);
	printk(KERN_INFO "\tuse \"mknod /dev/%s c %d 0\" for a device file", 
		DEVICE_NAME, major_number);

	//2 => create a char struct and add to the system
	mcdev = cdev_alloc(); //create own cdev structure. init cdev
	mcdev->ops = &fops;
	mcdev->owner = THIS_MODULE;
	//now that we created cdev, we add it to the kernel
	//int cdev_add(struct *cdev dev, dev_t num, unsigned int count)
	ret = cdev_add(mcdev, dev_num, 1);
	if (ret < 0){
		printk(KERN_ALERT "char driver - unable to add cdev to kernel");
		return ret;
	}
	//(4) initialize our semaphore
	sema_init(&virtual_device.sem, 1);
	return 0;
}

static void driver_exit(void){
	//(5) unregister everything in the reverse order
	cdev_del(mcdev);

	unregister_chrdev_region(dev_num, 1);
	printk(KERN_INFO "unloaded module");
}

//inform kernal to where to stop and where to start
module_init(driver_entry);
module_exit(driver_exit);






