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
MODULE_DESCRIPTION("A simple module that generates an ASCII maze in the proc file system using the Hunt and Kill algorithm");
MODULE_AUTHOR("Ethan E");

/* Forward declaration of the file operations structure */
static const struct file_operations ethan_proc_ops;

/*
Name: Ethan E
Date: 9/19/24
Description: Custom initialization function
*/
static int __init ethan_init(void) {
    proc_create(PROC_NAME, 0666, NULL, &ethan_proc_ops);
    printk(KERN_INFO "Ethan's module has been loaded.\n");
    return 0;
}

/*
Name: Ethan E
Date: 9/19/24
Description: Custom cleanup function for the kernel module.
*/
static void __exit ethan_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "Ethan's module has been unloaded.\n");
}

/* Maze dimensions */
#define MAZE_WIDTH 30
#define MAZE_HEIGHT 20

/* Maze generation functions */
static void init_maze(char *maze) {
    int i, j;
    for (i = 0; i < MAZE_HEIGHT; i++) {
        for (j = 0; j < MAZE_WIDTH; j++) {
            maze[i * (MAZE_WIDTH + 1) + j] = '#'; // Fill the maze with walls
        }
        maze[i * (MAZE_WIDTH + 1) + MAZE_WIDTH] = '\n'; // Newline at end of each row
    }
    maze[MAZE_WIDTH * MAZE_HEIGHT + MAZE_HEIGHT] = '\0'; // Null terminate the maze
}

static void carve_path(char *maze, int x, int y) {
    maze[y * (MAZE_WIDTH + 1) + x] = ' '; // Carve the path

    int directions[4][2] = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} }; // Right, Down, Left, Up
    int dir, nx, ny;
    int i;

    for (i = 0; i < 4; i++) {
        dir = prandom_u32() % 4; // Randomize the order of directions
        nx = x + directions[dir][0];
        ny = y + directions[dir][1];

        if (nx > 0 && nx < MAZE_WIDTH && ny > 0 && ny < MAZE_HEIGHT && maze[ny * (MAZE_WIDTH + 1) + nx] == '#') {
            carve_path(maze, nx, ny);
        }
    }
}

/*
Name: Ethan E
Date: 9/19/24
Description: Custom read function that outputs a generated ASCII maze using the Hunt and Kill algorithm.
*/
static ssize_t ethan_read(struct file *file, char __user *buf, size_t count, loff_t *pos) {
    struct timespec64 ts;
    char *maze;
    ssize_t len;
    int start_x, start_y;

    if (*pos > 0) return 0; // Prevent reading it multiple times

    // Allocate memory
    maze = kmalloc((MAZE_WIDTH * MAZE_HEIGHT) + MAZE_HEIGHT + 1, GFP_KERNEL); // Extra space for newlines and null terminator
    if (!maze) return -ENOMEM;

    // Seed
    ktime_get_real_ts64(&ts);
    prandom_seed(ts.tv_nsec);

    // Initialize the maze with walls
    init_maze(maze);

    // Random starting point
    start_x = prandom_u32() % (MAZE_WIDTH - 2) + 1;
    start_y = prandom_u32() % (MAZE_HEIGHT - 2) + 1;

    // Start carving paths
    carve_path(maze, start_x, start_y);

    // Create entrance and exit
    maze[start_y * (MAZE_WIDTH + 1)] = ' '; // Entrance at the first row
    maze[(MAZE_HEIGHT - 2) * (MAZE_WIDTH + 1) + (MAZE_WIDTH - 2)] = ' '; // Exit near the bottom right

    // Copy the maze to the user buffer
    len = simple_read_from_buffer(buf, count, pos, maze, MAZE_WIDTH * MAZE_HEIGHT + MAZE_HEIGHT);

    kfree(maze); // Free the memory
    return len;
}

/* Struct for file operations */
static const struct file_operations ethan_proc_ops = {
    .owner = THIS_MODULE,
    .read = ethan_read, // Custom read
};

/* Macros */
module_init(ethan_init);
module_exit(ethan_exit);
