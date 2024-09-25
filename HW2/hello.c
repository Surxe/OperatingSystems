#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h> // For copy_to_user and copy_from_user
#include <linux/slab.h> // For kmalloc and kfree
#include <linux/random.h> // For prandom_u32
#include <linux/timekeeping.h> // For ktime_get_real_ts64()
#include <linux/fs.h> // For file_operations

#define PROC_NAME "ethan_maze"
#define MAX_INPUT_LEN 32

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple module that generates an ASCII maze in the proc file system");
MODULE_AUTHOR("Ethan E");

/* Forward declaration of the file operations structure */
static const struct file_operations ethan_proc_ops;

/* Maze dimensions (default values) */
static int maze_width = 15;
static int maze_height = 15;
static char *maze;

/* Buffer for user input */
static char input_buffer[MAX_INPUT_LEN];

/* Struct for a maze cell's coordinates */
struct cell {
    int x;
    int y;
};

/* Check if the coordinates are within the bounds of the maze */
static int is_within_bounds(int x, int y) {
    return x >= 0 && x < maze_width && y >= 0 && y < maze_height;
}

/* Function to mark a passage and add frontier cells */
static void add_frontier(struct cell *frontiers, int *frontier_count, int x, int y) {
    if (is_within_bounds(x, y) && maze[y * (maze_width + 1) + x] == '#') {
        frontiers[*frontier_count].x = x;
        frontiers[*frontier_count].y = y;
        (*frontier_count)++;
    }
}

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
Description: Function to generate a maze using Prim's algorithm
*/
static void generate_maze(void) {
    struct cell frontiers[maze_width * maze_height];
    int frontier_count = 0;
    int i, j;

    // Initialize the maze's walls
    for (i = 0; i < maze_height; i++) {
        for (j = 0; j < maze_width; j++) {
            maze[i * (maze_width + 1) + j] = '#'; // Set walls
        }
        maze[i * (maze_width + 1) + maze_width] = '\n'; // Add newline at end of each row
    }

    // Pick a random starting cell
    int start_x = 1;
    int start_y = 1;
    maze[start_y * (maze_width + 1) + start_x] = ' '; // Mark as a passage

    // Add initial frontier cells
    add_frontier(frontiers, &frontier_count, start_x + 2, start_y);
    add_frontier(frontiers, &frontier_count, start_x - 2, start_y);
    add_frontier(frontiers, &frontier_count, start_x, start_y + 2);
    add_frontier(frontiers, &frontier_count, start_x, start_y - 2);

    // Prim's algorithm loop
    while (frontier_count > 0) {
        // Pick a random frontier cell
        int index = prandom_u32() % frontier_count;
        struct cell frontier = frontiers[index];

        // Find a neighboring passage cell
        int directions[4][2] = { {2, 0}, {-2, 0}, {0, 2}, {0, -2} };
        int passage_x = frontier.x;
        int passage_y = frontier.y;

        for (i = 0; i < 4; i++) {
            int nx = frontier.x + directions[i][0];
            int ny = frontier.y + directions[i][1];

            if (is_within_bounds(nx, ny) && maze[ny * (maze_width + 1) + nx] == ' ') {
                passage_x = (frontier.x + nx) / 2;
                passage_y = (frontier.y + ny) / 2;
                break;
            }
        }

        // Set the frontier and in-between cell to passage
        maze[frontier.y * (maze_width + 1) + frontier.x] = ' ';
        maze[passage_y * (maze_width + 1) + passage_x] = ' ';

        // Add new frontiers
        add_frontier(frontiers, &frontier_count, frontier.x + 2, frontier.y);
        add_frontier(frontiers, &frontier_count, frontier.x - 2, frontier.y);
        add_frontier(frontiers, &frontier_count, frontier.x, frontier.y + 2);
        add_frontier(frontiers, &frontier_count, frontier.x, frontier.y - 2);

        // Remove the processed frontier
        frontiers[index] = frontiers[--frontier_count];
    }

    // Mark the start position with 'S'
    maze[start_y * (maze_width + 1) + start_x] = 'S';

    // Set the ending position (maze_height-2, maze_width-2) as a walkway and mark it with 'E'
    int end_x = maze_width - 2;
    int end_y = maze_height - 2;
    maze[end_y * (maze_width + 1) + end_x] = 'E';
}

/*
Name: Ethan E
Date: 9/19/24
Description: Custom read function that outputs a generated ASCII maze
*/
static ssize_t ethan_read(struct file *file, char __user *buf, size_t count, loff_t *pos) {
    struct timespec64 ts;
    ssize_t len; 

    if (*pos > 0) return 0; // Prevent reading it many times

    // Allocate memory for the maze
    maze = kmalloc((maze_width * maze_height) + maze_height + 1, GFP_KERNEL); // Extra space for newlines and null terminator
    if (!maze) return -ENOMEM;

    // Seed random number generator
    ktime_get_real_ts64(&ts);
    prandom_seed(ts.tv_nsec);

    // Generate the maze
    generate_maze();

    maze[maze_width * maze_height + maze_height] = '\0'; // Null terminate the maze

    // Copy it to user buffer
    len = simple_read_from_buffer(buf, count, pos, maze, maze_width * maze_height + maze_height);

    kfree(maze); // Free the memory
    return len;
}

/*
Name: Ethan E
Date: 9/19/24
Description: Custom write function to accept parameters for maze dimensions
*/
static ssize_t ethan_write(struct file *file, const char __user *buf, size_t count, loff_t *pos) {
    char input[MAX_INPUT_LEN];
    int new_width, new_height;

    if (count > MAX_INPUT_LEN - 1)
        return -EINVAL; // Input too long

    if (copy_from_user(input, buf, count))
        return -EFAULT;

    input[count] = '\0'; // Null-terminate the input string

    // Parse the input for width and height
    if (sscanf(input, "%d %d", &new_width, &new_height) == 2) {
        // Ensure dimensions are valid
        if (new_width > 0 && new_height > 0 && new_width <= 100 && new_height <= 100) {
            maze_width = new_width;
            maze_height = new_height;
            printk(KERN_INFO "Maze dimensions set to: %dx%d\n", maze_width, maze_height);
        } else {
            printk(KERN_WARNING "Invalid maze dimensions: %dx%d\n", new_width, new_height);
        }
    } else {
        printk(KERN_WARNING "Invalid input format. Expected: <width> <height>\n");
    }

    return count;
}

/* Struct for file operations */
static const struct file_operations ethan_proc_ops = {
    .owner = THIS_MODULE,
    .read = ethan_read, // custom read
    .write = ethan_write, // custom write to accept parameters
};

/* Macros */
module_init(ethan_init);
module_exit(ethan_exit);
