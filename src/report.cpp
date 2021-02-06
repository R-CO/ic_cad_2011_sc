#include "report.hpp"

#include <fstream>
using std::ofstream;
#include <iostream>
using std::cout;
using std::endl;
#include <string>
using std::string;

#include <vector>
using std::vector;

Report g_report[10];

ofstream g_fout;

void printReport() {
  unsigned int i;

  cout << "It is printing result!!" << endl;

  if (g_report[0].str.size() > 0) {
    g_fout << "[ISO_1] [Power of isolation control pin can be OFF while "
              "isolation cell is ON] [Occurrence:"
           << g_report[0].str.size() << "]" << endl;
    for (i = 0; i < g_report[0].str.size(); i++) {
      g_fout << "\t-\t[#" << i + 1 << "] " << g_report[0].str[i] << endl;
    }
  }

  if (g_report[1].str.size() > 0) {
    g_fout << "[ISO_2] [Source and g_destination domains in this path are the "
              "same] [Occurrence:"
           << g_report[1].str.size() << endl;
    for (i = 0; i < g_report[1].str.size(); i++) {
      g_fout << "\t-\t[#" << i + 1 << "] " << g_report[1].str[i] << endl;
    }
  }

  if (g_report[2].str.size() > 0) {
    g_fout << "[ISO_3] [Isolation instance location does not match isolation "
              "rule] [Occurrence:"
           << g_report[2].str.size() << endl;
    for (i = 0; i < g_report[2].str.size(); i++) {
      g_fout << "\t-\t[#" << i + 1 << "] " << g_report[2].str[i] << endl;
    }
  }

  if (g_report[3].str.size() > 0) {
    g_fout << "[ISO_4] [Power domain crossing does not have user-defined "
              "isolation cell] [Occurrence:"
           << g_report[3].str.size() << "]" << endl;
    for (i = 0; i < g_report[3].str.size(); i++) {
      g_fout << "\t-\t[#" << i + 1 << "] " << g_report[3].str[i] << endl;
    }
  }

  if (g_report[4].str.size() > 0) {
    g_fout << "[ISO_5] [Isolation cell control pin is not connected to the "
              "specified signal] [Occurrence:"
           << g_report[4].str.size() << "]" << endl;
    for (i = 0; i < g_report[4].str.size(); i++) {
      g_fout << "\t-\t[#" << i + 1 << "] " << g_report[4].str[i] << endl;
    }
  }

  if (g_report[5].str.size() > 0) {
    g_fout << "[ISO_6] [Isolation instance control polarity is different than "
              "that specified by isolation rule] [Occurrence:"
           << g_report[5].str.size() << "]" << endl;
    for (i = 0; i < g_report[5].str.size(); i++) {
      g_fout << "\t-\t[#" << i + 1 << "] " << g_report[5].str[i] << endl;
    }
  }

  if (g_report[6].str.size() > 0) {
    g_fout << "[LS_1] [Level shifter input voltage value is different than "
              "specified by level shifter definition] [Occurrence:"
           << g_report[6].str.size() << "]" << endl;
    for (i = 0; i < g_report[6].str.size(); i++) {
      g_fout << "\t-\t[#" << i + 1 << "] " << g_report[6].str[i] << endl;
    }
  }

  if (g_report[7].str.size() > 0) {
    g_fout << "[LS_2] [Level shifter output voltage value is different than "
              "specified by level shifter definition] [Occurrence:"
           << g_report[7].str.size() << "]" << endl;
    for (i = 0; i < g_report[7].str.size(); i++) {
      g_fout << "\t-\t[#" << i + 1 << "] " << g_report[7].str[i] << endl;
    }
  }

  if (g_report[8].str.size() > 0) {
    g_fout << "[LS_3] [Power domain crossing does not have user-defined level "
              "shifter cell] [Occurrence:"
           << g_report[8].str.size() << "]" << endl;
    for (i = 0; i < g_report[8].str.size(); i++) {
      g_fout << "\t-\t[#" << i + 1 << "] " << g_report[8].str[i] << endl;
    }
  }

  if (g_report[9].str.size() > 0) {
    g_fout
        << "[LS_4] [Source and receiver signal voltages of level shifter cell "
           "are the same in this path] [Occurrence:"
        << g_report[9].str.size() << "]" << endl;
    for (i = 0; i < g_report[9].str.size(); i++) {
      g_fout << "\t-\t[#" << i + 1 << "] " << g_report[9].str[i] << endl;
    }
  }

  g_fout.close();
}