/*
 * USB Tempprobe kernel driver
 * Developed for Prof. Bart Massey's CS572 at Portland State University
 * 	Fall 2010
 *
 * Copyright (c) 2010 by Freddy Carl, Wil Cooley and Akbar Saidov.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * This driver is largely based on the USB Skeleton driver - 2.0
 * by Greg Kroah-Hartman (greg@kroah.com) and Sean Bruno's lighty.c.
 *
 */

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <linux/smp_lock.h>
#include <linux/usb.h>
#include <asm/uaccess.h>
#include "usb_tempprobe.h"

/*
 * Function prototypes
 */
static int __init usb_tempprobe_init(void);
static int tempprobe_probe(struct usb_interface *interface, const struct usb_device_id *id);
static int tempprobe_open(struct inode *inode, struct file *file);
static ssize_t tempprobe_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos);
int tempprobe_ioctl(struct inode * i_node, struct file * file, unsigned int cmd, unsigned long arg);
static void tempprobe_disconnect(struct usb_interface *interface);
static void tempprobe_delete(struct kref *kref);
static int tempprobe_release(struct inode *inode, struct file *file);
static void __exit usb_tempprobe_exit(void);


/* Define these values to match your devices */
/* This needs to change depending on the values of temmprobe */
#define USB_TEMPROBE_VENDOR_ID	0x16C0
#define USB_TEMPROBE_PRODUCT_ID	0x0FFF


/* table of devices that work with this driver */
static struct usb_device_id tempprobe_table [] = {
	{ USB_DEVICE(USB_TEMPROBE_VENDOR_ID, USB_TEMPROBE_PRODUCT_ID) },
	{ }					/* Terminating entry */
};
MODULE_DEVICE_TABLE (usb, tempprobe_table);

/* Get a minor range for your devices from the usb maintainer */
#define USB_TEMPPROBE_MINOR_BASE	100

/* Structure to hold all of our device specific stuff */
struct usb_tempprobe {
	struct usb_device *	udev;			/* the usb device for this device */
	struct usb_interface *	interface;		/* the interface for this device */
	unsigned char *		bulk_in_buffer;		/* the buffer to receive data */
	size_t			bulk_in_size;		/* the size of the receive buffer */
	__u8			bulk_in_endpointAddr;	/* the address of the bulk in endpoint */
	__u8			bulk_out_endpointAddr;	/* the address of the bulk out endpoint */
	struct kref		kref;
};
#define to_tempprobe_dev(d) container_of(d, struct usb_tempprobe, kref)

static struct usb_driver tempprobe_driver = {
	.name = "tempprobe",
	.id_table = tempprobe_table,
	.probe = tempprobe_probe,
	.disconnect = tempprobe_disconnect,
};

static struct file_operations tempprobe_fops = {
	.owner =	THIS_MODULE,
	.read =		tempprobe_read,
	.open =		tempprobe_open,
	.ioctl =	tempprobe_ioctl,
	.release =	tempprobe_release,
};

/* 
 * usb class driver info in order to get a minor number from the usb core,
 * and to have the device registered with devfs and the driver core
 */
static struct usb_class_driver tempprobe_class = {
	.name = "usb/tempprobe%d",
	.fops = &tempprobe_fops,
	.minor_base = USB_TEMPPROBE_MINOR_BASE,
};

static struct usb_driver tempprobe_driver;

//INIT

static int __init usb_tempprobe_init(void)
{
	int result;

	/* register this driver with the USB subsystem */
	result = usb_register(&tempprobe_driver);
	if (result)
		err("usb_register failed. Error number %d", result);

	return result;
}

//PROBE

