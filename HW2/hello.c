#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h> // For copy_to_user
#include <linux/slab.h> // For kmalloc and kfree
#include <linux/random.h> // For prandom_u32
#include <linux/timekeeping.h> // For ktime_get_real_ts64()
#include <linux/fs.h> // For file_operations
#include <linux/ktime.h>
#include <linux/kernel.h>

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


// Direction vectors for moving up, right, down, left
static const int directions[4][2] = {
    {-1, 0}, // Up
    {0, 1},  // Right
    {1, 0},  // Down
    {0, -1}  // Left
};

/*
Name: Ethan E
Date: 9/19/24
Description: Checks if a position is within bounds
*/
static int in_bounds(int x, int y, int width, int height) {
    return x >= 0 && x < height && y >= 0 && y < width;
}

/*
Name: Ethan E
Date: 9/19/24
Description: Checks if a position is a wall (for maze generation)
*/
static int is_wall(char *maze, int x, int y, int width, int height) {
    return x < 0 || x >= height || y < 0 || y >= width || maze[x * (width + 1) + y] == '#';
}

/*
Name: Ethan E
Date: 9/19/24
Description: Initialize the maze with walls
*/
static void initialize_maze(char *maze, int width, int height) {
    int i, j;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            maze[i * (width + 1) + j] = '#'; // Fill with walls
        }
        maze[i * (width + 1) + width] = '\n'; // Newline at end of each row
    }
    maze[width * height + height] = '\0'; // Null terminate the maze
}

/*
Name: Ethan E
Date: 9/19/24
Description: Prim's algorithm to generate the maze
*/
static void generate_maze(char *maze, int width, int height) {
    int x, y, nx, ny, i, dir;
    int start_x = 1, start_y = 1; // Start from the second cell to ensure the border remains
    int end_x, end_y;

    initialize_maze(maze, width, height);

    // Initialize the maze
    maze[start_x * (width + 1) + start_y] = ' '; // Starting point
    struct {
        int x, y;
    } walls[width * height]; // Stack to store walls
    int wall_count = 0;

    // Add initial walls around the starting point
    for (i = 0; i < 4; i++) {
        x = start_x + directions[i][0] * 2;
        y = start_y + directions[i][1] * 2;
        if (in_bounds(x, y, width, height)) {
            walls[wall_count++] = (struct {int x, y;}){x, y};
            maze[x * (width + 1) + y] = ' ';
        }
    }

    // Perform Prim's algorithm
    while (wall_count > 0) {
        dir = prandom_u32() % wall_count; // Randomly pick a wall
        x = walls[dir].x;
        y = walls[dir].y;

        // Check if this wall can be turned into a passage
        int valid = 0;
        for (i = 0; i < 4; i++) {
            nx = x + directions[i][0];
            ny = y + directions[i][1];
            if (in_bounds(nx, ny, width, height) && maze[nx * (width + 1) + ny] == ' ') {
                valid = 1;
                break;
            }
        }

        if (valid) {
            maze[x * (width + 1) + y] = ' '; // Open this wall

            // Add new walls
            for (i = 0; i < 4; i++) {
                nx = x + directions[i][0] * 2;
                ny = y + directions[i][1] * 2;
                if (in_bounds(nx, ny, width, height) && maze[nx * (width + 1) + ny] == '#') {
                    walls[wall_count++] = (struct {int x, y;}){nx, ny};
                    maze[nx * (width + 1) + ny] = ' ';
                }
            }
        }

        // Remove the wall from the stack
        wall_count--;
        walls[dir] = walls[wall_count];
    }

    // Set entrance and exit
    maze[start_x * (width + 1) + start_y] = 'E'; // Entrance
    end_x = height - 2;
    end_y = width - 2;
    maze[end_x * (width + 1) + end_y] = 'X'; // Exit
}

/*
Name: Ethan E
Date: 9/18/24
Description: Custom read function that outputs a generated ASCII maze
*/
static ssize_t ethan_read(struct file *file, char __user *buf, size_t count, loff_t *pos) {
    struct timespec64 ts;
    int maze_width = 40;
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

    // Generate the maze
    generate_maze(maze, maze_width, maze_height);

    // Copy it to user buffer
    len = simple_read_from_buffer(buf, count, pos, maze, maze_width * maze_height + maze_height);

    kfree(maze); // Free the memory
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
