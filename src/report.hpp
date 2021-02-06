#ifndef IC_CAD_2011_SC_REPORT_HPP
#define IC_CAD_2011_SC_REPORT_HPP

#include <fstream>
#include <string>
#include <vector>

struct Report {
  std::vector<std::string> str;
};

extern Report g_report[10];

extern std::ofstream g_fout;

void printReport();

#endif  // end of define IC_CAD_2011_SC_REPORT_HPP
