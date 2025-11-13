#include <iostream>
using namespace std;

// Constant declarations
const int MAX = 100;
const float PI = 3.14;
const string greeting = "Hello World";

// Normal identifier declarations
int counter;
float area;

// Valid assignments
counter = MAX;
area = counter * PI;

int temp = 20;
int @#hi=27;      //invalid identifier

int main() {
    cout << greeting << endl;
    if (area > 50.0) {
        cout << "Large area\n";
    }
    return 0;
}
