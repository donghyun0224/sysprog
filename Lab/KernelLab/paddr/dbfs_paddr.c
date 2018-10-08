#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <asm/pgtable.h>

#define SIZE 8192

MODULE_LICENSE("GPL");

static struct dentry *dir, *output;
static struct task_struct *task;

struct packet{
        pid_t pid;
        unsigned long vaddr;
        unsigned long paddr;
};

int i;
char stats[SIZE];
struct packet *pckt;

static ssize_t read_output(struct file *fp,
                        char __user *user_buffer,
                        size_t length,
                        loff_t *position)
{
        /*
        pid_t input_pid;
        struct pid * pid;
        for (i = 0; i < SIZE; i++) {
                stats[i] = '\0';
        }

        sscanf(user_buffer, "%u", &input_pid);
        pid = find_get_pid(input_pid);
        task = pid_task(pid, PIDTYPE_PID);

        // Implement read file operation

        pckt->pid = input_pid;
        pckt->vaddr = 1;
        pckt->paddr = 1;
        length += 12;
        */

        return length;
}

static const struct file_operations dbfs_fops = {
        // Mapping file operations with your functions
        .read = read_output
};

static int __init dbfs_module_init(void)
{
        // Implement init module

        dir = debugfs_create_dir("paddr", NULL);

        if (!dir) {
                printk("Cannot create paddr dir\n");
                return -1;
        }

        // Fill in the arguments below
        output = debugfs_create_file("output", 0666, dir, pckt, &dbfs_fops);

        return 0;
}

static void __exit dbfs_module_exit(void)
{
        // Implement exit module
}

module_init(dbfs_module_init);
module_exit(dbfs_module_exit);
