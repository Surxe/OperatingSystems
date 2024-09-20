#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h> // For copy_to_user
#include <linux/slab.h> // For kmalloc and kfree
#include <linux/random.h> // For prandom_u32
#include <linux/timekeeping.h> // For ktime_get_real_ts64()
#include <linux/fs.h> // For file_operations
#include <stdbool.h>

#define PROC_NAME "ethan_maze"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple module that generates an ASCII maze in the proc file system");
MODULE_AUTHOR("Ethan E");

/* Forward declaration of the file operations structure */
static const struct file_operations ethan_proc_ops;

#define MAZE_WIDTH 30
#define MAZE_HEIGHT 20

/* Struct for a cell in the maze */
struct Cell {
    int x;
    int y;
    bool inMaze; // Whether the cell is part of the maze
};

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
static void generate_maze(char *maze) {
    struct Cell cells[MAZE_WIDTH * MAZE_HEIGHT];
    int cellCount = 0;
    int i, j, randIndex;

    // Initialize the maze with walls
    for (i = 0; i < MAZE_HEIGHT; i++) {
        for (j = 0; j < MAZE_WIDTH; j++) {
            maze[i * (MAZE_WIDTH + 1) + j] = '#'; // Set walls
        }
        maze[i * (MAZE_WIDTH + 1) + MAZE_WIDTH] = '\n'; // Add newline at end of each row
    }

    // Start with the initial cell
    cells[cellCount++] = (struct Cell){1, 1, false};
    maze[1 * (MAZE_WIDTH + 1) + 1] = ' '; // Start point

    // Randomly add walls to the list
    while (cellCount > 0) {
        randIndex = prandom_u32() % cellCount;
        struct Cell selectedCell = cells[randIndex];
        int x = selectedCell.x;
        int y = selectedCell.y;
        cells[randIndex] = cells[--cellCount]; // Remove from list

        // Add neighbors
        if (x > 1 && maze[(y * (MAZE_WIDTH + 1)) + (x - 2)] == '#') {
            cells[cellCount++] = (struct Cell){x - 1, y, false};
            maze[(y * (MAZE_WIDTH + 1)) + (x - 1)] = ' '; // Create walkway
        }
        if (x < MAZE_WIDTH - 2 && maze[(y * (MAZE_WIDTH + 1)) + (x + 2)] == '#') {
            cells[cellCount++] = (struct Cell){x + 1, y, false};
            maze[(y * (MAZE_WIDTH + 1)) + (x + 1)] = ' '; // Create walkway
        }
        if (y > 1 && maze[((y - 2) * (MAZE_WIDTH + 1)) + x] == '#') {
            cells[cellCount++] = (struct Cell){x, y - 1, false};
            maze[((y - 1) * (MAZE_WIDTH + 1)) + x] = ' '; // Create walkway
        }
        if (y < MAZE_HEIGHT - 2 && maze[((y + 2) * (MAZE_WIDTH + 1)) + x] == '#') {
            cells[cellCount++] = (struct Cell){x, y + 1, false};
            maze[((y + 1) * (MAZE_WIDTH + 1)) + x] = ' '; // Create walkway
        }
    }

    // Set end position (MAZE_HEIGHT-1, MAZE_WIDTH-2) as a walkway
    maze[(MAZE_HEIGHT - 1) * (MAZE_WIDTH + 1) + (MAZE_WIDTH - 2)] = ' ';
}

/*
Name: Ethan E
Date: 9/19/24
Description: Custom read function that outputs a generated ASCII maze
*/
static ssize_t ethan_read(struct file *file, char __user *buf, size_t count, loff_t *pos) {
    char *maze;
    ssize_t len; 

    if (*pos > 0) return 0; // Prevent reading it many times

    // Allocate memory
    maze = kmalloc((MAZE_WIDTH * MAZE_HEIGHT) + MAZE_HEIGHT + 1, GFP_KERNEL); // Extra space for newlines and null terminator
    if (!maze) return -ENOMEM;

    // Generate the maze
    generate_maze(maze);

    maze[MAZE_WIDTH * MAZE_HEIGHT + MAZE_HEIGHT] = '\0'; // Null terminate the maze

    // Copy it to user buffer
    len = simple_read_from_buffer(buf, count, pos, maze, MAZE_WIDTH * MAZE_HEIGHT + MAZE_HEIGHT);

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
