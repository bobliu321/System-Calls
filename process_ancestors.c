#include <linux/sched.h>
#include <linux/cred.h>
#include <linux/kernel.h>
#include <linux/unistd.h>
#include "process_ancestors.h"

//Print testing purposes only
void print_info(int x, struct process_info *s);

asmlinkage long sys_process_ancestors(
        struct process_info info_array[],
        long size,
        long *num_filled);

asmlinkage long sys_process_ancestors(
        struct process_info info_array[],
        long size,
        long *num_filled) {

    //declaring struct
    struct process_info procInfo;
    struct task_struct *curTask;
    struct list_head *childList;
    struct list_head *siblingList;

    //initializing variables
    int childrenNum = 0;
    int siblingNum = 0;
    int index = 0;

    //Iterating through every process
    for (curTask = current; curTask != &init_task; curTask = curTask->parent) {
        //copying current info
        procInfo.pid = (long) curTask->pid;
        procInfo.state = curTask->state;
        procInfo.nvcsw = curTask->nvcsw;
        procInfo.nivcsw = curTask->nivcsw;
        strncpy(procInfo.name, curTask->comm, ANCESTOR_NAME_LEN);

        //Inserting children struct and counting number of children
        list_for_each(childList, &curTask->children){
            list_entry(childList, struct task_struct, children);
            childrenNum++;
        }

        //Inserting sibling struct and counting number of siblings
        list_for_each(siblingList, &curTask->sibling){
            list_entry(siblingList, struct task_struct, sibling);
            siblingNum++;
        }

        procInfo.num_children = (long) childrenNum;
        procInfo.num_siblings = (long) siblingNum;

        //coping current info into info_array
        info_array[index] = procInfo;
        //print_info(index, info_array);

        index++;
    }
    return 0;
}

void print_info(int x, struct process_info *s) {
    printk("--------------------------------------- \n");
    printk("Process ID: %ld \n", s[x].pid);
    printk("name : %s \n", s[x].name);
    printk("state: %ld \n", s[x].state);
    printk("User ID: %ld \n", s[x].uid);
    printk("# voluntary context switches: %ld \n", s[x].nvcsw);
    printk("# involuntary context switches: %ld \n", s[x].nivcsw);
    printk("# children #: %ld \n", s[x].num_children);
    printk("# sibling #: %ld \n", s[x].num_siblings);
    printk("--------------------------------------- \n");
}
