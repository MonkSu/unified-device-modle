#include <asm/io.h>
#include <asm/kernel-pgtable.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/atomic.h>
#include <linux/semaphore.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/miscdevice.h>

struct devdrv {
    struct cdev devdrv_cdev;
    struct miscdevice miscdev;
    struct class *devdrv_class;
    struct device *devdrv_device;
};

static struct devdrv *md;
static int major;        /* major number of device */
static int minor;        /* minor number of device */


/* device open method */
static int devdrv_open(struct inode *inode, struct file *file)
{
    struct devdrv *md = container_of(inode->i_cdev, struct devdrv, devdrv_cdev);

    printk(" %s %d Monk\n",__func__,__LINE__);
    return 0;
}

/* device close method */
static int devdrv_release(struct inode *inode, struct file *file)
{
    struct devdrv* md = container_of(inode->i_cdev, struct devdrv, devdrv_cdev);

    printk(" %s %d Monk\n",__func__,__LINE__);
    return 0;
}


static struct file_operations devdrv_fops = {
    .owner   = THIS_MODULE,
    .open    = devdrv_open,
    .release = devdrv_release,
};


static ssize_t devdrv_show_devicename(struct device *dev,
                     struct device_attribute *attr,
                     char *buf)
{
    printk(" %s %d Monk attribute show\n",__func__,__LINE__);
}

static DEVICE_ATTR(devicename, S_IRUGO, devdrv_show_devicename, NULL);

static struct attribute *device_attrs[] = {
    &dev_attr_devicename.attr,
    NULL,
};

static struct attribute_group device_group = {
    .name  = "device_attrs",
    .attrs = device_attrs,
};

static struct attribute_group* device_attrs_groups[] = {
    &device_group,
    NULL
};

static ssize_t devdrv_bin_write(struct file *filp, struct kobject *kobj,
              struct bin_attribute *bin_attr, char *buf, loff_t off,
              size_t count)
{
    printk(" %s %d Monk bin attribute write\n",__func__,__LINE__);
    return 1;
}

static ssize_t devdrv_bin_read(struct file *filp, struct kobject *kobj,
              struct bin_attribute *bin_attr, char *buf, loff_t off,
              size_t count)
{
    printk(" %s %d Monk bin attribute read\n",__func__,__LINE__);

    return 1;
}
static BIN_ATTR(state, S_IWUGO|S_IRUGO, devdrv_bin_read, devdrv_bin_write, 1);

static struct bin_attribute *device_bin_attrs[] = {
    &bin_attr_state,
    NULL,
};

static struct attribute_group device_bin_group = {
    .name      = "device_bin_attrs",
    .bin_attrs = device_bin_attrs,
};

static struct attribute_group *device_bin_groups[] = {
    &device_bin_group,
    NULL,
};

static ssize_t class_show_classname(struct device *dev,
                     struct device_attribute *attr,
                     char *buf)
{
    printk(" %s %d Monk attribute show\n",__func__,__LINE__);
}

static CLASS_ATTR(classname, S_IRUGO, class_show_classname, NULL);

static struct attribute *class_attrs[] = {
    &class_attr_classname.attr,
    NULL,
};

static struct attribute_group class_group = {
    .name  = "class_attrs",
    .attrs = class_attrs,
};

static struct attribute_group* class_attrs_groups[] = {
    &class_group,
    NULL,
};


static int devdrv_misc_open(struct inode *inode,struct file *file)
{
    printk(" %s %d Monk\n",__func__,__LINE__);
    return 0;
}

static int devdrv_misc_release(struct inode *inode, struct file *file)
{
    printk(" %s %d Monk\n",__func__,__LINE__);
    return 0;
}

static ssize_t devdrv_misc_read(struct file *filp, char __user *arg, size_t size, loff_t *offp)
{
    printk(" %s %d Monk\n",__func__,__LINE__);
    return 0;
}

static long devdrv_misc_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct miscdevice *miscdev = filp->private_data;
    struct devdrv *md = container_of(miscdev, struct devdrv, miscdev);
    char __user *user_space = (char*)arg;

    printk(" %s %d Monk\n",__func__,__LINE__);
    switch(cmd){
        default:
        break;
    }
    return 0;
}

static struct file_operations devdrv_misc_fops = {
   .owner          = THIS_MODULE,
   .open           = devdrv_misc_open,
   .release        = devdrv_misc_release,
   .read           = devdrv_misc_read,
   .unlocked_ioctl = devdrv_misc_ioctl,
};

