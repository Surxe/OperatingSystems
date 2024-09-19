#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/time.h>

#define PROC_NAME "ethan_maze"
#define MAZE_WIDTH 80
#define MAZE_HEIGHT 28

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple module that generates an ASCII maze using Prim's algorithm in the /proc file system");
MODULE_AUTHOR("Ethan Eisenga");

/*Name: Ethan Eisenga
  Date: 9/18/2024
  Description: Custom read function that outputs a generated ASCII maze using Prim's algorithm.
*/
static ssize_t ethan_read(struct file *file, char __user *buf, size_t count, loff_t *pos)
{
    char *maze;
    int i, j, x, y;
    ssize_t len;

    if (*pos > 0) return 0; // Prevent multiple reads

    // Allocate memory for the maze
    maze = kmalloc(MAZE_WIDTH * MAZE_HEIGHT + 1, GFP_KERNEL);
    if (!maze) return -ENOMEM;

    // Initialize the maze with walls
    for (i = 0; i < MAZE_HEIGHT; i++) {
        for (j = 0; j < MAZE_WIDTH; j++) {
            maze[i * MAZE_WIDTH + j] = '#';
        }
    }

    // Seed the random number generator
    struct timespec ts;
    getnstimeofday(&ts);
    prandom_seed(ts.tv_nsec);

    // Prim's algorithm initialization
    int maze_x = prandom_u32() % MAZE_WIDTH;
    int maze_y = prandom_u32() % MAZE_HEIGHT;
    maze[maze_y * MAZE_WIDTH + maze_x] = ' ';
    struct point {
        int x, y;
    };
    struct point *walls = kmalloc(MAZE_WIDTH * MAZE_HEIGHT * sizeof(struct point), GFP_KERNEL);
    int walls_count = 0;

    // Add the initial cell's walls to the list
    if (maze_x > 1) {
        walls[walls_count++] = (struct point) { maze_x - 1, maze_y };
    }
    if (maze_x < MAZE_WIDTH - 2) {
        walls[walls_count++] = (struct point) { maze_x + 1, maze_y };
    }
    if (maze_y > 1) {
        walls[walls_count++] = (struct point) { maze_x, maze_y - 1 };
    }
    if (maze_y < MAZE_HEIGHT - 2) {
        walls[walls_count++] = (struct point) { maze_x, maze_y + 1 };
    }

    // Generate the maze using Prim's algorithm
    while (walls_count > 0) {
        int wall_index = prandom_u32() % walls_count;
        struct point wall = walls[wall_index];

        if (wall.x >= 1 && wall.x < MAZE_WIDTH - 1 && wall.y >= 1 && wall.y < MAZE_HEIGHT - 1) {
            int adjacent_open = 0;
            if (maze[wall.y * MAZE_WIDTH + wall.x - 1] == ' ') adjacent_open++;
            if (maze[wall.y * MAZE_WIDTH + wall.x + 1] == ' ') adjacent_open++;
            if (maze[(wall.y - 1) * MAZE_WIDTH + wall.x] == ' ') adjacent_open++;
            if (maze[(wall.y + 1) * MAZE_WIDTH + wall.x] == ' ') adjacent_open++;

            if (adjacent_open == 1) {
                maze[wall.y * MAZE_WIDTH + wall.x] = ' ';
                if (wall.x > 1 && maze[wall.y * MAZE_WIDTH + wall.x - 2] == '#') {
                    walls[walls_count++] = (struct point) { wall.x - 2, wall.y };
                }
                if (wall.x < MAZE_WIDTH - 2 && maze[wall.y * MAZE_WIDTH + wall.x + 2] == '#') {
                    walls[walls_count++] = (struct point) { wall.x + 2, wall.y };
                }
                if (wall.y > 1 && maze[(wall.y - 2) * MAZE_WIDTH + wall.x] == '#') {
                    walls[walls_count++] = (struct point) { wall.x, wall.y - 2 };
                }
                if (wall.y < MAZE_HEIGHT - 2 && maze[(wall.y + 2) * MAZE_WIDTH + wall.x] == '#') {
                    walls[walls_count++] = (struct point) { wall.x, wall.y + 2 };
                }

                walls[wall_index] = walls[--walls_count];
            } else {
                walls[wall_index] = walls[--walls_count];
            }
        } else {
            walls[wall_index] = walls[--walls_count];
        }
    }

    // Null-terminate the maze
    maze[MAZE_WIDTH * MAZE_HEIGHT] = '\0';

    // Copy the maze to the user buffer
    len = simple_read_from_buffer(buf, count, pos, maze, MAZE_WIDTH * MAZE_HEIGHT);

    kfree(walls); // Free the allocated memory for walls
    kfree(maze);  // Free the allocated memory for the maze
    return len;
}

/* Struct to define proc operations */
static const struct proc_ops ethan_proc_ops = {
    .proc_read = ethan_read,  // Use the new proc_read field
};

/*Name: Ethan Eisenga
  Date: 9/18/2024
  Description: Custom initialization function for the kernel module.
*/
static int ethan_init(void)
{
    proc_create(PROC_NAME, 0666, NULL, &ethan_proc_ops);
    printk(KERN_INFO "Ethan's module has been loaded.\n");
    return 0;
}

/*Name: Ethan Eisenga
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
