
// Created by Hariharan Manickam for Financial Computing IE 523
// Assignment 10
// Part 1

#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <random>
#include <chrono>
#include "normdist.h"
using namespace std;

#define PI 3.141592654

double initial_stock_price, risk_free_rate, strike_price, expiration_time, volatility, barrier_price;
int no_of_divisions, no_of_trials;
double adjusted_call_option_price, adjusted_put_option_price;
double call_option_price_simulation, put_option_price_simulation;
double theoretical_call_price, theoretical_put_price;


// setting the seed, referred from Professor RS sample codes
unsigned seed = (unsigned) std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine generator (seed);


// u.i.i.d. generator
double get_uniform()
{
    std::uniform_real_distribution <double> distribution(0.0,1.0);
    double number = distribution(generator);
    return (number);
}


// referred to notes and sample codes given by Professor RS
double max(double a, double b)
{
    return (b < a )? a:b;
}


// referred to notes and sample codes given by Professor RS
double option_price_put_black_scholes(const double& S,      // spot price
                                      const double& K,      // Strike (exercise) price,
                                      const double& r,      // interest rate
                                      const double& sigma,  // volatility
                                      const double& time)
{
    double time_sqrt = sqrt(time);
    double d1 = (log(S/K)+r*time)/(sigma*time_sqrt) + 0.5*sigma*time_sqrt;
    double d2 = d1-(sigma*time_sqrt);
    return K*exp(-r*time)*N(-d2) - S*N(-d1);
};


double option_price_call_black_scholes(const double& S,       // spot (underlying) price
                                       const double& K,       // strike (exercise) price,
                                       const double& r,       // interest rate
                                       const double& sigma,   // volatility
                                       const double& time)      // time to maturity
{
    double time_sqrt = sqrt(time);
    double d1 = (log(S/K)+r*time)/(sigma*time_sqrt)+0.5*sigma*time_sqrt;
    double d2 = d1-(sigma*time_sqrt);
    return S*N(d1) - K*exp(-r*time)*N(d2);
};

// referred to notes and sample codes given by Professor RS
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


float closed_form_down_and_out_european_call_option()
{
    // I took this formula from Wilmott, Howison and Dewynne, "The Mathematics of Financial Derivatives"
    float K = (2*risk_free_rate)/(volatility*volatility);
    float A = option_price_call_black_scholes(initial_stock_price, strike_price,
                                             risk_free_rate, volatility, expiration_time);
    float B = (barrier_price*barrier_price)/initial_stock_price;
    float C = pow(initial_stock_price/barrier_price, -(K-1));
    float D = option_price_call_black_scholes(B, strike_price, risk_free_rate, volatility, expiration_time);
    return (A - D*C);
}


float closed_form_down_and_in_european_put_option()
{
    // just making it easier by renaming the global variables locally
    float S = initial_stock_price;
    float r = risk_free_rate;
    float T = expiration_time;
    float sigma = volatility;
    float H = barrier_price;
    float X = strike_price;

    // Took these formulae from some online reference
    float lambda = (r+((sigma*sigma)/2))/(sigma*sigma);
    float temp = 2*lambda - 2.0;
    float x1 = (log(S/H)/(sigma*sqrt(T))) + (lambda*sigma*sqrt(T));
    float y = (log(H*H/(S*X))/(sigma*sqrt(T))) + (lambda*sigma*sqrt(T));
    float y1 = (log(H/S)/(sigma*sqrt(T))) + (lambda*sigma*sqrt(T));
    return (-S*N(-x1) + X*exp(-r*T)*N(-x1 + sigma*sqrt(T)) +
            S*pow(H/S, 2*lambda)*(N(y)-N(y1)) -
            X*exp(-r*T)*pow(H/S, temp)*(N(y-sigma*sqrt(T))-N(y1-sigma*sqrt(T))));
}


float closed_form_down_and_out_european_put_option()
{
    float vanilla_put = option_price_put_black_scholes(initial_stock_price, strike_price,
                                              risk_free_rate, volatility, expiration_time);
    float put_down_in = closed_form_down_and_in_european_put_option();
    return (vanilla_put - put_down_in);
}


