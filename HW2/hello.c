#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h> // For copy_to_user
#include <linux/slab.h> // For kmalloc and kfree
#include <linux/random.h> // For prandom_u32
#include <linux/timekeeping.h> // For ktime_get_real_ts64()
#include <linux/fs.h> // For file_operations
#include <stdbool.h> // For bool type

#define PROC_NAME "ethan_maze"
#define WALL '#'
#define SPACE ' '

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple module that generates an ASCII maze in the proc file system");
MODULE_AUTHOR("Ethan E");

/* Forward declaration of the file operations structure */
static const struct file_operations ethan_proc_ops;

static void prims_algorithm(char *maze, int maze_width, int maze_height, int start_x, int start_y);

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

/*
Name: Ethan E
Date: 9/19/24
Description: Custom read function that outputs a generated ASCII maze
*/
static ssize_t ethan_read(struct file *file, char __user *buf, size_t count, loff_t *pos) {
    struct timespec64 ts;
    int maze_width = 30;
    int maze_height = 20;
    char *maze;
    ssize_t len; 

    if (*pos > 0) return 0; // Prevent reading it many times

    // Allocate memory
    maze = kmalloc((maze_width * maze_height) + maze_height + 1, GFP_KERNEL); // Extra space for newlines and null terminator
    if (!maze) return -ENOMEM;

    // Seed
    ktime_get_real_ts64(&ts);
    prandom_seed(ts.tv_nsec);

    // Initialize the maze's walls and walkways
    for (int i = 0; i < maze_height; i++) {
        for (int j = 0; j < maze_width; j++) {
            maze[i * (maze_width + 1) + j] = WALL; // Set walls
        }
        maze[i * (maze_width + 1) + maze_width] = '\n'; // Add newline at end of each row
    }

    // Set starting position (1,1) as a walkway, and ensure it's not enclosed
    maze[1 * (maze_width + 1) + 1] = SPACE; // Starting location
    maze[0 * (maze_width + 1) + 1] = SPACE; // Remove wall above entrance

    // Set ending position (maze_height-2, maze_width-2) as a walkway
    maze[(maze_height - 2) * (maze_width + 1) + (maze_width - 2)] = SPACE; // Ending location
    maze[(maze_height - 1) * (maze_width + 1) + (maze_width - 2)] = SPACE; // Remove wall below exit

    // Connect entrance and exit using Prim's algorithm
    prims_algorithm(maze, maze_width, maze_height, 1, 1);

    maze[maze_width * maze_height + maze_height] = '\0'; // Null terminate the maze

    // Copy it to user buffer
    len = simple_read_from_buffer(buf, count, pos, maze, maze_width * maze_height + maze_height);

    kfree(maze); // Free the memory
    return len;
}

/*
Name: Ethan E
Date: 9/19/24
Description: Connects the entrance and exit using Prim's algorithm
*/
static void prims_algorithm(char *maze, int maze_width, int maze_height, int start_x, int start_y) {
    // This is a simple version of Prim's algorithm to create a path.
    int x = start_x, y = start_y;
    int directions[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}}; // Right, Down, Left, Up
    int path_length = 0;

    // Create a stack for walls
    bool **in_mst = kmalloc(maze_height * sizeof(bool *), GFP_KERNEL);
    for (int i = 0; i < maze_height; i++) {
        in_mst[i] = kmalloc(maze_width * sizeof(bool), GFP_KERNEL);
        for (int j = 0; j < maze_width; j++) {
            in_mst[i][j] = false;
        }
    }

    in_mst[y][x] = true; // Mark starting point as part of the MST

    while (path_length < 20) { // Generate a fixed number of paths
        int direction_index = prandom_u32() % 4; // Random direction
        int new_x = x + directions[direction_index][0];
        int new_y = y + directions[direction_index][1];

        // Check bounds and if not already in MST
        if (new_x > 0 && new_x < maze_width - 1 && new_y > 0 && new_y < maze_height - 1 && !in_mst[new_y][new_x]) {
            maze[y * (maze_width + 1) + x] = SPACE; // Create path
            maze[new_y * (maze_width + 1) + new_x] = SPACE; // Create path
            in_mst[new_y][new_x] = true; // Mark new position as part of MST

            // Move to new position
            x = new_x;
            y = new_y;
            path_length++;
        }
    }

    // Free allocated memory
    for (int i = 0; i < maze_height; i++) {
        kfree(in_mst[i]);
    }
    kfree(in_mst);
}

/* Struct for file operations */
static const struct file_operations ethan_proc_ops = {
    .owner = THIS_MODULE,
    .read = ethan_read, // custom read
};

/* Macros */
module_init(ethan_init);
module_exit(ethan_exit);
