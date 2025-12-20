/*
 * kbleds.c − Blink keyboard leds until the module is unloaded.(modified for > 4.15)
 */
#include <linux/module.h>
#include <linux/configfs.h>
#include <linux/init.h>
#include <linux/tty.h>          /* For fg_console, MAX_NR_CONSOLES */
#include <linux/kd.h>           /* For KDSETLED */
#include <linux/vt.h>
#include <linux/console_struct.h>       /* For vc_cons */
#include <linux/vt_kern.h>
#include <linux/timer.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>

MODULE_DESCRIPTION("Example module illustrating the use of Keyboard LEDs.");
MODULE_LICENSE("GPL");

struct timer_list my_timer;
struct tty_driver *my_driver;
static int _kbledstatus = 0;
static int test = 7;

static struct kobject *example_kobject;

#define BLINK_DELAY   HZ/5
#define ALL_LEDS_ON   0x07
#define RESTORE_LEDS  0xFF

static ssize_t test_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
        return sprintf(buf, "%d\n", test);
}

static ssize_t test_store(struct kobject *kobj, struct kobj_attribute *attr,
                          const char *buf, size_t count)
{
        int ret;
        int new_value;
        
        ret = kstrtoint(buf, 0, &new_value);
        if (ret) {
                pr_err("kbleds: invalid value format\n");
                return ret;
        }
        
        if (new_value < 0 || new_value > 7) {
                pr_err("kbleds: value must be between 0 and 7\n");
                return -EINVAL;
        }
        
        test = new_value;
        pr_info("kbleds: test value changed to %d\n", test);
        
        return count;
}

static struct kobj_attribute test_attribute = __ATTR(test, 0660, test_show, test_store);

static void my_timer_func(struct timer_list *ptr)
{
        int *pstatus = &_kbledstatus;
        
        if (*pstatus == test)
                *pstatus = RESTORE_LEDS;
        else
                *pstatus = test;
        
        if (my_driver && my_driver->ops && my_driver->ops->ioctl) {
                (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED, *pstatus);
        } else {
                pr_err("kbleds: invalid driver or ops\n");
        }
        
        my_timer.expires = jiffies + BLINK_DELAY;
        add_timer(&my_timer);
}

static int __init kbleds_init(void)
{
        int i;
        int error = 0;
        
        printk(KERN_INFO "kbleds: loading\n");
        
        pr_debug("kbleds: initializing sysfs interface\n");
        
        example_kobject = kobject_create_and_add("systest", kernel_kobj);
        if (!example_kobject) {
                pr_err("kbleds: failed to create kobject\n");
                return -ENOMEM;
        }
        
        error = sysfs_create_file(example_kobject, &test_attribute.attr);
        if (error) {
                pr_err("kbleds: failed to create sysfs file\n");
                kobject_put(example_kobject);
                return error;
        }
        
        printk(KERN_INFO "kbleds: fgconsole is %x\n", fg_console);
        for (i = 0; i < MAX_NR_CONSOLES; i++) {
                if (!vc_cons[i].d)
                        break;
                printk(KERN_INFO "kbleds: console[%i/%i] #%i, tty %lx\n", i,
                       MAX_NR_CONSOLES, vc_cons[i].d->vc_num,
                       (unsigned long)vc_cons[i].d->port.tty);
        }
        printk(KERN_INFO "kbleds: finished scanning consoles\n");
        
        my_driver = vc_cons[fg_console].d->port.tty->driver;
        
        if (!my_driver || !my_driver->ops || !my_driver->ops->ioctl) {
                pr_err("kbleds: invalid tty driver\n");
                sysfs_remove_file(example_kobject, &test_attribute.attr);
                kobject_put(example_kobject);
                return -ENODEV;
        }
        
        timer_setup(&my_timer, my_timer_func, 0);
        my_timer.expires = jiffies + BLINK_DELAY;
        add_timer(&my_timer);
        
        printk(KERN_INFO "kbleds: module loaded successfully\n");
        printk(KERN_INFO "kbleds: test value is %d\n", test);
        printk(KERN_INFO "kbleds: use 'echo <value> > /sys/kernel/systest/test' to change LED pattern\n");
        
        return 0;
}

static void __exit kbleds_cleanup(void)
{
        printk(KERN_INFO "kbleds: unloading...\n");
        
        del_timer(&my_timer);
        
        if (my_driver && my_driver->ops && my_driver->ops->ioctl) {
                (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED, RESTORE_LEDS);
        }
        
        sysfs_remove_file(example_kobject, &test_attribute.attr);
        kobject_put(example_kobject);
        
        printk(KERN_INFO "kbleds: module unloaded\n");
}

module_init(kbleds_init);
module_exit(kbleds_cleanup);
