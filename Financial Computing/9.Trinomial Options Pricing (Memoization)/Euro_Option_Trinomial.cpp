
//This file contains only European Option pricing using trinomial model
//Created by Hariharan Manickam for IE 523 Fall 2022
//Homework 9

#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include "normdist.h"          // this defines the normal distribution from Odegaard's files
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

//referred to sample codes given by professor RS
double option_price_put_black_scholes(const double& S,      // spot price
                                      const double& K,      // Strike (exercise) price,
                                      const double& r,      // interest rate
                                      const double& sigma,  // volatility
                                      const double& time)   // time to maturity
{
    double time_sqrt = sqrt(time);
    double d1 = (log(S/K)+r*time)/(sigma*time_sqrt) + 0.5*sigma*time_sqrt;
    double d2 = d1-(sigma*time_sqrt);
    return K*exp(-r*time)*N(-d2) - S*N(-d1);
};

//referred to sample codes given by professor RS
double option_price_call_black_scholes(const double& S,       // spot (underlying) price
                                       const double& K,       // strike (exercise) price,
                                       const double& r,       // interest rate
                                       const double& sigma,   // volatility
                                       const double& time)    // time to maturity
{
    double time_sqrt = sqrt(time);
    double d1 = (log(S/K)+r*time)/(sigma*time_sqrt)+0.5*sigma*time_sqrt;
    double d2 = d1-(sigma*time_sqrt);
    return S*N(d1) - K*exp(-r*time)*N(d2);
};

//estimating for 6 sigma on either side
//Cut and paste from Professor RS sample codes
double N(const double& z) {
    if (z > 6.0) { return 1.0; }; // this guards against overflow
    if (z < -6.0) { return 0.0; };
    double b1 = 0.31938153;
    double b2 = -0.356563782;
    double b3 = 1.781477937;
    double b4 = -1.821255978;
    double b5 = 1.330274429;
    double p = 0.2316419;
    double c2 = 0.3989423;
    double a=fabs(z);
    double t = 1.0/(1.0+a*p);
    double b = c2*exp((-z)*(z/2.0));
    double n = ((((b5*t+b4)*t+b3)*t+b2)*t+b1)*t;
    n = 1.0-b*n;
    if ( z < 0.0 ) n = 1.0 - n;
    return n;
};

//referred to binomial euro option pricing sample codes given by Professor RS
//Memoization and conversion from binomial to trinomial done by me
double european_call_option(int k, int i)
{
    temp = 0.0;
    if (memoized_call_price_table[k][i+no_of_divisions] == -1)
    {
        if (k == no_of_divisions)
        {
            temp = max(0.0, (initial_stock_price*pow(up_factor, ((float) i))) - strike_price);
            memoized_call_price_table[k][i+no_of_divisions] = temp;
            return temp;
        }
        else
        {
            temp = ((uptick_prob*european_call_option(k+1,i+1) +
                     notick_prob*european_call_option(k+1, i) +
                     downtick_prob*european_call_option(k+1,i-1))/R);
            memoized_call_price_table[k][i+no_of_divisions] = temp;
            return temp;
            
        }
    }
    else
        return memoized_call_price_table[k][i+no_of_divisions];
}

double european_put_option(int k, int i)
{
    temp = 0.0;
    if (memoized_put_price_table[k][i+no_of_divisions] == -1)
    {
        if (k == no_of_divisions)
        {
            temp = max(0.0, strike_price - (initial_stock_price*pow(up_factor, ((float) i))));
            memoized_put_price_table[k][i+no_of_divisions] = temp;
            return temp;
        }
        else
        {
            temp = (((uptick_prob * european_put_option(k+1,i+1) +
                      notick_prob * european_put_option(k+1, i) +
                      downtick_prob * european_put_option(k+1,i-1)) / R));
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
    
//    Calculating option prices
    double call_price = european_call_option(0, 0);
    double put_price = european_put_option(0, 0);
    double call_price_BSM = option_price_call_black_scholes(initial_stock_price, strike_price, risk_free_rate, volatility, expiration_time);
    double put_price_BSM = option_price_put_black_scholes(initial_stock_price, strike_price, risk_free_rate, volatility, expiration_time);
    
    cout << "Recursive Trinomial European Option Pricing" << endl;
    cout << "Expiration Time (Years) = " << expiration_time << endl;
    cout << "Number of Divisions = " << no_of_divisions << endl;
    cout << "Risk Free Interest Rate = " << risk_free_rate << endl;
    cout << "Volatility (%age of stock value) = " << volatility*100 << endl;
    cout << "Initial Stock Price = " << initial_stock_price << endl;
    cout << "Strike Price = " << strike_price << endl;
    cout << "--------------------------------------" << endl;
    cout << "Up Factor = " << up_factor << endl;
    cout << "Uptick Probability = " << uptick_prob << endl;
    cout << "Notick Probability = " << notick_prob << endl;
    cout << "Downtick Probability = " << downtick_prob << endl;
    cout << "--------------------------------------" << endl;
    cout << "Trinomial Price of an European Call Option = " << call_price << endl;
    cout << "Call Price according to Black-Scholes = " << call_price_BSM << endl;
    cout << "--------------------------------------" << endl;
    cout << "Trinomial Price of an European Put Option = " << put_price << endl;
    cout << "Put Price according to Black-Scholes = " << put_price_BSM << endl;
    cout << "--------------------------------------" << endl;
    cout << "Verifying Put-Call Parity: S+P-C = Kexp(-r*T)" << endl;
    cout <<  initial_stock_price << " + " << put_price << " - " << call_price;
    cout << " = " << strike_price << "exp(-" << risk_free_rate << " * " << expiration_time << ")" << endl;
    cout << initial_stock_price + put_price - call_price << " = " << strike_price*exp(-risk_free_rate*expiration_time) << endl;
    cout << "--------------------------------------" << endl;
}
