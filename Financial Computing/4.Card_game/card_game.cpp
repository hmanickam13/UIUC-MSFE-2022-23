#include <iostream>
#include <algorithm>
#include <vector>

#include "card_game.h"

using std::vector;
using namespace std;

int main(int argc, char* const argv[])
{
    int no_of_cards;
    Card c;
    
    sscanf(argv[1], "%d", &no_of_cards);
    c.che(no_of_cards);
 
    return 0;
}