// simulates stock price
void simulate()
{
    
//    Initialise all values with 0
    vector<double> S(4,0);
    double avg_call_price = 0;
    double avg_put_price = 0;
    double payoff_call_sum = 0;
    double payoff_put_sum = 0;
    double avg_adjusted_call_sum = 0;
    double avg_adjusted_put_sum = 0;
    double adjusted_call_payoff_sum = 0.0;
    double adjusted_put_payoff_sum = 0.0;
//        keeps track of number of times S is greater than 0 to use in the denominator to find average
    int S_greater_than_0 = 0;
    
//    chop expiration time into no_of_divisions many segments
//    figure out the motion within each segment
    double delta_T = expiration_time/((double) no_of_divisions);
    double delta_R = (risk_free_rate - 0.5*pow(volatility,2))*delta_T;
    double delta_SD = volatility*sqrt(delta_T);
    
//    for loop for number of trials
    for (int i = 0; i < no_of_trials; i++)
    {
//            Assigning initial values
        S[0] = S[1] = S[2] = S[3] = initial_stock_price;
        double payoff_call = 0.0;
        double payoff_put = 0.0;
        double probability = 0.0;
        
//        for loop to find option price
        for (int j = 0; j < no_of_divisions; j++)
        {
//            Create the unit normal variates using the Box-Muller Transform
            double x = get_uniform();
            double y = get_uniform();
            double a =  sqrt(-2.0*log(x)) * cos(6.283185307999998*y);
            double b =  sqrt(-2.0*log(x)) * sin(6.283185307999998*y);
            
//            for loop to simulate 4 stock price paths
            for(int k = 0; k<3; k=k+2) // loop goes from 0 to 2
            {
                S[k] = S[k] * exp(delta_R + delta_SD*a); //check notes
                S[k+1] = S[k+1] * exp(delta_R - delta_SD*b);
            }

//                find if stock price crossed the barrier
            for(int l = 0; l<4; l++)
//                option value is 0 if it is below barrier price
                if(S[l]<=barrier_price)
                    S[l] = 0;
        }
    
        for(int m = 0; m<4; m++)
        {
//            checking if stock price is greater than 0.
            if(S[m]>0)
            {
//                Increment count
                S_greater_than_0++;
                
//                finding values for explicit simulation
                payoff_call = payoff_call + max(0, S[m]-strike_price);
                payoff_put = payoff_put + max(0, strike_price-S[m]);
                
//                finding values for verification of adjusted method
                probability = exp((-2*log(initial_stock_price/barrier_price)*log(strike_price/barrier_price))
                              /  (pow(volatility, 2)*expiration_time));
                
//                adjusted method values running sum
                adjusted_call_payoff_sum = adjusted_call_payoff_sum + max(0, S[m]-strike_price)*(1-probability);
                adjusted_put_payoff_sum = adjusted_put_payoff_sum + max(0, strike_price-S[m])*(1-probability);
            }
        }
//            because we have a payoff_call sum for 4 trials, we divide values by 4
            payoff_call = payoff_call/4;
//            keep running sum
            payoff_call_sum = payoff_call + payoff_call_sum;
//            we find avg for all the times stock price greater than 0
            avg_adjusted_call_sum = adjusted_call_payoff_sum/S_greater_than_0;
    
//            Repeat same for Put option
            payoff_put = payoff_put/4;
            payoff_put_sum = payoff_put + payoff_put_sum;
            avg_adjusted_put_sum = adjusted_put_payoff_sum/S_greater_than_0;
            
        }
//            calculate avg call price
            avg_call_price = payoff_call_sum/no_of_trials;
            
//            cal price is avg call price discounted to present time
            call_option_price_simulation = avg_call_price/(exp(risk_free_rate*expiration_time));

//            discounting and dividing by number of times stock was above 0 to find average price
            adjusted_call_option_price = avg_adjusted_call_sum/(exp(risk_free_rate*expiration_time));
    
//            similarly we calculate avg put price
            avg_put_price = payoff_put_sum/no_of_trials;
            put_option_price_simulation = avg_put_price/(exp(risk_free_rate*expiration_time));
            adjusted_put_option_price = avg_adjusted_put_sum/(exp(risk_free_rate*expiration_time));
}


// referred to notes and sample codes given by Professor RS
int main (int argc, char* argv[])
{

//   Input = 1 0.05 0.25 50 40 1000000 1000 20
    sscanf (argv[1], "%lf", &expiration_time);
    sscanf (argv[2], "%lf", &risk_free_rate);
    sscanf (argv[3], "%lf", &volatility);
    sscanf (argv[4], "%lf", &initial_stock_price);
    sscanf (argv[5], "%lf", &strike_price);
    sscanf (argv[6], "%d", &no_of_trials);
    sscanf (argv[7], "%d", &no_of_divisions);
    sscanf (argv[8], "%lf", &barrier_price);

    cout << "European Down and Out Option Pricing via Dynamic Programming (& Repeated Squaring)" << endl;
    cout << "Expiration Time (Years) = " << expiration_time << endl;
    cout << "Number of Divisions = " << no_of_divisions << endl;
    cout << "Risk Free Interest Rate = " << risk_free_rate << endl;
    cout << "Volatility (%age of stock value) = " << volatility*100 << endl;
    cout << "Initial Stock Price = " << initial_stock_price << endl;
    cout << "Strike Price = " << strike_price << endl;
    cout << "Barrier Price = " << barrier_price << endl;
    cout << "--------------------------------------" << endl;
    
    
//    Calling the function to find option prices
    simulate();
    
    cout << "The average Call price by explicit simulation  = " << call_option_price_simulation<< endl;
    cout << "The call price using the (1-p)-adjustment term = " << adjusted_call_option_price << endl;
    cout << "Theoretical Call Price (closed form solution)  = " << closed_form_down_and_out_european_call_option() << endl;
    cout << "--------------------------------------" << endl;
    cout << "The average Put price by explicit simulation  = " << put_option_price_simulation<< endl;
    cout << "The put price using the (1-p)-adjustment term = " << adjusted_put_option_price << endl;
    cout << "Theoretical Put Price (closed form solution)  = " <<closed_form_down_and_out_european_put_option() << endl;
    cout << "--------------------------------------" << endl;
}
