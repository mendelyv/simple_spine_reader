#include "../include/Utils.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

std::string Utils::get_current_time_str(std::string format) {
    std::time_t now = std::time(nullptr);
    std::tm localTime;
#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif
    std::ostringstream oss;
    oss << std::put_time(&localTime, format.c_str());
    return oss.str();
}
