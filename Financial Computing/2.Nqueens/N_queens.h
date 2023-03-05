#ifndef N_queens
#define N_queens
#include<iostream>
#include<vector>

using namespace std;
class Board
{
	// private data member: size of the board
	int size;
    
    // pointer-to-pointer initialization of the board
	int **chess_board;
    
//    keeps track of the number of solutions
    int x=0;
	
    
    // private member function: initializes the (n x n) chessboard
    void initialize(int n)
    {
        size = n;

        chess_board =  new int*[size];
        
        for(int i = 0; i < size; i++)
            chess_board[i] = new int[size];
        
        for(int i=0; i<size; i++)
            for(int j=0; j<size;j++)
                chess_board[i][j] = 0;
        
        

    }
    
	// private member function:  returns 'false' if
	// the (row, col) position is not safe.
	bool is_this_position_safe(int row, int col)
	{
		
		int i, j = 0;

		//checks left
		for (i = col; i >=0 ; i--)
			if (chess_board[row][i] == 1)
				return false; 

		//checks left lower diag
		for (i = row, j = col; i < size && j >= 0; i++, j--)
			if (chess_board[i][j] == 1)
				return false;

		//checks left upper diag
		for (i = row, j = col; i >= 0 && j >= 0; i--, j--)
			if (chess_board[i][j] == 1)
				return false;

		return true;
		// print_board(size);
	}
	
	
	// private member function: prints the board position
	void print_board(int size)
    {
        //initializing a vector to use to print out the chess board
        vector<vector < string> > board;
        
        for(int i=0; i<size; i++)
        {
            vector <string> temp;
            board.push_back(temp);
        }
        
        for(int i=0; i<size; i++)
        {
            for(int j=0;j<size;j++)
            {
                string value;
                if(chess_board[i][j] == 1)
                {
                    value = " Q ";
                    board[i].push_back(value);
                }
                else
                {
                    value = " - ";
                    board[i].push_back(value);
                }
            }
        }
        
        //        incrementing x because it prints the first solution
        ++x;
        cout<<size<<" x "<<size<<"- Queens problem solution: "<<x;
        
        for(int i=0; i<size; i++)
        {
            cout<<endl;
            for(int j=0; j<size; j++)
                cout<<board[i][j];
        }
        
        cout<<endl;
        
    }

	// private member function: recursive backtracking
	bool solve(int col)
    {
            if (col == size)
            {
                cout<<"Sucessful arrangement in recursion"<<endl;
                print_board(size);
                cout<<"========"<<endl;
                
//                return true;
            }
            cout<<endl<<"Recursion depth"<<col<<endl;
            for(int i = 0; i <size ; i++)
            {
                // cout<<endl<<"Before if condition Loop No: "<<i<<endl;
                if (is_this_position_safe(i, col))
                {
                    chess_board[i][col] = 1;
                    
                    if(solve(col + 1))
                        return true;
                    print_board(size);
                    cout<<"Failed at Recursion No: "<<col<<endl;
                    chess_board[i][col] = 0;
                }
                // else
                // {
                //     cout<<endl<<"No soln in Col no: "<<col<<" -- Row No: "<<i<<endl;
                // }
            }
            return false;
    }
    
//    Function to return x value
    int getx() {
        return x;
    }
	
public:
	
//    Solves the n-Queens problem by (recursive) backtracking
	void nQueens(int n)
	{
		initialize(n);
//        print_board(n);
        
		if (solve(0))
        {	//print_board(n);
            cout<<endl<<"All soln printed "<<"for n queens problem"<<endl;
        }
        else
			cout << "There are a total of " << getx() << " solutions to the "<< n << "x" <<
            size << "size-Queens Problem" << std::endl;

		//free(chess_board);
        
        for (int i = 0; i < size; i++)
            delete[] chess_board[i];

        delete[] chess_board;

	}
};
#endif
