#include "server.hpp"
#include <thread>
#include <chrono>

int main()
{
    Server server(5000);

    std::thread serverThread([&server]() {
           
        server.Start();        
    });
    
    std::this_thread::sleep_for(std::chrono::seconds(15));
    server.Stop();


    serverThread.join();

    return 0;
}