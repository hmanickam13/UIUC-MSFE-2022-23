#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include<numeric>

using namespace std;

class stable_marriage_instance
{
    // Private
    vector <vector <int> > Preference_of_men;
    vector <vector <int> > Preference_of_women;
    vector <int> match_for_men;
    vector <int> match_for_women;       
    int no_of_couples;

    // private member function: checks if anybody is free in boolean "my_array"
    // returns the index of the first-person who is free in "my_array"
    // if no one is free it returns a -1.
    int anybody_free(vector <bool> my_array)
    {   
        int x = -1;
        for(int i=0; i<no_of_couples; i++)
        {
            if(my_array[i])
                x = i;;
        } 
        return x;
    };
    
    // private member function: if index1 is ranked higher than index2
    // in a preference array called "my_array" it returns "true"; otherwise
    // it returns "false"
    bool rank_check (vector <int> my_array, int index1, int index2)
    {   
        int a,b = 0;

        for(int i = 0; i< no_of_couples; i++)
        {    if(my_array[i]==index1)
                a = i;
        }
        for(int j = 0; j< no_of_couples; j++)
        {
            if(my_array[j]==index2)
                b = j;
        }
        if(a<b)
            return true;
        else
            return false;
    };
    
    // private member function: implements the Gale-Shapley algorithm
    void Gale_Shapley()
    {
        vector <bool> is_man_free;
        vector <bool> is_woman_free;
        vector <vector <bool> > has_this_man_proposed_to_this_woman;
        
        int man_index, woman_index;
        
        // initializing everything
        for (int i = 0; i < no_of_couples; i++)
        {
            is_man_free.push_back(true);
            is_woman_free.push_back(true);

            match_for_men.resize(no_of_couples);
            match_for_women.resize(no_of_couples);

            vector<bool> row;
            for (int j = 0;j < no_of_couples; j++)
                row.push_back(false);
            
            has_this_man_proposed_to_this_woman.push_back(row);
        }

        // Gale-Shapley Algorithm
        while ((man_index = anybody_free(is_man_free)) >= 0)
        {   
            int woman_index;
            for(int j = 0; j<no_of_couples; j++)
            {   
                int x = Preference_of_men[man_index][j];

                if(!has_this_man_proposed_to_this_woman[man_index][x])
                {   
                    woman_index = x;
                    has_this_man_proposed_to_this_woman[man_index][x] = true;
                    break;
                }
            }
            
            if(is_woman_free[woman_index])
            {   
                match_for_men[man_index] = woman_index;
                match_for_women[woman_index] = man_index;

                is_man_free[man_index] = false;
                is_woman_free[woman_index] = false;
            }
            else
            {
                int fiance = match_for_women[woman_index];

                if(rank_check(Preference_of_women[woman_index], man_index, fiance))
                {
                    match_for_men[man_index] = woman_index;
                    match_for_women[woman_index] = man_index;
                    is_man_free[fiance] = true;
                    is_man_free[man_index] = false;
                    is_woman_free[woman_index] = false;
                    match_for_men[fiance] = 0;       
                }
                else
                    is_man_free[man_index] = true;
            }
        }          
    }
    
    // private member function: reads data
    void read_data(int argc, const char * argv[])
    {
        ifstream input_file(argv[1]);

        int x;

        if(input_file.is_open())
        {
            string str;
            input_file >> x;
            no_of_couples = x;
            cout << "Number of Couples = " << no_of_couples << endl;

            for(int i =0; i<no_of_couples;i++)
            {
                vector<int> row;
                
                for(int j = 0; j<no_of_couples; j++)
                {    
                    input_file>>x;
                    row.push_back(x);
                }
                Preference_of_men.push_back(row);
            }
            for(int i =0; i<no_of_couples;i++)
            {
                vector<int> row;
                
                for(int j = 0; j<no_of_couples; j++)
                {    
                    input_file>>x;
                    row.push_back(x);
                }
                Preference_of_women.push_back(row);
            }
        }
        else
            cout<<"File does not exist in the same directory";
       
        cout<<"Preference of men: "<<endl;        
        for(int i = 0; i<no_of_couples; i++)
        {   
            cout<<" Man "<<i<<"  : ";
            for(int j = 0; j<no_of_couples; j++)
                cout<<Preference_of_men[i][j]<<" ";
            cout<<endl;
        }
        
        cout<<endl<<"Preference of women: "<<endl;        
        for(int i = 0; i<no_of_couples; i++)
        {   
            cout<<"Woman "<<i<<" : ";
            for(int j = 0; j<no_of_couples; j++)
                cout<<Preference_of_women[i][j]<<" ";
            cout<<endl;
        }
    };
    
    // private member function: print solution
    void print_soln()
    {
        cout<<endl<<"Matching for Men"<<endl;
        for(int i = 0; i<no_of_couples; i++)
            cout<<"Man: "<<i<<" --> Woman: "<<match_for_men[i]<<endl;

        cout<<endl<<"Matching for Women"<<endl;
        for(int i = 0; i<no_of_couples; i++)
            cout<<"Woman: "<<i<<" --> Man: "<<match_for_women[i]<<endl;
        // write the appropriate code here
    };
    
 public:
    void solve_it(int argc, const char * argv[])
    {
        read_data(argc, argv);

        Gale_Shapley();

        print_soln();
    };
};

int main(int argc, const char * argv[])
{
    stable_marriage_instance x;
    x.solve_it(argc, argv);
};
