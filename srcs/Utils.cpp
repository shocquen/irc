#include <sstream>
#include <string>

std::string to_string(const int &n) {
  std::stringstream ss;
  ss << n;
  return ss.str();
}