static int devdrv_create_dev0(struct devdrv* md)
{
    int err;
    dev_t dev = 0;

    err = alloc_chrdev_region(&dev, 0, 1, "cdev");
    if (err < 0) {
        printk(" %s %d Failed to get major\n",__func__,__LINE__);
        return -ENOMEM;
    }
    major = MAJOR(dev);
    minor = MINOR(dev);
    printk(" %s %d Monk Get device %d:%d\n",__func__,__LINE__, major, minor);
    cdev_init(&md->devdrv_cdev, &devdrv_fops);
    err = cdev_add(&md->devdrv_cdev, dev, 1);
    if (err) {
        printk(" %s %d Failed to cdev add\n",__func__,__LINE__);
        return -ENOMEM;
    }
    return 0;
}

static int devdrv_create_dev1(struct devdrv* md)
{
    minor = 0;
    major = __register_chrdev(0, minor, 1, "cdev", &devdrv_fops);
    if (major < 0) {
        printk(" %s %d Failed to register cdev %d\n",__func__,__LINE__,major);
        return -ENOMEM;
    }
    printk(" %s %d Monk Get device %d:%d\n",__func__,__LINE__, major, minor);
    return 0;
}

static int devdrv_destroy_dev0(struct devdrv* md)
{
    cdev_del(&md->devdrv_cdev);
    return 0;
}

static int devdrv_destroy_dev1(struct devdrv* md)
{
    __unregister_chrdev(major, minor, 1, "cdev");
    return 0;
}

#define METHOD                      1
#define DESTROY_DEV_METHOD(method)  devdrv_destroy_dev##method(md)
#define CREATE_DEV_METHOD(method)   devdrv_create_dev##method(md)

#define CREATE_DEV()  do {       \
    if (METHOD) {                \
        CREATE_DEV_METHOD(1);    \
    } else {                     \
        CREATE_DEV_METHOD(0);    \
    }                            \
} while(0)                       \

#define DESTROY_DEV()  do {      \
    if (METHOD) {                \
        DESTROY_DEV_METHOD(1);   \
    } else {                     \
        DESTROY_DEV_METHOD(0);   \
    }                            \
} while(0)                       \

static struct device *add_dev = NULL;

static int __init devdrv_init(void)
{
    int err;
    dev_t devt = 0;

    md = kmalloc(sizeof(struct devdrv), GFP_KERNEL);
    if (!md) {
        return -ENOMEM;
    }

    CREATE_DEV();

#if 0
    devt = MKDEV(major, minor);
    add_dev = kzalloc(sizeof(*add_dev), GFP_KERNEL);
    if (!add_dev) {
        printk(" %s %d Failed to kzalloc dev. \n",__func__,__LINE__);
        return -ENOMEM;
    }
    device_initialize(add_dev);
    dev_set_name(add_dev, "%s", "add_dev");
    add_dev->devt = devt;

    err = device_add(add_dev);
    if (err) {
        printk(" %s %d Failed to add dev %d. \n",__func__,__LINE__,err);
        return -ENOMEM;
    }
#endif

    md->devdrv_class  = class_create(THIS_MODULE, "devdrv-class");
    class_create_file(md->devdrv_class, &class_attr_classname);

#if 1
    md->devdrv_device = device_create_with_groups(md->devdrv_class, NULL, devt, NULL,
                                                device_attrs_groups,"devdrv-class-cdev");
                                                //device_bin_groups,"devdrv-class-cdev");
#else
    md->devdrv_device = device_create(md->devdrv_class, NULL, devt, NULL,"devdrv-class-cdev");
    device_create_file(md->devdrv_device, &dev_attr_devicename);
#endif


    md->miscdev.minor = MISC_DYNAMIC_MINOR;
    md->miscdev.name  = "devdrv-misc";
    md->miscdev.fops  = &devdrv_misc_fops;

    err = misc_register(&md->miscdev);
    if(err) {
        goto err_misc;
    }

    return 0;

err_misc:
    DESTROY_DEV();
    kfree(md);
    return -ENOMEM;
}

static void __exit devdrv_exit(void)
{
    dev_t devno = MKDEV(major, 0);
    DESTROY_DEV();
    misc_deregister(&md->miscdev);
    device_destroy(md->devdrv_class, MKDEV(major, minor));
    class_destroy(md->devdrv_class);
    kfree(md);
}

module_init(devdrv_init);
module_exit(devdrv_exit);

MODULE_LICENSE("GPL");

