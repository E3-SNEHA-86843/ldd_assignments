#include<linux/module.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<linux/cdev.h>
#include<linux/kfifo.h>
#include "pchar_ioctl.h"
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/delay.h>

#define MAX 32

static struct timer_list timer;
static int count = 0;
static struct kfifo mybuf;
static dev_t devno;
static int major = 250;
static struct class *pclass;
static struct cdev pchar_cdev;
static int flag=0;
char ab;
void timer_callback(struct timer_list *tm){
	pr_info("%s: timer callback fun() called[%d]\n",THIS_MODULE->name,count++);
	int result;
	mod_timer(&timer, jiffies + msecs_to_jiffies(1000));
	result=kfifo_out(&mybuf,&ab,1);
	if(result < 0)
	{
		pr_err("%s kfifo out failed \n",THIS_MODULE->name);
	}
	pr_err("%s: element copied that is %c \n",THIS_MODULE->name,ab);
	flag=1;
}

static int pchar_open(struct inode *pinode,struct file *pfile){

	pr_info("%s:pchar_open() called \n",THIS_MODULE->name);
	return 0;
}
static int pchar_close(struct inode *pinode,struct file *pfile){

	pr_info("%s:pchar_close() called\n",THIS_MODULE->name);
	return 0;
}
static ssize_t pchar_write(struct file *pfile,const char __user *ubuf,size_t bufsize,loff_t *pf_pos){

	int nbytes,ret;
	pr_info("%s:pchar_write() called\n",THIS_MODULE->name);
	ret = kfifo_from_user(&mybuf,ubuf,bufsize,&nbytes);
	if(ret!=0){
	
		printk(KERN_INFO"%s:kfifo_from_user() failed\n",THIS_MODULE->name);
		return ret;
	}
	printk(KERN_INFO"%s:pchar_write() written %d bytes to mybuf\n",THIS_MODULE->name,nbytes);
	return nbytes;
}

static ssize_t pchar_read(struct file *pfile,char __user *ubuf, size_t bufsize,loff_t *pf_pos){

	int ret,nbytes;
	printk(KERN_INFO"%s:pchar_read() called\n",THIS_MODULE->name);
	timer_setup(&timer, timer_callback, 0);
	ret = kfifo_to_user(&mybuf,ubuf,bufsize,&nbytes);
	if(ret!=0){
	
		printk(KERN_ERR"%s:kfifo_to_user() failed\n",THIS_MODULE->name);
	return ret;
	}
	printk(KERN_INFO"%s:pchar_read() read %d bytes from mybuf\n",THIS_MODULE->name,nbytes);
	return nbytes;
}
static long pchar_ioctl(struct file *pfile,unsigned int cmd,unsigned long param){

	devinfo_t info;
	int ret,nbytes;

	switch(cmd){

		case FIFO_CLEAR:
			kfifo_reset(&mybuf);
			printk(KERN_INFO"%s:pchar_ioctl() dev buffer is cleared\n",THIS_MODULE->name);
			return 0;
		case FIFO_GETINFO:
			info.size = kfifo_size(&mybuf);
			info.len=kfifo_len(&mybuf);
			info.avil = kfifo_avail(&mybuf);
			ret = copy_to_user((void*)param,&info,sizeof(info));
			if(ret < 0){
				printk(KERN_ERR"%s:copy_to_user() failed\n",THIS_MODULE->name);
			return ret;
			}
			printk(KERN_INFO"%s:pchar_ioctl() read dev buffer info\n",THIS_MODULE->name);
			return 0;
		case  FIFO_START_TIMER:
			timer_setup(&timer, timer_callback, 0);
    		timer.expires = jiffies + 1 * HZ;
			add_timer(&timer);
			pr_info("%s: timer is setup & scheduled.\n", THIS_MODULE->name);
			return 0;
		case FIFO_STOP_TIMER:
			if(flag == 1){
				del_timer_sync(&timer);
    			pr_info("%s: timer is destroyed.\n", THIS_MODULE->name);
			}
			return 0;
		case FIFREEZE:
			//allocate temp array to store fifo contents - kmalloc() (of length same as fifo).
			void *temp_buf = kmalloc(kfifo_size(&mybuf),GFP_KERNEL); 
			if(temp_buf < 0){
					printk(KERN_ERR"%s:kmalloc() failed to store fifo content\n",THIS_MODULE->name);
					return -1;
			}
			// copy fifo contents into that temp array - kfifo_out()
             		ret = kfifo_out(&mybuf,temp_buf,kfifo_len(&mybuf));
					if(ret!=0){
					
							printk(KERN_ERR"%s:kfifo_out() failed to copy content\n",THIS_MODULE->name);
							return -1;
					}
					nbytes=ret;
            // release kfifo memory - kfifo_free()
            		kfifo_free(&mybuf);
            // allocate new memory for the fifo of size "param" (3rd arg) - kfifo_alloc()
            		ret = kfifo_alloc(&mybuf,MAX,GFP_KERNEL);
					if(ret!=0){
						printk(KERN_ERR"%s:kfifo_alloc() failed\n",THIS_MODULE->name);
						kfree(temp_buf);
						return -1;
					}
            // copy contents from temp memory into kfifo - kfifo_in()
					ret = kfifo_in(&mybuf,temp_buf,nbytes);
					if(ret!=nbytes){
					
						printk("%s:kfifo_in() failed while restoring data\n",THIS_MODULE->name);
						kfifo_free(&mybuf);
						kfree(temp_buf);
					}
            // release temp array - kfree()
					kfree(temp_buf);
            return 0;
	}
	printk(KERN_ERR"%s:invalid command in pchar_ioctl()\n",THIS_MODULE->name);
	return -EINVAL;
}
static struct file_operations pchar_fops = {
	.owner = THIS_MODULE,
	.open = pchar_open,
	.release = pchar_close,
	.write = pchar_write,
	.read = pchar_read,
	.unlocked_ioctl = pchar_ioctl
};

