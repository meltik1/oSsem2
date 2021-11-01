
# include <linux/module.h>
# include <linux/kernel.h>
# include <linux/init.h>
# include <linux/fs.h>
# include <linux/time.h>
#include <linux/slab.h>


#define MYFS_MAGIC_NUMBER 0x13131313
#define SLAB_NAME "NIZU_cache"


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Zuev");



static struct kmem_cache *my_cachep;


struct myfs_inode
{
    int i_mode;
    unsigned long i_ino;
} myfs_inode;


static struct myfs_inode **line = NULL;
static int number = 100;
module_param(number, int, 0);
static int sco = 0;



static struct inode * myfs_make_inode(struct super_block * sb, int mode)
{
    struct inode * ret = new_inode(sb);
    if (ret)
    {
        inode_init_owner(ret, NULL, mode);
        ret->i_size = PAGE_SIZE;
        ret->i_atime = ret->i_mtime = ret->i_ctime = current_time(ret);
        ret->i_private = &myfs_inode;
    }
    return ret;
}



static void myfs_put_super(struct super_block * sb)
{
    printk(KERN_DEBUG "NIZU super block destroyed!\n" ) ;
}


static struct super_operations const myfs_super_ops = {
        .put_super = myfs_put_super,	// деструктор суперблока
        .statfs = simple_statfs,	// заглушка из libfs
        .drop_inode = generic_delete_inode,	// заглушка из libfs
};


static int myfs_fill_sb(struct super_block* sb, void* data, int silent)
{
    struct inode* root = NULL;

    sb->s_blocksize = PAGE_SIZE;
    sb->s_blocksize_bits = PAGE_SHIFT;
    sb->s_magic = MYFS_MAGIC_NUMBER; // магическое число по которому драйвер фс может проверить, что на диске именно та самая фс, а не что-то еще
    sb->s_op = &myfs_super_ops;					// операции для суперблока

    root = myfs_make_inode(sb, S_IFDIR | 0755);	// создание inode, нуждается в ук-ле на сб. S_IFDIR - мы создаем каталог
    if (!root)
    {
        printk (KERN_ERR "NIZU inode allocation failed !\n") ;
        return -ENOMEM;
    }
    root->i_op = &simple_dir_inode_operations;	// inode-операции (предоставляются ядром)
    root->i_fop = &simple_dir_operations;		// файловые операции (предоставляются ядром)

    sb->s_root = d_make_root(root) ;			// создание дентри-структура для корневого каталога (устанавливаем ук-ль)
    if (!sb->s_root)
    {
        printk(KERN_ERR " MYFS root creation failed !\n") ;
        iput(root);		// Помещает индекс, уменьшая кол-во его исп-ия. Если счетчик исп-ия индекса = 0, индекс освобождается и может быть уничтожен
        return -ENOMEM;
    }
    return 0;
}


// ф-ия должна примонтировать у-во и вернуть стр-ру, описывающ корневой каталог фс
static struct dentry* myfs_mount (struct file_system_type *type, int flags, char const *dev, void *data)
{
    struct dentry* const entry = mount_bdev(type, flags, dev, data, myfs_fill_sb) ;	// myfs_fill_sb - ук-ль на ф-ию, которая вызовется,
    // чтоб проинициализировать суперблок
    if (IS_ERR(entry))
        printk(KERN_ERR "NIZU mounting failed !\n") ;
    else
        printk(KERN_DEBUG "NIZU mounted!\n") ;
    return entry;				// entry - корневой каталог фс (создаст его myfs_fill_sb)
}


static struct file_system_type myfs_type = {
        .owner = THIS_MODULE,
        .name = "nizu_fs",									// имя файловой системы
        .mount = myfs_mount,						// ук-ль на ф-ию монтирования фс
        .kill_sb = kill_block_super,				// ук-ль на ф-ию размонтирования фс
};


void co (void*p)
{
    *(int *)p = (int)p;		// ???
    sco ++;
}


static int __init myfs_init(void)
{
    line = kmalloc(sizeof(void*)*number,GFP_KERNEL);		
    if (!line)
    {
        printk(KERN_ERR "kmalloc error\n");
        return -ENOMEM;
    }
    int i = 0;
    for (i = 0; i < number; i ++)
        line[i] = NULL;

    my_cachep = kmem_cache_create(SLAB_NAME, sizeof(struct myfs_inode), 0,0, co);		// 1 - строка имени кэша
    // 2 - размер эл-ов кэша
    // 3 - смещ 1го эл от нач кэша
    // 4 - опицональные пар-ры
    // 5 - констр/дестр при размещ каждого эл-та
    if (!my_cachep)
    {
        printk(KERN_ERR "NIZU_MODULE cannot create slab-cache!\n");
        kmem_cache_destroy(my_cachep);
        kfree(line);
        return -ENOMEM;
    }
    for (i = 0; i < number; i ++)
    {
        if (NULL == (line[i] = kmem_cache_alloc(my_cachep, GFP_KERNEL)))		// связываем ячейку массива с слабкэшем
        {
            printk(KERN_ERR "NIZU kmem_cache_alloc error\n");
            int j = 0;
            for (j = 0; j < number; j ++)
                kmem_cache_free(my_cachep, line[i]);
        }
    }

    printk(KERN_INFO "NIZU allocate %d objects into slab: %s\n", number, SLAB_NAME);
    printk(KERN_INFO "NIZU object size %d bytes, full size %ld bytes\n", sizeof(struct myfs_inode), (long)sizeof(struct myfs_inode)*number);
    printk(KERN_INFO "NIZU constructor called %d times\n", sco);

    int ret = register_filesystem(& myfs_type);
    if(ret != 0)
    {
        printk(KERN_ERR "NIZU_MODULE cannot register filesystem!\n");
        return ret;
    }
    printk(KERN_DEBUG "NIZU_MODULE loaded !\n");
    return 0;
}


static void __exit myfs_exit(void)
{
    int i = 0;
    for ( i = 0; i < number; i ++) {
        kmem_cache_free(my_cachep, line[i]);
    }
    kmem_cache_destroy(my_cachep);
    kfree(line);
    int ret = unregister_filesystem(&myfs_type);
    if (ret != 0)
        printk(KERN_ERR "NIZU_MODULE cannot unregister filesystem !\n");
    printk(KERN_DEBUG "NIZU_MODULE unloaded !\n");
}


module_init(myfs_init);
module_exit(myfs_exit);
