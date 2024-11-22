#include "client.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

int main()
{
    try
    {
        uid_t userID = 1000;

        Client client("127.0.0.1", 5000, userID);
        client.SendData("mynameisguy");
        std::cout << "\n";

        client.SendData("mynameisnotjohnny");
        std::string res = client.ReceiveData(5);
        std::cout << "Received data: " << res << std::endl;
        std::cout << "\n";

        res = client.ReceiveData(10);
        std::cout << "Received data: " << res << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}