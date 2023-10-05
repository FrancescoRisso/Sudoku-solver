#include <stdio.h>

#define dim 9
#define fileName "emptySudoku.txt"

typedef enum { false, true } bool;

typedef struct {
	short val;
	bool possible[10];
} cell;


/*
	loadGrid
	---------------------------------------------------------------------
	Reads the initial grid of the sudoku game
	---------------------------------------------------------------------
	PARAMETERS:
		- grid: the matrix where to load
	---------------------------------------------------------------------
	OUTPUT:
		- true if all went smoothly
		- false if some problems occurred
*/
bool loadGrid(cell grid[][dim]);

/*
	insertValue
	---------------------------------------------------------------------
	Inserts a value into a certain cell of the sudoku grid, updating the
	possibilities of the cells in the same square, row and column
	---------------------------------------------------------------------
	PARAMETERS:
		- grid: the grid
		- val: the value to be inserted
		- row: the 0-based row where to insert
		- col: the 0-based column where to insert
	---------------------------------------------------------------------
	OUTPUT:
		-
*/
void insertValue(cell grid[][dim], short val, short row, short col);

/*
	printGrid
	---------------------------------------------------------------------
	Prints the grid
	---------------------------------------------------------------------
	PARAMETERS:
		- grid: the grid to print
*/
void printGrid(cell grid[][dim]);

/*
	countPossibilities
	---------------------------------------------------------------------
	Returns the number of possible values in a cell
	---------------------------------------------------------------------
	PARAMETERS:
		- grid: the sudoku grid where to search
		- r: the row of the cell
		- c: the column of the cell
	---------------------------------------------------------------------
	OUTPUT:
		- the number of available possibilities
*/
int countPossibilities(cell grid[][dim], int r, int c);

/*
	solveSudoku
	---------------------------------------------------------------------
	Resolves the sudoku
	---------------------------------------------------------------------
	PARAMETERS:
		- grid: the grid to solve
	---------------------------------------------------------------------
	OUTPUT:
		- true sudoku was complete
		- false if sudoku was impossible
*/
bool solveSudoku(cell grid[][dim]);

/*
	isSudokuComplete
	---------------------------------------------------------------------
	Checks whether the sudoku grid is complete and valid
	---------------------------------------------------------------------
	PARAMETERS:
		- grid: the grid
	---------------------------------------------------------------------
	OUTPUT:
		- true if the sudoku is valid
		- false if the sudoku is incomplete or invalid
*/
bool isSudokuComplete(cell grid[][dim]);


int main(int argc, char *argv[]) {
	cell grid[dim][dim];

	if(!loadGrid(grid)) return -1;

	if(solveSudoku(grid))
		printGrid(grid);
	else
		printf("Sudoku was impossible");

	return 0;
}

bool loadGrid(cell grid[][dim]) {
	int r, c, n;
	char tmp;
	FILE *f;

	// Init grid
	for(r = 0; r < dim; r++) {
		for(c = 0; c < dim; c++) {
			grid[r][c].val = 0;
			for(n = 0; n < 10; n++) grid[r][c].possible[n] = true;
		}
	}

	// Open file
	if((f = fopen(fileName, "r")) == NULL) {
		printf("Could not open file\n");
		return false;
	}

	// Load grid from file
	for(r = 0; r < dim; r++) {
		for(c = 0; c < dim; c++) {
			if(fscanf(f, " %c", &tmp) != 1) {
				printf("File is not correct");
				fclose(f);
				return false;
			}
			if(tmp != '-') insertValue(grid, tmp - '0', r, c);
		}
	}

	fclose(f);
	return true;
}

void insertValue(cell grid[][dim], short val, short row, short col) {
	int r, c, n, squareRow, squareCol;

	// Insert value
	grid[row][col].val = val;

	// Update possible for the cell
	for(n = 1; n < 10; n++) grid[row][col].possible[n] = false;

	// Update possible for the column
	for(r = 0; r < dim; r++) grid[r][col].possible[val] = false;

	// Update possible for the row
	for(c = 0; c < dim; c++) grid[row][c].possible[val] = false;

	// Update possilbe for the square
	squareRow = row / 3;
	squareCol = col / 3;
	for(r = 3 * squareRow; r < 3 * (squareRow + 1); r++) {
		for(c = 3 * squareCol; c < 3 * (squareCol + 1); c++) grid[r][c].possible[val] = false;
	}
}

