/*
 *  hint.h
 *  Sudoku
 *  Created by Prof. Ramavarapu Sreenivas 
 *  Inspired by: http://web.eecs.utk.edu/courses/spring2012/cs140/Notes/Sudoku/index.html
*/

#ifndef sudoku
#define sudoku

#include <vector>
#include <fstream>
#include <tuple>
#include <string>
#include <sstream>
#include <cstdlib>
#include<iostream>
using std::vector;
using namespace std;

class Sudoku
{ 
	// Private
	int puzzle[9][9];
	
    //function checks if any number is equal in the row
    bool row_valid(int row)
    {
        // for loop to iterate in row and get value
        for(int i = 0; i<9; i++)
        {
            // declare x as number in the grid
            int x = puzzle[row][i];
            if(x!=0)
            {
                // for loop to iterate in row and check equality
                for (int j = 0; j < 9 ; j++)
                    {
                        //condition to check if it is the same number but not from the same position
                        if (puzzle[row][j] == x &&  i!=j && x!=0)
                            return false;
                    }
            }
        }
        // cout<<"Row is valid";
        return true;
        // write code that checks if "row" is valid
    }
    
    // Private member function that checks if the named column is valid
    bool col_valid(int col)
    {
        for(int i = 0; i<9; i++)
        {
            int x = puzzle[i][col];

            if(x!=0)
            {
                for (int j = 0; j < 9 ; j++)
                    {
                        if (puzzle[j][col] == x &&  i!=j)
                                return false;
                    }
            }
        }
        // cout<<"Col is valid";

        return true;
        // check validity of "col"
    }
    
    // Private member function that checks if the named 3x3 block is valid
    bool block_valid(int row, int col)
    {
        //Row starting = a; Row ending = b
        //Column starting = c; Column ending = d
        int a,b,c,d;
        int x = puzzle[row][col];

        if(row <=2)
        {
            a = 0;
            b = 2;
            
            if(col<=2)
            {
                c = 0;
                d = 2;
            }
            else if(col>=6)
            { 
                c = 6;
                d = 8;
            }
            else
            {
                c = 3;
                d = 5;
            }
        }

        else if(row>=6)
        {
            a = 6;
            b = 8;
            
            if(col<=2)
            {
                c = 0;
                d = 2;
            }
            else if(col>=6)
            { 
                c = 6;
                d = 8;
            }
            else
            {
                c = 3;
                d = 5;
            }
        }
        else
        {
            a = 3;
            b = 5;

            if(col<=2)
            {
                c = 0;
                d = 2;
            }
            else if(col>=6)
            { 
                c = 6;
                d = 8;
            }
            else
            {
                c = 3;
                d = 5;
            }
        }    

        for(int i = a; i<=b; i++)
            {
                for(int j = c; j<=d; j++)
                {
                    if(puzzle[i][j] == x && row != i && col != j && x!=0)
                        return false;
                }
            }
        return true;  
        // check 3 x 3 block validity
    }
    
    
    // Private member function that checks if the positive-diagonal
    // (SW to NE diagonals) have no repeated digits. If the initial
    // partially-filled puzzle does not meet this requirement, I need
    // to flag this (see write-up). I am using tuples for this. This
    // way I can show which value is repeated along the diagonal multiple times
    // I took this from https://stackoverflow.com/questions/321068/returning-multiple-values-from-a-c-function

    std::tuple<bool, int> positive_diagonals_valid(int a, int b)
    {
        //declaring value of cell to x
        int x = puzzle[a][b];

        for(int i = a - 1, j = b + 1; i >= 0 && j <= 8; i--, j++)
        {
            if(puzzle[i][j] == x)
                return make_tuple(false, puzzle[a][b]);
        }

        for(int i = a + 1, j = b - 1; j >= 0 && i <= 8; i++, j--)
        {
            if(puzzle[i][j] == x)
                return make_tuple(false, puzzle[a][b]);
        }
        
        return make_tuple(true, 487);

     
        // write code that checks if there are any repeated
        // digits along the 15-many positive diagonals
        // If the intial puzzle does not meet this requirement,
        // I want to let the user know that digit X has appeared
        // multiple times along some positive diagonal (in the
        // partially-filled input puzzle). This way the user can
        // see the reason why there can be no solution to the
        // Dutch Miracle Sudoku puzzle instance.
        //
        // In my implementation, if the returned bool variable is false,
        // then the second-tuple is the X that was repeated along
        // some positive diagonal (see description of assignment
        // for a sample output).
    }
    
