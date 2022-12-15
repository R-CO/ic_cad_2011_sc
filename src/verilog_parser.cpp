#include "verilog_parser.hpp"

#include <fstream>
using std::ifstream;
#include <iostream>
using std::cerr;
using std::cout;
using std::endl;
#include <string>
using std::string;

#include <map>
using std::map;
#include <vector>
using std::vector;

#include "little_tools.hpp"

/*** Global Variables ***/
bool secondTime = false;
map<string, int> primitiveGate;
bool ok = true;
size_t libStart = 9999999;

/*** global variables ***/
map<string, string> libModule;
map<string, Node> nodeMap;  // using the hierarchy name to be the index
map<string, Node>::iterator nodeIter;

// map<string, D_L_S_C_pointer> levShCells;
vector<NodePointer> isoNodes;
vector<NodePointer> levShNodes;

/*** about top-module ***/
string topModule;  // to record the name of the top module in the design
map<string, PortDomain>
    topPortDomain;  // using the top level port name to be the index

/*** default power domain & default power mode ***/
map<string, bool> notYetSetDomain;  // using the hirarchy name to be the index

bool ReadVerilogFile(const char *fileName, vector<string> &fileContent,
                     int &fileCount) {
  string tempString;
  size_t begin, end;
  size_t stringSize;

  ifstream inFile(fileName, std::ios::in);
  if (!inFile) {
    return false;
  }

  fileContent.push_back(tempString);
  while (getline(inFile, tempString)) {
    begin = tempString.find_first_not_of(" \t");
    // to ignore the comments in the verilog file
    if (begin == string::npos || tempString.substr(begin, 2) == "//") {
      continue;
    }
    end = tempString.find_first_of(" \t", begin);
    stringSize = end - begin;
    if (tempString.substr(begin, stringSize) == "endmodule") {
      tempString += ';';
    }
    tempString += ' ';
    fileContent[fileCount] += tempString;
  }

  return true;
}

void StoreVerilogStatement(vector<string> &fileContent,
                           vector<string> &veriStatements,
                           const int fileCount) {
  size_t i;
  size_t statementBegin, statementEnd;
  size_t stringSize;

  for (i = 0; i < fileContent[fileCount].length(); i += 2) {
    statementBegin = i;
    statementEnd = fileContent[fileCount].find_first_of(';', statementBegin);
    stringSize = statementEnd - statementBegin + 2;
    veriStatements.push_back(
        fileContent[fileCount].substr(statementBegin, stringSize));
    i = statementEnd;
  }
}

void ParseVerilogFile(vector<string> &veriStatements,
                      map<string, Module> &moduleMap) {
  string currentModule;
  size_t i;

  for (i = 0; i != veriStatements.size(); i++) {
    // �B�z�H"module"�}�Y��statement
    if (veriStatements[i].find("module ") != string::npos) {
      if (secondTime == true) {
        currentModule = SplitModuleName(veriStatements[i]);
        continue;
      }
      currentModule = ModuleHandle(veriStatements[i], moduleMap);
      if (i >= libStart) {
        libModule[currentModule] = currentModule;
      }
    } else if (veriStatements[i].find("input ") != string::npos) {
      if (secondTime == true) {
        continue;
      }
      InputHandle(veriStatements[i], moduleMap, currentModule);
    } else if (veriStatements[i].find("output ") != string::npos) {
      if (secondTime == true) {
        continue;
      }
      OutputHandle(veriStatements[i], moduleMap, currentModule);
    } else if (veriStatements[i].find("inout ") != string::npos) {
      cout << "Warning: this program can not handle inout-type port." << endl;
      continue;
    } else if (veriStatements[i].find("wire ") != string::npos) {
      if (secondTime == true) {
        continue;
      }
      WireHandle(veriStatements[i], moduleMap, currentModule);
    } else if (veriStatements[i].find("endmodule;") != string::npos) {
      if (secondTime == true) {
        continue;
      }
      currentModule.clear();
    } else if (secondTime == true) {
      ok = InstanceHandle(veriStatements[i], moduleMap, currentModule);
    }
  }
}

string ModuleHandle(const string &veriStatement,
                    map<string, Module> &moduleMap) {
  size_t begin = 0, end = 0, stringSize = 0;
  Module tempModule;
  string tempModuleName;
  size_t commaCount = 0;
  size_t i = 0;
  string tempString;
  Port tempPort;

  // to split the module name out then store into the module table
  begin = veriStatement.find("module ");
  begin = veriStatement.find_first_not_of(' ', begin + strlen("module"));
  end = veriStatement.find_first_of(" (", begin);
  stringSize = end - begin;
  tempModuleName = veriStatement.substr(begin, stringSize);
  tempModule.moduleName = tempModuleName;
  tempModule.top = true;
  commaCount = CountComma(veriStatement);
  // to split the ports name out then store into the module table
  begin = veriStatement.find_first_not_of("( ", end);
  for (i = 0; i != commaCount; i++) {
    end = veriStatement.find_first_of(" ,", begin + 1);
    stringSize = end - begin;
    tempString = veriStatement.substr(begin, stringSize);
    tempPort.portName = tempString;
    tempPort.portOrder = i;
    tempModule.ports[tempString] = tempPort;
    begin = veriStatement.find_first_not_of(", ", end);
  }
  end = veriStatement.find_first_of(" )", begin);
  stringSize = end - begin;
  tempString = veriStatement.substr(begin, stringSize);
  tempPort.portName = tempString;  // port name
  tempPort.portOrder = i;          // port order
  tempModule.ports[tempString] = tempPort;
  moduleMap[tempModuleName] = tempModule;

  return tempModuleName;
}

