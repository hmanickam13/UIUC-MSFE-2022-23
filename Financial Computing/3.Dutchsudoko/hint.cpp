// Dutch Miracle Soduku Solver using Brute-Force Search implemented
// using recursive backtracking
//
// Written for IE523: Financial Computation by Prof. Sreenivas
// and GE523: Discrete Event Systems
//
// For details see
// https://app.crackingthecryptic.com/sudoku/hfpFTGNLrr
// 
#include <iostream>
#include "hint.h"

int main (int argc, char * const argv[]) 
{
	Sudoku x;

	x.read_puzzle(argc, argv);

	x.print_puzzle();
//	x.precheck(0,0);
	x.Solve(0,0);
	// x.print_puzzle();
	
	
    return 0;
}
