#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h> // For copy_to_user
#include <linux/slab.h> // For kmalloc and kfree
#include <linux/random.h> // For prandom_u32
#include <linux/timekeeping.h> // For ktime_get_real_ts64()
#include <linux/fs.h> // For file_operations

#define PROC_NAME "ethan_maze"
#define WALL '#'
#define SPACE ' '

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple module that generates an ASCII maze in the proc file system");
MODULE_AUTHOR("Ethan E");

/* Forward declaration of the file operations structure */
static const struct file_operations ethan_proc_ops;

/* Maze dimensions */
#define MAZE_WIDTH 80
#define MAZE_HEIGHT 28

/* Maze cell states */
#define CELL_WALL 0
#define CELL_PATH 1

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
Description: Prim's algorithm to generate an ASCII maze
*/
static void generate_maze(char *maze_buffer)
{
    int maze_array[MAZE_WIDTH][MAZE_HEIGHT]; // Changed name to maze_array to avoid conflict
    int x, y;
    int i, j, k, nx, ny;
    int dx[] = {1, 0, -1, 0};
    int dy[] = {0, 1, 0, -1};

    /* Allocate memory for walls array to avoid stack overflow */
    int (*walls)[2] = kmalloc_array(MAZE_WIDTH * MAZE_HEIGHT, sizeof(int[2]), GFP_KERNEL);
    if (!walls) {
        printk(KERN_ERR "Failed to allocate memory for walls.\n");
        return;
    }

    int num_walls = 0; // Moved up to fix C90 issue

    /* Initialize maze with walls */
    for (i = 0; i < MAZE_WIDTH; i++) {
        for (j = 0; j < MAZE_HEIGHT; j++) {
            maze_array[i][j] = CELL_WALL;
        }
    }

    /* Start with an initial cell */
    x = (MAZE_WIDTH / 2) * 2;
    y = (MAZE_HEIGHT / 2) * 2;
    maze_array[x][y] = CELL_PATH;

    /* Add initial walls */
    for (k = 0; k < 4; k++) {
        nx = x + dx[k] * 2;
        ny = y + dy[k] * 2;
        if (nx > 0 && ny > 0 && nx < MAZE_WIDTH && ny < MAZE_HEIGHT) {
            maze_array[nx][ny] = CELL_PATH;
            walls[num_walls][0] = nx;
            walls[num_walls][1] = ny;
            num_walls++;
        }
    }

    /* Generate the maze using Prim's algorithm */
    while (num_walls > 0) {
        int wall_index = prandom_u32() % num_walls;
        x = walls[wall_index][0];
        y = walls[wall_index][1];
        
        /* Remove wall from the list */
        walls[wall_index][0] = walls[num_walls - 1][0];
        walls[wall_index][1] = walls[num_walls - 1][1];
        num_walls--;

        /* Check neighbors */
        for (k = 0; k < 4; k++) {
            nx = x + dx[k] * 2;
            ny = y + dy[k] * 2;
            if (nx > 0 && ny > 0 && nx < MAZE_WIDTH && ny < MAZE_HEIGHT) {
                if (maze_array[nx][ny] == CELL_WALL) {
                    int cx = x + dx[k];
                    int cy = y + dy[k];
                    if (maze_array[cx][cy] == CELL_PATH) {
                        maze_array[nx][ny] = CELL_PATH;
                        maze_array[x][y] = CELL_PATH;

                        /* Add new walls */
                        int l; // Declare outside of loop to avoid mixed declarations
                        for (l = 0; l < 4; l++) {
                            int nnx = nx + dx[l] * 2;
                            int nny = ny + dy[l] * 2;
                            if (nnx > 0 && nny > 0 && nnx < MAZE_WIDTH && nny < MAZE_HEIGHT) {
                                if (maze_array[nnx][nny] == CELL_WALL) {
                                    walls[num_walls][0] = nnx;
                                    walls[num_walls][1] = nny;
                                    num_walls++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /* Convert maze array to character array */
    for (i = 0; i < MAZE_WIDTH; i++) {
        for (j = 0; j < MAZE_HEIGHT; j++) {
            maze_buffer[i * (MAZE_HEIGHT + 1) + j] = (maze_array[i][j] == CELL_PATH) ? SPACE : WALL;
        }
        maze_buffer[i * (MAZE_HEIGHT + 1) + MAZE_HEIGHT] = '\n'; // Add newline at end of each row
    }
    maze_buffer[MAZE_WIDTH * (MAZE_HEIGHT + 1)] = '\0'; // Terminate the maze

    kfree(walls); // Free the dynamically allocated memory for walls
}

/*
Name: Ethan E
Date: 9/18/24
Description: Custom read function that outputs a generated ASCII maze
*/
static ssize_t ethan_read(struct file *file, char __user *buf, size_t count, loff_t *pos) {
    struct timespec64 ts;
    char *maze_buffer;
    ssize_t len;

    if (*pos > 0) return 0; // Prevent reading it many times

    // Allocate memory
    maze_buffer = kmalloc(MAZE_WIDTH * (MAZE_HEIGHT + 1) + 1, GFP_KERNEL);
    if (!maze_buffer) return -ENOMEM;

    // Seed
    ktime_get_real_ts64(&ts);
    prandom_seed(ts.tv_nsec);

    // Generate the maze
    generate_maze(maze_buffer);

    // Copy it to user buffer
    len = simple_read_from_buffer(buf, count, pos, maze_buffer, MAZE_WIDTH * (MAZE_HEIGHT + 1));

    kfree(maze_buffer); // Free the memory
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