void InputHandle(const string &veriStatement, map<string, Module> &moduleMap,
                 const string &currentModule) {
  bool multiBits = false;
  int from = 0, to = 0;
  size_t commaCount = 0;
  size_t begin, end;
  size_t i = 0;
  int bit = 0;
  string tempString;
  Wire tempWire;
  Port tempPort;
  WireConnectInfo tempConInfo;
  map<string, Port>::iterator portIter;

  begin = veriStatement.find("input");
  begin = veriStatement.find_first_not_of(" \t", begin + strlen("input"));
  if (veriStatement.find('[') != string::npos) {
    multiBits = true;
    begin = veriStatement.find('[');
    begin = veriStatement.find_first_not_of(" \t[", begin);
    end = veriStatement.find_first_of(" \t:", begin);
    tempString = veriStatement.substr(begin, end - begin);
    to = atoi(tempString.c_str());
    begin = veriStatement.find_first_not_of(" \t:", end);
    end = veriStatement.find_first_of(" \t]", begin);
    tempString = veriStatement.substr(begin, end - begin);
    from = atoi(tempString.c_str());
    begin = veriStatement.find_first_not_of(" \t]", end);
  }
  commaCount = CountComma(veriStatement);
  for (i = 0; i < commaCount; i++) {
    end = veriStatement.find_first_of(" ,", begin);
    tempString = veriStatement.substr(begin, end - begin);
    if (moduleMap[currentModule].ports.count(tempString) != 0) {
      portIter = moduleMap[currentModule].ports.find(tempString);
      portIter->second.connectWireName = tempString;
      portIter->second.type = PortType::IN;
      portIter->second.multiBits = multiBits;
      portIter->second.inverted = false;
      portIter->second.from = from;
      portIter->second.to = to;
      tempWire.wireName = tempString;
      tempWire.multiBits = multiBits;
      tempWire.from = from;
      tempWire.to = to;
      tempConInfo.instanceType = currentModule;
      tempConInfo.instanceName = currentModule;
      tempConInfo.portName = tempString;
      tempConInfo.portType = PortType::IN;
      tempConInfo.portOrder = portIter->second.portOrder;
      tempWire.conInfo.push_back(tempConInfo);
      moduleMap[currentModule].wires[tempString] = tempWire;
      if (multiBits == true) {
        for (bit = from; bit <= to; bit++) {
          tempPort.portName = tempString + '[' + IntToString(bit) + ']';
          tempPort.portOrder = portIter->second.portOrder;
          tempPort.type = PortType::IN;
          tempPort.connectWireName = tempPort.portName;
          tempPort.multiBits = false;
          tempPort.inverted = false;
          tempPort.from = 0;
          tempPort.to = 0;
          moduleMap[currentModule].ports[tempPort.portName] = tempPort;
          tempWire.clear();
          tempWire.wireName = tempPort.portName;
          tempWire.multiBits = false;
          tempWire.from = from;
          tempWire.to = to;
          tempConInfo.portName = tempPort.portName;
          tempWire.conInfo.push_back(tempConInfo);
          moduleMap[currentModule].wires[tempWire.wireName] = tempWire;
        }
      }
      tempWire.clear();
      tempConInfo.clear();
    } else {
      cerr << "There is an port specific error in module \"" << currentModule
           << "!" << endl;
      return;
    }
    begin = veriStatement.find_first_not_of(" ,", end);
  }
  end = veriStatement.find_first_of(" ;", begin);
  tempString = veriStatement.substr(begin, end - begin);
  if (moduleMap[currentModule].ports.count(tempString) != 0) {
    portIter = moduleMap[currentModule].ports.find(tempString);
    portIter->second.type = PortType::IN;
    portIter->second.multiBits = multiBits;
    portIter->second.inverted = false;
    portIter->second.connectWireName = tempString;
    portIter->second.from = from;
    portIter->second.to = to;
    tempWire.wireName = tempString;
    tempWire.multiBits = multiBits;
    tempWire.from = from;
    tempWire.to = to;
    tempConInfo.instanceType = currentModule;
    tempConInfo.instanceName = currentModule;
    tempConInfo.portName = tempString;
    tempConInfo.portType = PortType::IN;
    tempConInfo.portOrder = portIter->second.portOrder;
    tempWire.conInfo.push_back(tempConInfo);
    moduleMap[currentModule].wires[tempWire.wireName] = tempWire;
    if (multiBits == true) {
      for (bit = from; bit <= to; bit++) {
        tempPort.portName = tempString + '[' + IntToString(bit) + ']';
        tempPort.portOrder = portIter->second.portOrder;
        tempPort.type = PortType::IN;
        tempPort.connectWireName = tempPort.portName;
        tempPort.multiBits = false;
        tempPort.inverted = false;
        moduleMap[currentModule].ports[tempPort.portName] = tempPort;
        tempWire.clear();
        tempWire.wireName = tempPort.portName;
        tempWire.multiBits = false;
        tempWire.from = 0;
        tempWire.to = 0;
        tempConInfo.portName = tempPort.portName;
        tempWire.conInfo.push_back(tempConInfo);
        moduleMap[currentModule].wires[tempWire.wireName] = tempWire;
      }
    }
  } else {
    cerr << "There is an port specific error in module \"" << currentModule
         << "\"." << endl;
    return;
  }
}

