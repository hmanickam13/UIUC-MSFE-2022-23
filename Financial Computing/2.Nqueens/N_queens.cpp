// N Queens Problem via (Backtracking, which is implemented by) Recursion 
// Written by Prof. Sreenivas for IE523: Financial Computing

#include <iostream>
#include "N_queens.h"

int main(int argc, char* const argv[])
{
    Board x;

//    int y;
    
//    cin>> y;
    int board_size;
    sscanf(argv[1], "%d", &board_size);

    x.nQueens(board_size);

    return 0;
}
