#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h> // For copy_to_user
#include <linux/slab.h> // For kmalloc and kfree
#include <linux/random.h> // For prandom_u32
#include <linux/timekeeping.h> // For ktime_get_real_ts64()
#include <linux/fs.h> // For file_operations
#include <linux/list.h> // For linked list

#define PROC_NAME "ethan_maze"
#define MAZE_WIDTH 30
#define MAZE_HEIGHT 20

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple module that generates an ASCII maze in the proc file system");
MODULE_AUTHOR("Ethan E");

/* Forward declaration of the file operations structure */
static const struct file_operations ethan_proc_ops;

struct cell {
    int x;
    int y;
    struct list_head list;
};

static struct list_head walls;

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
    char *maze;
    int i, j, random_index;
    ssize_t len; 

    if (*pos > 0) return 0; // Prevent reading it many times

    // Allocate memory for the maze
    maze = kmalloc((MAZE_WIDTH * MAZE_HEIGHT) + MAZE_HEIGHT + 1, GFP_KERNEL); // Extra space for newlines and null terminator
    if (!maze) return -ENOMEM;

    // Seed
    ktime_get_real_ts64(&ts);
    prandom_seed(ts.tv_nsec);

    // Initialize the maze with walls
    for (i = 0; i < MAZE_HEIGHT; i++) {
        for (j = 0; j < MAZE_WIDTH; j++) {
            maze[i * (MAZE_WIDTH + 1) + j] = '#'; // Set walls
        }
        maze[i * (MAZE_WIDTH + 1) + MAZE_WIDTH] = '\n'; // Add newline at end of each row
    }

    // Set entrance (1, 1) and exit (MAZE_HEIGHT-2, MAZE_WIDTH-2) as walkways
    maze[1 * (MAZE_WIDTH + 1) + 1] = ' ';
    maze[(MAZE_HEIGHT - 2) * (MAZE_WIDTH + 1) + (MAZE_WIDTH - 2)] = ' ';

    // Initialize walls list for Prim's algorithm
    INIT_LIST_HEAD(&walls);

    // Add initial walls adjacent to the entrance
    maze[0 * (MAZE_WIDTH + 1) + 1] = ' '; // Open entrance above
    maze[1 * (MAZE_WIDTH + 1) + 0] = ' '; // Open left of entrance
    maze[1 * (MAZE_WIDTH + 1) + 2] = ' '; // Open right of entrance

    // Add the walls to the list
    list_add(&((struct cell) {1, 0, LIST_HEAD_INIT(walls)}).list, &walls); // left of entrance
    list_add(&((struct cell) {1, 2, LIST_HEAD_INIT(walls)}).list, &walls); // right of entrance
    list_add(&((struct cell) {0, 1, LIST_HEAD_INIT(walls)}).list, &walls); // above entrance

    // Prim's algorithm to carve paths
    while (!list_empty(&walls)) {
        struct cell *current_wall;
        struct list_head *pos;

        // Pick a random wall from the list
        random_index = prandom_u32() % (int)list_size(&walls);
        pos = walls.next;
        for (i = 0; i < random_index; i++) {
            pos = pos->next;
        }

        current_wall = list_entry(pos, struct cell, list);

        // Check if it can be a walkway
        if (current_wall->x > 0 && current_wall->x < MAZE_HEIGHT - 1 &&
            current_wall->y > 0 && current_wall->y < MAZE_WIDTH - 1) {
            if (maze[(current_wall->x - 1) * (MAZE_WIDTH + 1) + current_wall->y] == ' ' &&
                maze[(current_wall->x + 1) * (MAZE_WIDTH + 1) + current_wall->y] == '#') {
                maze[current_wall->x * (MAZE_WIDTH + 1) + current_wall->y] = ' ';
                // Add new walls
                if (current_wall->x > 1) {
                    list_add(&((struct cell) {current_wall->x - 1, current_wall->y, LIST_HEAD_INIT(walls)}).list, &walls);
                }
                if (current_wall->x < MAZE_HEIGHT - 2) {
                    list_add(&((struct cell) {current_wall->x + 1, current_wall->y, LIST_HEAD_INIT(walls)}).list, &walls);
                }
                if (current_wall->y > 1) {
                    list_add(&((struct cell) {current_wall->x, current_wall->y - 1, LIST_HEAD_INIT(walls)}).list, &walls);
                }
                if (current_wall->y < MAZE_WIDTH - 2) {
                    list_add(&((struct cell) {current_wall->x, current_wall->y + 1, LIST_HEAD_INIT(walls)}).list, &walls);
                }
            }
        }

        // Remove the wall from the list
        list_del(pos);
    }

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
