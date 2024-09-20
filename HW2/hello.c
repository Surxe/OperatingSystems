#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h> // For copy_to_user
#include <linux/slab.h> // For kmalloc and kfree
#include <linux/random.h> // For prandom_u32
#include <linux/timekeeping.h> // For ktime_get_real_ts64()
#include <linux/fs.h> // For file_operations
#include <linux/bitmap.h> // For bitmap operations

#define PROC_NAME "ethan_maze"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A kernel module that generates an ASCII maze in the proc file system using Prim's algorithm");
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

/* Function prototypes */
static void generate_maze(char *maze, int width, int height);
static void prim_algorithm(char *maze, int width, int height);
static void add_entrance_and_exit(char *maze, int width, int height);

/*
Name: Ethan E
Date: 9/19/24
Description: Generates a maze using Prim's algorithm and adds entrance/exit.
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

    // Generate the maze
    generate_maze(maze, maze_width, maze_height);

    // Copy it to user buffer
    len = simple_read_from_buffer(buf, count, pos, maze, maze_width * maze_height + maze_height);

    kfree(maze); // Free the memory
    return len;
}

/*
Name: Ethan E
Date: 9/19/24
Description: Generates the maze using Prim's algorithm.
*/
static void generate_maze(char *maze, int width, int height) {
    int i, j;
    int num_cells = width * height;
    bool *visited = kmalloc(num_cells * sizeof(bool), GFP_KERNEL);
    struct timespec64 ts;

    if (!visited) return;

    memset(visited, 0, num_cells * sizeof(bool));
    ktime_get_real_ts64(&ts);
    prandom_seed(ts.tv_nsec);

    // Initialize the maze with walls
    memset(maze, '#', width * height);

    prim_algorithm(maze, width, height);

    add_entrance_and_exit(maze, width, height);

    // Set newlines and null terminator
    for (i = 0; i < height; i++) {
        maze[i * (width + 1) + width] = '\n';
    }
    maze[width * height + height] = '\0';

    kfree(visited);
}

/*
Name: Ethan E
Date: 9/19/24
Description: Implements Prim's algorithm for maze generation.
*/
static void prim_algorithm(char *maze, int width, int height) {
    int x, y, nx, ny;
    int dx[] = {1, 0, -1, 0};
    int dy[] = {0, 1, 0, -1};
    int num_cells = width * height;
    bool *visited = kmalloc(num_cells * sizeof(bool), GFP_KERNEL);
    struct timespec64 ts;
    int *walls;
    int num_walls;
    int i;

    if (!visited) return;

    memset(visited, 0, num_cells * sizeof(bool));
    ktime_get_real_ts64(&ts);
    prandom_seed(ts.tv_nsec);

    // Choose a starting point
    x = prandom_u32() % width;
    y = prandom_u32() % height;
    visited[y * width + x] = true;
    
    num_walls = 0;
    walls = kmalloc(num_cells * sizeof(int), GFP_KERNEL);

    if (!walls) {
        kfree(visited);
        return;
    }

    // Add initial walls
    for (i = 0; i < 4; i++) {
        nx = x + dx[i] * 2;
        ny = y + dy[i] * 2;
        if (nx > 0 && ny > 0 && nx < width - 1 && ny < height - 1) {
            walls[num_walls++] = ny * width + nx;
        }
    }

    // Run Prim's algorithm
    while (num_walls > 0) {
        int wall_idx = prandom_u32() % num_walls;
        int wall = walls[wall_idx];
        nx = wall % width;
        ny = wall / width;
        int removed = false;

        for (i = 0; i < 4; i++) {
            int nx2 = nx + dx[i];
            int ny2 = ny + dy[i];
            if (nx2 >= 0 && ny2 >= 0 && nx2 < width && ny2 < height) {
                if (visited[ny2 * width + nx2]) {
                    maze[ny * width + nx] = ' ';
                    visited[ny * width + nx] = true;
                    removed = true;
                    break;
                }
            }
        }

        // Remove wall from list
        if (removed) {
            walls[wall_idx] = walls[--num_walls];
        } else {
            num_walls--;
        }

        // Add new walls
        for (i = 0; i < 4; i++) {
            nx = nx + dx[i] * 2;
            ny = ny + dy[i] * 2;
            if (nx >= 0 && ny >= 0 && nx < width && ny < height) {
                if (!visited[ny * width + nx]) {
                    walls[num_walls++] = ny * width + nx;
                }
            }
        }
    }

    kfree(walls);
    kfree(visited);
}

/*
Name: Ethan E
Date: 9/19/24
Description: Adds entrance and exit to the maze.
*/
static void add_entrance_and_exit(char *maze, int width, int height) {
    int entrance_x = prandom_u32() % width;
    int exit_x = prandom_u32() % width;

    maze[0 * width + entrance_x] = ' ';
    maze[(height - 1) * width + exit_x] = ' ';
}

/* Struct for file operations */
static const struct file_operations ethan_proc_ops = {
    .owner = THIS_MODULE,
    .read = ethan_read, // custom read
};

/* Macros */
module_init(ethan_init);
module_exit(ethan_exit);
