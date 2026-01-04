#include <iostream>
#include <vector>

int main() {
    // Declare a vector of integers
    std::vector<int> arr;
    int numberOfElements;

    // Asking the user how many elements they want to input
    std::cout << "Enter the number of elements: ";
    std::cin >> numberOfElements;

    // Take input from the user
    std::cout << "Enter " << numberOfElements << " integers:" << std::endl;
    for (int i = 0; i < numberOfElements; i++) {
        int element;
        std::cin >> element;
        arr.push_back(element); // Add the element to the vector
    }

    // Print the elements
    std::cout << "You entered: ";
    for (int i = 0; i < arr.size(); i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