void OutputHandle(const string &veriStatement, map<string, Module> &moduleMap,
                  const string &currentModule) {
  bool multiBits = false;
  int from = 0, to = 0;
  size_t commaCount = 0;
  size_t begin, end;
  size_t i = 0;
  int bit = 0;
  string tempString;
  Wire tempWire;
  Port tempPort;
  WireConnectInfo tempConInfo;
  map<string, Port>::iterator portIter;

  begin = veriStatement.find("output");
  begin = veriStatement.find_first_not_of(" \t", begin + strlen("output"));
  if (veriStatement.find('[') != string::npos) {
    multiBits = true;
    begin = veriStatement.find('[');
    begin = veriStatement.find_first_not_of(" \t[", begin);
    end = veriStatement.find_first_of(" \t:", begin);
    tempString = veriStatement.substr(begin, end - begin);
    to = atoi(tempString.c_str());
    begin = veriStatement.find_first_not_of(" \t:", end);
    end = veriStatement.find_first_of(" \t]", begin);
    tempString = veriStatement.substr(begin, end - begin);
    from = atoi(tempString.c_str());
    begin = veriStatement.find_first_not_of(" \t]", end);
  }
  commaCount = CountComma(veriStatement);
  for (i = 0; i < commaCount; i++) {
    end = veriStatement.find_first_of(" ,", begin);
    tempString = veriStatement.substr(begin, end - begin);
    if (moduleMap[currentModule].ports.count(tempString) != 0) {
      portIter = moduleMap[currentModule].ports.find(tempString);
      portIter->second.connectWireName = tempString;
      portIter->second.type = PortType::OUT;
      portIter->second.multiBits = multiBits;
      portIter->second.inverted = false;
      portIter->second.from = from;
      portIter->second.to = to;
      tempWire.wireName = tempString;
      tempWire.multiBits = multiBits;
      tempWire.from = from;
      tempWire.to = to;
      tempConInfo.instanceType = currentModule;
      tempConInfo.instanceName = currentModule;
      tempConInfo.portName = tempString;
      tempConInfo.portType = PortType::OUT;
      tempConInfo.portOrder = portIter->second.portOrder;
      tempWire.conInfo.push_back(tempConInfo);
      moduleMap[currentModule].wires[tempString] = tempWire;
      if (multiBits == true) {
        for (bit = from; bit <= to; bit++) {
          tempPort.portName = tempString + '[' + IntToString(bit) + ']';
          tempPort.portOrder = portIter->second.portOrder;
          tempPort.type = PortType::OUT;
          tempPort.connectWireName = tempPort.portName;
          tempPort.multiBits = false;
          tempPort.inverted = false;
          tempPort.from = 0;
          tempPort.to = 0;
          moduleMap[currentModule].ports[tempPort.portName] = tempPort;
          tempWire.clear();
          tempWire.wireName = tempPort.portName;
          tempWire.multiBits = false;
          tempConInfo.portName = tempPort.portName;
          tempWire.conInfo.push_back(tempConInfo);
          moduleMap[currentModule].wires[tempWire.wireName] = tempWire;
        }
      }
    } else {
      cerr << "There is an port specific error in module \"" << currentModule
           << "!" << endl;
      return;
    }
    begin = veriStatement.find_first_not_of(" ,", end);
  }
  end = veriStatement.find_first_of(" ;", begin);
  tempString = veriStatement.substr(begin, end - begin);
  if (moduleMap[currentModule].ports.count(tempString) != 0) {
    portIter = moduleMap[currentModule].ports.find(tempString);
    portIter->second.connectWireName = tempString;
    portIter->second.type = PortType::OUT;
    portIter->second.multiBits = multiBits;
    portIter->second.inverted = false;
    portIter->second.from = from;
    portIter->second.to = to;
    tempWire.wireName = tempString;
    tempWire.multiBits = multiBits;
    tempWire.from = from;
    tempWire.to = to;
    tempConInfo.instanceType = currentModule;
    tempConInfo.instanceName = currentModule;
    tempConInfo.portName = tempString;
    tempConInfo.portType = PortType::OUT;
    tempConInfo.portOrder = portIter->second.portOrder;
    ;
    tempWire.conInfo.push_back(tempConInfo);
    moduleMap[currentModule].wires[tempString] = tempWire;
    if (multiBits == true) {
      for (bit = from; bit <= to; bit++) {
        tempPort.portName = tempString + '[' + IntToString(bit) + ']';
        tempPort.portOrder = portIter->second.portOrder;
        tempPort.type = PortType::OUT;
        tempPort.connectWireName = tempPort.portName;
        tempPort.multiBits = false;
        tempPort.inverted = false;
        tempPort.from = 0;
        tempPort.to = 0;
        moduleMap[currentModule].ports[tempPort.portName] = tempPort;
        tempWire.clear();
        tempWire.wireName = tempPort.portName;
        tempWire.multiBits = false;
        tempWire.from = 0;
        tempWire.to = 0;
        tempConInfo.portName = tempPort.portName;
        tempWire.conInfo.push_back(tempConInfo);
        moduleMap[currentModule].wires[tempWire.wireName] = tempWire;
      }
    }
  } else {
    cerr << "There is an port specific error in module \"" << currentModule
         << "\"." << endl;
    return;
  }
}

