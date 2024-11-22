#ifndef GF_USER_DATA
#define GF_USER_DATA

#include <string>
#include <mutex>

class UserData
{
public:
    UserData(const std::string& str = "");
    ~UserData() = default;

    void Sort();
    void AppendStr(const std::string& strToAppend);
    std::string GetStr() const;
    void RemoveChars(unsigned int n);

private:
    mutable std::mutex dataLock;
    std::string data;
};


#endif //GF_USER_DATA