#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h> // For copy_to_user
#include <linux/slab.h> // For kmalloc and kfree
#include <linux/random.h> // For prandom_u32
#include <linux/timekeeping.h> // For getnstimeofday()

#define PROC_NAME "ethan_maze"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple module that generates an ASCII maze in the proc file system");
MODULE_AUTHOR("Ethan E");

/*
Name: Ethan E
Date: 9/18/24
Description: Custom initialization function
*/
static int __init ethan_init(void) {
    proc_create(PROC_NAME, 0666, NULL, &ethan_proc_ops);
    printk(KERN_INFO "Ethan's module has been loaded.\n");
    return 0;
}

/*
Name: Ethan E
Date: 9/18/24
Description: Custom cleanup function for the kernel module.
*/
static void __exit ethan_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "Ethan's module has been unloaded.\n");
}

/*
Name: Ethan E
Date: 9/18/24
Description: Custom read function that outputs a generated ASCII maze
*/
static ssize_t ethan_read(struct file *file, char __user *buf, size_t count, loff_t *pos) {
    int maze_width = 80;
    int maze_height = 28;
    char *maze;
    int i, j, random_val;
    ssize_t len; 

    if (*pos > 0) return 0; // Prevent reading it many times

    // Allocate memory
    maze = kmalloc(maze_width * maze_height + 1, GFP_KERNEL);
    if (!maze) return -ENOMEM;

    // Seed
    struct timespec64 ts;
    ktime_get_real_ts64(&ts);
    prandom_seed(ts.tv_nsec);

    // Generate the maze
    for (i = 0; i < maze_height; i++) {
        for (j = 0; j < maze_width; j++) {
            random_val = prandom_u32() % 2; // 0 or 1
            maze[i * maze_width + j] = (random_val == 0) ? '#' : ' ';
        }
        maze[i * maze_width + maze_width] = '\n'; // Add newline at end of each row
    }
    maze[maze_width * maze_height + maze_height] = '\0'; // Terminate the maze

    // Copy it to user buffer
    len = simple_read_from_buffer(buf, count, pos, maze, maze_width * maze_height + maze_height);

    kfree(maze); // Free the memory
    return len;
}

/* Struct for file operations */
static const struct proc_ops ethan_proc_ops = {
    .proc_read = ethan_read, // custom read
};

/* Macros */
module_init(ethan_init);
module_exit(ethan_exit);