bool InstanceHandle(const string &veriStatement, map<string, Module> &moduleMap,
                    const string &currentModule) {
  vector<size_t> leftBig, leftSmall;
  size_t rightBig, rightSmall;
  size_t begin, end, tempBegin, tempEnd;
  string tempString;
  string insType, insName;
  string portName, wireName, tempPortName, tempWireName;
  WireConnectInfo tempConInfo;
  int portOrder = 0;
  size_t i = 0;
  int from, to, j, k;
  size_t commaCount;
  map<string, Port>::iterator portIter;
  Unit tempUnit;
  bool inverted = false, con = false;

  begin = veriStatement.find_first_not_of(" \t");
  end = veriStatement.find_first_of(" \t", begin);
  tempUnit.unitType = veriStatement.substr(begin, end - begin);
  if (moduleMap.count(tempUnit.unitType) != 0) {
    moduleMap[tempUnit.unitType].top = false;
  }
  end = veriStatement.find_first_not_of(" \t", end);
  if (veriStatement[end] != '(') {
    begin = end;
    end = veriStatement.find_first_of(" \t(", begin);
    tempUnit.unitName = veriStatement.substr(begin, end - begin);
  } else {
    tempUnit.unitName = tempUnit.unitType;
  }
  i = veriStatement.find_first_of('(', end) + 1;
  if (veriStatement.find('.') != string::npos) {
    while (veriStatement[i] != ';') {
      switch (veriStatement[i]) {
        case '.':
          begin = veriStatement.find_first_not_of(" \t.", i);
          end = veriStatement.find_first_of(" \t(", begin);
          portName = veriStatement.substr(begin, end - begin);
          if (moduleMap[tempUnit.unitType].ports.count(portName) > 0) {
            portIter = moduleMap[tempUnit.unitType].ports.find(portName);
            tempUnit.ports[portName] =
                moduleMap[tempUnit.unitType].ports[portName];
            tempUnit.ports[portName].inverted = false;
          }
          i = end;
          con = true;
          break;
        case '{':
          leftBig.push_back(i);
          if (leftBig.size() > 1) {
            cerr << "Under module: " << currentModule << endl
                 << veriStatement << endl
                 << "This program can not handle so many nested bus." << endl;
            return false;
          }
          break;
        case '}':
          rightBig = i;
          tempString =
              veriStatement.substr(leftBig.back(), rightBig - leftBig.back());
          commaCount = CountComma(tempString);
          if (commaCount > 0) {
            j = portIter->second.to;
            begin = tempString.find_first_not_of(" \t{");
            while (commaCount > 0) {
              end = tempString.find_first_of(" \t,", begin);
              wireName = tempString.substr(begin, end - begin);
              if (wireName.find('~') != string::npos) {
                inverted = true;
                tempBegin = wireName.find_first_not_of(" \t~");
                wireName = wireName.substr(tempBegin);
              }
              if (wireName.find('[') != string::npos &&
                  wireName.find(':') != string::npos) {
                GetFromTo(wireName, from, to);
                tempEnd = wireName.find('[');
                if (from == moduleMap[currentModule]
                                .wires[wireName.substr(0, tempEnd)]
                                .from &&
                    to == moduleMap[currentModule]
                              .wires[wireName.substr(0, tempEnd)]
                              .to) {
                  tempConInfo.instanceType = tempUnit.unitType;
                  tempConInfo.instanceName = tempUnit.unitName;
                  tempConInfo.portName = portIter->second.portName;
                  tempConInfo.portType = portIter->second.type;
                  tempConInfo.portOrder = portIter->second.portOrder;
                  moduleMap[currentModule]
                      .wires[wireName.substr(0, tempEnd)]
                      .conInfo.push_back(tempConInfo);
                  tempUnit.ports[portName].connectWireName =
                      wireName.substr(0, tempEnd);
                  tempUnit.ports[portName].inverted = inverted;
                  tempConInfo.clear();
                }
                for (k = to; k >= from; k--, j--) {
                  tempPortName = portName + '[' + IntToString(j) + ']';
                  tempWireName =
                      wireName.substr(0, tempEnd) + '[' + IntToString(k) + ']';
                  tempUnit.ports[tempPortName] =
                      moduleMap[tempUnit.unitType].ports[tempPortName];
                  tempUnit.ports[tempPortName].connectWireName = tempWireName;
                  tempUnit.ports[tempPortName].inverted = inverted;
                  tempConInfo.instanceType = tempUnit.unitType;
                  tempConInfo.instanceName = tempUnit.unitName;
                  tempConInfo.portName = tempPortName;
                  tempConInfo.portType = tempUnit.ports[tempPortName].type;
                  tempConInfo.portOrder =
                      tempUnit.ports[tempPortName].portOrder;
                  moduleMap[currentModule]
                      .wires[tempWireName]
                      .conInfo.push_back(tempConInfo);
                  tempConInfo.clear();
                }
              } else if (moduleMap[currentModule].wires[wireName].multiBits ==
                         true) {
                tempConInfo.instanceType = tempUnit.unitType;
                tempConInfo.instanceName = tempUnit.unitName;
                tempConInfo.portName = portIter->second.portName;
                tempConInfo.portType = portIter->second.type;
                tempConInfo.portOrder = portIter->second.portOrder;
                moduleMap[currentModule].wires[wireName].conInfo.push_back(
                    tempConInfo);
                tempUnit.ports[portName].connectWireName = wireName;
                tempConInfo.clear();
                from = moduleMap[currentModule].wires[wireName].from;
                to = moduleMap[currentModule].wires[wireName].to;
                for (k = to; k >= from; k--, j--) {
                  tempPortName = portName + '[' + IntToString(j) + ']';
                  tempWireName = wireName + '[' + IntToString(k) + ']';
                  tempUnit.ports[tempPortName] =
                      moduleMap[tempUnit.unitType].ports[tempPortName];
                  tempUnit.ports[tempPortName].connectWireName = tempWireName;
                  tempUnit.ports[tempPortName].inverted = inverted;
                  tempConInfo.instanceType = tempUnit.unitType;
                  tempConInfo.instanceName = tempUnit.unitName;
                  tempConInfo.portName = tempPortName;
                  tempConInfo.portType = tempUnit.ports[tempPortName].type;
                  tempConInfo.portOrder =
                      tempUnit.ports[tempPortName].portOrder;
                  moduleMap[currentModule]
                      .wires[tempWireName]
                      .conInfo.push_back(tempConInfo);
                  tempConInfo.clear();
                }
              } else {
                tempPortName = portName + '[' + IntToString(j) + ']';
                tempUnit.ports[tempPortName] =
                    moduleMap[tempUnit.unitType].ports[tempPortName];
                tempUnit.ports[tempPortName].connectWireName = wireName;
                tempUnit.ports[tempPortName].inverted = inverted;
                tempConInfo.instanceType = tempUnit.unitType;
                tempConInfo.instanceName = tempUnit.unitName;
                tempConInfo.portName = tempPortName;
                tempConInfo.portType = tempUnit.ports[tempPortName].type;
                tempConInfo.portOrder = tempUnit.ports[tempPortName].portOrder;
                moduleMap[currentModule].wires[wireName].wireName = wireName;
                moduleMap[currentModule].wires[wireName].multiBits = false;
                moduleMap[currentModule].wires[wireName].conInfo.push_back(
                    tempConInfo);
                tempConInfo.clear();
                j--;
              }
              inverted = false;
              begin = tempString.find_first_not_of(" \t,", end);
              commaCount--;
            }
            end = tempString.find_first_of(" \t}", begin);
            wireName = tempString.substr(begin, end - begin);
            if (wireName.find('~') != string::npos) {
              inverted = true;
              tempBegin = wireName.find_first_not_of(" \t~");
              wireName = wireName.substr(tempBegin);
            }
            if (wireName.find('[') != string::npos &&
                wireName.find(':') != string::npos) {
              GetFromTo(wireName, from, to);
              tempEnd = wireName.find('[');
              if (from == moduleMap[currentModule]
                              .wires[wireName.substr(0, tempEnd)]
                              .from &&
                  to == moduleMap[currentModule]
                            .wires[wireName.substr(0, tempEnd)]
                            .to) {
                tempConInfo.instanceType = tempUnit.unitType;
                tempConInfo.instanceName = tempUnit.unitName;
                tempConInfo.portName = portIter->second.portName;
                tempConInfo.portType = portIter->second.type;
                tempConInfo.portOrder = portIter->second.portOrder;
                moduleMap[currentModule]
                    .wires[wireName.substr(0, tempEnd)]
                    .conInfo.push_back(tempConInfo);
                tempUnit.ports[portName].connectWireName =
                    wireName.substr(0, tempEnd);
                tempUnit.ports[portName].inverted = inverted;
                tempConInfo.clear();
              }
              for (k = to; k >= from; k--, j--) {
                tempPortName = portName + '[' + IntToString(j) + ']';
                tempWireName =
                    wireName.substr(0, tempEnd) + '[' + IntToString(k) + ']';
                tempUnit.ports[tempPortName] =
                    moduleMap[tempUnit.unitType].ports[tempPortName];
                tempUnit.ports[tempPortName].connectWireName = tempWireName;
                tempUnit.ports[tempPortName].inverted = inverted;
                tempConInfo.instanceType = tempUnit.unitType;
                tempConInfo.instanceName = tempUnit.unitName;
                tempConInfo.portName = tempPortName;
                tempConInfo.portType = tempUnit.ports[tempPortName].type;
                tempConInfo.portOrder = tempUnit.ports[tempPortName].portOrder;
                moduleMap[currentModule].wires[tempWireName].conInfo.push_back(
                    tempConInfo);
                tempConInfo.clear();
              }
            } else if (moduleMap[currentModule].wires[wireName].multiBits ==
                       true) {
              tempConInfo.instanceType = tempUnit.unitType;
              tempConInfo.instanceName = tempUnit.unitName;
              tempConInfo.portName = portIter->second.portName;
              tempConInfo.portType = portIter->second.type;
              tempConInfo.portOrder = portIter->second.portOrder;
              moduleMap[currentModule].wires[wireName].conInfo.push_back(
                  tempConInfo);
              tempUnit.ports[portName].connectWireName = wireName;
              tempConInfo.clear();
              from = moduleMap[currentModule].wires[wireName].from;
              to = moduleMap[currentModule].wires[wireName].to;
              for (j = portIter->second.to, k = to; k >= from; k--, j--) {
                tempPortName = portName + '[' + IntToString(j) + ']';
                tempWireName = wireName + '[' + IntToString(k) + ']';
                tempUnit.ports[tempPortName] =
                    moduleMap[tempUnit.unitType].ports[tempPortName];
                tempUnit.ports[tempPortName].connectWireName = tempWireName;
                tempUnit.ports[tempPortName].inverted = inverted;
                tempConInfo.instanceType = tempUnit.unitType;
                tempConInfo.instanceName = tempUnit.unitName;
                tempConInfo.portName = tempPortName;
                tempConInfo.portType = tempUnit.ports[tempPortName].type;
                tempConInfo.portOrder = tempUnit.ports[tempPortName].portOrder;
                moduleMap[currentModule].wires[tempWireName].conInfo.push_back(
                    tempConInfo);
                tempConInfo.clear();
              }
            } else {
              tempPortName = portName + '[' + IntToString(j) + ']';
              tempUnit.ports[tempPortName] =
                  moduleMap[tempUnit.unitType].ports[tempPortName];
              tempUnit.ports[tempPortName].connectWireName = wireName;
              tempUnit.ports[tempPortName].inverted = inverted;
              tempConInfo.instanceType = tempUnit.unitType;
              tempConInfo.instanceName = tempUnit.unitName;
              tempConInfo.portName = tempPortName;
              tempConInfo.portType = tempUnit.ports[tempPortName].type;
              tempConInfo.portOrder = tempUnit.ports[tempPortName].portOrder;
              moduleMap[currentModule].wires[wireName].wireName = wireName;
              moduleMap[currentModule].wires[wireName].multiBits = false;
              moduleMap[currentModule].wires[wireName].conInfo.push_back(
                  tempConInfo);
              tempConInfo.clear();
              j--;
            }
          } else {
            begin = tempString.find_first_not_of(" \t{");
            end = tempString.find_first_of(" \t}", begin);
            wireName = tempString.substr(begin, end - begin);
            if (wireName.find('~') != string::npos) {
              inverted = true;
              tempBegin = wireName.find_first_not_of(" \t~");
              wireName = wireName.substr(tempBegin);
            }
            if (wireName.find('[') != string::npos &&
                wireName.find(':') != string::npos) {
              GetFromTo(wireName, from, to);
              tempEnd = wireName.find('[');
              if (from == moduleMap[currentModule]
                              .wires[wireName.substr(0, tempEnd)]
                              .from &&
                  to == moduleMap[currentModule]
                            .wires[wireName.substr(0, tempEnd)]
                            .to) {
                tempConInfo.instanceType = tempUnit.unitType;
                tempConInfo.instanceName = tempUnit.unitName;
                tempConInfo.portName = portIter->second.portName;
                tempConInfo.portType = portIter->second.type;
                tempConInfo.portOrder = portIter->second.portOrder;
                moduleMap[currentModule]
                    .wires[wireName.substr(0, tempEnd)]
                    .conInfo.push_back(tempConInfo);
                tempUnit.ports[portName].connectWireName =
                    wireName.substr(0, tempEnd);
                tempUnit.ports[portName].inverted = inverted;
                tempConInfo.clear();
              }
              for (j = portIter->second.to, k = to; k >= from; k--, j--) {
                tempPortName = portName + '[' + IntToString(j) + ']';
                tempWireName =
                    wireName.substr(0, tempEnd) + '[' + IntToString(k) + ']';
                tempUnit.ports[tempPortName] =
                    moduleMap[tempUnit.unitType].ports[tempPortName];
                tempUnit.ports[tempPortName].connectWireName = tempWireName;
                tempUnit.ports[tempPortName].inverted = inverted;
                tempConInfo.instanceType = tempUnit.unitType;
                tempConInfo.instanceName = tempUnit.unitName;
                tempConInfo.portName = tempPortName;
                tempConInfo.portType = tempUnit.ports[tempPortName].type;
                tempConInfo.portOrder = tempUnit.ports[tempPortName].portOrder;
                moduleMap[currentModule].wires[tempWireName].conInfo.push_back(
                    tempConInfo);
                tempConInfo.clear();
              }
            } else if (moduleMap[currentModule].wires[wireName].multiBits ==
                       true) {
              tempConInfo.instanceType = tempUnit.unitType;
              tempConInfo.instanceName = tempUnit.unitName;
              tempConInfo.portName = portIter->second.portName;
              tempConInfo.portType = portIter->second.type;
              tempConInfo.portOrder = portIter->second.portOrder;
              moduleMap[currentModule].wires[wireName].conInfo.push_back(
                  tempConInfo);
              tempUnit.ports[portName].connectWireName = wireName;
              tempConInfo.clear();
              from = moduleMap[currentModule].wires[wireName].from;
              to = moduleMap[currentModule].wires[wireName].to;
              for (j = portIter->second.to, k = to; k >= from; k--, j--) {
                tempPortName = portName + '[' + IntToString(j) + ']';
                tempWireName = wireName + '[' + IntToString(k) + ']';
                tempUnit.ports[tempPortName] =
                    moduleMap[tempUnit.unitType].ports[tempPortName];
                tempUnit.ports[tempPortName].connectWireName = tempWireName;
                tempUnit.ports[tempPortName].inverted = inverted;
                tempConInfo.instanceType = tempUnit.unitType;
                tempConInfo.instanceName = tempUnit.unitName;
                tempConInfo.portName = tempPortName;
                tempConInfo.portType = tempUnit.ports[tempPortName].type;
                tempConInfo.portOrder = tempUnit.ports[tempPortName].portOrder;
                moduleMap[currentModule].wires[tempWireName].conInfo.push_back(
                    tempConInfo);
                tempConInfo.clear();
              }
            } else {
              tempUnit.ports[portName] =
                  moduleMap[tempUnit.unitType].ports[portName];
              tempUnit.ports[portName].connectWireName = wireName;
              tempUnit.ports[portName].inverted = inverted;
              tempConInfo.instanceType = tempUnit.unitType;
              tempConInfo.instanceName = tempUnit.unitName;
              tempConInfo.portName = tempPortName;
              tempConInfo.portType = tempUnit.ports[portName].type;
              tempConInfo.portOrder = tempUnit.ports[portName].portOrder;
              moduleMap[currentModule].wires[wireName].wireName = wireName;
              moduleMap[currentModule].wires[wireName].multiBits = false;
              moduleMap[currentModule].wires[wireName].conInfo.push_back(
                  tempConInfo);
              tempConInfo.clear();
              j--;
            }
            inverted = false;
          }
          leftBig.pop_back();
          break;
        case ',':
          if (leftBig.size() == 0) {
            portOrder++;
          }
          break;
        case '(':
          leftSmall.push_back(i);
          break;
        case ')':
          if (leftSmall.size() == 0) {
            break;
          }
          rightSmall = i;
          tempString = veriStatement.substr(leftSmall.back(),
                                            rightSmall - leftSmall.back());
          leftSmall.pop_back();
          if (tempString.find('{') != string::npos) {
            break;
          }
          tempBegin = tempString.find_first_not_of(" \t(");
          tempEnd = tempString.find_first_of(" \t)", tempBegin);
          wireName = tempString.substr(tempBegin, tempEnd - tempBegin);
          if (wireName.find('~') != string::npos) {
            inverted = true;
            tempBegin = wireName.find_first_not_of(" \t~");
            wireName = wireName.substr(tempBegin);
          }
          if (wireName.find('[') != string::npos &&
              wireName.find(':') != string::npos) {
            GetFromTo(wireName, from, to);
            tempEnd = wireName.find('[');
            if (from == moduleMap[currentModule]
                            .wires[wireName.substr(0, tempEnd)]
                            .from &&
                to == moduleMap[currentModule]
                          .wires[wireName.substr(0, tempEnd)]
                          .to) {
              tempConInfo.instanceType = tempUnit.unitType;
              tempConInfo.instanceName = tempUnit.unitName;
              tempConInfo.portName = portIter->second.portName;
              tempConInfo.portType = portIter->second.type;
              tempConInfo.portOrder = portIter->second.portOrder;
              moduleMap[currentModule]
                  .wires[wireName.substr(0, tempEnd)]
                  .conInfo.push_back(tempConInfo);
              tempUnit.ports[portName].connectWireName =
                  wireName.substr(0, tempEnd);
              tempUnit.ports[portName].inverted = inverted;
              tempConInfo.clear();
            }
            for (j = portIter->second.to, k = to; k >= from; k--, j--) {
              tempPortName = portName + '[' + IntToString(j) + ']';
              tempWireName =
                  wireName.substr(0, tempEnd) + '[' + IntToString(k) + ']';
              tempUnit.ports[tempPortName] =
                  moduleMap[tempUnit.unitType].ports[tempPortName];
              tempUnit.ports[tempPortName].connectWireName = tempWireName;
              tempUnit.ports[tempPortName].inverted = inverted;
              tempConInfo.instanceType = tempUnit.unitType;
              tempConInfo.instanceName = tempUnit.unitName;
              tempConInfo.portName = tempPortName;
              tempConInfo.portType = tempUnit.ports[tempPortName].type;
              tempConInfo.portOrder = tempUnit.ports[tempPortName].portOrder;
              moduleMap[currentModule].wires[tempWireName].conInfo.push_back(
                  tempConInfo);
              tempConInfo.clear();
            }
          }
          if (moduleMap[currentModule].wires[wireName].multiBits == true) {
            tempUnit.ports[portName] =
                moduleMap[tempUnit.unitType].ports[portName];
            tempUnit.ports[portName].connectWireName = wireName;
            tempUnit.ports[portName].inverted = inverted;
            tempConInfo.instanceType = tempUnit.unitType;
            tempConInfo.instanceName = tempUnit.unitName;
            tempConInfo.portName = tempPortName;
            tempConInfo.portType = tempUnit.ports[portName].type;
            tempConInfo.portOrder = tempUnit.ports[portName].portOrder;
            moduleMap[currentModule].wires[wireName].conInfo.push_back(
                tempConInfo);
            tempConInfo.clear();
            from = moduleMap[currentModule].wires[wireName].from;
            to = moduleMap[currentModule].wires[wireName].to;
            for (j = portIter->second.to, k = to; k >= from; k--, j--) {
              tempPortName = portName + '[' + IntToString(j) + ']';
              tempWireName = wireName + '[' + IntToString(k) + ']';
              tempUnit.ports[tempPortName] =
                  moduleMap[tempUnit.unitType].ports[tempPortName];
              tempUnit.ports[tempPortName].connectWireName = tempWireName;
              tempUnit.ports[tempPortName].inverted = inverted;
              tempConInfo.instanceType = tempUnit.unitType;
              tempConInfo.instanceName = tempUnit.unitName;
              tempConInfo.portName = tempPortName;
              tempConInfo.portType = tempUnit.ports[tempPortName].type;
              tempConInfo.portOrder = tempUnit.ports[tempPortName].portOrder;
              moduleMap[currentModule].wires[tempWireName].conInfo.push_back(
                  tempConInfo);
              tempConInfo.clear();
            }
          } else {
            tempUnit.ports[portName] =
                moduleMap[tempUnit.unitType].ports[portName];
            tempUnit.ports[portName].connectWireName = wireName;
            tempUnit.ports[portName].inverted = inverted;
            tempConInfo.instanceType = tempUnit.unitType;
            tempConInfo.instanceName = tempUnit.unitName;
            tempConInfo.portName = portName;
            tempConInfo.portType = tempUnit.ports[portName].type;
            tempConInfo.portOrder = tempUnit.ports[portName].portOrder;
            moduleMap[currentModule].wires[wireName].wireName = wireName;
            moduleMap[currentModule].wires[wireName].multiBits = false;
            moduleMap[currentModule].wires[wireName].conInfo.push_back(
                tempConInfo);
            tempConInfo.clear();
          }
          inverted = false;
          break;
        default:
          break;
      }
      if (con == true) {
        con = false;
        continue;
      }
      i++;
    }
  } else {
    commaCount = CountComma(veriStatement);
    begin = veriStatement.find_first_not_of(" \t(", end);
    tempConInfo.instanceType = tempUnit.unitType;
    tempConInfo.instanceName = tempUnit.unitName;
    for (portOrder = 0; portOrder < (int)commaCount; portOrder++) {
      end = veriStatement.find_first_of(" \t,", begin);
      wireName = veriStatement.substr(begin, end - begin);
      if (wireName.find('~') != string::npos) {
        inverted = true;
        tempBegin = wireName.find_first_not_of(" \t~");
        wireName = wireName.substr(tempBegin);
      }
      portName = IntToString(portOrder);
      tempUnit.ports[portName].portName = portName;
      tempUnit.ports[portName].portOrder = portOrder;
      tempUnit.ports[portName].multiBits = false;
      tempUnit.ports[portName].inverted = inverted;
      tempUnit.ports[portName].connectWireName = wireName;
      tempConInfo.portName = portName;
      tempConInfo.portOrder = portOrder;
      if (portOrder == 0) {
        tempUnit.ports[portName].type = PortType::OUT;
        tempConInfo.portType = PortType::OUT;
      } else {
        tempUnit.ports[portName].type = PortType::IN;
        tempConInfo.portType = PortType::IN;
      }
      inverted = false;
      moduleMap[currentModule].wires[wireName].wireName = wireName;
      moduleMap[currentModule].wires[wireName].multiBits = false;
      moduleMap[currentModule].wires[wireName].conInfo.push_back(tempConInfo);
      begin = veriStatement.find_first_not_of(" \t,", end);
    }
    end = veriStatement.find_first_of(" \t)", begin);
    wireName = veriStatement.substr(begin, end - begin);
    if (wireName.find('~') != string::npos) {
      inverted = true;
      tempBegin = wireName.find_first_not_of(" \t~");
      wireName = wireName.substr(tempBegin);
    }
    portName = IntToString(portOrder);
    tempUnit.ports[portName].portName = portName;
    tempUnit.ports[portName].portOrder = portOrder;
    tempUnit.ports[portName].multiBits = false;
    tempUnit.ports[portName].inverted = inverted;
    tempUnit.ports[portName].connectWireName = wireName;
    tempConInfo.portName = portName;
    tempConInfo.portOrder = portOrder;
    tempUnit.ports[portName].type = PortType::IN;
    tempConInfo.portType = PortType::IN;
    moduleMap[currentModule].wires[wireName].wireName = wireName;
    moduleMap[currentModule].wires[wireName].multiBits = false;
    moduleMap[currentModule].wires[wireName].conInfo.push_back(tempConInfo);
    inverted = false;
  }
  moduleMap[currentModule].units[tempUnit.unitName] = tempUnit;
  return true;
}

