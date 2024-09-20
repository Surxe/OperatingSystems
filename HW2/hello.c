/*
Name: Ethan E
Date: 9/19/24
Description: Function to generate a maze using Prim's algorithm
*/
static void generate_maze(char *maze) {
    struct Cell *cells;
    int cellCount = 0;
    int i, j, randIndex;
    struct Cell selectedCell; // Declare here

    // Allocate memory for cells
    cells = kmalloc(MAZE_WIDTH * MAZE_HEIGHT * sizeof(struct Cell), GFP_KERNEL);
    if (!cells) return; // Handle memory allocation failure

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
        selectedCell = cells[randIndex]; // Assign to the declared variable
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

    // Free allocated memory
    kfree(cells);
}
