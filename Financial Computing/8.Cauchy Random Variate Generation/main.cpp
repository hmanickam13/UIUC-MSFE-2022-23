//
//
//  Random Variate Generation
//
//  Created by Haiharan Manickam on 11/10/22.
//
// Unit Normal variate Generator using the Box-Muller Transform
// Refered to Sample codes created by Prof. Sreenivas for IE523: Financial Computing

// Using the C++ STL to generate i . i .d. Cauchy RVs
#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <random>
#include <chrono>
#include <cmath>
using namespace std;

#define STANDARD_CAUCHY_DENSITY(x) 1/(3.141592654*(1+(x*x)))
double UNIT_NORMAL_DENSITY(double x){
    double y = (exp(-x*x/2)/pow((2*3.141592654),(-2)));
    return y;
}

// cf http://www.cplusplus.com/reference/random/uniform real distribution/operator()/
// If you want to set a seed −− do it only after debug phase is completed
// otherwise errors will not be repeatable.
unsigned seed = (unsigned) chrono::system_clock::now().time_since_epoch().count();
default_random_engine generator(seed);

//default_random_engine generator;

//referred notes
double get_uniform()
{
    // http://www.cplusplus.com/reference/random/exponential_distribution/
    std::uniform_real_distribution<double> distribution(0.0,1.0);
    double number = distribution(generator);
    return (number);
}
//referred notes
double get_cauchy()
{
    cauchy_distribution <double> distribution (0.0 ,1.0);
    // see http://www.cplusplus.com/reference/random/cauchydistribution/ for details
    return (distribution(generator));
}

////we find z score as it is a normalized
////Makes it easier to understand data when plotting if data is normalized
////https://stackoverflow.com/questions/7616511/calculate-mean-and-standard-deviation-from-a-vector-of-samples-in-c-using-boos
// void normalize_data(vector<double> &v)
//{
//     double sum = std::accumulate(std::begin(v), std::end(v), 0.0);
//     double m =  sum / v.size();
//     double accum = 0.0;
//     std::for_each (std::begin(v), std::end(v), [&](const double d) {
//         accum += (d - m) * (d - m);
//     });
//     double stdev = sqrt(accum / (v.size()-1));
//
////     this loop finds z score
//     for(int i=0; i<100; i++)
//         v[i] = ((v[i]-m)/stdev);
//
////     we add the absolute value of the minimum value in the vector to make sure all values are non negative
//     for(int i=0; i<100; i++)
//         v[i] = v[i] - *min_element(v.begin(), v.end());
//}

int main (int argc, char* argv[])
{
    float y, pdf_standard_cauchy[100];
    int no_of_trials, count[100];
    
    sscanf (argv[1], "%d", &no_of_trials);
    ofstream pdf_comparison_file(argv[2]);
    
    
//    Comment out this line and enter a file name as argumnent and the line which writed into the file towards the end of the code
//    in order to get a output csv file of number of trials & arithmetic mean
//    I used this for the 4th part of the assignment
//    ofstream cauchy_mean_file(argv[3]);

//    referred notes
    for (int i=0; i<100; i++){
        count[i] = 0;
        pdf_standard_cauchy[i] = 0.0;
    }
    
//    referred notes
    for (int i = 0; i < no_of_trials; i++) {
        y = get_cauchy();
        for (int j = 0; j < 100; j++)
            if ( (y >= ((float) (j-51)/10)) && (y < ((float) (j-50)/10)) )
                count[j]++;
    }
    
//    creating vector to store sample 100 values to generate for plotting
//    referred notes
    vector<double> cauchy;
    vector<double> unit_normal;
    for (int j = 0; j < 100; j++) {
        cauchy.push_back(0.01*STANDARD_CAUCHY_DENSITY((double) (j-50)/10));
//        unit_normal.push_back(0.1*UNIT_NORMAL_DENSITY((double) (j-50)/10));
    }
    
//    normalize both vectors
//    normalize_data(cauchy);
//    normalize_data(unit_normal);
    
//    writing the data onto a file in a csv format
    for (int j = 0; j < 100; j++) {
        pdf_comparison_file << ((double) (j-50)/10)<<" , "<< cauchy[j]<<" , "<< STANDARD_CAUCHY_DENSITY(9DOUBLE) (J-50)/10))<<endl;
    }
    
//    This part of the code helps to understand why the mean of a randomly generated cauchy variable does not converge to 0
//    creating vectors to store values for the 4th part of the assignment
    vector<int> no_of_trials_vec;
    vector<double>cauchy_arithmetic_mean;
    for(int i = 0; i<20; i++)
    {
        //    calculates arithmetic mean
        y=0.0;
        for (int j = 0; j < no_of_trials; j++)
            y += get_cauchy();
        
        double mean = y/((double) no_of_trials);
        
        cout <<"Sample run "<<i<<" | Mean of "<< no_of_trials<< "−many Cauchy RVs is = " <<mean<<endl;
        
//        cauchy_mean_file << no_of_trials << " , " <<mean<<endl;
        
        no_of_trials+=10000;
    }

}
