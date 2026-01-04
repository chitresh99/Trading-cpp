#include <iostream>
using namespace std;

int main() {
    int life; //const is a qualifier
    
    life = 4;

    int card;
    card = 40;
    int my_card = card;
    
    //pointer points to something -> It can only point to a memory
    int *myp;
    myp = &my_card; //this pointing to the memory address of card
    
    my_card = *myp;
    
    int hello = *myp;

    cout <<"Value of card id "<<my_card<<endl;
    cout <<"Value of card id "<<hello<<endl;


    return 0;
}