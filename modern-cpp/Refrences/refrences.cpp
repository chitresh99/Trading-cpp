#include <iostream>
using namespace std;

int main()
{

    int score = 200;
    int *myp = &score;
    int hello = *myp;
    
    
    cout<<hello<<endl;
    
    //a memory address needs a refrence  

    int &another_score = score;
    another_score = 800;

    cout <<score<<endl;
    
    //any update you are making to the refrence you will be making change to the actual value
    //pointers are easy they point to the same value 
    //refrences are tricky
    //when a refrence is pointing to any variable whether it's int ,double etc 
    //any change in that will change the reference 

    return 0;
}