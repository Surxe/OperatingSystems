#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h> // For copy_to_user
#include <linux/slab.h> // For kmalloc and kfree
#include <linux/random.h> // For prandom_u32
#include <linux/timekeeping.h> // For ktime_get_real_ts64()
#include <linux/fs.h> // For file_operations

#define PROC_NAME "ethan_maze"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple module that generates an ASCII maze in the proc file system");
MODULE_AUTHOR("Ethan E");

/* Forward declaration of the file operations structure */
static const struct file_operations ethan_proc_ops;

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
    struct timespec64 ts;
    int maze_width = 30;
    int maze_height = 20;
    char *maze;
    int i, j, random_val, x, y;
    ssize_t len;

    if (*pos > 0) return 0; // Prevent reading it many times

    // Allocate memory
    maze = kmalloc((maze_width * maze_height) + maze_height + 1, GFP_KERNEL); // Extra space for newlines and null terminator
    if (!maze) return -ENOMEM;

    // Seed
    ktime_get_real_ts64(&ts);
    prandom_seed(ts.tv_nsec);

    // Initialize the maze with walls
    for (i = 0; i < maze_height; i++) {
        for (j = 0; j < maze_width; j++) {
            maze[i * (maze_width + 1) + j] = '#'; // All walls
        }
        maze[i * (maze_width + 1) + maze_width] = '\n'; // Add newline at end of each row
    }
    maze[maze_width * maze_height + maze_height] = '\0'; // Null terminate the maze

    // Prim's algorithm initialization
    int *walls = kmalloc(maze_width * maze_height * sizeof(int), GFP_KERNEL);
    int wall_count = 0;

    // Start point
    x = prandom_u32() % (maze_width / 2) * 2 + 1; // Ensure it's odd to avoid boundaries
    y = prandom_u32() % (maze_height / 2) * 2 + 1; // Ensure it's odd to avoid boundaries

    maze[y * (maze_width + 1) + x] = ' '; // Start point
    walls[wall_count++] = y * maze_width + x;

    while (wall_count > 0) {
        // Choose a random wall
        int wall_index = prandom_u32() % wall_count;
        int wall = walls[wall_index];
        int wx = wall % maze_width;
        int wy = wall / maze_width;

        // Remove the wall from the list
        walls[wall_index] = walls[--wall_count];

        // Check if it's a valid wall
        int cx, cy, count = 0;
        if (wx > 1 && maze[(wy) * (maze_width + 1) + wx - 2] == ' ') { cx = wx - 2; cy = wy; count++; }
        if (wx < maze_width - 2 && maze[(wy) * (maze_width + 1) + wx + 2] == ' ') { cx = wx + 2; cy = wy; count++; }
        if (wy > 1 && maze[(wy - 2) * (maze_width + 1) + wx] == ' ') { cx = wx; cy = wy - 2; count++; }
        if (wy < maze_height - 2 && maze[(wy + 2) * (maze_width + 1) + wx] == ' ') { cx = wx; cy = wy + 2; count++; }

        if (count == 1) { // It is a valid wall
            maze[wy * (maze_width + 1) + wx] = ' '; // Break the wall

            // Add new walls
            if (wx > 1 && maze[(wy) * (maze_width + 1) + wx - 2] == '#') walls[wall_count++] = wy * maze_width + (wx - 2);
            if (wx < maze_width - 2 && maze[(wy) * (maze_width + 1) + wx + 2] == '#') walls[wall_count++] = wy * maze_width + (wx + 2);
            if (wy > 1 && maze[(wy - 2) * (maze_width + 1) + wx] == '#') walls[wall_count++] = (wy - 2) * maze_width + wx;
            if (wy < maze_height - 2 && maze[(wy + 2) * (maze_width + 1) + wx] == '#') walls[wall_count++] = (wy + 2) * maze_width + wx;
        }
    }

    // Add entrance and exit
    maze[0 * (maze_width + 1) + 1] = ' '; // Entrance
    maze[(maze_height - 1) * (maze_width + 1) + maze_width - 2] = ' '; // Exit

    // Copy it to user buffer
    len = simple_read_from_buffer(buf, count, pos, maze, maze_width * maze_height + maze_height);

    kfree(walls); // Free the walls memory
    kfree(maze); // Free the maze memory
    return len;
}

/* Struct for file operations */
static const struct file_operations ethan_proc_ops = {
    .owner = THIS_MODULE,
    .read = ethan_read, // custom read
};

/* Macros */
module_init(ethan_init);
module_exit(ethan_exit);
