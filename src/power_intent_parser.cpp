#include "power_intent_parser.hpp"

#include <cstdlib>
#include <cstring>
#include <fstream>
using std::ifstream;
#include <iostream>
using std::cerr;
using std::endl;
#include <string>
using std::string;

#include <map>
using std::map;
#include <vector>
using std::vector;

#include "data_structure_define.hpp"
#include "little_tools.hpp"

bool ReadPowerFile(const char *, vector<string> &);
void ParsePowerFile(const vector<string> &, map<string, Module> &);
void CPD_Handle(const string &);  // create_power_domain handle
void CIR_Handle(const string &);  // create_isolation_rule handle
void DIC_Handle(const string &,
                map<string, Module> &);  // define_isolation_cell handle
void CLSR_Handle(const string &);        // create_level_shifter_rule handle
void DLSC_Handle(const string &);        // define_level_shifter_cell handle
void CNC_Handle(const string &);         // create_nominal_condition handle
void CPM_Handle(const string &);         // create_power_mode handle

/*** global variables ***/
map<string, C_P_D>
    createPowDomain;  // using the power domain name to be the index
map<string, C_I_R> createIsoRule;  // using from + to to be the index
map<string, D_I_C>
    defineIsoCell;  // using the isolation cell name to be the index
map<string, C_L_S_R> createLevShRule;  // using from + to to be the index
map<string, D_L_S_C>
    defineLevShCell;  // using the level shifter cell name to be the index
map<string, C_N_C>
    createNomCondition;  // using the nominal condition name to be the index
vector<C_P_M> createPowMode;

/*** default power domain & default power mode ***/
string defaultDomain;
string defaultMode;

bool ReadPowerFile(const char *fileName, vector<string> &powerStatement) {
  /*** in this function, it will combine the same statement
       that cross multiple line into one line ***/
  string statement;
  size_t staCount = 0;
  size_t pos;

  ifstream inFile(fileName, std::ios::in);
  if (!inFile) {
    return false;
  }

  powerStatement.push_back(string());
  while (getline(inFile, statement)) {
    powerStatement[staCount] += statement;
    powerStatement[staCount] += " ";
    pos = statement.find("\\");
    if (pos != string::npos && (statement.find("#") > pos)) {
      powerStatement[staCount][powerStatement[staCount].find("\\")] = ' ';
    } else {
      staCount++;
      powerStatement.push_back(string());
    }
  }
  return true;
}

void ParsePowerFile(const vector<string> &powerStatement,
                    map<string, Module> &moduleMap) {
  size_t i;

  for (i = 0; i != powerStatement.size(); i++) {
    if (powerStatement[i].find("define_isolation_cell") != string::npos) {
      DIC_Handle(powerStatement[i], moduleMap);
    } else if (powerStatement[i].find("create_power_domain") != string::npos) {
      CPD_Handle(powerStatement[i]);
    } else if (powerStatement[i].find("create_isolation_rule") !=
               string::npos) {
      CIR_Handle(powerStatement[i]);
    } else if (powerStatement[i].find("create_level_shifter_rule") !=
               string::npos) {
      CLSR_Handle(powerStatement[i]);
    } else if (powerStatement[i].find("define_level_shifter_cell") !=
               string::npos) {
      DLSC_Handle(powerStatement[i]);
    } else if (powerStatement[i].find("create_nominal_condition") !=
               string::npos) {
      CNC_Handle(powerStatement[i]);
    } else if (powerStatement[i].find("create_power_mode") != string::npos) {
      CPM_Handle(powerStatement[i]);
    } else if (powerStatement[i] != " " && powerStatement[i] != "") {
      cerr << "Warning: this program can not handle this:" << powerStatement[i]
           << "***" << endl;
    }
  }
}

