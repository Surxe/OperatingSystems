#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/time.h>

#define PROC_NAME "ethan_maze"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple module that generates an ASCII maze in the /proc file system");
MODULE_AUTHOR("Ethan E");

/*Name: Ethan E
  Date: 9/18/2024
  Description: Custom read function that outputs a generated ASCII maze.
*/
static ssize_t ethan_read(struct file *file, char __user *buf, size_t count, loff_t *pos)
{
    int maze_width = 80;
    int maze_height = 28;
    char *maze;
    int i, j, random_val;
    ssize_t len;

    if (*pos > 0) return 0; // Prevent multiple reads

    // Allocate memory for the maze
    maze = kmalloc(maze_width * maze_height + 1, GFP_KERNEL);
    if (!maze) return -ENOMEM;

    // Seed the random number generator
    struct timespec ts;
    getnstimeofday(&ts);
    prandom_seed(ts.tv_nsec);

    // Generate the maze
    for (i = 0; i < maze_height; i++) {
        for (j = 0; j < maze_width; j++) {
            random_val = prandom_u32() % 2; // Random 0 or 1
            maze[i * maze_width + j] = (random_val == 0) ? '#' : ' ';
        }
    }
    maze[maze_width * maze_height] = '\0'; // Null-terminate the maze

    // Copy the maze to the user buffer
    len = simple_read_from_buffer(buf, count, pos, maze, maze_width * maze_height);

    kfree(maze); // Free the allocated memory
    return len;
}

/* Struct to define proc operations */
static const struct proc_ops ethan_proc_ops = {
    .proc_read = ethan_read,  // Use the new proc_read field
};

/*Name: Ethan E
  Date: 9/18/2024
  Description: Custom initialization function for the kernel module.
*/
static int ethan_init(void)
{
    proc_create(PROC_NAME, 0666, NULL, &ethan_proc_ops);
    printk(KERN_INFO "Ethan's module has been loaded.\n");
    return 0;
}

/*Name: Ethan E
  Date: 9/18/2024
  Description: Custom cleanup function for the kernel module.
*/
static void ethan_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "Ethan's module has been unloaded.\n");
}

/* Macros to attach the initialization and cleanup functions */
module_init(ethan_init);
module_exit(ethan_exit);
