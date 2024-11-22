#include "client.hpp"

#include <iostream>
#include <string>

int main()
{
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
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}