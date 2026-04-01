#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init proc_analyzer_init(void)
{
    printk(KERN_INFO "proc_analyzer: module loaded\n");
    return 0;
}

static void __exit proc_analyzer_exit(void)
{
    printk(KERN_INFO "proc_analyzer: module unloaded\n");
}

module_init(proc_analyzer_init);
module_exit(proc_analyzer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Proc Analyzer Team");
MODULE_DESCRIPTION("Kernel-Level Process Behavior Analyzer");
MODULE_VERSION("0.1");