#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h> // For copy_to_user
#include <linux/slab.h> // For kmalloc and kfree
#include <linux/random.h> // For prandom_u32
#include <linux/timekeeping.h> // For ktime_get_real_ts64()
#include <linux/fs.h> // For file_operations

#define PROC_NAME "ethan_maze"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple module that generates an ASCII maze in the proc file system using Kruskal's algorithm");
MODULE_AUTHOR("Ethan E");

/* Forward declaration of the file operations structure */
static const struct file_operations ethan_proc_ops;

struct Cell {
    int x, y;
};

struct Edge {
    struct Cell start;
    struct Cell end;
};

static struct Edge* edges;
static int edge_count;
static int maze_width = 30;
static int maze_height = 20;

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
    kfree(edges);
    printk(KERN_INFO "Ethan's module has been unloaded.\n");
}

/*
Name: Ethan E
Date: 9/19/24
Description: Function to generate edges for the maze.
*/
static void generate_edges(void) {
    int i, j;
    edge_count = 0;
    edges = kmalloc(sizeof(struct Edge) * (maze_width * maze_height * 2), GFP_KERNEL);
    
    for (i = 0; i < maze_height; i++) {
        for (j = 0; j < maze_width; j++) {
            if (j < maze_width - 1) {
                edges[edge_count++] = (struct Edge){{j, i}, {j + 1, i}}; // Horizontal edge
            }
            if (i < maze_height - 1) {
                edges[edge_count++] = (struct Edge){{j, i}, {j, i + 1}}; // Vertical edge
            }
        }
    }
}

/*
Name: Ethan E
Date: 9/19/24
Description: Function to find the root of a set for union-find.
*/
static int find(int *parent, int i) {
    if (parent[i] != i) {
        parent[i] = find(parent, parent[i]); // Path compression
    }
    return parent[i];
}

/*
Name: Ethan E
Date: 9/19/24
Description: Function to union two sets.
*/
static void union_sets(int *parent, int x, int y) {
    int xroot = find(parent, x);
    int yroot = find(parent, y);
    if (xroot != yroot) {
        parent[yroot] = xroot;
    }
}

/*
Name: Ethan E
Date: 9/19/24
Description: Function to generate the maze using Kruskal's algorithm.
*/
static void generate_maze(char *maze) {
    int i, *parent;

    // Initialize the maze with walls
    for (i = 0; i < maze_height; i++) {
        memset(maze + i * (maze_width + 1), '#', maze_width);
        maze[i * (maze_width + 1) + maze_width] = '\n'; // Newline at end of each row
    }
    maze[maze_width * maze_height + maze_height] = '\0'; // Null terminate

    // Generate edges
    generate_edges();

    // Initialize union-find structure
    parent = kmalloc(sizeof(int) * (maze_width * maze_height), GFP_KERNEL);
    for (i = 0; i < maze_width * maze_height; i++) {
        parent[i] = i;
    }

    // Shuffle edges
    for (i = edge_count - 1; i > 0; i--) {
        int j = prandom_u32() % (i + 1);
        struct Edge temp = edges[i];
        edges[i] = edges[j];
        edges[j] = temp;
    }

    // Apply Kruskal's algorithm
    for (i = 0; i < edge_count; i++) {
        int start_index = edges[i].start.y * maze_width + edges[i].start.x;
        int end_index = edges[i].end.y * maze_width + edges[i].end.x;
        
        int start_root = find(parent, start_index);
        int end_root = find(parent, end_index);

        if (start_root != end_root) {
            union_sets(parent, start_index, end_index);
            // Set walls to spaces in maze
            maze[edges[i].start.y * (maze_width + 1) + edges[i].start.x] = ' ';
            maze[edges[i].end.y * (maze_width + 1) + edges[i].end.x] = ' ';
        }
    }

    // Create entrance and exit
    maze[0 * (maze_width + 1) + 1] = ' '; // Entrance at (0, 1)
    maze[(maze_height - 1) * (maze_width + 1) + (maze_width - 2)] = ' '; // Exit at (height-1, width-2)

    kfree(parent);
}

/*
Name: Ethan E
Date: 9/19/24
Description: Custom read function that outputs the generated ASCII maze.
*/
static ssize_t ethan_read(struct file *file, char __user *buf, size_t count, loff_t *pos) {
    char *maze;
    ssize_t len; 

    if (*pos > 0) return 0; // Prevent reading it many times

    // Allocate memory for maze
    maze = kmalloc((maze_width * maze_height) + maze_height + 1, GFP_KERNEL);
    if (!maze) return -ENOMEM;

    // Generate the maze
    generate_maze(maze);

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
