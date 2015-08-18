/*************************************************************************
    > File Name: box.c
    > Author: chknight
    > Mail: heatch@163.com 
    > Created Time: Wed 05 Aug 2015 09:51:49 AM AEST
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char **block; /*block is usced to store the +*/
char **cell;  /*block cell is used to store the */
char **vWall; /*vWall is used to store the |*/
char **hWall; /*hWall is used to store the -*/
int **cellCount; /*cellCountis used to store the count number*/
char* users;  /*used to store the character of the users*/
int * winCells; /*used to stored every number get how many cells*/
int height; /*height is the input of height*/
int width; /*width is the input of the width*/
int playNumber; /*playNumber is the number of the player*/
int currentUser; /*used to store the user played now*/
int finishedCell; /*used to store how many cells is occupyed*/
int read_number_from_string(char* string); 
int change_string_to_number(const char* string);
int initialize(char**, int);
char** init_pointer(int width, int height, char type);
void print_map();
int get_number();
char get_type();
int getX();
int getY();
int check_command(int, int, char);
int play_game();
int get_command(int*, int*, char*);
void add_block(int, int, char);
void clear();
int update_cell(int, int, char);
int read_from_file(char*);
int get_current_user(FILE*);
int set_blocks(FILE*);
int set_cells(FILE*);
int set_row(FILE*, char**, int, int, char);
int store();
void stroe_in_file(FILE*);
void output_row(FILE*, char**, int, int);
void print_cells(FILE* file);
void print_cell_row(FILE*, int);

struct List {
    char context;
    struct List* next;
};


int main(int argc, char** argv) {
    int status = 0; 
    if (argc != 4 && argc != 5) {
        fprintf(stderr, "Usage: boxes height width playercount [filename]\n");
        return 1;
    }
    status = initialize(argv, argc);
    if (status == 2) {
        fprintf(stderr, "Invalid grid dimensions\n");
        return status;
    } else if (status == 3) {
        fprintf(stderr, "Invalid player count\n");
        return status;
    } else if (status == 4) {
        fprintf(stderr, "Invalid grid file\n");
        return status;
    } else if (status == 5) {
        fprintf(stderr, "Error reading grid contents\n");
        return status;
    }
    status = play_game();
    if (status == 6) {
        fprintf(stderr, "End of user input\n");
    }
    return status;
}

/*To get a number from the string
*/
int read_number_from_string(char* string) {
    int result = 0;
    while(*string != ' ' && *string != '\n') {
        if (*string >= '0' && *string <= '9') {
            result *= 10;
            result += (*string) - '0';
            string++;
        } else {
            return -1;
        }
    }
    string++;
    return result;
}


/*To change a string to a number
 */
int change_string_to_number(const char* string) {
    int index = 0;
    int result = 0;
    if (string == NULL) {
        return -1;
    }
    while (string[index] != '\0') {
        if (string[index] >= '0' && string[index] <= '9') {
            result *= 10;
            result += string[index] - '0';
            index++;
        } else {
            return -1;
        }
    }
    return result;
}

/*check the initialize input in command line whether satisfy the require ment
 */
int check_the_command_input() {
    if (height == -1 || width == -1) {
        return 2;
    } else if (height > 999 || height <= 1) {
        return 2;
    } else if (width > 999 || width <= 1) {
        return 2;
    } else if (playNumber > 100 || playNumber <= 1 || playNumber == -1) {
        return 3;
    } else {
        return 0;
    }
}

/*initialize all the variable
 */
