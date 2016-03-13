#include <triekeyiterator.h>

#include <iostream>

int main(int argc, char * argv[])
{
    IntegralKeyIter<int, 16> ikeyIter(0);

    for (; ikeyIter; ++ikeyIter) {
        std::cout << ikeyIter.value() << " ";
    }

    std::cout << "\n";


    StringKeyIter skeyIter("ME");
    for (; skeyIter; ++skeyIter) {
        std::cout << skeyIter.value() << " ";
    }

    std::cout << "\n";
}
