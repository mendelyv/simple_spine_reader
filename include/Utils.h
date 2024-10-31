#ifndef __MY_UTILS_H__
#define __MY_UTILS_H__

#include <string>
class Utils {
    public:
        static std::string get_current_time_str(std::string format = "%Y-%m-%d_%H-%M-%S");
};

#endif // !__MY_UTILS_H__
