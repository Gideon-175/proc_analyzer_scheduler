#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched/signal.h>
#include <linux/sched/signal.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/slab.h>




#define PROC_NAME "proc_analyzer"

/* new */
struct proc_stat {
    pid_t pid;
    u64 last_cpu_time;
    u64 switch_count;
    struct list_head list;
};

/* new */
static LIST_HEAD(stat_list);
static DEFINE_SPINLOCK(stat_lock);

static struct proc_stat *get_proc_stat(pid_t pid)
{
    struct proc_stat *p;

    list_for_each_entry(p, &stat_list, list) {
        if (p->pid == pid)
            return p;
    }

    p = kmalloc(sizeof(*p), GFP_KERNEL);
    if (!p)
        return NULL;

    p->pid = pid;
    p->last_cpu_time = 0;
    p->switch_count = 0;
    INIT_LIST_HEAD(&p->list);
    list_add(&p->list, &stat_list);

    return p;
}


/*
 * /proc show function
 * Iterates over all processes in the system
 */





// static int proc_analyzer_show(struct seq_file *m, void *v)
// {
//     struct task_struct *task;
//     u64 total_time_ms;

//     seq_printf(m, "PID\tTGID\tCPU(ms)\tCOMM\n");

//     for_each_process(task) {
//         total_time_ms = (task->utime + task->stime) / 1000000;

//         seq_printf(m, "%d\t%d\t%llu\t%s\n",
//                    task->pid,
//                    task->tgid,
//                    total_time_ms,
//                    task->comm);
//     }

//     return 0;
// }

static int proc_analyzer_show(struct seq_file *m, void *v)
{
    struct task_struct *task;
    struct proc_stat *ps;
    u64 curr_cpu_time;

    seq_printf(m, "PID\tCPU(ms)\tSWITCHES\tCOMM\n");

    spin_lock(&stat_lock);

    for_each_process(task) {
        curr_cpu_time = (task->utime + task->stime) / 1000000;

        ps = get_proc_stat(task->pid);
        if (!ps)
            continue;

        if (curr_cpu_time > ps->last_cpu_time)
            ps->switch_count++;

        ps->last_cpu_time = curr_cpu_time;

        seq_printf(m, "%d\t%llu\t%llu\t\t%s\n",
                   task->pid,
                   curr_cpu_time,
                   ps->switch_count,
                   task->comm);
    }

    spin_unlock(&stat_lock);

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
    printk(KERN_INFO "proc_analyzer: process list observer loaded\n");
    return 0;
}

// static void __exit proc_analyzer_exit(void)
// {
//     remove_proc_entry(PROC_NAME, NULL);
//     printk(KERN_INFO "proc_analyzer: process list observer unloaded\n");
// }

static void __exit proc_analyzer_exit(void)
{
    struct proc_stat *p, *tmp;

    remove_proc_entry(PROC_NAME, NULL);

    spin_lock(&stat_lock);
    list_for_each_entry_safe(p, tmp, &stat_list, list) {
        list_del(&p->list);
        kfree(p);
    }
    spin_unlock(&stat_lock);

    printk(KERN_INFO "proc_analyzer: unloaded\n");
}


module_init(proc_analyzer_init);
module_exit(proc_analyzer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Proc Analyzer Team");
MODULE_DESCRIPTION("Kernel-Level Process Behavior Analyzer");
MODULE_VERSION("0.4");