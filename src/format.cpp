#include <string>
#include <vector>

using namespace std;

std::vector<std::string> split(const std::string& str, const std::string delimeter) {
    vector<string> output = vector<string>();
    if (str.find(delimeter) == string::npos) {
        output.push_back(str);
        return output;
    }
    string strcp = str;
    int idx;
    while ( (idx = strcp.find(delimeter)) != string::npos) {
        output.push_back(strcp.substr(0, idx));
        if (idx + 1 < strcp.length()) {
            strcp = strcp.substr(idx+1);
        } else {
            strcp = "";
        }
    }
    return output;
}

std::string join(const std::vector<std::string>& vec, const std::string delimeter) {
    if (vec.size() < 1) return "";
    string output = vec[0];
    for (int i = 1; i < vec.size(); i++) {
        output += delimeter + vec[i];
    }
    return output;
}

std::string join(const std::string arr[], int size, const std::string delimeter) {
    if (size < 1) return "";
    string output = arr[0];
    for (int i = 1; i < size; i++) {
        output += delimeter + arr[i];
    }
    return output;
}