void WireHandle(const string &veriStatement, map<string, Module> &moduleMap,
                const string &currentModule) {
  bool multiBits(false);
  size_t i(0);
  int bit(0);
  size_t begin, end;
  size_t commaCount;
  string tempString;
  Wire tempWire;

  begin = veriStatement.find("wire");
  begin = veriStatement.find_first_not_of(" \t", begin + strlen("wire"));
  if (veriStatement.find('[') != string::npos) {
    multiBits = true;
    begin = veriStatement.find_first_not_of(" \t[", begin);
    end = veriStatement.find_first_of(" \t:", begin);
    tempString = veriStatement.substr(begin, end - begin);
    tempWire.to = atoi(tempString.c_str());
    begin = veriStatement.find_first_not_of(" \t:", end);
    end = veriStatement.find_first_of(" \t]", begin);
    tempString = veriStatement.substr(begin, end - begin);
    tempWire.from = atoi(tempString.c_str());
    begin = veriStatement.find_first_not_of(" \t]", end);
  }
  commaCount = CountComma(veriStatement);
  for (i = 0; i < commaCount; i++) {
    end = veriStatement.find_first_of(" \t,", begin);
    tempString = veriStatement.substr(begin, end - begin);
    tempWire.wireName = tempString;
    tempWire.multiBits = multiBits;
    moduleMap[currentModule].wires[tempWire.wireName] = tempWire;
    if (multiBits == true) {
      for (bit = tempWire.from; bit < tempWire.to; bit++) {
        tempWire.wireName = tempString + '[' + IntToString(bit) + ']';
        tempWire.multiBits = false;
        moduleMap[currentModule].wires[tempWire.wireName] = tempWire;
      }
    }
    begin = veriStatement.find_first_not_of(" \t,", end);
  }
  end = veriStatement.find_first_of(" \t;", begin);
  tempString = veriStatement.substr(begin, end - begin);
  tempWire.wireName = tempString;
  tempWire.multiBits = multiBits;
  moduleMap[currentModule].wires[tempWire.wireName] = tempWire;
  if (multiBits == true) {
    for (bit = tempWire.from; bit < tempWire.to; bit++) {
      tempWire.wireName = tempString + '[' + IntToString(bit) + ']';
      tempWire.multiBits = false;
      moduleMap[currentModule].wires[tempWire.wireName] = tempWire;
    }
  }
}

