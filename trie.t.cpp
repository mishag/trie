#include "trie.h"

#include <string>
#include <memory>
#include <iostream>

typedef std::shared_ptr<std::string> StringPtr;

int main(int argc, char * argv[])
{
    TrieMap<std::string> tmap;

    StringPtr valPtr(new std::string("world"));

    tmap.insert("hello", valPtr);

    valPtr = tmap.find("hello");

    assert(valPtr);
    // std::cout << "hello " << *valPtr << "\n";
    assert(*valPtr == "world");

    valPtr = tmap.find("hi");
    assert(!valPtr);

    valPtr = tmap.find("hell");
    assert(!valPtr);

    assert(tmap.count("hello") == 1);
    assert(tmap.count("hell") == 0);
    assert(tmap.count("bye") == 0);

    StringPtr anotherVal(new std::string("universe"));
    StringPtr val = tmap.insert("hello", anotherVal);
    assert(*val == "world");

    val = tmap.insert("hello", anotherVal, true);
    assert(*val == "universe");
    val = tmap.find("hello");
    assert(*val == "universe");

    val.reset(new std::string("on wheels"));
    tmap.insert("hell", val);
    val.reset(new std::string("vibes"));
    tmap.insert("good", val);

    TrieMap<std::string>::ConstIterator it(tmap);
    // std::cout << it.key() << " => " << *(it.value()) << "\n";
    // ++it;
    // std::cout << it.key() << " => " << *(it.value()) << "\n";

    for (; it; ++it) {
        std::cout << it.key() << " => " << *(it.value()) << "\n";
    }

    TrieMap<std::string>::ConstIterator it1(tmap, "he");
    for (; it1; ++it1) {
        std::cout << it1.key() << " => " << *(it1.value()) << "\n";
    }

    return 0;
}
