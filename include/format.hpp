
#include <string>
#include <vector>
#include <stdexcept>
#include <memory>

using namespace std;

template<typename ... Args>
std::string format(const std::string& format, Args ... args) {
  int size_s = snprintf( nullptr, 0, format.c_str(), args ... ) + 1;
  if( size_s <= 0 ) { throw runtime_error( "Error during formatting." ); }
  auto size = static_cast<size_t>( size_s );
  unique_ptr<char[]> buf( new char[ size ] );
  snprintf( buf.get(), size, format.c_str(), args ... );
  return string( buf.get(), buf.get() + size - 1 );
};

std::vector<std::string> split(const std::string& str, const std::string delimeter);

std::string join(const std::vector<std::string>& vec, const std::string delimeter);

std::string join(const std::string arr[], int size, const std::string delimeter);