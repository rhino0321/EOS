#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/uaccess.h> //copy_to/from_user()
#include <linux/gpio.h>
#include <linux/delay.h>
//GPIO
//LED is connected to this GPIO
#define GPIO_21 (21)
#define GPIO_16 (16)
#define GPIO_12 (12)
#define GPIO_25 (25)
dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);
/*************** Driver functions **********************/
static int
etx_open(struct inode *inode, struct file *file);
static int
etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp,
char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp,
const char *buf, size_t len, loff_t * off);
static void led(char);
/******************************************************/
//File operation structure
static struct file_operations fops =
{
.owner
= THIS_MODULE,
.read
= etx_read,
.write
= etx_write,
.open
= etx_open,
.release
= etx_release,
};

/*
** This function will be called when we open the Device file
*/
static int etx_open(struct inode *inode, struct file *file)
{
pr_info("Device File Opened...!!!\n");
return 0;
}
/*
** This function will be called when we close the Device file
*/
static int etx_release(struct inode *inode, struct file *file)
{
pr_info("Device File Closed...!!!\n");
return 0;
}
/*
** This function will be called when we read the Device file
*/
static ssize_t etx_read(struct file *filp,
char __user *buf, size_t len, loff_t *off)
{
uint8_t gpio_state_21= 0;
uint8_t gpio_state_16 = 0;
uint8_t gpio_state_12= 0;
uint8_t gpio_state_25 = 0;
//reading GPIO value
gpio_state_21 = gpio_get_value(GPIO_21);
gpio_state_16 = gpio_get_value(GPIO_16);
gpio_state_12 = gpio_get_value(GPIO_12);
gpio_state_25 = gpio_get_value(GPIO_25);
//write to user
len = 1;
if( copy_to_user(buf, &gpio_state_21, len) >0 || copy_to_user(buf, &gpio_state_16, len) > 0 || copy_to_user(buf, &gpio_state_12, len) > 0 || copy_to_user(buf, &gpio_state_25, len) > 0)  {
pr_err("ERROR: Not all the bytes have been copied to user\n");
}
pr_info("Read function : GPIO_21 = %d \n", gpio_state_21);
pr_info("Read function : GPIO_16 = %d \n", gpio_state_16);
return 0;
}
/*
** This function will be called when we write the Device file
*/
static ssize_t etx_write(struct file *filp,
const char __user *buf, size_t len, loff_t *off)
{
uint8_t rec_buf[10] = {0};
if( copy_from_user( rec_buf, buf, len ) > 0) {
pr_err("ERROR: Not all the bytes have been copied from user\n");
}

led(rec_buf[0]);
mdelay(1000);
led(rec_buf[1]);
mdelay(1000);
led(rec_buf[2]);
mdelay(1000);
led(rec_buf[3]);
mdelay(1000);
led(rec_buf[4]);
mdelay(1000);
led(rec_buf[5]);
mdelay(1000);
led(rec_buf[6]);
mdelay(1000);
led(rec_buf[7]);
mdelay(1000);
led(rec_buf[8]);
mdelay(1000);
led('0');
/*pr_info("Write Function : GPIO_21 Set = %c\n", rec_buf[0]);
pr_info("Write Function : GPIO_16 Set = %c\n", rec_buf[1]);
if (rec_buf[0] == '1') {
//set the GPIO value to HIGH
gpio_set_value(GPIO_21, 0);
gpio_set_value(GPIO_16, 1);
} else if (rec_buf[0] == '2') {
//set the GPIO value to LOW
gpio_set_value(GPIO_21, 1);
gpio_set_value(GPIO_16, 0);
} else {
    pr_err("Unknown command : Please provide either 1 or 0 \n");
}*/

return len;
}
/*
** Module Init function
*/
static void led(char c)
{
    if (c == '0')
    {
        gpio_set_value(GPIO_21,0);
        gpio_set_value(GPIO_16,0);
        gpio_set_value(GPIO_12,0);
        gpio_set_value(GPIO_25,0);
    }
    else if (c == '1')
    {
        gpio_set_value(GPIO_21,0);
        gpio_set_value(GPIO_16,0);
        gpio_set_value(GPIO_12,0);
        gpio_set_value(GPIO_25,1);
    }
    else if (c == '2')
    {
        gpio_set_value(GPIO_21,0);
        gpio_set_value(GPIO_16,0);
        gpio_set_value(GPIO_12,1);
        gpio_set_value(GPIO_25,0);
    }
    else if (c == '3')
    {
        gpio_set_value(GPIO_21,0);
        gpio_set_value(GPIO_16,0);
        gpio_set_value(GPIO_12,1);
        gpio_set_value(GPIO_25,1);
    }
    else if (c == '4')
    {
        gpio_set_value(GPIO_21,0);
        gpio_set_value(GPIO_16,1);
        gpio_set_value(GPIO_12,0);
        gpio_set_value(GPIO_25,0);
    }
    else if (c == '5')
    {
        gpio_set_value(GPIO_21,0);
        gpio_set_value(GPIO_16,1);
        gpio_set_value(GPIO_12,0);
        gpio_set_value(GPIO_25,1);
    }
    else if (c == '6')
    {
        gpio_set_value(GPIO_21,0);
        gpio_set_value(GPIO_16,1);
        gpio_set_value(GPIO_12,1);
        gpio_set_value(GPIO_25,0);
    }
    else if (c == '7')
    {
        gpio_set_value(GPIO_21,0);
        gpio_set_value(GPIO_16,1);
        gpio_set_value(GPIO_12,1);
        gpio_set_value(GPIO_25,1);
    }
    else if (c == '8')
    {
        gpio_set_value(GPIO_21,1);
        gpio_set_value(GPIO_16,0);
        gpio_set_value(GPIO_12,0);
        gpio_set_value(GPIO_25,0);
    }
    else if (c == '9')
    {
        gpio_set_value(GPIO_21,1);
        gpio_set_value(GPIO_16,0);
        gpio_set_value(GPIO_12,0);
        gpio_set_value(GPIO_25,1);
    }
}