string SplitModuleName(const string &veriStatement) {
  size_t begin, end;
  string moduleName;

  begin = veriStatement.find("module ");
  begin = veriStatement.find_first_not_of(" \t", begin + strlen("module"));
  end = veriStatement.find_first_of(" \t(", begin);
  moduleName = veriStatement.substr(begin, end - begin);

  return moduleName;
}

void CreatePrimitiveGate(void) {
  primitiveGate["and"] = 0;
  primitiveGate["or"] = 1;
  primitiveGate["nand"] = 2;
  primitiveGate["nor"] = 3;
  primitiveGate["xor"] = 4;
  primitiveGate["xnor"] = 5;
  primitiveGate["not"] = 6;
  primitiveGate["buf"] = 7;
}

void FindTopModule(map<string, Module> &moduleMap) {
  map<string, Module>::iterator it;

  for (it = moduleMap.begin(); it != moduleMap.end(); ++it) {
    if (it->second.top == true && libModule.count(it->second.moduleName) == 0) {
      topModule = it->second.moduleName;
      break;
    }
  }
}

rco::VerilogParser::VerilogParser() {
  // do nothing
}

rco::VerilogParser::~VerilogParser() {
  // do nothing
}

bool rco::VerilogParser::parseFile(const std::string &file_path,
                                   ModuleMap &module_map) {
  if (readFile(file_path) == false) {
    return false;
  }

  return parse(module_map);
}