void CPD_Handle(const string &powerStatement) {
  size_t begin, end;
  size_t subBegin, subEnd;
  size_t spaceCount;
  size_t i;
  string tempString;
  string subStatement;
  const string command("create_power_domain");
  C_P_D tempCPD;

  tempCPD.defDomain = false;
  begin = powerStatement.find(command);
  begin = powerStatement.find_first_not_of(" \t", begin + command.size());
  end = powerStatement.find_first_of(" \t", begin);
  tempString = powerStatement.substr(begin, end - begin);
  while (true) {
    if (tempString == "-name") {
      begin = powerStatement.find_first_not_of(" \t{", end);
      end = powerStatement.find_first_of(" \t}", begin);
      tempString = powerStatement.substr(begin, end - begin);
      tempCPD.name = tempString;
    } else if (tempString == "-instances") {
      begin = powerStatement.find_first_not_of(" \t", end);
      end = powerStatement.find_first_of(" \t", begin);
      tempString = powerStatement.substr(begin, end - begin);
      if (tempString.find('{') != string::npos) {
        begin = powerStatement.find_first_not_of(" \t{", end);
        end = powerStatement.find_first_of('}', begin);
        subStatement = powerStatement.substr(begin, end - begin);
        subEnd = subStatement.find_last_not_of(" \t");
        subStatement = subStatement.substr(0, subEnd + 1);
        spaceCount = CountSpace(subStatement);
        subEnd = 0;
        for (i = 0; i < spaceCount; i++) {
          subBegin = subStatement.find_first_not_of(" \t", subEnd);
          subEnd = subStatement.find_first_of(" \t", subBegin);
          tempString = subStatement.substr(subBegin, subEnd - subBegin);
          tempCPD.instances.push_back(tempString);
        }
        subBegin = subStatement.find_first_not_of(" \t", subEnd);
        tempString = subStatement.substr(subBegin);
        tempCPD.instances.push_back(tempString);
      } else {
        tempCPD.instances.push_back(tempString);
      }
    } else if (tempString == "-boundary_ports") {
      begin = powerStatement.find_first_not_of(" \t", end);
      end = powerStatement.find_first_of(" \t", begin);
      tempString = powerStatement.substr(begin, end - begin);
      if (tempString.find('{') != string::npos) {
        begin = powerStatement.find_first_not_of(" \t{", begin);
        end = powerStatement.find_first_of('}', begin);
        subStatement = powerStatement.substr(begin, end - begin);
        subEnd = subStatement.find_last_not_of(" \t");
        subStatement = subStatement.substr(0, subEnd + 1);
        spaceCount = CountSpace(subStatement);
        subEnd = 0;
        for (i = 0; i < spaceCount; i++) {
          subBegin = subStatement.find_first_not_of(" \t", subEnd);
          subEnd = subStatement.find_first_of(" \t", subBegin);
          tempString = subStatement.substr(subBegin, subEnd - subBegin);
          tempCPD.ports.push_back(tempString);
        }
        subBegin = subStatement.find_first_not_of(" \t", subEnd);
        tempString = subStatement.substr(subBegin);
        tempCPD.ports.push_back(tempString);
      } else {
        tempCPD.ports.push_back(tempString);
      }
    } else if (tempString == "-default") {
      tempCPD.defDomain = true;
    }
    begin = powerStatement.find_first_not_of(" \t}", end);
    if (begin == string::npos) {
      break;
    }
    end = powerStatement.find_first_of(" \t", begin);
    tempString = powerStatement.substr(begin, end - begin);
  }
  if (tempCPD.defDomain == true) {
    defaultDomain = tempCPD.name;
  }
  createPowDomain[tempCPD.name] = tempCPD;
}

