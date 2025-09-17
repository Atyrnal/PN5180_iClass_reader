
#include <string>
#include <vector>

template<typename ... Args>
std::string format(const std::string& format, Args ... args);

std::vector<std::string> split(const std::string& str, const std::string delimeter);

std::string join(const std::vector<std::string>& vec, const std::string delimeter);