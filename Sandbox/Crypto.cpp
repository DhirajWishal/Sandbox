#include <iostream>
#include <memory>
#include "Timer.h"

class S {
public:
    S() { std::cout << "Constructor\n"; }
    ~S() { std::cout << "Destructor\n"; }
};

//int main() {
//    S* arr = new S[4];
//    {
//        std::unique_ptr<S> ptr = std::unique_ptr<S>(arr);
//        int x = 10;
//    }
//}

const int Consts::x = 100;