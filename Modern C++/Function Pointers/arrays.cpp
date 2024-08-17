#include <iostream>
using namespace std;

int main()
{

    int integer_array[6] = {1, 2, 3, 4, 5, 6};

    cout << integer_array[2] << endl;

    int another_array[4];
    another_array[0] = 9;
    another_array[1] = 19;

    *another_array = 29;
    cout << another_array[1] << endl;

    int *ap = another_array;
    int hello = *ap + 1;
    cout <<hello <<endl;
    return 0;
}