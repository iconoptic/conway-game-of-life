#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <ctime>
#include <cstdlib>
#include <curses.h>

bool** genGrid (int rows, int cols)
{
	bool** grid = 0;
	grid = new bool*[rows];
	srand(time(0));
	for (int i = 0; i < rows; i++)
	{
			grid[i] = new bool[cols];
			for (int j = 0; j < cols; j++)
			{
					if ( rand() % 100 < 25 ) grid[i][j] = false;
					else grid[i][j] = true;
			}
	}
	return grid;
}

bool** genGridGliderGun (int rows, int cols)
{
		bool **grid = 0;
		grid = new bool*[rows];
		bool gL[rows][cols] = {
				{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true},
				{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true,false,true},
				{false,false,false,false,false,false,false,false,false,false,false,false,true,true,false,false,false,false,false,false,true,true,false,false,false,false,false,false,false,false,false,false,false,false,true,true},
				{false,false,false,false,false,false,false,false,false,false,false,true,false,false,false,true,false,false,false,false,true,true,false,false,false,false,false,false,false,false,false,false,false,false,true,true},
				{true,true,false,false,false,false,false,false,false,false,true,false,false,false,false,false,true,false,false,false,true,true},
				{true,true,false,false,false,false,false,false,false,false,true,false,false,false,true,false,true,true,false,false,false,false,true,false,true},
				{false,false,false,false,false,false,false,false,false,false,true,false,false,false,false,false,true,false,false,false,false,false,false,false,true},
				{false,false,false,false,false,false,false,false,false,false,false,true,false,false,false,true},
				{false,false,false,false,false,false,false,false,false,false,false,false,true,true},
		};
		for (int i = 0; i < rows; i++) 
		{
				grid[i] = new bool[cols];
				for (int j = 0; j < cols; j++)
				{
						if (gL[i][j]) grid[i][j] = true;
						else grid[i][j] = false;
				}

		}
		return grid;
}

void delGrid(bool** grid, int rows)
{
		for (int i = 0; i < rows; i++) delete [] grid[i];
		delete [] grid;
		grid = 0;
}

bool** stepGrid (bool** grid, int rows, int cols)
{
		bool **newGrid;
		newGrid = new bool*[rows];
		for (int i = 0; i < rows; i++)
		{
				newGrid[i] = new bool[cols];
				for (int j = 0; j < cols; j++)
				{
						int lN = 0; //live neighbor counter

						//Check states for neighboring cells
						if (i != 0)
						{
								if (grid[i-1][j]) lN++;
								if (j != 0 && grid[i-1][j-1]) lN++;
								if (j != cols-1 && grid[i-1][j+1]) lN++;
						}
						if (i != rows-1)
						{
								if (grid[i+1][j]) lN++;
								if (j != 0 && grid[i+1][j-1]) lN++;
								if (j != cols-1 && grid[i+1][j+1]) lN++;
						}
						if (j != 0 && grid[i][j-1]) lN++;
						if (j != cols-1 && grid[i][j+1]) lN++;

						//Determine state of this cell
						if (grid[i][j]) 
						{
								if (lN < 2 || lN > 3) newGrid[i][j] = false;
								else newGrid[i][j] = true;
						}
						else
						{
								if (lN == 3) newGrid[i][j] = true;
								else newGrid[i][j] = false;
						}

						//printw("[%d,%d]: %d\n", i, j, lN);
				}
		}
		delGrid(grid, rows);
		return newGrid;
}

void printGrid (bool** grid, int rows, int cols)
{
	erase();
	for (int i = 0; i < rows; i++)
	{
			for (int j = 0; j < cols; j++)
			{
					if (grid[i][j]) mvprintw(i, j, "0");
					//grid[i][j] ? printw("0") : printw(" ");// printw("■") : printw(" ");
			}
			//printw("\n");
	}
	refresh();
}

bool **gridCp (bool **origGrid, int rows, int cols)
{
		bool **grid;
		grid = new bool*[rows];
		for (int i = 0; i < rows; i++)
		{
				grid[i] = new bool[cols];
				for (int j = 0; j < cols; j++)
				{
						grid[i][j] = origGrid[i][j];
				}
		}
		return grid;
}

bool sameGrid (bool** lGrid, bool** curGrid, int rows, int cols)
{
		for (int i = 0; i < rows; i++)
		{
				for (int j = 0; j < cols; j++)
				{
						if (lGrid[i][j] != curGrid[i][j]) return false;
				}
		}
		return true;
}

int main (int argc, char **argv)
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	int rows = w.ws_row, cols = w.ws_col;
	int generation = 0;
    bool** lastGrid, **llGrid, **lllGrid, **llllGrid, **curGrid = genGrid(rows, cols);

	WINDOW *win = initscr();
	idlok(win, true);
	printGrid(curGrid, rows, cols);

	while (1)
	{
			if (generation > 3) 
			{
					if (generation > 5) delGrid(lllGrid, rows);// llllGrid = gridCp(lllGrid,rows,cols);
					if (generation > 4)
					{
							lllGrid = gridCp(curGrid, rows, cols);
							delGrid(llGrid, rows);
					}
					llGrid = gridCp(lastGrid,rows,cols);
					delGrid(lastGrid, rows);
			}
			lastGrid = gridCp(curGrid,rows,cols);
			curGrid = stepGrid(curGrid, rows, cols);
			generation ++;
			printGrid(curGrid, rows, cols);
			if (sameGrid(lastGrid, curGrid, rows, cols)) {
					delGrid(curGrid, rows);
					curGrid = genGrid(rows, cols);
					generation = 0;
			}
			else if (generation > 10) 
			{
					if (sameGrid(llGrid,curGrid, rows, cols))
					{
							delGrid(curGrid, rows);
							curGrid = genGrid(rows, cols);
							generation = 0;
					}
					if (sameGrid(lllGrid, curGrid, rows, cols))
					{
							delGrid(curGrid, rows);
							curGrid = genGrid(rows,cols);
							generation = 0;
					}
					/*if (sameGrid(llllGrid, curGrid, rows, cols))
					{
							curGrid = genGrid(rows,cols);
							generation = 0;
					}*/
			}
			//sleep(1);
			//usleep(10000);
	}
	
	return 0;  // make sure your main returns int
}
