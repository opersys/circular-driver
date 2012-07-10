/*
 * circular-char.c
 *
 * Copyright (C) 2012 Opersys inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License, not any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define BUF_SIZE 200

static char buf[BUF_SIZE];
static char *read_ptr;
static char *write_ptr;

static int device_open(struct inode *inode, struct file *file)
{
  printk("device_open called \n");

  return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
  printk("device_release called \n");

  return 0;
}

static ssize_t device_read(struct file *filp,	/* see include/linux/fs.h   */
			   char *buffer,	/* buffer to fill with data */
			   size_t length,	/* length of the buffer     */
			   loff_t * offset)
{
  int chars_read = 0;

  printk("device_read called \n");

  while(length && *read_ptr && (read_ptr != write_ptr)) {
    put_user(*(read_ptr++), buffer++);

    printk("Reading %c \n", *read_ptr);

    if(read_ptr >= buf + BUF_SIZE)
      read_ptr = buf;

    chars_read++;
    length--;
  }

  return chars_read;
}

static ssize_t
device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
  int i;

  printk("device_write called \n");

  for(i = 0; i < len; i++) {
    get_user(*write_ptr, buff++);
    printk("Writing %c \n", *write_ptr);
    write_ptr++;
    if (write_ptr >= buf + BUF_SIZE)
      write_ptr = buf;
  }

  return len;
}

static struct file_operations fops = {
  .open = device_open,
  .release = device_release,
  .read = device_read,
  .write = device_write,
};

static struct miscdevice circ_char_misc = {
  .minor = MISC_DYNAMIC_MINOR,
  .name = "circchar",
  .fops = &fops,
};

int circ_char_enter(void)
{
  int retval;

  retval = misc_register(&circ_char_misc);
  printk("CIRC Driver got retval %d\n", retval);
  printk("mmap is %08X\n", (int) fops.mmap);

  read_ptr = buf;
  write_ptr = buf;

  return 0;
}

void circ_char_exit(void)
{
  misc_deregister(&circ_char_misc);
}

module_init(circ_char_enter);
module_exit(circ_char_exit);
