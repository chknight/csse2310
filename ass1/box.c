/*************************************************************************
    > File Name: box.c
    > Author: chknight
    > Mail: heatch@163.com 
    > Created Time: Wed 05 Aug 2015 09:51:49 AM AEST
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

char **block;
char **cell;
char **vWall;
char **hWall;
int **cellCount;
int height;
int width;
int playNumber;
int finishedCell;
int readNumberFromString(char* string); 
int changeStringToNumber(const char* string);
int initialize(char**, int);
void printMap();
int currentUser;
char* users;
int * winCells;
int getNumber();
char getType();
int getX();
int getY();
int checkCommand(int, int, char);
int playGame();
int getCommand(int*, int*, char*);
void addBlock(int, int, char);
void clear();
int updateCell(int, int, char);
int readFromFile(char*);
int getCurrentUser(FILE*);
int setBlocks(FILE*);
int setCells(FILE*);
int setRow(FILE*, char**, int, int, char);
int store();

struct list {
	char context;
	struct list* next;
};


int main(int argc, char** argv) {
	if (argc != 4 && argc != 5) {
		printf("Usage: boxes height width playercount [fillename]\n");
		return 1;
	}
	int status = initialize(argv, argc);
	if (status == 2) {
		printf("Invalid grid dimensions\n");
		return status;
	} else if (status == 3) {
		printf("Invalid player count\n");
		return status;
	} else if (status == 4) {
		printf("Invalid grid file\n");
		return status;
	} else if (status == 5) {
		printf("Error reading grid contents\n");
		return status;
	}
	playGame();
	return 0;
}

int slen (const char* str) {
	if (str == NULL) {
		printf("The string does not exit!\n");
		return 0;
	}
	int count = 0;
	while (str[count] != '\0') {
		count++;
	}
	return count;
}


/*To get a number from the string
*/
int readNumberFromString(char* string) {
	int result = 0;
	while(*string != ' ' && *string != '\n') {
		if (*string >= '0' && *string <= '9') {
		  result *= 10;
		  result += (*string)-'0';
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
int changeStringToNumber(const char* string) {
	if (string == NULL) {
		return -1;
	}
	int index = 0;
	int result = 0;
	while (string[index] != '\0') {
		if (string[index] >= '0' && string[index] <= '9') {
			result *= 10;
			result += string[index]-'0';
			index++;
		} else {
			return -1;
		}
	}
	return result;
}

/*check the initialize input in command line whether satisfy the require ment
 */
int checkTheCommandInput() {
	if (height == -1 || width == -1) {
		return 2;
	} else if (height > 999 || height < 1) {
		return 2;
	} else if (width > 999 || width < 1) {
		return 2;
	} else if (playNumber > 100 || playNumber < 1 || playNumber == -1) {
		return 3;
	} else {
		return 0;
	}
}

/*initialize all the variable
 */
int initialize(char** argv, int argc) {
	height = changeStringToNumber(argv[1]);
	width = changeStringToNumber(argv[2]);
	playNumber = changeStringToNumber(argv[3]);

	if (checkTheCommandInput() == 2) {
		return 2;
	} else if (checkTheCommandInput() == 3) {
		return 3;
	}

	currentUser = 0;
	users = (char*)malloc(sizeof(char)*playNumber);
	winCells = (int*)malloc(sizeof(int)*playNumber);

	int i, j;

	for (i = 0; i < playNumber; ++i) {
		users[i] = 'A'+i;
		winCells[i] = 0;
	}
	
	finishedCell = 0;
	hWall = (char**)malloc(sizeof(char*)*(height+1));
	vWall = (char**)malloc(sizeof(char*)*height);
	block = (char**)malloc(sizeof(char*)*(height+1));
	cell = (char**)malloc(sizeof(char*)*height);
	cellCount = (int**)malloc(sizeof(int*)*height);

	for (i = 0; i < height; ++i) {
		vWall[i] = (char*)malloc(sizeof(char)*(width+1));
		for (j = 0; j < width+1; ++j)
			vWall[i][j] = ' ';
	}

	for (i = 0; i < height; ++i) {
		cell[i] = (char*)malloc(sizeof(char)*(width));
		cellCount[i] = (int*)malloc(sizeof(int)*width);
		for (j = 0; j < width; ++j) {

			cellCount[i][j] = 0;
		  cell[i][j] = ' ';
		}
	}

	for (i = 0; i < height+1; ++i) {
		hWall[i] = (char*)malloc(sizeof(char)*(width));
		for (j = 0; j < width; ++j)
			hWall[i][j] = ' ';
	}

	for (i = 0; i < height+1; ++i) {
		block[i] = (char*)malloc(sizeof(char)*(width+1));
		for (j = 0; j < width+1; ++j)
			block[i][j] = '+';
	}

	if (argc == 5) {
		int status = readFromFile(argv[4]);

		if (status != 0)
			return status;
	}

	printMap();
	return 0;
}



/*Print the map to the screen
 */
void printMap() {
	int i, j;
	for (i = 0; i < height * 2 + 1 ; i++) {
		if (i % 2 == 0) {
			for (j = 0; j < width * 2 + 1; j++) {
				if (j % 2 == 0)
					printf("%c",block[i/2][j/2]);
				else
					printf("%c",hWall[i/2][j/2]);
			}
			printf("\n");
		} else {
			for (j = 0; j < width * 2 + 1; j++) {
				if (j % 2 == 0)
					printf("%c",vWall[i/2][j/2]);
				else
					printf("%c", cell[i/2][j/2]);
			}
			printf("\n");
		}
	}
}

/*receive the command given by the user
 */
int getCommand(int *x, int *y, char *type) {

	*y = getNumber(0);
	if (*y == -1)
		return 0;
	if (*y == -2)
		return 2;
	*x = getNumber(1);
	if (*x == -1)
		return 0;
	*type = getType();
	if (*type == '\0')
		return 0;
	if (checkCommand (*x, *y, *type) == 0)
		return 0;
	else
		return 1;
}

/*get a number from the input
 */
int getNumber(int order) {
	char temp;
	int result = 0;
	int index = 0;
	while((temp = getchar()) != ' ') {
		if (temp >= '0' && temp <= '9') {
		  result *= 10;
		  result += temp-'0';
			index++;
		} else {
			if (order == 0 && index == 0 && temp == 'w') {
				return store();
			}

			/*if the input includig some error character such as 'a', the function
			 *while return -1 as a sambol
			 */
			if (temp != '\n')
				clear();
			return -1;
		}
	}
	return result;
}

/*
//get the X index in the command
int getX() {
	int x = getNumber();
	if (x > width || x < 0) {

		return -1;
	} else {
		return x;
	}
}

//get the y index in the command
int getY() {
	int y = getNumber();
	if (y > height || y < 0) {
		return -1;
	} else {
		return y;
	}
}
*/

/*get the type of operation of the user
 */
char getType() {
	char type = getchar();
	char symbol = getchar();
	if (symbol != '\n') {
		return '\0';
		clear();
	} else if (type != 'v' && type != 'h') {
		return '\0';
	} else {
		return type;
	}
}

/*check whether the command is correct
 */
int checkCommand(int x, int y, char type) {
	if (type == '\0') {
		return 0;
	}
	if (x < 0 || y < 0 || x > width || y > height)
		return 0;
	if (type == 'h') {
		if (x >= width)
			return 0;
		if (hWall[y][x] == '-')
			return 0;
	} else {
		if (y >= height)
			return 0;
		if (vWall[y][x] == '|')
			return 0;
	}
	return 1;
}

/*add a block in the map
 */
void addBlock(int x, int y, char type) {
	if (type == 'h')
		hWall[y][x] = '-';
	else
		vWall[y][x] = '|';
}

/*the loop is used to play the game
 */
int playGame() {
	int x, y;
	char type;
	int i, j;
	while (1) {
		printf("%c> ", users[currentUser]);
		if (getCommand(&x, &y, &type) == 1) {
		  addBlock(x, y, type);
			currentUser = (currentUser+ updateCell(x, y, type)) % playNumber;
			printMap();
			if (finishedCell == width*height) {
				int max = 0;
				int lastOne = 0;
				for (i = 0; i < playNumber; ++i) {
					if (winCells[i] >= max) {
						max = winCells[i];
						lastOne = i;
					}
				}
				printf ("Winners(s):");
				for (i = 0; i < playNumber; ++i) {
					printf(" %c", users[i]);
					if (i < lastOne)
						printf(",");
					else
						printf("\n");
				}
				break;
			}
		} 
	}
}

/*if mistake input occur, read the rest char
 */
void clear() {
	char temp;
	while ((temp = getchar())!= '\n');
}

/*update the cell count
 */
int updateCell(int x, int y, char type) {

	int result = 1;

	if (type == 'h') {
		if (y - 1 >= 0) {
			cellCount[y-1][x]++;
			if (cellCount[y-1][x] == 4 && cellCount[y-1][x] != ' ') {
				result = 0;
				cell[y-1][x] = users[currentUser];
				finishedCell++;
			}
		}
		if (y < height) {
			cellCount[y][x]++;
			if (cellCount[y][x] == 4 && cellCount[y][x] != ' ' ) {
				result = 0;
				cell[y][x] = users[currentUser];
				finishedCell++;
			}
		}
	} else {
	  if (x < width) {
			cellCount[y][x]++;
			if (cellCount[y][x] == 4 && cellCount[y][x] != ' ') {
				result = 0;
				finishedCell++;
				cell[y][x] = users[currentUser];
			}
		}
		if (x - 1 >= 0) {
			cellCount[y][x-1]++;
			if (cellCount[y][x-1] == 4 && cellCount[y][x] != ' ') {
				result = 0;
				finishedCell++;
				cell[y][x-1] = users[currentUser];
			}
		}
	}
	return result;
}


/*read the information from the input file
 */
int readFromFile(char* path) {
	FILE *inputFile = fopen(path, "r");
	if (inputFile == NULL) {
		return 4;
	}
	currentUser = getCurrentUser(inputFile);
	if (currentUser == -1) {
	  fclose(inputFile);
		return 5;
	} else if (currentUser == -2) {
	  fclose(inputFile);
		return 5;
	}

	int status;
	status = setBlocks(inputFile);
	if (status == 5)
		return 5;

	status = setCells(inputFile);
	if (status == 5)
		return 5;
	

	fclose(inputFile);
	return 0;
}

/*get current user in the file
 */
int getCurrentUser(FILE* file) {
	int result = 0;
	char temp = '0';
	while ((temp = fgetc(file)) != '\n'&& temp != EOF) {
		if (temp >= '0' && temp <= '9') {
			result *= 10;
			result += temp - '0';
		} else {
			return -1;
		}
	}
	if (result <= playNumber && result >= 1)
		return result-1;
	else
		return -2;
}

/*
 * set block in the file
 */
int setBlocks(FILE* file) {
	int i, j;
	for (i = 0; i < height; ++i) {
		if (setRow(file, hWall, i, width, '-') == 5)
			return 5;
		if (setRow(file, vWall, i, width+1, '|') == 5)
			return 5;
	}
	return setRow(file, hWall, i, width, '-');
}

int setRow(FILE*file, char**blocks, int row, int col, char type) {
	char temp = 'a';
	int i;
	for (i = 0; i < col; ++i) {
		temp = fgetc(file);
		if (temp != '0' && temp != '1')
			return 5;
		if (temp == '1') {
			blocks[row][i] = type;
			if (type == '-') {
				updateCell(i, row, 'h');
			} else {
				updateCell(i, row, 'v');
			}
		}
	}
	temp = fgetc(file);
	if (temp == '\n')
		return 0;
	else
		return 5;
}

/*
 * set the cells in the map
 */
int setCells(FILE* file) {
	int row, col;
	char temp;
	for (row = 0; row < height; ++row) {
		for (col = 0; col < width; ++col) {
			temp = fgetc(file);
			if (temp >= '0' && temp <= '9') {
				int owner = temp-'0'-1;
				if (owner >= 0 && owner < playNumber) {
					if (cellCount[row][col] != 4) {
						return 5;
					} else {
						cell[row][col] = users[owner];
					}
				} else if (owner == -1) {
					if (cellCount[row][col] == 4)
						return 5;
				} else {
					return 5;
				}
			} else {
				return 5;
			}
			temp = fgetc(file);
			if (col < width-1 && temp != ',') {
				return 5;
			} else if (col == width) {
					if (temp != '\n' && temp != EOF)
						return 5;
			}
		}
	}
	return 0;
}

int store() {
	char temp = getchar();
	struct list* next = NULL;
	struct list* start = NULL;
	int length = 0;
	while ((temp = getchar()) != '\n') {
		if (next == NULL) {
		  next = (struct list*)malloc(sizeof(struct list));
			start = next;
			next->context = temp;
		} else {
			next->next = (struct list*)malloc(sizeof(struct list));
			next = next->next;
			next->context = temp;
			next->next = NULL;
		}
		length++;
	}
	char* path = malloc(sizeof(char)*(length+1));
	int index = 0;
	next = start;
	while (next != NULL) {
		printf("%c\n", next->context);
		path[index] = next->context;
		index++;
		next = next -> next;
	}
	while (next != NULL) {
		next = start -> next;
		free(start);
		start = next;
	}
	FILE* output = fopen(path, "w");
	if (output == NULL) {
		printf("Can not open file fo write\n");
	} else {
	  printf("Save complete\n");
	  return -2;
	}
}
