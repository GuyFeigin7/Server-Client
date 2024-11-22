#include "user_data.hpp"

#include <iostream>
#include <algorithm>

UserData::UserData(const std::string &str) : data(str)
{
    std::cout << "UserData initialized with: " << data << std::endl;
}

void UserData::Sort()
{
    std::lock_guard<std::mutex> lock(dataLock);
    
    std::cout << "UserData::Sort called." << std::endl;
    std::cout << "Data before sort: " << data << std::endl; // Add this line
    std::sort(data.begin(), data.end());
    std::cout << "Data after sort: " << data << std::endl;  // Add this line
}

void UserData::AppendStr(const std::string &strToAppend)
{
    std::lock_guard<std::mutex> lock(dataLock);

    std::cout << "UserData::AppendStr called." << std::endl;

    data += strToAppend;
}

std::string UserData::GetStr() const
{
    std::lock_guard<std::mutex> lock(dataLock);

    return data;
}

void UserData::RemoveChars(unsigned int n)
{
    std::lock_guard<std::mutex> lock(dataLock);

    std::cout << "UserData::RemoveChars called." << std::endl;

    data.erase(0, n);
}
