//
//  main.cpp
//  simulation
//
//  Created by Haiharan Manickam on 11/1/22.
//

#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <random>
#include <chrono>
#include <cmath>
#include <fstream>

#define HEADS 1
#define TAILS 0
using namespace std;

//function to calculate factorial, used in prob_alice_winning
double factorial(int n)
{
      if(n > 1)
        return n * factorial(n - 1);
      else
        return 1;
}

//calculate theoretical probability values for alice
double prob_alice_winning(float probability_of_heads_alice, float probability_of_heads_bob, int n)
{
    double prob_of_alice_winning = 0;
    for(int r=0; r<n; r++)
    {
        double x=0, y = 0;
        x = (factorial(n))/(factorial(n-r)*factorial(r))*(pow(probability_of_heads_bob,r))*(pow((1-probability_of_heads_bob),(n-r)));

        for(int s=r+1; s<=n; s++)
            y += (factorial(n))/(factorial(n-s)*factorial(s))*(pow(probability_of_heads_alice,s))*(pow((1-probability_of_heads_alice),(n-s)));
        
        prob_of_alice_winning += x*y;
    }
    return prob_of_alice_winning;
}

//function to find optimal number of tosses
double find_n(float probability_of_heads_alice, float probability_of_heads_bob)
{
//    starting with 1 toss
    int n=1;
    while ((prob_alice_winning(probability_of_heads_alice, probability_of_heads_bob, n))<(prob_alice_winning(probability_of_heads_alice, probability_of_heads_bob, n+1)))
    {
//        incrementing number of tosses
        n++;
        
//        stopping condition is checking if the next probabibility is decreasing, if it is, break out of the loop and return value
//        no need to increment n again
        if((prob_alice_winning(probability_of_heads_alice, probability_of_heads_bob, n))>(prob_alice_winning(probability_of_heads_alice, probability_of_heads_bob, n+1)))
            break;
    }
    return n;
}

int main(int argc, const char * argv[])
{
    double probability_of_heads_alice, probability_of_heads_bob;
    int optimal_no_of_tosses;
    
//    getting probability of alice and bob as input
    sscanf(argv[1], "%lf", &probability_of_heads_alice);
    sscanf(argv[2], "%lf", &probability_of_heads_bob);

//    assigning the value returned by the function to a variable
    optimal_no_of_tosses = find_n(probability_of_heads_alice, probability_of_heads_bob);

//    printing output
    cout<<"Probability of Heads for Alice                        : "<<probability_of_heads_alice<<endl;
    cout<<"Probability of Heads for Bob                          : "<<probability_of_heads_bob<<endl;
    cout<<"Number of trials that maximizes probability for Alice : "<<optimal_no_of_tosses<<endl;

    return 0;
}
