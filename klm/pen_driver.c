//http://stackoverflow.com/questions/15308770/read-usb-bulk-message-on-linux
//Book easylinuxdevicesdrivers.pdf p.117
//github.com/johnwyz88/linux-device-drivers/blob/master/pen_driver.c
//http://opensourceforu.com/2011/12/data-transfers-to-from-usb-devices/
//rmmod usb_storage
//insmod pen_driver.ko
//insert USB only test with Bus 003 Device 023: ID 05dc:a838 Lexar Media, Inc
//sudo ./userapp 
//w:write r:read
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>  // .ko use linux/usb.h

#define MIN(a,b) (((a) <= (b)) ? (a) : (b))
#define BULK_EP_OUT 0x01
#define BULK_EP_IN 0x82
#define MAX_PKT_SIZE 512
//#define MAX_PKT_SIZE 1024

static struct usb_device *device;
static struct usb_class_driver class;
static unsigned char bulk_buf[MAX_PKT_SIZE];

static int pen_open(struct inode *i, struct file *f)
{
    return 0;
}
static int pen_close(struct inode *i, struct file *f)
{
    return 0;
}
static ssize_t pen_read(struct file *f, char __user *buf, size_t cnt, loff_t *off)
{
    int retval;
    //int read_cnt = 3; //default
    int read_cnt;
    printk(KERN_INFO "pen_read--------%s\n", bulk_buf);
    /* Read the data from the bulk endpoint */
    retval = usb_bulk_msg(device, usb_rcvbulkpipe(device, BULK_EP_IN),
        bulk_buf, MAX_PKT_SIZE, &read_cnt, 5000);

    if (retval)
    {
        printk(KERN_ERR "Bulk message returned %d\n", retval);
        return retval;
    }

    if (copy_to_user(buf, bulk_buf, MIN(cnt, read_cnt)))    
    {
        return -EFAULT;
    }
    printk("%s, %s, %d\n", buf, bulk_buf, MIN(cnt, read_cnt)); 
    return MIN(cnt, read_cnt);
    
}
static ssize_t pen_write(struct file *f, const char __user *buf, size_t cnt, loff_t *off)
{
    int retval;
    int wrote_cnt = MIN(cnt, MAX_PKT_SIZE);
    printk(KERN_INFO "pen_write-------\n");
    if (copy_from_user(bulk_buf, buf, MIN(cnt, MAX_PKT_SIZE)))
    {
        return -EFAULT;
    }

    /* Write the data into the bulk endpoint */
    retval = usb_bulk_msg(device, usb_sndbulkpipe(device, BULK_EP_OUT),
        bulk_buf, MIN(cnt, MAX_PKT_SIZE), &wrote_cnt, 5000);
    
    if (retval)
    {
        printk(KERN_ERR "Bulk message returned %d\n", retval);
        return retval;
    }

    printk("%s\n", bulk_buf); 
    return wrote_cnt;
}

static struct file_operations fops =
{
    .open = pen_open,
    .release = pen_close,
    .read = pen_read,
    .write = pen_write,
};

static int pen_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    int retval;

    device = interface_to_usbdev(interface);

    class.name = "usb/pen%d";
    class.fops = &fops;
    if ((retval = usb_register_dev(interface, &class)) < 0)
    {
        /* Something prevented us from registering this driver */
        err("Not able to get a minor for this device.");
    }
    else
    {
        printk(KERN_INFO "Minor obtained: %d\n", interface->minor);
    }

    return retval;
}

static void pen_disconnect(struct usb_interface *interface)
{
    usb_deregister_dev(interface, &class);
}

/* Table of devices that work with this driver */
static struct usb_device_id pen_table[] =
{
    { USB_DEVICE(0x05dc, 0xa838) },  //LEXAR 16GB USB
    // { USB_DEVICE(0x05dc, 0xa205) },  //LEXAR 16GB USB
    {} /* Terminating entry */
};

MODULE_DEVICE_TABLE (usb, pen_table);

static struct usb_driver pen_driver =
{
    .name = "pen_driver",
    .probe = pen_probe,
    .disconnect = pen_disconnect,
    .id_table = pen_table,
};

static int __init pen_init(void)
{
    int result;

    /* Register this driver with the USB subsystem */
    printk(KERN_INFO "registering Driver with Kernel");
    if ((result = usb_register(&pen_driver)))
    {
        err("usb_register failed. Error number %d", result);
    }
    return result;
}

static void __exit pen_exit(void)
{
    /* Deregister this driver with the USB subsystem */
    printk(KERN_INFO "unregistering Driver with Kernel");
    usb_deregister(&pen_driver);
}

module_init(pen_init);
module_exit(pen_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("thomasalvatran <thomasalvatran@gmail.com>");
MODULE_DESCRIPTION("USB Bulk/Network Pen Device Driver");


//Try read/write you woll get timeout or broken pipe error non-conforming SCSI comman

//File operation use for char devices and copy_from_user /copy_to_user