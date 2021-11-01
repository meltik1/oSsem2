#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/workqueue.h>
#include <linux/slab.h>

#define IRQ 1


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Zuev");



static int my_dev_id;
static int irq_q = 0;
static struct workqueue_struct *my_workqueue;
typedef struct
{
    struct work_struct my_work;
    int x;
} my_work_t;
my_work_t *work, *work2;
//static void my_workqueue_function(unsigned long data);
//DECLARE_WORK(my_workqueue, my_workqueue_function);


static void my_workqueue_function(struct work_struct *work)
{
    irq_q ++;
    my_work_t *my_work = (my_work_t*)work;
    printk(KERN_INFO "***my_work.x %d, counter: %d***\n", my_work->x, irq_q);
    kfree((void*)work);
}


irqreturn_t my_irq_handler (int irq, void *dev)
{
    if (irq == IRQ)
    {
        if (my_workqueue)
        {
            work=(my_work_t *)kmalloc(sizeof(my_work_t),GFP_KERNEL);
            if (work)
            {
                INIT_WORK( (struct work_struct *)work, my_workqueue_function);
                work->x=1;
                queue_work(my_workqueue, (struct work_struct *)work);
            }
            work2=(my_work_t *)kmalloc(sizeof(my_work_t),GFP_KERNEL);
            if (work2)
            {
                INIT_WORK( (struct work_struct *)work2, my_workqueue_function);
                work2->x=2;
                queue_work(my_workqueue, (struct work_struct *)work2);
            }
        }
        return IRQ_HANDLED;						// прерывание обработано
    }
    return IRQ_NONE;
}


static int __init my_workqueue_init(void)
{
    int res = request_irq(IRQ, my_irq_handler, IRQF_SHARED, "***my_workqueue***" , &my_dev_id);	// номер прерывания, ук-ль на обработчик, флаги, текст, разделение линий
    if (res)
    {
        printk(KERN_ERR "***Unable to register my_irq_handler***\n");
        return res;
    }
    my_workqueue = create_workqueue("my_workqueue");
    if (!my_workqueue)
    {
        free_irq(IRQ,  &my_dev_id);
        printk(KERN_ERR "***Unable to create workqueue***\n");
        return -ENOMEM;
    }
    printk(KERN_INFO "***Module loaded***\n");
    return 0;
}


static void __exit my_workqueue_exit(void)
{
    flush_workqueue(my_workqueue);		// никакая из задач очереди не работает
    destroy_workqueue(my_workqueue);	// избавляемяся от очереди задач
    free_irq(IRQ,  &my_dev_id);		// удаляет обработчик и отключает линию прерывания
    printk(KERN_INFO "***Module unloaded***\n");
}


module_init(my_workqueue_init);
module_exit(my_workqueue_exit);