bool rco::VerilogParser::readFile(const std::string &file_path) {
  ifstream in_file(file_path, std::ios::in);
  if (in_file.is_open() == false) {
    return false;
  }
  this->statements_.clear();

  std::string line_buffer(80, '\0');
  while (getline(in_file, line_buffer)) {
    Trim(line_buffer, " \t");
    // to ignore the empty line or comments in the verilog file
    if (line_buffer.empty() || strncmp(line_buffer.c_str(), "//", 2) == 0) {
      continue;
    }

    this->statements_.emplace_back(line_buffer);
  }

  return true;
}

bool rco::VerilogParser::parse(ModuleMap &module_map) {
  string currentModule;
  size_t i;

  for (i = 0; i != this->statements_.size(); i++) {
    //
    if (this->statements_[i].find("module ") != string::npos) {
      if (secondTime == true) {
        currentModule = SplitModuleName(this->statements_[i]);
        continue;
      }
      currentModule = ModuleHandle(this->statements_[i], module_map);
      if (i >= libStart) {
        libModule[currentModule] = currentModule;
      }
    } else if (this->statements_[i].find("input ") != string::npos) {
      if (secondTime == true) {
        continue;
      }
      InputHandle(this->statements_[i], module_map, currentModule);
    } else if (this->statements_[i].find("output ") != string::npos) {
      if (secondTime == true) {
        continue;
      }
      OutputHandle(this->statements_[i], module_map, currentModule);
    } else if (this->statements_[i].find("inout ") != string::npos) {
      cout << "Warning: this program can not handle inout-type port." << endl;
      continue;
    } else if (this->statements_[i].find("wire ") != string::npos) {
      if (secondTime == true) {
        continue;
      }
      WireHandle(this->statements_[i], module_map, currentModule);
    } else if (this->statements_[i].find("endmodule") != string::npos) {
      if (secondTime == true) {
        continue;
      }
      currentModule.clear();
    } else if (secondTime == true) {
      ok = InstanceHandle(this->statements_[i], module_map, currentModule);
    }
  }

  return true;
}
