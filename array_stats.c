#include <asm/uaccess.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include "array_stats.h"

asmlinkage long sys_array_stats(
        struct array_stats *stats,
        long data[],
        long size);

asmlinkage long sys_array_stats(
        struct array_stats *stats,
        long data[],
        long size) {
    //initialize
    int i;
    long buf = 0;
    struct array_stats *statsTemp = kmalloc(sizeof(struct array_stats), GFP_KERNEL);

    //check if input is empty
    if (size <= 0) {
        printk("ERROR: Input must be not empty\n");
        kfree(statsTemp);
        return -EINVAL;
    } else {

        for (i = 0; i < size; i++) {
            if (copy_from_user(&buf, &data[i], sizeof(long)) != 0) {
                printk("ERROR: Cannot copy data\n");
                kfree(statsTemp);
                return -EFAULT;
            } else {
                //Initialize min,max,sum as data{0}
                if (i == 0) {
                    statsTemp->min = buf;
                    statsTemp->max = buf;
                    statsTemp->sum = buf;
                    continue;
                }
                //Compare current data with data in buf
                if (statsTemp->min > buf)
                    statsTemp->min = buf;

                if ((statsTemp->max) < buf)
                    statsTemp->max = buf;

                //Sum every data in the iteration
                statsTemp->sum += buf;
            }
        }

        //check if copy succeed
        if (copy_to_user(&(stats->min), &(statsTemp->min), sizeof(long)) != 0 ||
            copy_to_user(&(stats->max), &(statsTemp->max), sizeof(long)) != 0 ||
            copy_to_user(&(stats->sum), &(statsTemp->sum), sizeof(long)) != 0) {
            printk("ERROR: Copying to stats \n");
            kfree(statsTemp);
            return -EFAULT;
        }

        kfree(statsTemp);
        return 0;
    }
}
