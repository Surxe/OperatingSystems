#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h> // For copy_to_user
#include <linux/slab.h> // For kmalloc and kfree
#include <linux/random.h> // For prandom_u32
#include <linux/timekeeping.h> // For ktime_get_real_ts64()
#include <linux/fs.h> // For file_operations

#define PROC_NAME "ethan_maze"
#define MAZE_WIDTH 30
#define MAZE_HEIGHT 20

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple module that generates an ASCII maze in the proc file system");
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

/*
Name: Ethan E
Date: 9/19/24
Description: Custom read function that outputs a generated ASCII maze
*/
static ssize_t ethan_read(struct file *file, char __user *buf, size_t count, loff_t *pos) {
    char *maze;
    ssize_t len; 

    if (*pos > 0) return 0; // Prevent reading it multiple times

    // Allocate memory for the maze
    maze = kmalloc((MAZE_WIDTH * MAZE_HEIGHT) + MAZE_HEIGHT + 1, GFP_KERNEL); // Extra space for newlines and null terminator
    if (!maze) return -ENOMEM;

    // Generate the maze
    generate_hunt_and_kill_maze(maze);

    // Copy it to user buffer
    len = simple_read_from_buffer(buf, count, pos, maze, MAZE_WIDTH * MAZE_HEIGHT + MAZE_HEIGHT);

    kfree(maze); // Free the memory
    return len;
}

/*
Name: Ethan E
Date: 9/19/24
Description: Generates the maze using the Hunt and Kill algorithm
*/
static void generate_hunt_and_kill_maze(char *maze) {
    int x, y, i, j;
    int visited[MAZE_HEIGHT][MAZE_WIDTH] = {0};
    int walls[MAZE_HEIGHT][MAZE_WIDTH] = {0};
    
    // Initialize the maze with walls
    for (i = 0; i < MAZE_HEIGHT; i++) {
        for (j = 0; j < MAZE_WIDTH; j++) {
            maze[i * (MAZE_WIDTH + 1) + j] = '#'; // Fill with walls
            walls[i][j] = 1; // All walls initially
        }
    }

    // Start point
    x = 1; 
    y = 1;
    maze[y * (MAZE_WIDTH + 1) + x] = ' '; // Create entrance
    visited[y][x] = 1; // Mark as visited
    walls[y][x] = 0;

    // Main loop for maze generation
    while (1) {
        // Randomly pick a direction
        int found = 0;
        int direction[4][2] = {{0, 2}, {2, 0}, {0, -2}, {-2, 0}};
        for (i = 0; i < 4; i++) {
            int dir = prandom_u32() % 4;
            int nx = x + direction[dir][0];
            int ny = y + direction[dir][1];
            if (nx > 0 && nx < MAZE_WIDTH - 1 && ny > 0 && ny < MAZE_HEIGHT - 1 && !visited[ny][nx]) {
                maze[y * (MAZE_WIDTH + 1) + x + direction[dir][0] / 2] = ' '; // Remove wall
                maze[ny * (MAZE_WIDTH + 1) + nx] = ' '; // Create path
                visited[ny][nx] = 1; // Mark as visited
                x = nx;
                y = ny;
                found = 1;
                break;
            }
        }

        if (!found) {
            // Hunt for unvisited cells
            int hunted = 0;
            for (i = 1; i < MAZE_HEIGHT - 1; i++) {
                for (j = 1; j < MAZE_WIDTH - 1; j++) {
                    if (!visited[i][j]) {
                        // Check if it can connect to a visited cell
                        if (visited[i-1][j] || visited[i+1][j] || visited[i][j-1] || visited[i][j+1]) {
                            // Move to this cell
                            x = j;
                            y = i;
                            maze[y * (MAZE_WIDTH + 1) + x] = ' '; // Create path
                            visited[y][x] = 1; // Mark as visited
                            hunted = 1;
                            break;
                        }
                    }
                }
                if (hunted) break;
            }
            if (!hunted) break; // Exit if no more cells to hunt
        }
    }

    // Create exit
    maze[(MAZE_HEIGHT - 2) * (MAZE_WIDTH + 1) + (MAZE_WIDTH - 2)] = ' '; // Exit point

    // Null terminate the maze
    maze[MAZE_WIDTH * MAZE_HEIGHT + MAZE_HEIGHT] = '\0'; // Null terminate
}

/* Struct for file operations */
static const struct file_operations ethan_proc_ops = {
    .owner = THIS_MODULE,
    .read = ethan_read, // custom read
};

/* Macros */
module_init(ethan_init);
module_exit(ethan_exit);
