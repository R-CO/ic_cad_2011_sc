#ifndef IC_CAD_2011_SC_LITTLE_TOOLS_HPP
#define IC_CAD_2011_SC_LITTLE_TOOLS_HPP

#include <string>

size_t CountComma(const std::string &str);
std::string IntToString(const int number);
size_t CountSpace(const std::string &str);
void GetFromTo(const std::string &str, int &from, int &to);

void TrimLeft(std::string &str, const std::string &char_list);
void TrimRight(std::string &str, const std::string &char_list);
void Trim(std::string &str, const std::string &char_list);

#endif  // end of define IC_CAD_2011_SC_LITTLE_TOOLS_HPP