static int __init etx_driver_init(void)
{
/*Allocating Major number*/
if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
pr_err("Cannot allocate major number\n");
goto r_unreg;
}
pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
/*Creating cdev structure*/
cdev_init(&etx_cdev,&fops);
/*Adding character device to the system*/
if((cdev_add(&etx_cdev,dev,1)) < 0){
pr_err("Cannot add the device to the system\n");
goto r_del;
}
/*Creating struct class*/
if((dev_class = class_create(THIS_MODULE,"etx_class")) == NULL){
pr_err("Cannot create the struct class\n");
goto r_class;
}
/*Creating device*/
if((device_create(dev_class,NULL,dev,NULL,"etx_device")) == NULL){
pr_err( "Cannot create the Device \n");
goto r_device;
}
//Checking the GPIO is valid or not
if(gpio_is_valid(GPIO_21) == false || gpio_is_valid(GPIO_16) == false || gpio_is_valid(GPIO_12) == false || gpio_is_valid(GPIO_25) == false){
pr_err("GPIO is not valid\n");
goto r_device;
}
//Requesting the GPIO
if(gpio_request(GPIO_21,"GPIO_21") < 0 || gpio_request(GPIO_16,"GPIO_16") < 0 || gpio_request(GPIO_12,"GPIO_12") < 0 || gpio_request(GPIO_25,"GPIO_25") < 0) {
pr_err("ERROR: GPIO request\n");
goto r_gpio;
}

//configure the GPIO as output
gpio_direction_output(GPIO_21, 0);
gpio_direction_output(GPIO_16, 0);
gpio_direction_output(GPIO_12, 0);
gpio_direction_output(GPIO_25, 0);
/* Using this call the GPIO 21 will be visible in /sys/class/gpio/
** Now you can change the gpio values by using below commands also.
** echo 1 > /sys/class/gpio/gpio21/value (turn ON the LED)
** echo 0 > /sys/class/gpio/gpio21/value (turn OFF the LED)
** cat /sys/class/gpio/gpio21/value (read the value LED)
**
** the second argument prevents the direction from being changed.
*/
gpio_export(GPIO_21, false);
gpio_export(GPIO_16, false);
gpio_export(GPIO_12, false);
gpio_export(GPIO_25, false);
pr_info("Device Driver Insert...Done!!!\n");
return 0;
r_gpio:
gpio_free(GPIO_21);
gpio_free(GPIO_16);
gpio_free(GPIO_12);
gpio_free(GPIO_25);
r_device:
device_destroy(dev_class,dev);
r_class:
class_destroy(dev_class);
r_del:
cdev_del(&etx_cdev);
r_unreg:
unregister_chrdev_region(dev,1);
return -1;
}
/*
** Module exit function
*/
static void __exit etx_driver_exit(void)
{
gpio_unexport(GPIO_21);
gpio_free(GPIO_21);
gpio_unexport(GPIO_16);
gpio_free(GPIO_16);
gpio_unexport(GPIO_12);
gpio_free(GPIO_12);
gpio_unexport(GPIO_25);
gpio_free(GPIO_25);
device_destroy(dev_class,dev);
class_destroy(dev_class);
cdev_del(&etx_cdev);
unregister_chrdev_region(dev, 1);
pr_info("Device Driver Remove...Done!!\n");
}
module_init(etx_driver_init);
module_exit(etx_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <embetronicx@gmail.com>");
MODULE_DESCRIPTION("A simple device driver - GPIO Driver");
MODULE_VERSION("1.32");