static int tempprobe_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct usb_tempprobe *dev = NULL;
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	size_t buffer_size;
	int i;
	int retval = -ENOMEM;

	/* allocate memory for our device state and initialize it */
	dev = kmalloc(sizeof(struct usb_tempprobe), GFP_KERNEL);
	if (dev == NULL) {
		err("Out of memory");
		goto error;
	}
	memset(dev, 0x00, sizeof (*dev));
	kref_init(&dev->kref);

	dev->udev = usb_get_dev(interface_to_usbdev(interface));
	dev->interface = interface;

	/* set up the endpoint information */
	/* use only the first bulk-in and bulk-out endpoints */
	
	printk(KERN_NOTICE "Looking for interface endpoints\n");
	
	
	iface_desc = interface->cur_altsetting;
	printk(KERN_NOTICE "There are %i endpoints\n", iface_desc->desc.bNumEndpoints);
	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
		endpoint = &iface_desc->endpoint[i].desc;
		
		if (!dev->bulk_in_endpointAddr &&
		    (endpoint->bEndpointAddress & USB_DIR_IN) &&
		    ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
					== USB_ENDPOINT_XFER_INT)) {
					printk(KERN_NOTICE"Found bulk_in endpoint\n");
			/* we found a bulk in endpoint */
			buffer_size = endpoint->wMaxPacketSize;
			dev->bulk_in_size = buffer_size;
			dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
			dev->bulk_in_buffer = kmalloc(buffer_size, GFP_KERNEL);
			if (!dev->bulk_in_buffer) {
				err("Could not allocate bulk_in_buffer");
				printk(KERN_NOTICE "Could not allocate bulk_in_buffer\n");
				goto error;
			}
		}

		if (!dev->bulk_out_endpointAddr &&
		    !(endpoint->bEndpointAddress & USB_DIR_IN) &&
		    ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
					== USB_ENDPOINT_XFER_INT)) {
					printk(KERN_NOTICE"Found bulk_out endpoint\n");
			/* we found a bulk out endpoint */
			dev->bulk_out_endpointAddr = endpoint->bEndpointAddress;
		}
	}
	if (!(dev->bulk_in_endpointAddr)) {// && dev->bulk_out_endpointAddr)) {
		err("Could not find both bulk-in and bulk-out endpoints");
		printk(KERN_NOTICE "Could not find bulk-in\n");

		goto error;
	}

	/* save our data pointer in this interface device */
	usb_set_intfdata(interface, dev);

	/* we can register the device now, as it is ready */
	retval = usb_register_dev(interface, &tempprobe_class);
	if (retval) {
		/* something prevented us from registering this driver */
		err("Not able to get a minor for this device.");
		usb_set_intfdata(interface, NULL);
		goto error;
	}

	/* let the user know what node this device is now attached to */
	printk(KERN_NOTICE "USB Tempprobe device now attached to USBtempprobe-%d", interface->minor);
	return 0;

error:
	if (dev)
		kref_put(&dev->kref, tempprobe_delete);
	return retval;
}

//OPEN

static int tempprobe_open(struct inode *inode, struct file *file)
{
	struct usb_tempprobe *dev;
	struct usb_interface *interface;
	int subminor;
	int retval = 0;

	subminor = iminor(inode);

	interface = usb_find_interface(&tempprobe_driver, subminor);
	if (!interface) {
		err ("%s - error, can't find device for minor %d",
		     __FUNCTION__, subminor);
		retval = -ENODEV;
		goto exit;
	}

	dev = usb_get_intfdata(interface);
	if (!dev) {
		retval = -ENODEV;
		goto exit;
	}
	
	/* increment our usage count for the device */
	kref_get(&dev->kref);

	/* save our object in the file's private structure */
	file->private_data = dev;

exit:
	return retval;
}

//READ

static ssize_t tempprobe_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos)
{
	struct usb_tempprobe *dev;
	int retval = 0;
  int actual_size;

	dev = (struct usb_tempprobe *)file->private_data;
	
	/* This would need to be modified according to what data we will be reading in from tempprobe */
	
	/* do a blocking bulk read to get data from the device */
	/// removed usb_bulk_msg, usb_rcvbulkpipe with:
	retval = usb_interrupt_msg(dev->udev,
			      usb_rcvintpipe(dev->udev, dev->bulk_in_endpointAddr),
			      dev->bulk_in_buffer,
			      min(dev->bulk_in_size, count),
			      &actual_size, 5000);  //HZ*10

	/* if the read was successful, copy the data to userspace */
	if (!retval) {
    printk(KERN_NOTICE "Read was successful\n");
		if (copy_to_user(buffer, dev->bulk_in_buffer, actual_size))
			retval = -EFAULT;
		else {
			retval = count;
			printk(KERN_NOTICE "copy_to_user was successful\n");
		}
	}

	return retval;
}


//IOCTL

static void temmprobe_write_intr_callback(struct urb *urb, struct pt_regs *regs)
{
	/* sync/async unlink faults aren't errors */
	if (urb->status &&
			!(urb->status == -ENOENT ||
				urb->status == -ECONNRESET ||
				urb->status == -ESHUTDOWN)) {
		dbg("%s - nonzero write bulk status received: %d",
		__FUNCTION__, urb->status);
	}
	
	printk(KERN_NOTICE "urb status %d=0 is successful\n", urb->status);
	/* free up our allocated buffer */
	usb_buffer_free(urb->dev, urb->transfer_buffer_length,
	urb->transfer_buffer, urb->transfer_dma);
}

