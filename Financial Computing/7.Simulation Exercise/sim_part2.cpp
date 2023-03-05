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

//vector to store theoretical value & experimental value
vector<double> theoretical_value;
vector<double> experimental_value;

//setting seed for random number generator
unsigned seed = (unsigned) chrono::system_clock::now().time_since_epoch().count();
default_random_engine generator;

//  uniform random generator needed to simulate unfair coin toss in simulate_coin_toss function
double get_uniform()
{
    std::uniform_real_distribution <double> distribution(0.0, 1.0);
    double number = distribution(generator);
    return (number);
}

//simulating coin toss 30 times
void simulate_coin_toss(double probability_of_heads_alice, double probability_of_heads_bob)
{
    int no_of_trials = 1000000;
    int alice_win_count[30];
//    loop for number of tosses
    for(int no_of_tosses=0; no_of_tosses<30; no_of_tosses++)
    {
//        variables that keep track of number of heads for alice & bob
        int head_count_alice;
        int head_count_bob;
        
        alice_win_count[no_of_tosses]=0;
//        loop for number of trials
        for(int trial=0; trial<no_of_trials; trial++)
        {
            head_count_alice=0;
            head_count_bob=0;
//            loop to find who won
            for(int toss=0; toss<=no_of_tosses; toss++)
            {
                if(get_uniform() < probability_of_heads_alice)
                    head_count_alice++;
                
                if(get_uniform() < probability_of_heads_bob)
                    head_count_bob++;
            }
            if(head_count_alice>head_count_bob)
                alice_win_count[no_of_tosses]++;
        }
        
        double x = alice_win_count[no_of_tosses];
        double z = x/no_of_trials;
//        pushing the experimental value into an array to call later
        experimental_value.push_back(z);
    }
    
};

//function that calculates factorial used in calculating theoretical value
double factorial(int n)
{
      if(n > 1)
        return n * factorial(n - 1);
      else
        return 1;
}

//fucntion that is called
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

//find theoretical value for n tosses as n incremements
double find_theoretical_values(float probability_of_heads_alice, float probability_of_heads_bob)
{
    double a = 0;
    int n=1;
    while (n<=30)
    {
//        pushing values back to an array to call later
        a = prob_alice_winning(probability_of_heads_alice, probability_of_heads_bob, n);
        theoretical_value.push_back(a);
        
//incrementing number of tosses
        n++;
    }
    return n;
}

int main(int argc, const char * argv[])
{
//    declaring variables
    double probability_of_heads_alice, probability_of_heads_bob;
    long no_of_trials;
    
//    getting input
    sscanf(argv[1], "%ld", &no_of_trials);
    sscanf(argv[2], "%lf", &probability_of_heads_alice);
    sscanf(argv[3], "%lf", &probability_of_heads_bob);
    ofstream outfile_1(argv[4]);

//  output
    cout<<"Probability of Heads for Alice : "<<probability_of_heads_alice<<endl;
    cout<<"Probability of Heads for Bob   : "<<probability_of_heads_bob<<endl;
    
    find_theoretical_values(probability_of_heads_alice, probability_of_heads_bob);
    simulate_coin_toss(probability_of_heads_alice, probability_of_heads_bob);

//    printing the experimental value and theoretical value to a text file
    for(int j = 0; j<30; j++)
        outfile_1<<j+1<<", "<<experimental_value[j]<<", "<<theoretical_value[j]<<endl;
    return 0;
}