void DIC_Handle(const string &powerStatement, map<string, Module> &moduleMap) {
  size_t begin, end;
  string tempString;
  const string command("define_isolation_cell");
  D_I_C tempDIC;
  map<string, Unit>::iterator it;

  begin = powerStatement.find(command);
  begin = powerStatement.find_first_not_of(" \t", begin + command.size());
  end = powerStatement.find_first_of(" \t", begin);
  tempString = powerStatement.substr(begin, end - begin);
  while (true) {
    if (tempString == "-cells") {
      begin = powerStatement.find_first_not_of(" \t{", end);
      end = powerStatement.find_first_of(" \t}", begin);
      tempString = powerStatement.substr(begin, end - begin);
      tempDIC.cell = tempString;
      /*** to know the gate type of the isolation cell ***/
      it = moduleMap[tempString].units.begin();
      if (it->second.unitType == "and" || it->second.unitType == "AND") {
        tempDIC.gateType = IsoGateType::AND;
      } else if (it->second.unitType == "or" || it->second.unitType == "OR") {
        tempDIC.gateType = IsoGateType::OR;
      } else if (it->second.unitType == "nand" ||
                 it->second.unitType == "NAND") {
        tempDIC.gateType = IsoGateType::NAND;
      } else if (it->second.unitType == "nor" || it->second.unitType == "NOR") {
        tempDIC.gateType = IsoGateType::NOR;
      }
    } else if (tempString == "-enable") {
      begin = powerStatement.find_first_not_of(" \t{", end);
      end = powerStatement.find_first_of(" \t}", begin);
      tempString = powerStatement.substr(begin, end - begin);
      tempDIC.enable = tempString;
    } else if (tempString == "-valid_location") {
      begin = powerStatement.find_first_not_of(" \t{", end);
      end = powerStatement.find_first_of(" \t}", begin);
      tempString = powerStatement.substr(begin, end - begin);
      tempDIC.location = tempString;
    }
    begin = powerStatement.find_first_not_of(" \t}", end);
    if (begin == string::npos) {
      break;
    }
    end = powerStatement.find_first_of(" \t", begin);
    tempString = powerStatement.substr(begin, end - begin);
  }
  defineIsoCell[tempDIC.cell] = tempDIC;
}

void CIR_Handle(const string &powerStatement) {
  size_t begin, end;
  string tempString;
  const string command("create_isolation_rule");
  C_I_R tempCIR;

  begin = powerStatement.find(command);
  begin = powerStatement.find_first_not_of(" \t", begin + command.size());
  end = powerStatement.find_first_of(" \t", begin);
  tempString = powerStatement.substr(begin, end - begin);
  while (true) {
    if (tempString == "-name") {
      begin = powerStatement.find_first_not_of(" \t", end);
      end = powerStatement.find_first_of(" \t", begin);
      tempString = powerStatement.substr(begin, end - begin);
      tempCIR.name = tempString;
    } else if (tempString == "-from") {
      begin = powerStatement.find_first_not_of(" \t", end);
      end = powerStatement.find_first_of(" \t", begin);
      tempString = powerStatement.substr(begin, end - begin);
      tempCIR.from = tempString;
    } else if (tempString == "-to") {
      begin = powerStatement.find_first_not_of(" \t", end);
      end = powerStatement.find_first_of(" \t", begin);
      tempString = powerStatement.substr(begin, end - begin);
      tempCIR.to = tempString;
    } else if (tempString == "-isolation_condition") {
      begin = powerStatement.find_first_not_of(" \t{", end);
      end = powerStatement.find_first_of(" \t}", begin);
      tempString = powerStatement.substr(begin, end - begin);
      if (tempString.find('!') != string::npos) {
        tempCIR.negation = true;
        tempString = tempString.substr(tempString.find('!') + 1);
      } else {
        tempCIR.negation = false;
      }
      tempCIR.condition = tempString;
    } else if (tempString == "-isolation_output") {
      begin = powerStatement.find_first_not_of(" \t", end);
      end = powerStatement.find_first_of(" \t", begin);
      tempString = powerStatement.substr(begin, end - begin);
      if (tempString == "high") {
        tempCIR.output = true;
      } else {
        tempCIR.output = false;
      }
    }
    begin = powerStatement.find_first_not_of(" \t", end);
    if (begin == string::npos) {
      break;
    }
    end = powerStatement.find_first_of(" \t", begin);
    tempString = powerStatement.substr(begin, end - begin);
  }
  createIsoRule[tempCIR.from + tempCIR.to] = tempCIR;
}

void CLSR_Handle(const string &powerStatement) {
  size_t begin, end;
  string tempString;
  const string command("create_level_shifter_rule");
  C_L_S_R tempCLSR;

  begin = powerStatement.find(command);
  begin = powerStatement.find_first_not_of(" \t", begin + command.size());
  end = powerStatement.find_first_of(" \t", begin);
  tempString = powerStatement.substr(begin, end - begin);
  while (true) {
    if (tempString == "-name") {
      begin = powerStatement.find_first_not_of(" \t", end);
      end = powerStatement.find_first_of(" \t", begin);
      tempString = powerStatement.substr(begin, end - begin);
      tempCLSR.name = tempString;
    } else if (tempString == "-from") {
      begin = powerStatement.find_first_not_of(" \t", end);
      end = powerStatement.find_first_of(" \t", begin);
      tempString = powerStatement.substr(begin, end - begin);
      tempCLSR.from = tempString;
    } else if (tempString == "-to") {
      begin = powerStatement.find_first_not_of(" \t", end);
      end = powerStatement.find_first_of(" \t", begin);
      tempString = powerStatement.substr(begin, end - begin);
      tempCLSR.to = tempString;
    }
    begin = powerStatement.find_first_not_of(" \t", end);
    if (begin == string::npos) {
      break;
    }
    end = powerStatement.find_first_of(" \t", begin);
    tempString = powerStatement.substr(begin, end - begin);
  }
  createLevShRule[tempCLSR.from + tempCLSR.to] = tempCLSR;
}

