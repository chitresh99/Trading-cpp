#include <iostream>
using namespace std;


// T is the place holder for any data type
template <typename T>
T add(T a, T b){
    return a + b;
}

int main() {
    cout<<add(3,4)<<endl;
    cout<<add(2.5,3.5)<<endl;
    return 0;
}