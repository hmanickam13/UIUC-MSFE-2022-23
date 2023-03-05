#include <iostream>
#include "po.h"

int main (int argc, char * const argv[])
{
    portfolio_optimization x;

    x.read_bond_data(argc, argv);
    x.test();
    
    return 0;
}
