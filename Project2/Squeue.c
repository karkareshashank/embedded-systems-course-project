/*
 *  Squeue.c
 * It implements a shared queue device as a IPC.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/pci.h>
#include <linux/param.h>
#include <linux/list.h>
#include <asm/uaccess.h>

#include "structure.h"

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Shashank Karkare");


#define DEVICE_NAME		"Squeue"
#define QUEUE_SIZE		10
#define NUM_DEVICE		2


/* Structure to hold tokens in link list */
struct queue{
		struct token* tok;
		struct list_head list;
	};


/* per-device structure */
struct my_dev{
	struct 	cdev 		cdev;			// cdev structure assiciated with our device
	struct list_head 	head;			// Queue to hold tokens
	int 			queue_state;		// Holds how many tokens are in the queue
	struct list_head*	start;
	struct list_head*	end;
//	struct  token*  	queue[QUEUE_SIZE];	// Queue to hold tokens
	char 			name[20];		// name of the device
//	int 			start; 			// enqueueing position
//	int 			end;			// dequeueing position
}*my_devp[NUM_DEVICE];


static dev_t my_dev_number[NUM_DEVICE];	// Alloted device number //
struct class *my_dev_class[NUM_DEVICE]; // Tie with device model //





/*
 * Open Squeue
 */
int squeue_open(struct inode *inode, struct file *file)
{
	struct my_dev *my_squeuep;

	my_squeuep = container_of(inode->i_cdev, struct my_dev,cdev);

	file->private_data = my_squeuep;

	return 0;
}



/*
 * Release Squeue
 */ 
int squeue_release(struct inode *inode, struct file *file)
{
	struct my_dev  *my_squeue;

	my_squeue = file->private_data;

	return 0;
}





/*
 * Read to my driver
 * Handles concurency issue using reader/writer spinlock 
 */
ssize_t squeue_read(struct file *file, char __user *buf , size_t count, loff_t *ppos)

{
	int 	     		res;
	struct token*		temp;
	ssize_t 		size;
	struct my_dev*  	ptr;
	struct list_head* 	pos;
	struct queue*		qtemp;

	ptr = file->private_data;

	// If the queue is empty .. return -1 
	if(ptr->queue_state == 0)
		return -1;

	//// Needs lock here ////
	pos = ptr->head.next;
	qtemp = container_of(pos,struct queue,list);
	temp = qtemp->tok;
	size = sizeof(struct token) + 80;
	res = copy_to_user((void __user *) buf , (void *)temp, size);

	list_del(pos);
	ptr->queue_state--;
	/// Needs unlock here ////


	kfree(temp);
	return res;

}






/*
 * Write to my driver
 */
ssize_t squeue_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{

	int res;
	struct queue  temp;
	struct token* data;

	
	struct my_dev *my_devp = file->private_data;

	if(my_devp->queue_state == QUEUE_SIZE)
		return -1;

	data = kmalloc(count,GFP_KERNEL);

	//// Locking needed here ////
	res = copy_from_user((void *)data, (void __user *)buf, count);	
	INIT_LIST_HEAD(&temp.list);
	temp.tok = data;
	list_add_tail(&temp.list,&my_devp->head);	
	my_devp->queue_state++;
	
	//// unlocking needed here //////

	return res;

}












/* File operations structure. Defined in linux/fs.h */
static struct file_operations My_fops = {
    .owner   = THIS_MODULE,             /* Owner          */
    .open    = squeue_open,             /* Open method    */
    .release = squeue_release,          /* Release method */
    .read    = squeue_read,             /* Read method    */
    .write   = squeue_write,            /* Write method   */
};





/*
 * Driver Initialization
 */
int __init My_driver_init(void)
{
	int ret;			// Variable to store the return value form copy_to/from_user function
	int i;				// loop variable to loop over all devices	
	char new_device_name[20];	// To store device name appended with 1 or 2
	

	for (i = 0; i  < NUM_DEVICE; i++)
	{
		strcpy(new_device_name, DEVICE_NAME);
			
		if(i == 0)
			strcat(new_device_name,"1");
		else
			strcat(new_device_name,"2");

		/* Request dynamic allocation of a device major number */
		if (alloc_chrdev_region(&my_dev_number[i], 0, 1, new_device_name) < 0) {
			printk(KERN_DEBUG "Can't register device\n"); return -1;
		}

		/* Populate sysfs entries */
		my_dev_class[i] = class_create(THIS_MODULE, new_device_name);

	
		/* Allocate memory for the per-device structure */
		my_devp[i] = kmalloc(sizeof(struct my_dev), GFP_KERNEL);
		if (!my_devp[i]) {
			printk("Bad Kmalloc\n"); return -ENOMEM;
		}

		/* initializing the queue start and end with zero	
		my_devp[i]->start 	= 0;
		my_devp[i]->end		= 0;
		*/

		/* Initializing the queue head */
		INIT_LIST_HEAD(&my_devp[i]->head);
		my_devp[i]->queue_state = 0;
		my_devp[i]->start = NULL;
		my_devp[i]->end   = NULL;
	
		/* Request I/O region */
		sprintf(my_devp[i]->name, new_device_name);


		/* Connect the file operations with the cdev */
		cdev_init(&my_devp[i]->cdev, &My_fops);
		my_devp[i]->cdev.owner = THIS_MODULE;

		/* Connect the major/minor number to the cdev */
		ret = cdev_add(&my_devp[i]->cdev, (my_dev_number[i]), 1);

		if (ret) {
			printk("Bad cdev\n");
			return ret;
		}

		/* Send uevents to udev, so it'll create /dev nodes */
		device_create(my_dev_class[i], NULL, MKDEV(MAJOR(my_dev_number[i]), 0), NULL, new_device_name);		
	}

	printk("My Driver Initialized.\n");
	return 0;
}


/* Driver Exit */
void __exit My_driver_exit(void)
{
	int i;

	for(i = 0; i < NUM_DEVICE; i++)
	{
		/* Release the major number */
		unregister_chrdev_region((my_dev_number[i]), 1);

		/* Destroy device */
		device_destroy (my_dev_class[i], MKDEV(MAJOR(my_dev_number[i]), 0));
		cdev_del(&my_devp[i]->cdev);
		kfree(my_devp[i]);
	
		/* Destroy driver_class */
		class_destroy(my_dev_class[i]);
	}

	printk("My Driver removed.\n");
}

module_init(My_driver_init);
module_exit(My_driver_exit);

