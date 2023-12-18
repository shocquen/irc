#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

std::string to_string(const int &n) {
  std::stringstream ss;
  ss << n;
  return ss.str();
}

std::vector<std::string> split(std::string str, std::string delim) {
  std::vector<std::string> ret;
  if (str.empty())
    return ret;

  size_t start = 0;
  size_t i = str.find(delim, start);
  for (;i != str.npos; i = str.find(delim, start)) {
    size_t len = i - start;
    ret.push_back(str.substr(start, len));
    start += (len + delim.size());
  }
  ret.push_back(str.substr(start));

  return ret;
}