/*
* IOCTL
*/
int tempprobe_ioctl(struct inode * i_node, struct file * file, unsigned int cmd, unsigned long arg)
{
	int err = 0;
	int retval = 0;
	struct urb *usb_led;
	char command; // a, b
	char *buf;
	struct usb_tempprobe *dev = file->private_data;
	/*
				* extract the type and number bitfields, and don't decode
				* wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
				*/
	if (_IOC_TYPE(cmd) != TEMPPROBE_MAGIC)
	{
		printk(KERN_NOTICE
		"lighty_ioctl: !TEMPPROBE_MAGIC\n");
		return -ENOTTY;
	}
	if (_IOC_NR(cmd) > TEMPPROBE_IOCTL_MAX)
	{
		printk(KERN_NOTICE
		"lighty_ioctl:  > TEMPPROBE_IOC_MAXNR\n");
		return -ENOTTY;
	}

	/*
				* If not root/sysadmin, go away 
				*/
	if (! capable (CAP_SYS_ADMIN))
		return -EPERM;

	/*
				* the direction is a bitmask, and VERIFY_WRITE catches R/W
				* transfers. `Type' is user-oriented, while
				* access_ok is kernel-oriented, so the concept of "read" and
				* "write" is reversed
				*/
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	if (err)
	{
		printk(KERN_NOTICE
		"lighty_ioctl: access !ok\n");
		return -EFAULT;
	}

	switch(cmd) {
	case ENUM_CMD:
		printk(KERN_NOTICE
		"ENUM\n");
		command = 'a';
		break;
	case RDALL_CMD:
		printk(KERN_NOTICE
		"RDALL\n");
		command = 'b';
		break;
	default:
		printk(KERN_NOTICE
		"Not a known command %x\n", cmd);
		return -ENOMEM;
	}
	usb_led = usb_alloc_urb(0, GFP_KERNEL);
	if (!usb_led) {
		return -ENOMEM;
	}
	buf = usb_buffer_alloc(dev->udev, 64, GFP_KERNEL, &usb_led->transfer_dma);
	if (!buf) {
		printk (KERN_NOTICE "usb_buffer_alloc failed\n");
		usb_buffer_free(dev->udev, usb_led->transfer_buffer_length,
		usb_led->transfer_buffer, usb_led->transfer_dma);
		return -ENOMEM;
	}
	buf[0] = command; // a, b
	usb_fill_int_urb(usb_led, dev->udev, usb_sndintpipe(dev->udev, dev->bulk_out_endpointAddr), buf,
	64, (usb_complete_t)temmprobe_write_intr_callback, dev, 250);
	usb_led->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
	if( (retval = usb_submit_urb(usb_led, GFP_KERNEL)) ) {
		err("%s - failed submitting write urb, error %d", __FUNCTION__, retval);
	}
	return 0;
}

//DISCONNECT

static void tempprobe_disconnect(struct usb_interface *interface)
{
	struct usb_tempprobe *dev;
	int minor = interface->minor;

	/* prevent tempprobe_open() from racing tempprobe_disconnect() */
	lock_kernel();

	dev = usb_get_intfdata(interface);
	usb_set_intfdata(interface, NULL);

	/* give back our minor */
	usb_deregister_dev(interface, &tempprobe_class);

	unlock_kernel();

	/* decrement our usage count */
	kref_put(&dev->kref, tempprobe_delete);

	info("USB Tempprobe #%d now disconnected", minor);
}

//DELETE

static void tempprobe_delete(struct kref *kref)
{	
	struct usb_tempprobe *dev = to_tempprobe_dev(kref);

	usb_put_dev(dev->udev);
	kfree (dev->bulk_in_buffer);
	kfree (dev);
}

//RELEASE

static int tempprobe_release(struct inode *inode, struct file *file)
{
	struct usb_tempprobe *dev;

	dev = (struct usb_tempprobe *)file->private_data;
	if (dev == NULL)
		return -ENODEV;

	/* decrement the count on our device */
	kref_put(&dev->kref, tempprobe_delete);
	return 0;
}

//EXIT

static void __exit usb_tempprobe_exit(void)
{
	/* deregister this driver with the USB subsystem */
	usb_deregister(&tempprobe_driver);
}

module_init (usb_tempprobe_init);
module_exit (usb_tempprobe_exit);

MODULE_LICENSE("GPL");
