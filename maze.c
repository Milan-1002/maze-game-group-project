#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

// ANSI Color codes
#define ANSI_RESET "\x1b[0m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"


// Constants for maze configuration
#define MAX_ROWS 60
#define MAX_COLS 60
#define PLAYER 'P'
#define WALL '#'
#define PATH ' '
#define EXIT 'E'

// Constants for arrow key codes
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define SPECIAL_KEY 224
#define ESC 27

// Structure to hold game state
typedef struct {
    char maze[MAX_ROWS][MAX_COLS];
    int playerRow;
    int playerCol;
    int rows;
    int cols;
    time_t startTime;
    time_t endTime;
} MazeGame;

// Function declarations
void displayWelcome(void);
int chooseLevel(void);
int loadMaze(MazeGame *game, const char *filename);
void displayMaze(const MazeGame *game);
int handleMovement(MazeGame *game, int direction);
void displayGameOver(const MazeGame *game);
void clearScreen(void);
void setCursorPosition(int x, int y);
void hideCursor(void);

int main() {
    MazeGame game;
    int gameOver = 0;
    const char *mazeFiles[] = {
        "level1_maze.txt",
        "level2_maze.txt",
        "level3_maze.txt",
        "level4_maze.txt",
        "level5_maze.txt"
    };

    // Hide the console cursor for better visibility
    hideCursor();

    // Display welcome message
    displayWelcome();

    // Get level choice from user
    int level = chooseLevel();
    if (level < 1 || level > 5) {
        printf("Invalid level selection. Exiting...\n");
        return 1;
    }

    // Load maze from file
    if (!loadMaze(&game, mazeFiles[level - 1])) {
        printf("Error loading maze file!\n");
        return 1;
    }


    // Record start time
    game.startTime = time(NULL);

    // Main game loop
    while (!gameOver) {
        clearScreen();
        displayMaze(&game);

        // Get player move (arrow keys generate two characters)
        int key = getch();
        if (key == SPECIAL_KEY) {
            key = getch(); // Get the actual arrow key code
            gameOver = handleMovement(&game, key);
        } else if (key == ESC) {
            gameOver = 1; // Exit on ESC key
        }

        // Check if player reached exit
        if (gameOver) {
            game.endTime = time(NULL);
            clearScreen();
            displayGameOver(&game);
        }
    }

    // Reset terminal colors
    printf(ANSI_RESET);

    return 0;
}

void displayWelcome(void) {
    clearScreen();
    printf("********************************\n");
    printf("*        MAZE OUT GAME         *\n");
    printf("********************************\n\n");
    printf("Controls:\n");
    printf("Move Up\n");
    printf("Move Down\n");
    printf("Move Left\n");
    printf("Move Right\n");
    printf("ESC - Quit Game\n\n");
    printf("Press any key to continue...");
    getch();
}

int chooseLevel(void) {
    int choice;
    clearScreen();
    printf("Choose your level:\n");
    printf("1. Level 1\n");
    printf("2. Level 2\n");
    printf("3. Level 3\n");
    printf("4. Level 4\n");
    printf("5. Level 5\n");
    printf("Enter your choice (1-5): ");
    scanf("%d", &choice);
    return choice;
}

int loadMaze(MazeGame *game, const char *filename) {
    FILE *fp = fopen(filename, "r");

    if (!fp) return 0;

    // Read maze dimensions
    fscanf(fp, "%d %d", &game->rows, &game->cols);

    // Read maze layout
    char c;
    fscanf(fp, "%c", &c); // Consume newline

    for (int i = 0; i < game->rows; i++) {
        for (int j = 0; j < game->cols; j++) {
            fscanf(fp, "%c", &game->maze[i][j]);
            if (game->maze[i][j] == PLAYER) {
                game->playerRow = i;
                game->playerCol = j;
            }
        }
        fscanf(fp, "%c", &c); // Consume newline
    }

    fclose(fp);
    return 1;
}

void displayMaze(const MazeGame *game) {
    printf("\nUse Arrow Keys to move. Press ESC to quit.\n\n");

    for (int i = 0; i < game->rows; i++) {
        for (int j = 0; j < game->cols; j++) {
            if (game->maze[i][j] == WALL) {
                printf(ANSI_COLOR_RED "%c" ANSI_RESET, WALL);
            } else if (game->maze[i][j] == PLAYER) {
                printf(ANSI_COLOR_GREEN "%c" ANSI_RESET, PLAYER);
            } else if (game->maze[i][j] == EXIT) {
                printf(ANSI_COLOR_YELLOW "%c" ANSI_RESET, EXIT);
            } else {
                printf("%c", game->maze[i][j]);
            }
        }
        printf("\n");
    }
}

int handleMovement(MazeGame *game, int direction) {
    int newRow = game->playerRow;
    int newCol = game->playerCol;

    // Calculate new position based on arrow key
    switch (direction) {
        case KEY_UP: newRow--; break;
        case KEY_DOWN: newRow++; break;
        case KEY_LEFT: newCol--; break;
        case KEY_RIGHT: newCol++; break;
        default: return 0;
    }

    // Check if new position is valid
    if (newRow < 0 || newRow >= game->rows || 
        newCol < 0 || newCol >= game->cols || 
        game->maze[newRow][newCol] == WALL) {
        return 0;
    }

    // Check if player reached exit
    if (game->maze[newRow][newCol] == EXIT) {
        return 1;
    }

    // Update player position
    game->maze[game->playerRow][game->playerCol] = PATH;
    game->maze[newRow][newCol] = PLAYER;
    game->playerRow = newRow;
    game->playerCol = newCol;

    return 0;
}

void displayGameOver(const MazeGame *game) {
    double timeTaken = difftime(game->endTime, game->startTime);
    printf("\n********************************\n");
    printf("*          GAME OVER!          *\n");
    printf("********************************\n");
    printf("\nTime taken: %.2f seconds\n", timeTaken);
    printf("\nPress any key to exit...");
    getch();
}

void clearScreen(void) {
    system("cls");
}

void setCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor(void) {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}