int initialize(char** argv, int argc) {
    int i;
    int j;
    height = change_string_to_number(argv[1]);
    width = change_string_to_number(argv[2]);
    playNumber = change_string_to_number(argv[3]);

    if (check_the_command_input() == 2) {
        return 2;
    } else if (check_the_command_input() == 3) {
        return 3;
    }
    currentUser = 0;
    users = (char*)malloc(sizeof(char) * playNumber);
    winCells = (int*)malloc(sizeof(int) * playNumber);
    for (i = 0; i < playNumber; ++i) {
        users[i] = 'A' + i;
        winCells[i] = 0;
    }
    finishedCell = 0;
    hWall = init_pointer(width, height + 1, ' ');
    vWall = init_pointer(width + 1, height, ' ');
    block = init_pointer(width + 1, height + 1, '+');
    cell = (char**)malloc(sizeof(char*) * height);
    cellCount = (int**)malloc(sizeof(int*) * height);
    for (i = 0; i < height; ++i) {
        cell[i] = (char*)malloc(sizeof(char) * (width));
        cellCount[i] = (int*)malloc(sizeof(int) * width);
        for (j = 0; j < width; ++j) {
            cellCount[i][j] = 0;
            cell[i][j] = ' ';
        }
    }
    if (argc == 5) {
        int status = read_from_file(argv[4]);
        if (status != 0) {
            return status;
        }
    }
    print_map();
    return 0;
}

/*ininit a pointer char***/
char** init_pointer(int width, int height, char type) {
    int row, col;
    char** pointer = (char**)malloc(sizeof(char*) * height);
    for (row = 0; row < height; ++row) {
        pointer[row] = (char*)malloc(sizeof(char) * width);
        for (col = 0; col < width; ++col) {
            pointer[row][col] = type; 
        }
    }
    return pointer;
}

/*Print the map to the screen
 */
void print_map() {
    int i, j;
    for (i = 0; i < height * 2 + 1; i++) {
        if (i % 2 == 0) {
            for (j = 0; j < width * 2 + 1; j++) {
                if (j % 2 == 0) {
                    printf("%c", block[i / 2][j / 2]);
                } else {
                    printf("%c", hWall[i / 2][j / 2]);
                }
            }
            printf("\n");
        } else {
            for (j = 0; j < width * 2 + 1; j++) {
                if (j % 2 == 0) {
                    printf("%c", vWall[i / 2][j / 2]);
                } else {
                    printf("%c", cell[i / 2][j / 2]);
                }
            }
            printf("\n");
        }
    }
}

/*receive the command given by the user
 */
int get_command(int *x, int *y, char *type) {

    *y = get_number(0);
    if (*y == -1) {
        return 0;
    }
    if (*y == -2) {
        return 2;
    } else if (*y == -3) {
        return 3;
    }
    *x = get_number(1);
    if (*x == -1) {
        return 0;
    } else if (*y == -3) {
        return 3;
    }
    *type = get_type();
    if (*type == '\0') {
        return 0;
    }
    if (*type == 'a') {
        return 3;
    }
    if (check_command(*x, *y, *type) == 0) {
        return 0;
    }
    return 1;
}

/*get a number from the input
 */
int get_number(int order) {
    char temp;
    int result = 0;
    int index = 0;
    while((temp = getchar()) != ' ') {
        if (temp >= '0' && temp <= '9') {
            result *= 10;
            result += temp - '0';
            index++;
        } else {
            if (order == 0 && index == 0 && temp == 'w') {
                return store();
            }
			/*
			 * when the EOF is input, return -3
			 */
            if (temp == EOF) {
                return -3;
            }

	/*if the input includig some error character such as 'a', the function
	 *while return -1 as a sambol
	 */
            if (temp != '\n') {
                clear();
            }
            return -1;
        }
    }
    return result;
}

/*get the type of operation of the user
 */
char get_type() {
    char type = getchar();
    char symbol = getchar();
    if (symbol != '\n') {
        clear();
        return '\0';
    } else if (type != 'v' && type != 'h') {
        return '\0';
		/*
		 * when the EOF is ipuht. reutrn -3
		 */
    } else if (type == EOF) {
        return 'a';
    } else {
        return type;
    }
}

/*check whether the command is correct
 */
int check_command(int x, int y, char type) {
    if (type == '\0') {
        return 0;
    }
    if (x < 0 || y < 0 || x > width || y > height) {
        return 0;
    }
    if (type == 'h') {
        if (x >= width) {
            return 0;
        }
        if (hWall[y][x] == '-') {
            return 0;
        }
    } else {
        if (y >= height) {
            return 0;
        }
        if (vWall[y][x] == '|') {
            return 0;
        }
    }
    return 1;
}

/*add a block in the map
 */
