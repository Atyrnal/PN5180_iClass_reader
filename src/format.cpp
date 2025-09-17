#include <string>
#include <vector>

using namespace std;


template<typename ... Args>
string format(const string& format, Args ... args) {
  int size_s = snprintf( nullptr, 0, format.c_str(), args ... ) + 1;
  if( size_s <= 0 ) { throw runtime_error( "Error during formatting." ); }
  auto size = static_cast<size_t>( size_s );
  unique_ptr<char[]> buf( new char[ size ] );
  snprintf( buf.get(), size, format.c_str(), args ... );
  return string( buf.get(), buf.get() + size - 1 );
}

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