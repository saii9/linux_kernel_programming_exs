#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/usb.h>


//probe function : called only when the device is plugged
//only when no other driver is installed
static int usb_probe(struct usb_interface *interface, const struct usb_device_id *id) {
	printk(KERN_INFO "[*] pendrive plugged in [%04x:%04x]\n", id->idVendor, id->idProduct);
	return 0;
}

static void usb_disconnect(struct usb_interface *interface) {
	printk(KERN_INFO "[*] pendrive disconnected");
	return;
}

// this driver can support as many devices as possble
static struct usb_device_id usb_table[] =
{
	{ USB_DEVICE(0x058f, 0x6387) }, //info from lsusb
	{} //terminating entry
};

//usb driver
//058f:6387
static struct usb_driver driver = {
	.name = "myTestUSB-driver",
	.id_table = usb_table,
	.probe = usb_probe,
	.disconnect = usb_disconnect
};

static int __init usb_init(void){
	int ret = -1;
	printk(KERN_INFO "[*] pendrive constructor");
	printk(KERN_INFO "[*] reg driver with the kernel");
	ret = usb_register(&driver);
	printk(KERN_INFO "[*] registration complete");	
	return ret;
}

static void __exit usb_exit(void){
	printk(KERN_INFO "[*] pendrive destructor");
	usb_deregister(&driver);
	printk(KERN_INFO "[*] deregistration complete");	
}


module_init(usb_init);
module_exit(usb_exit);