void add_block(int x, int y, char type) {
    if (type == 'h') {
        hWall[y][x] = '-';
    } else {
        vWall[y][x] = '|';
    }
}

/*the loop is used to play the game
 */
int play_game() {
    int x, y;
    char type;
    int i;
    int status = 0;
    while (1) {
        printf("%c> ", users[currentUser]);
        status = get_command(&x, &y, &type);
        if (status == 1) {
            add_block(x, y, type);
            currentUser = (currentUser + update_cell(x, y, type)) % playNumber;
            print_map();
			/*printf("\n");*/
            if (finishedCell == width * height) {
                int max = 0;
                int lastOne = 0;
                for (i = 0; i < playNumber; ++i) {
                    if (winCells[i] >= max) {
                        max = winCells[i];
                        lastOne = i;
                    }
                }
				/*printf("\n");*/
                printf("Winner(s):");
                for (i = 0; i <= lastOne; ++i) {
                    if (winCells[i] == max) {
                        printf(" %c", users[i]);
                        if (i < lastOne) {
                            printf(",");
                        } else {
                            printf("\n");
                        }
                    }
                }
                break;
            }
        } else if (status == 3) {
            return 6;
        }
    }
    return 0;
}

/*if mistake input occur, read the rest char
 */
void clear() {
    char temp;
    while ((temp = getchar()) != '\n') {
    }
}

/*update the cell count
 */
int update_cell(int x, int y, char type) {
    int result = 1;

    if (type == 'h') {
        if (y - 1 >= 0) {
            cellCount[y - 1][x]++;
            if (cellCount[y - 1][x] == 4 && cellCount[y - 1][x] != ' ') {
                result = 0;
                cell[y - 1][x] = users[currentUser];
                finishedCell++;
                winCells[currentUser]++;
            }
        }
        if (y < height) {
            cellCount[y][x]++;
            if (cellCount[y][x] == 4 && cellCount[y][x] != ' ') {
                result = 0;
                cell[y][x] = users[currentUser];
                finishedCell++;
                winCells[currentUser]++;
            }
        }
    } else {
        if (x < width) {
            cellCount[y][x]++;
            if (cellCount[y][x] == 4 && cellCount[y][x] != ' ') {
                result = 0;
                finishedCell++;
                cell[y][x] = users[currentUser];
                winCells[currentUser]++;
            }
        }
        if (x - 1 >= 0) {
            cellCount[y][x - 1]++;
            if (cellCount[y][x - 1] == 4 && cellCount[y][x] != ' ') {
                result = 0;
                finishedCell++;
                cell[y][x - 1] = users[currentUser];
                winCells[currentUser]++;
            }
        }
    }
    return result;
}


/*read the information from the input file
 */
int read_from_file(char* path) {
    FILE *inputFile = fopen(path, "r");
    int status = 0;
    if (inputFile == NULL) {
        return 4;
    }
    currentUser = get_current_user(inputFile);
    if (currentUser == -1) {
        fclose(inputFile);
        return 5;
    } else if (currentUser == -2) {
        fclose(inputFile);
        return 5;
    }

    status = set_blocks(inputFile);
    if (status == 5) {
        return 5;
    }

    status = set_cells(inputFile);
    if (status == 5) {
        return 5;
    }
	

    fclose(inputFile);
    return 0;
}

/*get current user in the file
 */
int get_current_user(FILE* file) {
    int result = 0;
    char temp = '0';
    while ((temp = fgetc(file)) != '\n' && temp != EOF) {
        if (temp >= '0' && temp <= '9') {
            result *= 10;
            result += temp - '0';
        } else {
            return -1;
        }
    }
    if (result <= playNumber && result >= 1) {
        return result - 1;
    } else {
        return -2;
    }
}

/*
 * set block in the file
 */
int set_blocks(FILE* file) {
    int i;
    for (i = 0; i < height; ++i) {
        if (set_row(file, hWall, i, width, '-') == 5) {
            return 5;
        }
        if (set_row(file, vWall, i, width + 1, '|') == 5) {
            return 5;
        }
    }
    return set_row(file, hWall, i, width, '-');
}

