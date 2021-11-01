#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>

#define MAX_COOKIE_LENGTH PAGE_SIZE

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Zuev Nikita");

static struct proc_dir_entry *proc_entry;
char *cookie_pot;
int write_index;
int read_index;
char buf[256];

static ssize_t fortune_write(struct file *file, const char __user *ubuf,size_t count, loff_t *ppos)
{
    if (count > MAX_COOKIE_LENGTH - write_index + 1)
    {
        printk(KERN_DEBUG "Big count\n");
        return -ENOSPC;
    }

    if (copy_from_user(cookie_pot + write_index, ubuf, count))
    {
        printk(KERN_DEBUG "Copy_from_user error\n");
        return -EFAULT;
    }

    write_index += count;
    cookie_pot[write_index - 1] = 0;
    printk(KERN_DEBUG "Good write\n");

    return count;
}

static ssize_t fortune_read(struct file *file, char __user *ubuf,size_t count, loff_t *ppos)
{
    int len = 0;

    if (*ppos > 0)
        return 0;

    if (read_index >= write_index)
    {
        read_index = 0;
    }


        len = sprintf(buf, "%s\n", cookie_pot + read_index);
        copy_to_user(ubuf, buf, len);
        read_index += len;
        printk(KERN_DEBUG "Len: %d\n", len);
        *ppos += len;


    return len;
}

static const struct proc_ops ops  = {
        .proc_read = fortune_read,
        .proc_write = fortune_write
};


static int simple_init(void)
{
    cookie_pot = (char *)vmalloc(MAX_COOKIE_LENGTH);

    if (!cookie_pot)
        return -ENOMEM;

    memset(cookie_pot, 0, MAX_COOKIE_LENGTH);
    proc_entry = proc_create("fort", 0345, NULL, &ops);

    if (proc_entry == NULL)
    {
        vfree(cookie_pot);
        printk(KERN_DEBUG "fortune: Couldn't create proc entry\n");
        return -ENOMEM;
    }

    struct proc_dir_entry *dir = proc_mkdir("fort_dir", NULL);


    struct proc_dir_entry *symlink = proc_symlink("fort_symlink", NULL, "/proc/fortune");

    if ((dir == NULL) || (symlink == NULL))
    {
        vfree(cookie_pot);
        printk(KERN_DEBUG "fortune: Couldn't create proc dir, symlink \n");
        return -ENOMEM;
    }

    write_index = 0;
    read_index = 0;
    printk(KERN_DEBUG "с: Init\n");

    return 0;
}

static void simple_cleanup(void)
{
    remove_proc_entry("fort", NULL);
    remove_proc_entry("fort_dir", NULL);
    remove_proc_entry("fort_symlink", NULL);
    vfree(cookie_pot);
    printk(KERN_DEBUG "fortune: Clean\n");
}

module_init(simple_init);
module_exit(simple_cleanup);