    // Private member funtion that checks if adjacent cells
    // (along the positive diagonal) touching at a corner
    // have a difference of atleast 4
    std::tuple<bool, int, int> adjacent_cells_along_positive_diagonals_have_at_least_a_difference_of_4(int a, int b)
    {

        if((a - 1) >= 0 && b + 1 <= 8)
            if(puzzle[a - 1][b + 1] != 0)
                if(abs(puzzle[a - 1][b + 1] - puzzle[a][b]) < 4)
                    return make_tuple(false, a, b);
                    
        if((a + 1) <= 8 && (b - 1) >= 0)
            if(puzzle[a + 1][b - 1] != 0)
                if(abs(puzzle[a + 1][b - 1] - puzzle[a][b]) < 4)
                    return make_tuple(false, a, b);
        
        return std::make_tuple(true, 12, 13);
        
         // write code that checks if all non-zero entries in the
        // puzzle (filled so far) have met the "minimum-difference
        // of 4" rule.
        //
        // In my implementation, if the returned bool variable is false,
        // then the two ints (i and j, say) identify puzzle[i][j] and
        // puzzle[i-1][j+1] that do not meet the "minimum-difference
        // of 4" (see program assignment description for a sample output)
    }
    
	
public:
	// Public member function that reads the incomplete puzzle
	// we are not doing any checks on the input puzzle -- that is,
	// we are assuming they are indeed valid
    void read_puzzle(int argc, char * const argv[])
    {
        // int value_read;
        vector<vector<int>> V;
        string eachrow;
        ifstream input_file(argv[1]);

        if(input_file.is_open())
        {
            // vector<int> v2;
            while(getline(input_file, eachrow))
            {
                // v2.push_back(value_read);
                istringstream line_ss(eachrow);
                vector<int> row;
                for (int element; line_ss>>element;)
                    row.push_back(element);
                
                // after iterating row in text file, add vector into 2D vector
                V.push_back(row);
            }
        }
        else
            cout<<"File does not exist in the same directory";
       
        for(int i = 0; i<9; i++)
            for(int j = 0; j<9; j++)
                puzzle[i][j] = V[i][j];
 
        // write code that reads the input puzzle using the
        // guidelines of figure 23 of the bootcamp material
    }
	
	// Public member function that prints the puzzle when called
	void print_puzzle()
	{
		//std::cout << number_of_calls << std::endl;
		std::cout << std::endl << "Board Position" << std::endl;
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				// check if we have a legitimate integer between 1 and 9
				// if ((puzzle[i][j] >= 1) && (puzzle[i][j] <= 9))
				// {
					// printing initial value of the puzzle with some formatting
					std::cout << puzzle[i][j] << " ";
				// }
				// else 
                // {
				// 	// printing initial value of the puzzle with some formatting
				// 	std::cout << "X ";
				// }
			}
			std::cout << std::endl;

            
		}
        row_valid(3);
	}

    // Public member function that (recursively) implements the brute-force
    // search for possible solutions to the incomplete Sudoku puzzle
    bool Solve(int row, int col)
    {
        int count = 0;
        static int x=0;
        
        //for loop checking if puzzle is completely filled
        for(int i = row; i<9; i++)
        {
            for(int j = 0; j<9; j++)
                {
                    // cout<<"reached";
                    if(puzzle[i][j] == 0)
                        count++;
                    
                    if(count == 0 && i == 8 && j == 8)
                    {   
                        x++;
                        if(x == 20)
                            return true;
                        cout<<"You have solved the puzzle"<<endl<<"Soln No: "<<x;
                        // return true;
                        // print_puzzle();
                        
                    }
                }
        }
       

        if(puzzle[row][col]==0)
        {
            for(int a = 1; a<=9; a++)
                {
                    puzzle[row][col] = a;

                    bool bool1, bool2;
                    int rep_no, row_no, col_no;
                    
                    tie(bool1, rep_no) = positive_diagonals_valid(row, col);

                    tie(bool2, row_no, col_no) = adjacent_cells_along_positive_diagonals_have_at_least_a_difference_of_4(row, col);
                    
                    if(row_valid(row))
                    {
                        if(col_valid(col))
                        {
                             cout<<"reached";
                            if(block_valid(row, col))
                            {
                                if(bool1==1)
                                {
                                    if(bool2==1)
                                    {
                                        if(Solve(row+1, col+1))
                                            {
                                                print_puzzle();
                                                return true;
                                            }

                                    }
                                }
                                
                            }
                        }

                    }
                    
                    puzzle[row][col] = 0;
                    return false;
                }
        }
        return false;
    }
        
       
    
    
        // this part of the code identifies the row and col number of the
        // first incomplete (i.e. 0) entry in the puzzle.  If the puzzle has
        // no zeros, the variable row will be 9 => the puzzle is done, as
        // each entry is row-, col- and block-valid...
        
        // use the pseudo code of figure 3 of the description

    void precheck(int row, int col)
    {
        for(int i = row; i<=8; i++)
        {
            if(row_valid(i)==0)
                cout<<"Initial puzzle has repeating values in Row no: "<<row<<endl;
        }  
        
        for(int j = col; j<=8; j++)
        {
            if((col_valid(j))==0)
                cout<<"Initial puzzle has repeating values in Row no: "<<col<<endl;
        }

        bool a1, a2;
        int val, row1, col1;

        for(int i = 0; i<=8; i++)
            for(int j = 0; j<=8; j++)
            {
                if(block_valid(i,j)==0)
                    cout<<"Initial puzzle does not satisfy block validity"<<endl;
                
                tie(a1, val) = positive_diagonals_valid(row, col);

                tie(a2, row1, col1) = adjacent_cells_along_positive_diagonals_have_at_least_a_difference_of_4(row, col);

                if(a1 == 0)
                    cout<<"The number "<<val<<" repeats in the positive diagonal"<<endl;
                
                if(a2 == 0)
                    cout<<"puzzle["<<i<<"]["<<j<<"] = "<<puzzle[i][j]<<" has an adjacent value which has a difference less than 4"<<endl;
            }
        
    }
};
#endif