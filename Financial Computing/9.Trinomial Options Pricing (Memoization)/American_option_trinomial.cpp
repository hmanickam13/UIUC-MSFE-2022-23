
//This file contains only American Option pricing using trinomial model
//Created by Hariharan Manickam for IE 523 Fall 2022
//Homework 9

#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <cstdlib>
using namespace std;

float initial_stock_price, expiration_time, strike_price, temp;
int no_of_divisions;
//We use double instead of float for precision(more decimals)
double up_factor, uptick_prob, downtick_prob, notick_prob, R, risk_free_rate, volatility;
double **memoized_call_price_table, **memoized_put_price_table;

//referred to sample codes given by professor RS
double max(float a, float b)
{
    return (b < a )? a:b;
}

//referred to binomial american option pricing ddynamic programing sample codes given by Professor RS
//Memoization added by me
double american_call_option(int k, int i, float current_stock_price)
{
    temp = 0.0;
    if (memoized_call_price_table[k][i+no_of_divisions]==-1)
    {
        if(k == no_of_divisions)
        {
            temp = max(0.0, (current_stock_price - strike_price));
            memoized_call_price_table[k][i+no_of_divisions] = temp;
            return temp;
        }
        else
        {
            temp = max((current_stock_price - strike_price),
                       ((uptick_prob * american_call_option(k+1, i+1, current_stock_price * up_factor)) +
                       (notick_prob * american_call_option(k+1, i, current_stock_price)) +
                       (downtick_prob * american_call_option(k+1, i-1, current_stock_price / up_factor)))/R);
            memoized_call_price_table[k][i+no_of_divisions] = temp;
            return temp;
        }
    }
    else
        return memoized_call_price_table[k][i+no_of_divisions];
}

//referred to binomial american option pricing ddynamic programing sample codes given by Professor RS
//Memoization added by me
double american_put_option(int k, int i, float current_stock_price)
{
    temp = 0.0;
    if (memoized_put_price_table[k][i+no_of_divisions]==-1)
    {
        if(k == no_of_divisions)
        {
            temp = max(0.0, (strike_price - current_stock_price));
            memoized_put_price_table[k][i+no_of_divisions] = temp;
            return temp;
        }
        else
        {
            temp = max((strike_price - current_stock_price),
                       ((uptick_prob * american_put_option(k+1, i+1, current_stock_price * up_factor)) +
                       (notick_prob * american_put_option(k+1, i, current_stock_price)) +
                       (downtick_prob * american_put_option(k+1, i-1, current_stock_price / up_factor)))/R);
            memoized_put_price_table[k][i+no_of_divisions] = temp;
            return temp;
        }
    }
    else
        return memoized_put_price_table[k][i+no_of_divisions];
}


//    Referred from sample codes given by Professor RS and modified by me
int main (int argc, char* argv[])
{
    
    sscanf (argv[1], "%f", &expiration_time);
    sscanf (argv[2], "%d", &no_of_divisions);
    sscanf (argv[3], "%lf", &risk_free_rate);
    sscanf (argv[4], "%lf", &volatility);
    sscanf (argv[5], "%f", &initial_stock_price);
    sscanf (argv[6], "%f", &strike_price);
    
//    Initialise memozied pointer of pointer for call and put. Then Assign -1 for all points
    memoized_call_price_table = new double*[no_of_divisions+1];
    memoized_put_price_table = new double*[no_of_divisions+1];

//    Row length = no_of_divisions+1
//    Column length = (no_of_divisions*2) + 1. Because min(value of i) = (-no_of_divisions)
//    to take i as an index, we add no_of_divisions to i
//    Example, k = 1, i = 1, no_of_divisions = 5000.
//    memoized_call_price_table[k][i] = memoized_call_price_table[1][5001]
//    Same will be observed when storing data in the option functions above.
    for (int i = 0; i <= no_of_divisions; i++){
        memoized_call_price_table[i] = new double[no_of_divisions+1+no_of_divisions];
        memoized_put_price_table[i] = new double[no_of_divisions+1+no_of_divisions];
    }
    for (int i = 0; i <= no_of_divisions; i++)
        for (int j = 0; j <= no_of_divisions+no_of_divisions+1; j++){
            memoized_call_price_table[i][j] = -1.;
            memoized_put_price_table[i][j] = -1.;
        }
    
    R = exp(risk_free_rate*expiration_time/((float) no_of_divisions));
    up_factor = exp(volatility*sqrt((2*expiration_time)/((float) no_of_divisions)));
    uptick_prob = pow((sqrt(R) - 1/sqrt(up_factor))/(sqrt(up_factor)-1/sqrt(up_factor)),2);
    downtick_prob = pow((sqrt(up_factor) - sqrt(R))/(sqrt(up_factor)-1/sqrt(up_factor)),2);
    notick_prob = 1 - uptick_prob - downtick_prob;
    
//    Calculating call and put price
    double call_price = american_call_option(0, 0,initial_stock_price);
    double put_price = american_put_option(0, 0, initial_stock_price);
    
    cout << "Recursive Trinomial American-Asian Option Pricing" << endl;
    cout << "Expiration Time (Years) = " << expiration_time << endl;
    cout << "Number of Divisions = " << no_of_divisions << endl;
    cout << "Risk Free Interest Rate = " << risk_free_rate << endl;
    cout << "Volatility (%age of stock value) = " << volatility*100 << endl;
    cout << "Initial Stock Price = " << initial_stock_price << endl;
    cout << "Strike Price = " << strike_price << endl;
    cout << "--------------------------------------" << endl;
    cout << "Up Factor = " << up_factor << endl;
    cout << "Up-tick Probability = " << uptick_prob << endl;
    cout << "No-tick Probability = " << 1-uptick_prob-downtick_prob << endl;
    cout << "Down-tick Probability = " << downtick_prob << endl;
    cout << "--------------------------------------" << endl;
    cout << "Trinomial Price of an American Call Option = " << call_price << endl;
    cout << "Trinomial Price of an American Put Option = " << put_price << endl;
    cout << "--------------------------------------" << endl;
    
//    Put call parity ignored for American Option
//    cout << "Let us verify the Put-Call Parity: S+P-C = Kexp(-r*T) for American Options" << endl;
//    cout <<  initial_stock_price << " + " << put_price << " - " << call_price;
//    cout << " = " << strike_price << "exp(-" << risk_free_rate << " * " << expiration_time << ")" << endl;
//    cout << initial_stock_price + put_price - call_price << " ?=? " << strike_price*exp(-risk_free_rate*expiration_time) << endl;
//    if (abs(initial_stock_price + put_price - call_price - strike_price*exp(-risk_free_rate*expiration_time)) <= 1e-3)
//        cout << "Looks like Put-Call Parity holds within three decimal places" << endl;
//    else
//        cout << "Looks like Put-Call Parity does NOT hold" << endl;
//    cout << "--------------------------------------" << endl;
    
}

