#include <trieset.h>
#include <triekeyiterator.h>

#include <string>
#include <cstdlib>

int main(int argc, char * argv[])
{
    TrieSet<std::string, StringKeyIter> trieSet;

    std::string cmd;
    std::string str;
    for (;;) {
        std::cout << "> ";
        std::cin >> cmd;
        if (cmd == "insert") {                 // insert
            std::cin >> str;
            trieSet.insert(str);
            std::cout << "OK. Size = " 
                      << trieSet.size() 
                      << "\n";
        } else if (cmd == "erase") {          // erase
            std::cin >> str;            
            trieSet.erase(str);
            std::cout << "OK. Size = " 
                      << trieSet.size() 
                      << "\n";
        } else if (cmd == "size") {           // size
            std::cout << trieSet.size() 
                      << "\n";
        } else if (cmd == "count") {          // count
            std::cin >> str;
            std::cout << trieSet.count(str) 
                      << "\n";
        } else if (cmd == "clear") {
            trieSet.clear();
            std::cout << "OK. Size = " 
                      << trieSet.size() 
                      << "\n";
        } else if (cmd == "quit") {           // quit
            exit(0);
        } else {
            std::cout << "Invalid command.\n";
        }
    }
}
