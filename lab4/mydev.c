#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
MODULE_LICENSE("GPL");
static int my_open(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *filp,
char __user *buf, size_t len,loff_t * off);
static ssize_t my_write(struct file *filp,
const char *buf, size_t len, loff_t * off);
static void led(char, int);
char ans[5][17];
int flag=0;
char seg_for_c[27][17] = {
    "1111001100010001", // A
    "0000011100000101", // b
    "1100111100000000", // C
    "0000011001000101", // d
    "1000011100000001", // E
    "1000001100000001", // F
    "1001111100010000", // G
    "0011001100010001", // H
    "1100110001000100", // I
    "1100010001000100", // J
    "0000000001101100", // K
    "0000111100000000", // L
    "0011001110100000", // M
    "0011001110001000", // N
    "1111111100000000", // O
    "1000001101000001", // P
    "0111000001010000", //q
    "1110001100011001", //R
    "1101110100010001", //S
    "1100000001000100", //T
    "0011111100000000", //U
    "0000001100100010", //V
    "0011001100001010", //W
    "0000000010101010", //X
    "0000000010100100", //Y
    "1100110000100010", //Z
    "0000000000000000"
};
// File Operations
static ssize_t my_read(struct file *filp, char *buf, size_t len, loff_t *off) {
printk("call read\n");
len = 16;
if( copy_to_user(buf, &ans[flag], len) >0)  {
pr_err("ERROR: Not all the bytes have been copied to user\n");
}
flag++;
return 0;
}

static ssize_t my_write(struct file *filp,const char *buf, size_t len, loff_t *off)
{
printk("call write\n");
uint8_t rec_buf[10] = {0};
if( copy_from_user( rec_buf, buf, len ) > 0) {
pr_err("ERROR: Not all the bytes have been copied from user\n");
}
led(rec_buf[0], 0);
//mdelay(1000);
led(rec_buf[1], 1);
//mdelay(1000);
led(rec_buf[2], 2);
//mdelay(1000);
led(rec_buf[3], 3);
//mdelay(1000);
led(rec_buf[4], 4);
//mdelay(1000);
return len;
}

static void led(char c, int index)
{
    int i=c-97, j;
    for (j=0; j<16; j++)
        ans[index][j] = seg_for_c[i][j];
}

static int my_open(struct inode *inode, struct file *fp) {
printk("call open\n");
return 0;
}

struct file_operations my_fops = {
.read=my_read,
.write=my_write,
.open=my_open
};

#define MAJOR_NUM 200
#define DEVICE_NAME "my_dev"
static int my_init(void) {
printk("call init\n");
if(register_chrdev(MAJOR_NUM, DEVICE_NAME, &my_fops) < 0) {
printk("Can not get major %d\n", MAJOR_NUM);
return (-EBUSY);
}
printk("My device is started and the major is %d\n", MAJOR_NUM);
return 0;
}
static void my_exit(void) {
unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
printk("call exit\n");
}
module_init(my_init);
module_exit(my_exit);
