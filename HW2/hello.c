#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/timekeeping.h>
#include <linux/fs.h>

#define PROC_NAME "ethan_maze"
#define MAZE_WIDTH 30
#define MAZE_HEIGHT 20

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple module that generates an ASCII maze in the proc file system");
MODULE_AUTHOR("Ethan E");

// Define the Cell structure
struct Cell {
    int x;
    int y;
    int visited; // Change to int for C90 compatibility
};

static void generate_maze(char *maze) {
    struct Cell *cells;
    int cellCount = 0;
    int i, j, randIndex;
    struct Cell selectedCell;

    // Allocate memory for cells
    cells = kmalloc(MAZE_WIDTH * MAZE_HEIGHT * sizeof(struct Cell), GFP_KERNEL);
    if (!cells) return;

    // Initialize the maze with walls
    for (i = 0; i < MAZE_HEIGHT; i++) {
        for (j = 0; j < MAZE_WIDTH; j++) {
            maze[i * (MAZE_WIDTH + 1) + j] = '#'; // Set walls
        }
        maze[i * (MAZE_WIDTH + 1) + MAZE_WIDTH] = '\n'; // Add newline
    }

    // Start with the initial cell
    cells[cellCount++] = (struct Cell){1, 1, 0}; // Use 0 for "not visited"
    maze[1 * (MAZE_WIDTH + 1) + 1] = ' '; // Start point

    // Randomly add walls to the list
    while (cellCount > 0) {
        randIndex = prandom_u32() % cellCount;
        selectedCell = cells[randIndex];
        int x = selectedCell.x;
        int y = selectedCell.y;
        cells[randIndex] = cells[--cellCount]; // Remove from list

        // Add neighbors
        if (x > 1 && maze[(y * (MAZE_WIDTH + 1)) + (x - 2)] == '#') {
            cells[cellCount++] = (struct Cell){x - 1, y, 0};
            maze[(y * (MAZE_WIDTH + 1)) + (x - 1)] = ' '; // Create walkway
        }
        if (x < MAZE_WIDTH - 2 && maze[(y * (MAZE_WIDTH + 1)) + (x + 2)] == '#') {
            cells[cellCount++] = (struct Cell){x + 1, y, 0};
            maze[(y * (MAZE_WIDTH + 1)) + (x + 1)] = ' '; // Create walkway
        }
        if (y > 1 && maze[((y - 2) * (MAZE_WIDTH + 1)) + x] == '#') {
            cells[cellCount++] = (struct Cell){x, y - 1, 0};
            maze[((y - 1) * (MAZE_WIDTH + 1)) + x] = ' '; // Create walkway
        }
        if (y < MAZE_HEIGHT - 2 && maze[((y + 2) * (MAZE_WIDTH + 1)) + x] == '#') {
            cells[cellCount++] = (struct Cell){x, y + 1, 0};
            maze[((y + 1) * (MAZE_WIDTH + 1)) + x] = ' '; // Create walkway
        }
    }

    // Set end position (MAZE_HEIGHT-1, MAZE_WIDTH-2) as a walkway
    maze[(MAZE_HEIGHT - 1) * (MAZE_WIDTH + 1) + (MAZE_WIDTH - 2)] = ' ';

    // Free allocated memory
    kfree(cells);
}
