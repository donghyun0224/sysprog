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

struct packet *pckt;

static ssize_t read_output(struct file *fp,
                        char __user *user_buffer,
                        size_t length,
                        loff_t *position)
{

		// Initialization pckt->pid, pckt->vaddr
		pckt = (struct packet *)user_buffer;

		// TODO: Implement page table model.
		// Currently, pid and vaddr is valid but paddr is zero.


		// Initialized task
		struct pid *pid;
		pid = find_get_pid(pckt->pid);
		task = pid_task(pid, PIDTYPE_PID);


		// Decalre pgd, pud, pmd, pte
		pgd_t *pgd;
		pud_t *pud;
		pmd_t *pmd;
		pte_t *pte;

		unsigned long page_addr = 0;
		unsigned long page_offset = 0;


		// pgd, pud, pmd, pte phase
		pgd = pgd_offset(task->mm, pckt->vaddr);
		// Something printk statement

		pud = pud_offset(pgd, pckt->vaddr);
		// Something printk statement

		pmd = pmd_offset(pud, pckt->vaddr);
		// Something printk statement

		pte = pte_offset_kernel(pmd, pckt->vaddr);
		// Something printk statement


		// paddr phase
		page_addr = pte_val(*pte) & PTE_PFN_MASK;
		page_offset = pckt->vaddr & ~PTE_PFN_MASK;
		pckt->paddr = page_addr | page_offset;

		length += sizeof(struct packet);
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
        output = debugfs_create_file("output", 0644, dir, pckt, &dbfs_fops);

        return 0;
}

static void __exit dbfs_module_exit(void)
{
        // Implement exit module
		debugfs_remove(output);
		debugfs_remove_recursive(dir);
}

module_init(dbfs_module_init);
module_exit(dbfs_module_exit);