void printGrid(cell grid[][dim]) {
	int r, n;
	for(n = 0; n < dim / 3; n++) {
		for(r = 3 * n; r < 3 * (n + 1); r++)
			printf("\t %d %d %d | %d %d %d | %d %d %d\n", grid[r][0].val, grid[r][1].val, grid[r][2].val, grid[r][3].val, grid[r][4].val,
				grid[r][5].val, grid[r][6].val, grid[r][7].val, grid[r][8].val);
		if(r != dim) printf("\t-------|-------|-------\n");
		r++;
	}
}

int countPossibilities(cell grid[][dim], int r, int c) {
	int n, cnt = 0;
	for(n = 1; n < 10; n++) cnt += grid[r][c].possible[n];
	return cnt;
}

bool solveSudoku(cell grid[][dim]) {
	int r, c, poss, rMinPoss, cMinPoss, n, i;
	bool moveDone = true;
	cell gridCopy[dim][dim];

	while(moveDone) {
		moveDone = false;
		rMinPoss = cMinPoss = -1;

		// Check every cell
		for(r = 0; (r < dim) && !moveDone; r++) {
			for(c = 0; (c < dim) && !moveDone; c++) {
				// Only consider cell if empty
				if(grid[r][c].val == 0) {
					// Count its possibilities
					poss = countPossibilities(grid, r, c);

					// If 0 possibilities, sudoku is impossible
					if(poss == 0) return false;

					// If 1 possibility, insert it
					if(poss == 1) {
						for(n = 1; !grid[r][c].possible[n]; n++) {}
						insertValue(grid, n, r, c);
						moveDone = true;

					} else {
						// If more than 1 possibility, search the cell with less possibilities
						if(rMinPoss == -1 || poss < countPossibilities(grid, rMinPoss, cMinPoss)) {
							rMinPoss = r;
							cMinPoss = c;
						}
					}
				}
			}
		}
	}

	if(isSudokuComplete(grid)) return true;

	// If sudoku is not completed in the "logical" way, analize every possibility for the cell with the least possibilities
	poss = countPossibilities(grid, rMinPoss, cMinPoss);
	n = 1;
	for(i = 0; i < poss; i++) {
		// First, create a clone of the grid
		for(r = 0; r < dim; r++) {
			for(c = 0; c < dim; c++) gridCopy[r][c] = grid[r][c];
		}

		// Then, pick one element in the cell with the least possibilities
		for(; !grid[rMinPoss][cMinPoss].possible[n]; n++) {}
		insertValue(gridCopy, n, rMinPoss, cMinPoss);

		// If sudoku is complete, copy it to the original grid, and return true
		if(solveSudoku(gridCopy)) {
			for(r = 0; r < dim; r++) {
				for(c = 0; c < dim; c++) grid[r][c] = gridCopy[r][c];
			}
			return true;
		}
		n++;
	}

	return false;
}

bool isSudokuComplete(cell grid[][dim]) {
	int r, c, occurrences[10], n, square, pos;

	occurrences[0] = 1;

	// Check for row correctness
	for(r = 0; r < dim; r++) {
		for(n = 1; n < 10; n++) occurrences[n] = 0;
		for(c = 0; c < dim; c++) occurrences[grid[r][c].val]++;
		for(n = 1; n < 10; n++)
			if(occurrences[n] != 1) return false;
	}

	// Check for column correctness
	for(c = 0; c < dim; c++) {
		for(n = 1; n < 10; n++) occurrences[n] = 0;
		for(r = 0; r < dim; r++) occurrences[grid[r][c].val]++;
		for(n = 1; n < 10; n++)
			if(occurrences[n] != 1) return false;
	}

	// Check for squares correctness
	for(square = 0; square < dim; square++) {
		for(n = 1; n < 10; n++) occurrences[n] = 0;
		for(pos = 0; pos < dim; pos++) occurrences[grid[3 * (square / 3) + pos / 3][3 * (square % 3) + pos % 3].val]++;
		for(n = 1; n < 10; n++)
			if(occurrences[n] != 1) return false;
	}

	// No problems were found, return true
	return true;
}