/*set the block of every row*/
int set_row(FILE*file, char**blocks, int row, int col, char type) {
    char temp = 'a';
    int i;
    for (i = 0; i < col; ++i) {
        temp = fgetc(file);
        if (temp != '0' && temp != '1') {
            return 5;
        }
        if (temp == '1') {
            blocks[row][i] = type;
            if (type == '-') {
                update_cell(i, row, 'h');
            } else {
                update_cell(i, row, 'v');
            }
        }
    }
    temp = fgetc(file);
    if (temp == '\n') {
        return 0;
    } else {
        return 5;
    }
}

/*
 * set the cells in the map
 */
int set_cells(FILE* file) {
    int row, col;
    char temp;

    int i;
    for (i = 0; i < playNumber; ++i) {
        winCells[i] = 0;
    }

    for (row = 0; row < height; ++row) {
        for (col = 0; col < width; ++col) {
            int owner = 0;
            while ((temp = fgetc(file)) != ','
                    && temp != '\n' && temp != EOF) {
                if (temp >= '0' && temp <= '9') {
                    owner *= 10;
                    owner += temp - '0';
                } else {
                    return 5;
                }
            }
            if (owner > playNumber) {
                return 5;
            }
            if (temp != ',' && temp < width - 1) {
                return 5;
            }
            if (cellCount[row][col] == 4 && owner > 0) {
                cell[row][col] = users[owner - 1];
                winCells[owner - 1]++;
            } else if (cellCount[row][col] == 4 && owner == 0) {
                return 5;
            } else if (cellCount[row][col] < 4 && owner > 0) {
                return 5;
            }
        }
    }
    return 0;
}

/*store the informantion into the file*/
int store() {
    char temp = getchar();
    char* path = NULL;
    struct List* next = NULL;
    struct List* start = NULL;
    int length = 0;
    int index = 0;
    FILE* output = NULL;
    if (temp != ' ') {
        clear();
        return -1;
    }
    while ((temp = getchar()) != '\n') {
        if (next == NULL) {
            next = (struct List*)malloc(sizeof(struct List));
            start = next;
            next->context = temp;
        } else {
            next->next = (struct List*)malloc(sizeof(struct List));
            next = next->next;
            next->context = temp;
            next->next = NULL;
        }
        length++;
    }
    path = malloc(sizeof(char) * (length + 1));
    next = start;
    while (next != NULL) {
        path[index] = next->context;
        index++;
        next = next->next;
    }
    path[index] = '\0';
    while (next != NULL) {
        next = start->next;
        free(start);
        start = next;
    }
    output = fopen(path, "w");
    if (output == NULL) {
        fprintf(stderr, "Can not open file for write\n");
        return -2;
    } else {
        stroe_in_file(output);
        fflush(output);
        fprintf(stderr, "Save complete\n");
        fclose(output);
        return -2;
    }
}

/*store the information in to FILE* file*/
void stroe_in_file(FILE* file) {
    int row;
    fprintf(file, "%d\n", currentUser + 1);
    for (row = 0; row < height; ++row) {
        output_row(file, hWall, row, width);
        output_row(file, vWall, row, width + 1);
    }
    output_row(file, hWall, row, width);
    print_cells(file);
}

/*store a row of wall into the file*/
void output_row(FILE* file, char** walls, int row, int max) {
    int col;
    for (col = 0; col < max; ++col) {
        if (walls[row][col] == ' ') {
            fprintf(file, "0");
        } else {
            fprintf(file, "1");
        }
    }
    fprintf(file, "\n");
}

/*store the cells into the file*/
void print_cells(FILE* file) {
    int i;
    for (i = 0; i < height; ++i) {
        print_cell_row(file, i);
        fprintf(file, "\n");
    }
}

/*store a line of cell row into the file*/
void print_cell_row(FILE* file, int row) {
    int col;
    for (col = 0; col < width - 1; ++col) {
        if (cell[row][col] == ' ') {
            fprintf(file, "%d,", 0);
        } else {
            fprintf(file, "%d,", cell[row][col] - 'A' + 1);
        }
    }
    if (cell[row][col] == ' ') {
        fprintf(file, "%d", 0);
    } else {
        fprintf(file, "%d", cell[row][col] - 'A' + 1);
    }
}
