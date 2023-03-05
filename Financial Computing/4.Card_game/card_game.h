#ifndef card_game
#define card_game

#include <algorithm>
#include <vector>

using std::vector;
using namespace std;

class Card
{
    double **check;

    void initialize(int n)
    {
        int x  = n/2;

        check =  new double*[x];
        
        for(int i = 1; i <= x; i++)
            check[i] = new double[x];
        
        for(int i=1; i<=x; i++)
            for(int j=1; j<=x;j++)
                check[i][j] = -1;              
    }

public:

    double value;
    
    double cardgame(int red_left, int black_left)
    {   
        double value;

        if(red_left == 0)
        {   
            return((double) black_left);
        }

        if(black_left == 0)
        {   
            return (0);
        }

        //if grid has value -1, call the recusrive function and find out the value
        if(check[red_left][black_left] == -1)
        {
            double term1 = ((double)red_left/(red_left + black_left)) * cardgame(red_left - 1, black_left);
            
            double term2 = ((double)black_left/(red_left + black_left)) * cardgame(red_left, black_left - 1);
        
            check[red_left][black_left] = max((term1 + term2), (double)(black_left-red_left));
            
            value = check[red_left][black_left];
        }

        else
        {    
            value = check[red_left][black_left];
        }

        return value;
    }

    //    Main function calling the recursive function and printing the output
	void che(int n)
	{
        initialize(n);
        double money;
        cout<<"Total Number of Cards: "<<n<<endl;
        money = cardgame(n/2, n/2);
        cout<<"Value of playing game: "<<money<<endl;
    }
};
#endif