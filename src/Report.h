
#ifndef __REPORT_H
#define __REPORT_H

struct REP {
  vector<string> str;
} Report[10];

ofstream fout;

void print_Report();

void print_Report() {
  unsigned int i;

  cout << "It is printing result!!" << endl;

  if (Report[0].str.size() > 0) {
    fout << "[ISO_1] [Power of isolation control pin can be OFF while "
            "isolation cell is ON] [Occurrence:"
         << Report[0].str.size() << "]" << endl;
    for (i = 0; i < Report[0].str.size(); i++) {
      fout << "\t-\t[#" << i + 1 << "] " << Report[0].str[i] << endl;
    }
  }

  if (Report[1].str.size() > 0) {
    fout << "[ISO_2] [Source and destination domains in this path are the "
            "same] [Occurrence:"
         << Report[1].str.size() << endl;
    for (i = 0; i < Report[1].str.size(); i++) {
      fout << "\t-\t[#" << i + 1 << "] " << Report[1].str[i] << endl;
    }
  }

  if (Report[2].str.size() > 0) {
    fout << "[ISO_3] [Isolation instance location does not match isolation "
            "rule] [Occurrence:"
         << Report[2].str.size() << endl;
    for (i = 0; i < Report[2].str.size(); i++) {
      fout << "\t-\t[#" << i + 1 << "] " << Report[2].str[i] << endl;
    }
  }

  if (Report[3].str.size() > 0) {
    fout << "[ISO_4] [Power domain crossing does not have user-defined "
            "isolation cell] [Occurrence:"
         << Report[3].str.size() << "]" << endl;
    for (i = 0; i < Report[3].str.size(); i++) {
      fout << "\t-\t[#" << i + 1 << "] " << Report[3].str[i] << endl;
    }
  }

  if (Report[4].str.size() > 0) {
    fout << "[ISO_5] [Isolation cell control pin is not connected to the "
            "specified signal] [Occurrence:"
         << Report[4].str.size() << "]" << endl;
    for (i = 0; i < Report[4].str.size(); i++) {
      fout << "\t-\t[#" << i + 1 << "] " << Report[4].str[i] << endl;
    }
  }

  if (Report[5].str.size() > 0) {
    fout << "[ISO_6] [Isolation instance control polarity is different than "
            "that specified by isolation rule] [Occurrence:"
         << Report[5].str.size() << "]" << endl;
    for (i = 0; i < Report[5].str.size(); i++) {
      fout << "\t-\t[#" << i + 1 << "] " << Report[5].str[i] << endl;
    }
  }

  if (Report[6].str.size() > 0) {
    fout << "[LS_1] [Level shifter input voltage value is different than "
            "specified by level shifter definition] [Occurrence:"
         << Report[6].str.size() << "]" << endl;
    for (i = 0; i < Report[6].str.size(); i++) {
      fout << "\t-\t[#" << i + 1 << "] " << Report[6].str[i] << endl;
    }
  }

  if (Report[7].str.size() > 0) {
    fout << "[LS_2] [Level shifter output voltage value is different than "
            "specified by level shifter definition] [Occurrence:"
         << Report[7].str.size() << "]" << endl;
    for (i = 0; i < Report[7].str.size(); i++) {
      fout << "\t-\t[#" << i + 1 << "] " << Report[7].str[i] << endl;
    }
  }

  if (Report[8].str.size() > 0) {
    fout << "[LS_3] [Power domain crossing does not have user-defined level "
            "shifter cell] [Occurrence:"
         << Report[8].str.size() << "]" << endl;
    for (i = 0; i < Report[8].str.size(); i++) {
      fout << "\t-\t[#" << i + 1 << "] " << Report[8].str[i] << endl;
    }
  }

  if (Report[9].str.size() > 0) {
    fout << "[LS_4] [Source and receiver signal voltages of level shifter cell "
            "are the same in this path] [Occurrence:"
         << Report[9].str.size() << "]" << endl;
    for (i = 0; i < Report[9].str.size(); i++) {
      fout << "\t-\t[#" << i + 1 << "] " << Report[9].str[i] << endl;
    }
  }

  fout.close();
}

#endif
