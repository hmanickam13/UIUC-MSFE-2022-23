//
//  po.h
//  Portfolio_Optimization
//
//  Created by Haiharan Manickam on 10/20/22.
//

#ifndef po_h
#define po_h
#define EPSILON 0.01

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <numeric>

//#include "newmatap.h"                // need matrix applications
#include "gurobi_c++.h"
//#include "newmatio.h"                // need matrix output routines

//#ifdef use_namespace
//using namespace NEWMAT;              // access NEWMAT namespace
using namespace std;
using std::vector;
//#endif

class portfolio_optimization
{
    //    Initializing all variables,vectors and vector of vector
    int number_of_cashflows;
    float debt_obligation_amount;
    int time_when_debt_is_due;
    
    vector<float> PV;
    vector<int> Maturity;
    vector<float> inner_ydc;
    vector<float> duration;
    vector<float> convexity;
    vector<float> yield_to_maturity;
    vector<float> debt_as_percentage_of_bond;
    vector<float> percentage_of_facevalue_to_buy;
    
    vector<vector<float>> Cashflow;
    
    
    //    Numerator used in Newton raphson method f(x)
    float my_function(vector<float>cash_flow, int maturity, float pv, float rate)
    {
        float result = pv*(pow((1+rate),(maturity)));
        for(int i=0; i<maturity; i++)
            result = result - (cash_flow[i] * (pow((1+rate), (maturity-1-i))));
        return result;
    };
    
    //    Denominator used in Newton raphson method f'(x)
    float derivative_function(vector<float>cash_flow, int maturity, float pv, float rate)
    {
        float result = maturity * pv * (pow((1+rate), (maturity-1)));
        for(int i=0; i<(maturity-1); i++)
            result = result - (maturity - 1 - i) * cash_flow[i] * (pow((1+rate), (maturity-2-i)));
        return result;
    };
    
    //    Newton Raphson method
    float Newton_Raphson(vector<float>cash_flow, int maturity, float pv, float rate)
    {
        while (abs(my_function(cash_flow, maturity, pv, rate)) > EPSILON)
        {
//            cout<<(my_function(cash_flow, maturity, pv, rate))<<" "<<derivative_function(cash_flow, maturity, pv, rate)<<endl;
//            cout<<"rate: "<<rate<<endl;
            rate = rate - (my_function(cash_flow, maturity, pv, rate)/derivative_function(cash_flow, maturity, pv, rate));
        }
        return rate;
    };
    
    //    Function to calculate duration of the bond
    float get_duration(vector<float>cash_flow, int maturity, float pv, float rate)
    {
        float duration = 0.0;
        for(int i=0; i<maturity; i++)
            duration = duration + (cash_flow[i] *(i+1))/(pow((1+rate), i+1));
        duration = duration/pv;
        return duration;
    };
    
    //    Function to calculate convexity of the bond
    float get_convexity(vector<float>cash_flow, int maturity, float pv, float rate)
    {
        float convexity = 0.0;
        for(int i=0; i<maturity; i++)
            convexity = convexity + (cash_flow[i]*((i+1)*(i+2))/(pow((1+rate), i+3)));
        convexity = convexity/pv;
        return convexity;
    };
    
    //    Function which calls the functions for Yield, Duration and Convexity and prints output 1
    void ydc_calculator(vector<vector<float>> cash_flow, vector<int> maturity, vector<float> pv)
    {
        for(int i = 0; i<number_of_cashflows; i++)
        {
            float r = Newton_Raphson(cash_flow[i], maturity[i], pv[i], 0.0);
            yield_to_maturity.push_back(r);
            
            float d = get_duration(cash_flow[i], maturity[i], pv[i], r);
            duration.push_back(d);
            
            float c = get_convexity(cash_flow[i], maturity[i], pv[i], r);
            convexity.push_back(c);
        }
        
        cout<<"Debt Amount: $"<<debt_obligation_amount<<endl;
        cout<<"Debt is due in: "<<time_when_debt_is_due<<" years"<<endl;
        cout<<"Number of Bonds: "<<number_of_cashflows<<endl;;
        
        //        Loop to print for each cashflow
        for(int i=0; i< number_of_cashflows; i++)
        {
            cout<<"------------------------------------------------------------"<<endl;
            cout<<"Cashflow #"<<i+1<<endl;
            cout<<"PV = $"<<PV[i]<<endl;
            cout<<"Maturity = "<<Maturity[i]<<" years"<<endl;
            cout<<"Yield to maturity = "<<yield_to_maturity[i]*100<<"%"<<endl;
            cout<<"Duration = "<<duration[i]<<endl;
            cout<<"Convexity = "<<convexity[i];
            cout<<endl;
        }
        cout<<"************************************************************"<<endl;
    };
    
    //    Function which calculates the answer for output 2
    void percentage_calculator(vector<float> present_value, vector<float> yield_to_maturity, float debt_obligation, int time_when_debt_is_due, int number_of_cashflows)
    {
        cout<<endl;
        for(int i = 0; i<number_of_cashflows; i++)
        {
            float value_of_bond_at_time_when_debt_is_due = PV[i]*(pow((1+yield_to_maturity[i]), time_when_debt_is_due));
            float percentage = debt_obligation_amount/value_of_bond_at_time_when_debt_is_due;
            
            percentage_of_facevalue_to_buy.push_back(percentage);
            cout<<"For $"<<debt_obligation<<", we have to buy: "<<100*percentage_of_facevalue_to_buy[i]<<"% of Bond "<<i+1<<endl;
        }
        
        cout<<endl;
        cout<<"************************************************************"<<endl;
        cout<<endl;
    }
    
