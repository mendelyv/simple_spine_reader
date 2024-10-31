#ifndef __MY_LOGS_H__
#define __MY_LOGS_H__

#include <string>
#include <vector>

class Logs {
    protected:
        std::vector<std::string> datas;
    public:
        bool silent;
        void log(std::string msg);
        void warn(std::string msg);
        void error(std::string msg);
        void print(const std::string& msg);
        bool save();
};

#endif // !__MY_LOGS_H__
