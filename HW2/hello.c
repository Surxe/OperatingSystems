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

/* Union-Find structure for Kruskal's algorithm */
struct UnionFind {
    int *parent;
    int *rank;
    int size;
};

/*
Name: Ethan E
Date: 9/19/24
Description: Initializes the Union-Find data structure.
*/
struct UnionFind *create_union_find(int size) {
    struct UnionFind *uf = kmalloc(sizeof(struct UnionFind), GFP_KERNEL);
    uf->parent = kmalloc(size * sizeof(int), GFP_KERNEL);
    uf->rank = kmalloc(size * sizeof(int), GFP_KERNEL);
    uf->size = size;

    for (int i = 0; i < size; i++) {
        uf->parent[i] = i;
        uf->rank[i] = 0;
    }
    return uf;
}

/*
Name: Ethan E
Date: 9/19/24
Description: Finds the root of an element.
*/
int find(struct UnionFind *uf, int x) {
    if (uf->parent[x] != x)
        uf->parent[x] = find(uf, uf->parent[x]); // Path compression
    return uf->parent[x];
}

/*
Name: Ethan E
Date: 9/19/24
Description: Unions two sets.
*/
void union_sets(struct UnionFind *uf, int x, int y) {
    int rootX = find(uf, x);
    int rootY = find(uf, y);
    if (rootX != rootY) {
        if (uf->rank[rootX] > uf->rank[rootY]) {
            uf->parent[rootY] = rootX;
        } else if (uf->rank[rootX] < uf->rank[rootY]) {
            uf->parent[rootX] = rootY;
        } else {
            uf->parent[rootY] = rootX;
            uf->rank[rootX]++;
        }
    }
}

/*
Name: Ethan E
Date: 9/19/24
Description: Generates the maze using Kruskal's algorithm.
*/
void generate_maze(char *maze) {
    struct UnionFind *uf = create_union_find((MAZE_WIDTH / 2) * (MAZE_HEIGHT / 2));
    int walls[MAZE_WIDTH * MAZE_HEIGHT][4]; // Store wall positions
    int wall_count = 0;

    // Initialize the maze with walls
    for (int i = 0; i < MAZE_HEIGHT; i++) {
        for (int j = 0; j < MAZE_WIDTH; j++) {
            maze[i * (MAZE_WIDTH + 1) + j] = '#';
        }
    }

    // Create walls between cells
    for (int i = 0; i < MAZE_HEIGHT / 2; i++) {
        for (int j = 0; j < MAZE_WIDTH / 2; j++) {
            if (j < (MAZE_WIDTH / 2) - 1) {
                walls[wall_count][0] = 2 * i + 1;
                walls[wall_count][1] = 2 * j + 2; // Right wall
                walls[wall_count][2] = (i * (MAZE_WIDTH / 2) + j);
                walls[wall_count][3] = (i * (MAZE_WIDTH / 2) + j + 1);
                wall_count++;
            }
            if (i < (MAZE_HEIGHT / 2) - 1) {
                walls[wall_count][0] = 2 * i + 2;
                walls[wall_count][1] = 2 * j + 1; // Down wall
                walls[wall_count][2] = (i * (MAZE_WIDTH / 2) + j);
                walls[wall_count][3] = ((i + 1) * (MAZE_WIDTH / 2) + j);
                wall_count++;
            }
        }
    }

    // Shuffle walls
    for (int i = wall_count - 1; i > 0; i--) {
        int j = prandom_u32() % (i + 1);
        int temp[4];
        memcpy(temp, walls[i], sizeof(temp));
        memcpy(walls[i], walls[j], sizeof(temp));
        memcpy(walls[j], temp, sizeof(temp));
    }

    // Create passages
    for (int i = 0; i < wall_count; i++) {
        int wall = walls[i][0] * MAZE_WIDTH + walls[i][1];
        int cell1 = walls[i][2];
        int cell2 = walls[i][3];

        if (find(uf, cell1) != find(uf, cell2)) {
            maze[wall] = ' '; // Remove wall
            union_sets(uf, cell1, cell2);
        }
    }

    // Set entrance and exit
    maze[1 * (MAZE_WIDTH + 1)] = ' '; // Entrance
    maze[(MAZE_HEIGHT - 2) * (MAZE_WIDTH + 1) + (MAZE_WIDTH - 2)] = ' '; // Exit

    kfree(uf->parent);
    kfree(uf->rank);
    kfree(uf);
}

/*
Name: Ethan E
Date: 9/19/24
Description: Custom read function that outputs a generated ASCII maze
*/
static ssize_t ethan_read(struct file *file, char __user *buf, size_t count, loff_t *pos) {
    struct timespec64 ts;
    char *maze;
    ssize_t len; 

    if (*pos > 0) return 0; // Prevent reading it many times

    // Allocate memory
    maze = kmalloc((MAZE_WIDTH * MAZE_HEIGHT) + MAZE_HEIGHT + 1, GFP_KERNEL); // Extra space for newlines and null terminator
    if (!maze) return -ENOMEM;

    // Seed
    ktime_get_real_ts64(&ts);
    prandom_seed(ts.tv_nsec);

    // Generate the maze
    generate_maze(maze);

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
