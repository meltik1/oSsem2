#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/vmalloc.h>


#define IRQ 1
#define FILE_NAME "tasklet_file"


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Zuev");


static int my_dev_id;
static void my_tasklet_function(unsigned long data);
DECLARE_TASKLET_OLD(my_tasklet, my_tasklet_function);


static int tasklet_state(struct seq_file* sf, void* v)
{
    seq_printf(sf, "***tasklet. state: %d,  data: %d***\n", my_tasklet.state,  my_tasklet.data);
    return 0;
}


static int tasklet_open(struct inode * i, struct file* file)
{
    return single_open(file, tasklet_state, NULL);
}


static struct proc_ops ops = 		// используется для опр-я обратных вызовов чтения и записи . proc_ops - тк новая версия ядра
        {
                .proc_open = tasklet_open,
                .proc_release = single_release,
                .proc_read = seq_read
        };


static void my_tasklet_function(unsigned long data)
{
    my_tasklet.data += 1;
    printk(KERN_INFO "handled\n");
}


//DECLARE_TASKLET(my_tasklet, my_tasklet_function, (unsigned long) &my_tasklet_data);		// статическая регистрация такслета. тасклетхэндлер - обработчик тасклета


irqreturn_t my_irq_handler (int irq, void *dev)
{
    if (irq == IRQ)
    {
        tasklet_schedule(&my_tasklet);
        return IRQ_HANDLED;						// прерывание обработано
    }
    return IRQ_NONE;
}


static int __init my_tasklet_init(void)
{
    struct proc_dir_entry *entry;
    entry = proc_create(FILE_NAME, S_IRUGO, NULL, &ops);
    if (!entry)
    {
        printk(KERN_ERR "***Unable to create file***\n");
        return -ENOMEM;
    }

    int res = request_irq(IRQ, my_irq_handler, IRQF_SHARED, "***my_tasklet***" , &my_dev_id);	// номер прерывания, ук-ль на обработчик, флаги, текст, разделение линий
    if (res)
    {
        printk(KERN_ERR "***Unable to register my_irq_handler***\n");
    }
    else
    {
        printk(KERN_INFO "***Module loaded***\n");
    }
    return res;
}


static void __exit my_tasklet_exit(void)
{
    remove_proc_entry(FILE_NAME, NULL);
    tasklet_kill(&my_tasklet);
    free_irq(IRQ,  &my_dev_id);		// удаляет обработчик и отключает линию прерывания
    printk(KERN_INFO "***Module unloaded***\n");
}


module_init(my_tasklet_init);
module_exit(my_tasklet_exit);
