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
#include <linux/rwsem.h>

#include "structure.h"

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Shashank Karkare");


#define DEVICE_NAME		"Squeue"
#define QUEUE_SIZE		10
#define NUM_DEVICE		2


/* per-device structure */
struct my_dev{
	struct 	cdev 		cdev;			// cdev structure assiciated with our device
	struct  token*	 	queue[10];		// Queue to hold tokens
	int			qstart;			// Enqueue Position in the queue
	int			qend;			// Dequeue Position in the queue
	int 			queue_state;		// Holds how many tokens are in the queue
	char 			name[20];		// name of the device
	struct rw_semaphore	sem;			// Lock for queues read and write operations
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
	printk("%s: Device opening\n",my_squeuep->name);

	return 0;
}



/*
 * Release Squeue
 */ 
int squeue_release(struct inode *inode, struct file *file)
{
	struct my_dev  *my_squeue;

	my_squeue = file->private_data;
	printk("%s: Device closing\n",my_squeue->name);

	return 0;
}





/*
 * Read to my driver
 * Handles concurency issue using reader/writer spinlock 
 */
ssize_t squeue_read(struct file *file, char __user *buf , size_t count, loff_t *ppos)

{
	int 	     		res = 0;
	struct token*		temp;
	ssize_t 		size;
	struct my_dev*  	ptr;

	size = (sizeof(struct token) + 80);
	ptr = file->private_data;

	// If the queue is empty .. return -1 
	if(ptr->queue_state == 0)
		return -1;

	//// Needs lock here ////
	
	down_read(&ptr->sem);
	temp = ptr->queue[ptr->qend];
	res = copy_to_user((void __user *)buf, (void *)temp,size);

	ptr->queue[ptr->qend] = NULL;
	if(ptr->qend == QUEUE_SIZE-1)
		ptr->qend = 0;
	else
		ptr->qend++;
	ptr->queue_state--;
	kfree(temp);
	up_read(&ptr->sem);

	return res;

}






/*
 * Write to my driver
 */
ssize_t squeue_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{

	int res = 0;
	struct token* data;

	
	struct my_dev *ptr = file->private_data;

	if(ptr->queue_state == QUEUE_SIZE)
		return -1;

	data = kmalloc(count,GFP_KERNEL);

	//// Locking needed here ////
	down_write(&ptr->sem);
	res = copy_from_user((void *)data, (void __user *)buf, count);
	ptr->queue[ptr->qstart] = data;
	ptr->queue_state++;
	if(ptr->qstart == QUEUE_SIZE-1)
		ptr->qstart = 0;
	else
		ptr->qstart++;
	up_write(&ptr->sem);

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
	int j;				// loop variable for array of pointers
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

		/* initializing the queue start and end with zero	*/
		for(j = 0;j < QUEUE_SIZE;j++)
			my_devp[i]->queue[j] = NULL;
		my_devp[i]->qstart = 0;
		my_devp[i]->qend   = 0;
		my_devp[i]->queue_state = 0;


		/* initializing the semaphores */
		init_rwsem(&my_devp[i]->sem);
	
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

