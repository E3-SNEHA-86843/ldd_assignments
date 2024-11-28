#include <linux/module.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/sched/task.h>


static int __init desd_init(void){
    struct task_struct *trav;
    printk(KERN_INFO "%s:desd_init()\n", THIS_MODULE->name);
    list_for_each_entry(trav,&current->tasks, tasks) {
        pr_info("%s: pid=%d, cmd=%s\n", THIS_MODULE->name, trav->pid, trav->comm);
    }
    return 0;
}

static void __exit desd_exit(void){
    printk(KERN_INFO "%s:desd_exit()\n", THIS_MODULE->name);
    pr_info("%s: current process pid=%d, cmd=%s\n", THIS_MODULE->name, current->pid, current->comm);
    
}

module_init(desd_init);
module_exit(desd_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("HELLO KERNEL");
MODULE_AUTHOR("desd");