void DLSC_Handle(const string &powerStatement) {
  size_t begin, end;
  size_t subBegin, subEnd;
  size_t i;
  string tempString;
  string subStatement;
  const string command("define_level_shifter_cell");
  D_L_S_C tempDLSC;

  begin = powerStatement.find(command);
  begin = powerStatement.find_first_not_of(" \t", begin + command.size());
  end = powerStatement.find_first_of(" \t", begin);
  tempString = powerStatement.substr(begin, end - begin);
  while (true) {
    if (tempString == "-cells") {
      begin = powerStatement.find_first_not_of(" \t{", end);
      end = powerStatement.find_first_of(" \t}", begin);
      subStatement = powerStatement.substr(begin, end - begin);
      subEnd = subStatement.find_last_not_of(" \t");
      subStatement = subStatement.substr(0, subEnd + 1);
      auto spaceCount = CountSpace(subStatement);
      subEnd = 0;
      for (i = 0; i < spaceCount; i++) {
        subBegin = subStatement.find_first_not_of(" \t", subEnd);
        subEnd = subStatement.find_first_of(" \t", subBegin);
        tempString = subStatement.substr(subBegin, subEnd - subBegin);
        tempDLSC.cell = tempString;
      }
      subBegin = subStatement.find_first_not_of(" \t", subEnd);
      tempString = subStatement.substr(subBegin);
      tempDLSC.cell = tempString;
    } else if (tempString == "-input_voltage_range") {
      begin = powerStatement.find_first_not_of(" \t", end);
      end = powerStatement.find_first_of(" \t", begin);
      tempString = powerStatement.substr(begin, end - begin);
      if (tempString.find(':') != string::npos) {
        end = powerStatement.find_first_of(" \t:", begin);
        tempString = powerStatement.substr(begin, end - begin);
        tempDLSC.inLow = (float)atof(tempString.c_str());
        begin = powerStatement.find_first_not_of(" \t:", end);
        end = powerStatement.find_first_of(" \t", begin);
        tempString = powerStatement.substr(begin, end - begin);
        tempDLSC.inHigh = (float)atof(tempString.c_str());
      } else {
        tempDLSC.inLow = (float)atof(tempString.c_str());
        tempDLSC.inHigh = tempDLSC.inLow;
      }
    } else if (tempString == "-output_voltage_range") {
      begin = powerStatement.find_first_not_of(" \t", end);
      end = powerStatement.find_first_of(" \t", begin);
      tempString = powerStatement.substr(begin, end - begin);
      if (tempString.find(':') != string::npos) {
        end = powerStatement.find_first_of(" \t:", begin);
        tempString = powerStatement.substr(begin, end - begin);
        tempDLSC.outLow = (float)atof(tempString.c_str());
        begin = powerStatement.find_first_not_of(" \t:", end);
        end = powerStatement.find_first_of(" \t", begin);
        tempString = powerStatement.substr(begin, end - begin);
        tempDLSC.outHigh = (float)atof(tempString.c_str());
      } else {
        tempDLSC.outLow = (float)atof(tempString.c_str());
        tempDLSC.outHigh = tempDLSC.outLow;
      }
    } else if (tempString == "-direction") {
      begin = powerStatement.find_first_not_of(" \t", end);
      end = powerStatement.find_first_of(" \t", begin);
      tempString = powerStatement.substr(begin, end - begin);
      tempDLSC.direction = tempString;
    } else if (tempString == "-valid_location") {
      begin = powerStatement.find_first_not_of(" \t", end);
      end = powerStatement.find_first_of(" \t", begin);
      tempString = powerStatement.substr(begin, end - begin);
      tempDLSC.location = tempString;
    }
    begin = powerStatement.find_first_not_of(" \t", end);
    if (begin == string::npos) {
      break;
    }
    end = powerStatement.find_first_of(" \t", begin);
    tempString = powerStatement.substr(begin, end - begin);
  }
  defineLevShCell[tempDLSC.cell] = tempDLSC;
}

