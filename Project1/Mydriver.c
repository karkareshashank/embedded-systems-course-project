/* My driver  for Lab 1*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/pci.h>
#include <linux/spinlock.h>


#define DEVICE_NAME                 "My_device1"

/* per device structure */
struct My_dev {
	struct cdev cdev;               /* The cdev structure */
	char name[20];                  /* Name of device*/
	char in_string[80];			/* buffer for the input string */
} *my_devp;

static dev_t my_dev_number;      /* Allotted device number */
struct class *my_dev_class;          /* Tie with the device model */


/*
* Open My driver
*/
int My_driver_open(struct inode *inode, struct file *file)
{
	struct My_dev *my_devp;
	
//	printk("\nopening\n");

	/* Get the per-device structure that contains this cdev */
	my_devp = container_of(inode->i_cdev, struct My_dev, cdev);

	/* Easy access to cmos_devp from rest of the entry points */
	file->private_data = my_devp;

	printk("\n%s is openning\n", my_devp->name);
	return 0;
}

/*
 * Release My driver
 */
int My_driver_release(struct inode *inode, struct file *file)
{
	struct My_dev *my_devp = file->private_data;
	
	printk("\n%s is closing\n", my_devp->name);
	
	return 0;
}


/*
 * Write to my driver
 */
ssize_t My_driver_write(struct file *file, const char *buf,
           size_t count, loff_t *ppos)
{
	int res;
	struct My_dev *my_devp = file->private_data;
	
	res = copy_from_user((void *)&my_devp->in_string, (void __user *)buf, count);	

	printk("\n%s \n", my_devp->in_string);

	return res;
}


/* File operations structure. Defined in linux/fs.h */
static struct file_operations My_fops = {
    .owner = THIS_MODULE,           /* Owner */
    .open = My_driver_open,              /* Open method */
    .release = My_driver_release,        /* Release method */
    .write = My_driver_write,            /* Write method */
};

/*
 * Driver Initialization
 */
int __init My_driver_init(void)
{
	int ret;

	/* Request dynamic allocation of a device major number */
	if (alloc_chrdev_region(&my_dev_number, 0, 1, DEVICE_NAME) < 0) {
			printk(KERN_DEBUG "Can't register device\n"); return -1;
	}

	/* Populate sysfs entries */
	my_dev_class = class_create(THIS_MODULE, DEVICE_NAME);

	
	/* Allocate memory for the per-device structure */
	my_devp = kmalloc(sizeof(struct My_dev), GFP_KERNEL);
		
	if (!my_devp) {
		printk("Bad Kmalloc\n"); return -ENOMEM;
	}

	/* Request I/O region */
	sprintf(my_devp->name, DEVICE_NAME);


	/* Connect the file operations with the cdev */
	cdev_init(&my_devp->cdev, &My_fops);
	my_devp->cdev.owner = THIS_MODULE;

	/* Connect the major/minor number to the cdev */
	ret = cdev_add(&my_devp->cdev, (my_dev_number), 1);

	if (ret) {
		printk("Bad cdev\n");
		return ret;
	}

	/* Send uevents to udev, so it'll create /dev nodes */
	device_create(my_dev_class, NULL, MKDEV(MAJOR(my_dev_number), 0), NULL, DEVICE_NAME);		
	

	printk("My Driver Initialized.\n");
	return 0;
}
/* Driver Exit */
void __exit My_driver_exit(void)
{
	
	/* Release the major number */
	unregister_chrdev_region((my_dev_number), 1);

	/* Destroy device */
	device_destroy (my_dev_class, MKDEV(MAJOR(my_dev_number), 0));
	cdev_del(&my_devp->cdev);
	kfree(my_devp);
	
	/* Destroy driver_class */
	class_destroy(my_dev_class);

	printk("My Driver removed.\n");
}

module_init(My_driver_init);
module_exit(My_driver_exit);
MODULE_LICENSE("GPL v2");
