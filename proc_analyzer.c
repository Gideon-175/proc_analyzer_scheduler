#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#define PROC_NAME "proc_analyzer"

static int proc_analyzer_show(struct seq_file *m, void *v)
{
    seq_printf(m, "proc_analyzer loaded successfully\n");
    seq_printf(m, "This is STEP 3: /proc interface test\n");
    return 0;
}

static int proc_analyzer_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_analyzer_show, NULL);
}

static const struct proc_ops proc_analyzer_ops = {
    .proc_open    = proc_analyzer_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

static int __init proc_analyzer_init(void)
{
    proc_create(PROC_NAME, 0, NULL, &proc_analyzer_ops);
    printk(KERN_INFO "proc_analyzer: /proc entry created\n");
    return 0;
}

static void __exit proc_analyzer_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "proc_analyzer: /proc entry removed\n");
}

module_init(proc_analyzer_init);
module_exit(proc_analyzer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Proc Analyzer Team");
MODULE_DESCRIPTION("Kernel-Level Process Behavior Analyzer");
MODULE_VERSION("0.2");