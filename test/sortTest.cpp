#include <iostream>
#include <string>
#include <algorithm>

int main()
{
    std::string str("dbace");
    std::sort(str.begin(), str.end());
    std::cout << str << std::endl;

    return 0;
}