static int __init pchar_init(void){

	int ret;
	struct device *pdevice;
	printk(KERN_INFO "%s:pchar_init() called\n",THIS_MODULE->name);
	
	devno = MKDEV(major,0);
	ret = alloc_chrdev_region(&devno,0,1,"pchar");
	if(ret != 0){
		printk(KERN_ERR"%s:alloc_chrdev_region() failed\n",THIS_MODULE->name);
		return ret;
	}
	major = MAJOR(devno);
	printk(KERN_INFO"%s:alloc_chrdev_region() device num:%d\n",THIS_MODULE->name ,major);
	pclass = class_create("pchar_class");
	if(IS_ERR(pclass)){
		printk(KERN_ERR"%s:class_create() failed\n",THIS_MODULE->name);
		unregister_chrdev_region(devno,1);
		return -1;
	}
	printk(KERN_INFO"%s: class_create() created pchar_class\n",THIS_MODULE->name);
	pdevice = device_create(pclass,NULL,devno,NULL,"pchar");
	if(IS_ERR(pdevice)){
		printk(KERN_ERR"%s:device_create() failed\n",THIS_MODULE->name);
		class_destroy(pclass);
		unregister_chrdev_region(devno,1);
		return -1;
	}
	printk(KERN_INFO "%s: device_create() created pchar device.\n", THIS_MODULE->name);
	pchar_cdev.owner = THIS_MODULE;
    	cdev_init(&pchar_cdev, &pchar_fops);
    	ret = cdev_add(&pchar_cdev, devno, 1);
    	if(ret != 0) {
        	printk(KERN_ERR "%s: cdev_add() failed.\n", THIS_MODULE->name);
        	device_destroy(pclass, devno);
        	class_destroy(pclass);
        	unregister_chrdev_region(devno, 1);
        	return -1;
    	}
	printk(KERN_INFO "%s: cdev_add() added device in kernel.\n", THIS_MODULE->name);

    // allocate kfifo
    	ret = kfifo_alloc(&mybuf, MAX, GFP_KERNEL);
    	if(ret != 0) {
        	printk(KERN_ERR "%s: kfifo_alloc() failed.\n", THIS_MODULE->name);
        	cdev_del(&pchar_cdev);
        	device_destroy(pclass, devno);
        	class_destroy(pclass);
        	unregister_chrdev_region(devno, 1);
        	return ret;
    	}
    	printk(KERN_INFO "%s: kfifo_alloc() allocated fifo of size %d.\n", THIS_MODULE->name, kfifo_size(&mybuf));
    	return 0;
}
static void __exit pchar_exit(void){
	printk(KERN_INFO"%s:pchar_exit() called\n",THIS_MODULE->name);
	
	kfifo_free(&mybuf);
	printk(KERN_INFO "%s: kfifo_free() released kfifo.\n", THIS_MODULE->name);
	cdev_del(&pchar_cdev);
    printk(KERN_INFO "%s: cdev_del() removed device from kernel.\n", THIS_MODULE->name);
    // destroy device file
    device_destroy(pclass, devno);
    printk(KERN_INFO "%s: device_destroy() destroyed pchar device.\n", THIS_MODULE->name);
    // destroy device class
    class_destroy(pclass);
    printk(KERN_INFO "%s: class_destroy() destroyed pchar_class.\n", THIS_MODULE->name);
    // release device number
    unregister_chrdev_region(devno, 1);
    printk(KERN_INFO "%s: unregister_chrdev_region() released device num: %d.\n", THIS_MODULE->name, major);

}
module_init(pchar_init);
module_exit(pchar_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Pseudo Char Device Driver");
MODULE_AUTHOR("Nilesh Ghule <nilesh@sunbeaminfo.com>");