void CNC_Handle(const string &powerStatement) {
  size_t begin, end;
  string tempString;
  const string command("create_nominal_condition");
  C_N_C tempCNC;

  begin = powerStatement.find(command);
  begin = powerStatement.find_first_not_of(" \t", begin + command.size());
  end = powerStatement.find_first_of(" \t", begin);
  tempString = powerStatement.substr(begin, end - begin);
  while (true) {
    if (tempString == "-name") {
      begin = powerStatement.find_first_not_of(" \t", end);
      end = powerStatement.find_first_of(" \t", begin);
      tempString = powerStatement.substr(begin, end - begin);
      tempCNC.name = tempString;
    } else if (tempString == "-voltage") {
      begin = powerStatement.find_first_not_of(" \t", end);
      end = powerStatement.find_first_of(" \t", begin);
      tempString = powerStatement.substr(begin, end - begin);
      tempCNC.voltage = atof(tempString.c_str());
    }
    begin = powerStatement.find_first_not_of(" \t", end);
    if (begin == string::npos) {
      break;
    }
    end = powerStatement.find_first_of(" \t", begin);
    tempString = powerStatement.substr(begin, end - begin);
  }
  createNomCondition[tempCNC.name] = tempCNC;
}

void CPM_Handle(const string &powerStatement) {
  size_t begin, end;
  size_t subBegin, subEnd, mid;
  size_t spaceCount = 0;
  size_t i;
  string tempString, tempCondition;
  string subStatement;
  const string command("create_power_mode");
  C_P_M tempCPM;

  tempCPM.defMode = false;
  begin = powerStatement.find(command);
  begin = powerStatement.find_first_not_of(" \t", begin + command.size());
  end = powerStatement.find_first_of(" \t", begin);
  tempString = powerStatement.substr(begin, end - begin);
  while (true) {
    if (tempString == "-name") {
      begin = powerStatement.find_first_not_of(" \t", end);
      end = powerStatement.find_first_of(" \t", begin);
      tempString = powerStatement.substr(begin, end - begin);
      tempCPM.name = tempString;
    } else if (tempString == "-domain_conditions") {
      begin = powerStatement.find_first_not_of(" \t{", end);
      end = powerStatement.find_first_of('}', begin);
      subStatement = powerStatement.substr(begin, end - begin);
      subEnd = subStatement.find_last_not_of(" \t}");
      subStatement = subStatement.substr(0, subEnd + 1);
      spaceCount = CountSpace(subStatement);
      subEnd = 0;
      for (i = 0; i < spaceCount; i++) {
        subBegin = subStatement.find_first_not_of(" \t", subEnd);
        subEnd = subStatement.find_first_of('@', subBegin);
        tempString = subStatement.substr(subBegin, subEnd - subBegin);
        subBegin = subStatement.find_first_not_of('@', subEnd);
        subEnd = subStatement.find_first_of(" \t", subBegin);
        tempCondition = subStatement.substr(subBegin, subEnd - subBegin);
        tempCPM.condition[tempString] = tempCondition;
      }
      subBegin = subStatement.find_first_not_of(" \t", subEnd);
      mid = subStatement.find('@', subBegin);
      tempString = subStatement.substr(subBegin, mid - subBegin);
      tempCPM.condition[tempString] = subStatement.substr(mid + 1);
    } else if (tempString == "-default") {
      tempCPM.defMode = true;
    }
    begin = powerStatement.find_first_not_of(" \t", end);
    if (begin == string::npos) {
      break;
    }
    end = powerStatement.find_first_of(" \t", begin);
    tempString = powerStatement.substr(begin, end - begin);
  }
  if (tempCPM.defMode == true) {
    defaultMode = tempCPM.name;
  }
  createPowMode.push_back(tempCPM);
}
