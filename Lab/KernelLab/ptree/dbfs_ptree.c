#include <linux/debugfs.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/slab.h> // library for debugfs_blob_wrapper

#define SIZE 8192 // Some large number

MODULE_LICENSE("GPL");

static struct dentry *dir, *inputdir, *ptreedir;
static struct task_struct *curr;
struct debugfs_blob_wrapper *myblob; // Maximal data type

// Simple stack

struct stack {
	struct task_struct *task;
	struct stack *next;
};


char stats[SIZE];
int i;

static ssize_t write_pid_to_input(struct file *fp, 
                                const char __user *user_buffer, 
                                size_t length, 
                                loff_t *position)
{
        pid_t input_pid;
        struct pid * pid;

		// Initialize buffer as zero
        for (i = 0; i < SIZE; i++){
                stats[i] = '\0';
        }

		// Now, input_pid is correct pid and curr is what correct current task_struct
        sscanf(user_buffer, "%u", &input_pid);
        pid = find_get_pid(input_pid);
        curr = pid_task(pid, PIDTYPE_PID);
		// Find task_struct using input_pid. Hint: pid_task <- I used it!

		
		// TODO: Make a simple stack
		struct stack *stack, *temp;

		stack = (struct stack *) kmalloc(sizeof(struct stack), GFP_KERNEL);
		temp = (struct stack *) kmalloc(sizeof(struct stack), GFP_KERNEL);

		temp->next = NULL;
		temp->task = curr;

		// canaria will save us
		
        // Tracing process tree from input_pid to init(1) process
        while(1) {
			stack->next = temp;
            if (curr->pid == 1) break;
            curr = curr->real_parent;
			stack->task = curr;
			temp = stack;
			stack = (struct stack *) kmalloc(sizeof(struct stack), GFP_KERNEL);
        }

		stack = stack->next;

		while(1) {
			curr = stack->task;
            length += sprintf(stats + length, "%s (%d)\n", curr->comm, curr->pid);
			stack = stack->next;
			if (stack == NULL) break;
		}

        return length;
}

static const struct file_operations dbfs_fops = {
        .write = write_pid_to_input,
};

static int __init dbfs_module_init(void)
{

        int stats_size;
        int struct_size;

        // Implement init module code

        dir = debugfs_create_dir("ptree", NULL);


        struct_size = sizeof(struct debugfs_blob_wrapper);
        stats_size = 8192 * sizeof(char);

        myblob = (struct debugfs_blob_wrapper *) kmalloc(struct_size, GFP_KERNEL);

        myblob->data = (void *) stats;
        myblob->size = (unsigned long) stats_size;

        inputdir = debugfs_create_file("input", 0644, dir, NULL, &dbfs_fops);
        ptreedir = debugfs_create_blob("ptree", 0644, dir, myblob); // Find suitable debugfs API

        return 0;
}

static void __exit dbfs_module_exit(void)
{
        // Implement exit module code
        kfree(myblob);
        debugfs_remove(ptreedir);
        debugfs_remove(inputdir);
        debugfs_remove_recursive(dir);
}

module_init(dbfs_module_init);
module_exit(dbfs_module_exit);