    //    Function which uses gurobi to solve and find the optimal bond portfolio
    void get_optimal_portfolio(vector<float> convexity, vector<float> duration, int no_of_cfs, vector<float> percentage_of_facevalue_to_buy, vector<float>PV, vector<int> maturity, vector<vector<float>> cashflow, float time_when_debt_is_due)
    {
        //        Create an environment
        GRBEnv env = GRBEnv(true);
        env.start();
        
        //        Creating an empty model
        GRBModel model = GRBModel(env);
        
        //        Creating array for lambda
        GRBVar Lambda[no_of_cfs];
        
        //        Assigning a range for each value of Lambda
        for(int i = 0; i<no_of_cfs; i++)
        {
            //            Create variables
            Lambda[i] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "L =");
        }
        
        //        We add our linear constraints
        GRBLinExpr L = 0;
        GRBLinExpr L_D = 0;
        GRBLinExpr C = 0;
        
        //        Loop to calculate the Convexity for a portfolio
        for(int i = 0; i<number_of_cashflows; i++)
        {
            C += Lambda[i]*convexity[i];
            L_D += Lambda[i]*duration[i];
            L += Lambda[i];
        }
        
        //        We set objective of solver maximize convexity
        model.setObjective(C, GRB_MAXIMIZE);
        
        //      Adding limiting constraints
        model.addConstr(L_D == time_when_debt_is_due);
        model.addConstr(L == 1);
        
        //        Optimizing the model
        model.optimize();
        
//        Getting the status of the optimizer
        int optimstatus = model.get(GRB_IntAttr_Status);
        
//        Print solution only if solution found
        if(optimstatus == GRB_OPTIMAL)
        {
            //        Printing the solution for output 3
            cout<<endl;
            cout<<"************************************************************"<<endl;
            cout<<endl;
            cout<<"Largest convextity we can get is "<<model.get(GRB_DoubleAttr_ObjVal)<<endl;
            
            cout<<endl;
            cout<<"************************************************************"<<endl;
            cout<<endl;
            cout<<"To imminize against small changes in r for $1 of Present Value, you should buy: "<<endl;
            
            //        Printing solution
            for(int i=0; i<number_of_cashflows; i++)
            {
                //            getting value of lambda for ith bond
                float lambda = Lambda[i].get(GRB_DoubleAttr_X);
                
                //            printing values only if lambda is non zero
                if(lambda>0)
                    cout<<"$"<<lambda<<" of cashflow "<<i+1<<endl;
            }
            
            cout<<endl;
            cout<<"************************************************************"<<endl;
            cout<<endl;
            
            
            //        Print ctual value to buy now for future debt payment
            cout<<"To imminize against small changes in r for $"<<debt_obligation_amount<<" of Future Debt Obligation, you shouldbuy: "<<endl;
            for(int i=0; i<number_of_cashflows; i++)
            {
                //            Calculate face value of a bond
                float facevalue = cashflow[i][maturity[i]-1] - Cashflow[i][maturity[i]-2];
                
                //            getting value of lambda for ith bond
                float lambda = Lambda[i].get(GRB_DoubleAttr_X);
                
                //            printing values only if lambda is non zero
                if(lambda>0)
                    cout<<"$"<<facevalue<<" * "<<lambda<<"% * "<<percentage_of_facevalue_to_buy[i]<<"% = "<<facevalue*lambda*percentage_of_facevalue_to_buy[i]<<" of cashflow "<<i+1<<endl;
            }
            cout<<endl<<"************************************************************"<<endl;
        }
        else
        {
            cout<<"************************************************************"<<endl;
      
            cout<<"There is no portfolio that meets duration consstraing of "<<time_when_debt_is_due<<"years"<<endl;
            cout<<"************************************************************"<<endl;
        }
        
//      Freeing the environment we started for Gurobi
        void GRBfreeenv(GRBenv *env);
      
    };
    
public:

//    function to call the private member functions to print desired output
    void test()
    {
        ydc_calculator(Cashflow, Maturity, PV);
        
        percentage_calculator(PV, yield_to_maturity, debt_obligation_amount, time_when_debt_is_due, number_of_cashflows);
        
        get_optimal_portfolio(convexity, duration, number_of_cashflows, percentage_of_facevalue_to_buy, PV, Maturity, Cashflow, time_when_debt_is_due);
    }
    
//    function that reads the data from the input file
    void read_bond_data(int argc, char * const argv[])
    {
        string eachrow;
        ifstream input_file(argv[1]);
        
        if(input_file.is_open())
        {
            input_file >> number_of_cashflows;
            
            for(int i = 0; i<number_of_cashflows; i++)
            {
                float x;
                input_file>>x;
                PV.push_back(x);
                
                int y;
                input_file>>y;
                Maturity.push_back(y);
                
                vector<float> inner_cashflow;
                for(int j=0; j<y; j++)
                {
                    float z;
                    input_file>>z;
                    inner_cashflow.push_back(z);
                }
                Cashflow.push_back(inner_cashflow);
            }
            
            input_file>>debt_obligation_amount;
            input_file>>time_when_debt_is_due;
        }
        else
            cout<<"File does not exist in the same directory";
    };
};

#endif /* po_h */
