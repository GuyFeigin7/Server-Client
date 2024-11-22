#include "client.hpp"

#include <iostream>
#include <string>

bool Test1()
{
    bool success = false;

    try
    {
        uid_t userID = getuid();

        Client client("127.0.0.1", 5000, userID);
        client.SendData("hello");
        std::string res = client.ReceiveData(5);
        std::cout << "Received data: " << res << std::endl;
        std::cout << "\n";

        client.SendData("hello");
        res = client.ReceiveData(2);
        std::cout << "Received data: " << res << std::endl;
        std::cout << "\n";

        client.SendData("world");
        res = client.ReceiveData(8);
        std::cout << "Received data: " << res << std::endl;

        res = client.ReceiveData(5);

        success = true;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return success;
}

bool Test2()
{
    bool success = false;

    try
    {
        uid_t userID = getuid();

        Client client("127.0.0.1", 5000, userID);
        client.SendData("thequickbrownfoxjumpsoverthelazydog");
        std::cout << client.ReceiveData(10) << "\n";
        std::cout << client.ReceiveData(5) << "\n";
        std::cout << client.ReceiveData(5) << "\n";

        success = true;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return success;
}

int main()
{
    
    /* 
    std::cout << "Running Test1...\n\n";
    
    if (Test1() == false)
    {
        std::cout << "Test1 failed.\n";
        return -1;
    }
    else
    {
        std::cout << "Test1 passed.\n";
    } */

    std::cout << "Running Test2...\n\n";
    if (Test2() == false)
    {
        std::cout << "Test2 failed.\n";
        return -1;
    }
    else
    {
        std::cout << "Test2 passed.\n";
    }

    return 0;
}