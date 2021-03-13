#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
using namespace std;

#include "checker.hpp"
#include "data_structure_define.hpp"
#include "little_tools.hpp"
#include "node_testing.hpp"
#include "power_intent_parser.hpp"
#include "report.hpp"
#include "verilog_parser.hpp"
#include "verilog_testing.hpp"

#define TEST 0
#define WINDOWS 0
#define FINAL 1

/*** argument process ***/
bool ArgumentHandle(const int, char *[], map<string, Module> &);

/*** about the data tree ***/
void CreateRoot(void);
void CreateTreeNode(const string &, map<string, Module> &, const string &);
void SetIntoSpecificDomain(map<string, Module> &);
void SetIntoSameDomain(NodePointer &, const string &);
void SetIntoDefaultDomain(map<string, Module> &);
void CreateGraph(map<string, Module> &);
void SetPortInfo(map<string, Module> &);

int main(int argc, char *argv[]) {
  map<string, Module> moduleMap;
  vector<string> fileContent;
  vector<string> veriStatements;
  vector<string> powerStatements;

  if (!ArgumentHandle(argc, argv, moduleMap)) {
#if WINDOWS
    system("pause");
#endif
    return EXIT_FAILURE;
  }
  CreatePrimitiveGate();
  CreateRoot();
  CreateTreeNode(topModule, moduleMap, "");
  SetIntoSpecificDomain(moduleMap);
  SetIntoDefaultDomain(moduleMap);
  SetPortInfo(moduleMap);
  CreateGraph(moduleMap);
#if FINAL
  Checker(moduleMap);
  printReport();
#endif

#if TEST
  // testVerilogParser(moduleMap);
  NodeTesting();
#endif

#if WINDOWS
  system("pause");
#endif

  return EXIT_SUCCESS;
}

bool ArgumentHandle(const int argc, char *argv[],
                    map<string, Module> &moduleMap) {
  map<string, vector<string> > arg;
  vector<string> fileContent, veriStatements, powerStatements, fileList;
  int fileCount = 0;
  int i = 0, j = 0;
  map<string, vector<string> >::iterator it;
  bool keep = true;

  /*if ( argc < 9 ) {
      cerr << "Please check your arguments." << endl;
      return false;
  }*/

  arg["-design"] = fileList;
  arg["-power_intent"] = fileList;
  arg["-library"] = fileList;
  arg["-report"] = fileList;
  for (i = 1; i < argc; i++) {
    if (arg.count(argv[i]) > 0) {
      for (j = i + 1; j < argc; j++) {
        if (arg.count(argv[j]) == 0) {
          arg[argv[i]].push_back(argv[j]);
        } else {
          i = j - 1;
          break;
        }
      }
    } else if (i != argc - 1) {
      cerr << argv[i] << ": option not found." << endl;
    }
  }

  if (arg["-design"].size() < 1) {
    cerr << "There should be at least one design file(xxx.v)." << endl;
    keep = false;
  }
  if (arg["-power_intent"].size() != 1) {
    cerr << "There should be one power intent file(xxx.pi)." << endl;
    keep = false;
  }
  if (arg["-library"].size() < 1) {
    cerr << "There should be at least one library file(xxx.v)." << endl;
    keep = false;
  }
  if (arg["-report"].size() != 1) {
    cerr << "There should be at least one report file name." << endl;
    keep = false;
  }
  if (keep == false) {
    return false;
  }
  /*** To read and to parse the design files ***/
  for (size_t i = 0; i != arg["-design"].size(); i++) {
    if (!ReadVerilogFile(arg["-design"][i].c_str(), fileContent, fileCount)) {
      cerr << "Can not open the file(" << arg["-design"][i] << ")." << endl;
      return false;
    }
    StoreVerilogStatement(fileContent, veriStatements, fileCount);
    ParseVerilogFile(veriStatements, moduleMap);
    fileCount++;
  }
  libStart = veriStatements.size();
  /*** To read and to parse the library files ***/
  for (size_t i = 0; i != arg["-library"].size(); i++) {
    if (!ReadVerilogFile(arg["-library"][i].c_str(), fileContent, fileCount)) {
      cerr << "Can not open the file(" << arg["-library"][i] << ")." << endl;
      return false;
    }
    StoreVerilogStatement(fileContent, veriStatements, fileCount);
    ParseVerilogFile(veriStatements, moduleMap);
    if (!ok) {
      cerr << "This program terminated when parsing the file: "
           << arg["-library"][i] << endl;
      return false;
    }
    fileCount++;
  }
  secondTime = true;
  ParseVerilogFile(veriStatements, moduleMap);
  FindTopModule(moduleMap);
  /*** To read and to parse the power intent files ***/
  if (!ReadPowerFile(arg["-power_intent"].back().c_str(), powerStatements)) {
    cerr << "Can not open the file(" << arg["-power_intent"].back() << ")."
         << endl;
    return false;
  }
  ParsePowerFile(powerStatements, moduleMap);
  fileContent.clear();
  veriStatements.clear();
  powerStatements.clear();

  g_fout.open(arg["-report"].back().c_str());
  if (!g_fout) {
    cerr << "Can not open the file(" << arg["-report"].back() << ")." << endl;
    return false;
  }

  return true;
}

void CreateRoot(void) {
  nodeMap[topModule].hiraName = topModule;
  nodeMap[topModule].type = topModule;
  nodeMap[topModule].name = topModule;
  nodeMap[topModule].father = nullptr;
}

void CreateTreeNode(const string &moduleName, map<string, Module> &moduleMap,
                    const string &nodeName) {
  map<string, Unit>::iterator unitIter;
  string tempName;
  NodePointer tempPointer;

  if (moduleMap.count(moduleName) > 0 && libModule.count(moduleName) == 0) {
    for (unitIter = moduleMap[moduleName].units.begin();
         unitIter != moduleMap[moduleName].units.end(); ++unitIter) {
      tempName = nodeName + "/" + unitIter->first;
      if (tempName[0] == '/') {
        tempName = tempName.substr(1);
      }
      notYetSetDomain[tempName] = true;
      nodeMap[tempName].hiraName = tempName;
      nodeMap[tempName].type = unitIter->second.unitType;
      if (defineIsoCell.count(nodeMap[tempName].type) > 0) {
        isoNodes.push_back(&nodeMap[tempName]);
      } else if (defineLevShCell.count(nodeMap[tempName].type) > 0) {
        levShNodes.push_back(&nodeMap[tempName]);
      }
      nodeMap[tempName].name = unitIter->second.unitName;
      nodeMap[tempName].domainDef = false;
      if (nodeName == "") {
        nodeMap[tempName].father = &nodeMap[moduleName];
        tempPointer = &nodeMap[tempName];
        nodeMap[moduleName].children.push_back(tempPointer);
      } else {
        nodeMap[tempName].father = &nodeMap[nodeName];
        tempPointer = &nodeMap[tempName];
        nodeMap[nodeName].children.push_back(tempPointer);
      }
      CreateTreeNode(unitIter->second.unitType, moduleMap, tempName);
    }
  }
}

void SetIntoSpecificDomain(map<string, Module> &moduleMap) {
  map<string, C_P_D>::iterator iter;
  vector<string> domainDef;
  size_t i, j;
  string domain;
  map<string, bool>::iterator k;
  PortDomain portDomain;

  for (iter = createPowDomain.begin(); iter != createPowDomain.end(); ++iter) {
    for (i = 0; i < iter->second.instances.size(); i++) {
      nodeMap[iter->second.instances[i]].domain = iter->second.name;
      nodeMap[iter->second.instances[i]].domainDef = true;
      notYetSetDomain.erase(iter->second.instances[i]);
      domainDef.push_back(iter->second.instances[i]);
    }
    for (i = 0; i < iter->second.ports.size(); i++) {
      portDomain.portName = iter->second.ports[i];
      portDomain.domain = iter->second.name;
      topPortDomain[portDomain.portName] = portDomain;
    }
  }

  for (i = 0; i < domainDef.size(); i++) {
    domain = nodeMap[domainDef[i]].domain;
    for (j = 0; j < nodeMap[domainDef[i]].children.size(); j++) {
      SetIntoSameDomain(nodeMap[domainDef[i]].children[j], domain);
    }
  }
}

void SetIntoSameDomain(NodePointer &pointer, const string &domain) {
  size_t i;

  if (pointer->domainDef == false) {
    pointer->domain = domain;
    pointer->domainDef = true;
    notYetSetDomain.erase(pointer->hiraName);
    for (i = 0; i < pointer->children.size(); i++) {
      SetIntoSameDomain(pointer->children[i], domain);
    }
  }
}

void SetIntoDefaultDomain(map<string, Module> &moduleMap) {
  map<string, bool>::iterator iter;
  map<string, Port>::iterator it;
  PortDomain portDomain;

  /*** set instances into default domain ***/
  for (iter = notYetSetDomain.begin(); iter != notYetSetDomain.end(); ++iter) {
    nodeMap[iter->first].domain = defaultDomain;
    // notYetSetDomain.erase(iter);
  }
  notYetSetDomain.clear();

  /*** set ports of the top level into default domain ***/
  for (it = moduleMap[topModule].ports.begin();
       it != moduleMap[topModule].ports.end(); ++it) {
    if (topPortDomain.count(it->first) != 1) {
      portDomain.portName = it->first;
      portDomain.domain = defaultDomain;
      topPortDomain[it->first] = portDomain;
    }
  }
}

void CreateGraph(map<string, Module> &moduleMap) {
  map<string, Port>::iterator portIter;
  unsigned int i = 0;
  WireConnectInfo *tempCon;
  bool isoNode = false;
  bool con = false;  // to continue?
  set<string> pre, suc;

  for (nodeIter = nodeMap.begin(); nodeIter != nodeMap.end(); ++nodeIter) {
    if (nodeIter->second.type == topModule) {
      continue;
    }
    if (defineIsoCell.count(nodeIter->second.type) > 0) {
      isoNode = true;
    }
    for (portIter = moduleMap[nodeIter->second.father->type]
                        .units[nodeIter->second.name]
                        .ports.begin();
         portIter != moduleMap[nodeIter->second.father->type]
                         .units[nodeIter->second.name]
                         .ports.end();
         ++portIter) {
      if (isoNode == true) {
        if (defineIsoCell[nodeIter->second.type].enable ==
            portIter->second.portName) {
          con = true;
        }
      }
      if (con == true) {
        con = false;
        continue;
      }
      if (portIter->second.type == PortType::IN) {
        for (i = 0; i < moduleMap[nodeIter->second.father->type]
                            .wires[portIter->second.connectWireName]
                            .conInfo.size();
             i++) {
          tempCon = &moduleMap[nodeIter->second.father->type]
                         .wires[portIter->second.connectWireName]
                         .conInfo[i];
          if (tempCon->instanceType == nodeIter->second.father->type) {
            if (nodeIter->second.father->type == topModule) {
              if (pre.count(topModule) == 0) {
                pre.insert(topModule);
                nodeIter->second.pre.push_back(topModule);
              }
            } else {
              if (pre.count(nodeIter->second.father->hiraName) == 0) {
                pre.insert(nodeIter->second.father->hiraName);
                nodeIter->second.pre.push_back(
                    nodeIter->second.father->hiraName);
              }
            }
            continue;
          }
          if (moduleMap[tempCon->instanceType].ports[tempCon->portName].type ==
              PortType::OUT) {
            if (nodeIter->second.father->type == topModule) {
              if (pre.count(tempCon->instanceName) == 0) {
                pre.insert(tempCon->instanceName);
                nodeIter->second.pre.push_back(tempCon->instanceName);
              }
            } else {
              if (pre.count(nodeIter->second.father->hiraName + '/' +
                            tempCon->instanceName) == 0) {
                pre.insert(nodeIter->second.father->hiraName + '/' +
                           tempCon->instanceName);
                nodeIter->second.pre.push_back(
                    nodeIter->second.father->hiraName + '/' +
                    tempCon->instanceName);
              }
            }
          }
        }
      } else {
        for (i = 0; i < moduleMap[nodeIter->second.father->type]
                            .wires[portIter->second.connectWireName]
                            .conInfo.size();
             i++) {
          tempCon = &moduleMap[nodeIter->second.father->type]
                         .wires[portIter->second.connectWireName]
                         .conInfo[i];
          if (tempCon->instanceType == nodeIter->second.father->type) {
            if (nodeIter->second.father->type == topModule) {
              if (suc.count(topModule) == 0) {
                suc.insert(topModule);
                nodeIter->second.suc.push_back(topModule);
              }
            } else {
              if (suc.count(nodeIter->second.father->hiraName) == 0) {
                suc.insert(nodeIter->second.father->hiraName);
                nodeIter->second.suc.push_back(
                    nodeIter->second.father->hiraName);
              }
            }
            continue;
          }
          if (moduleMap[tempCon->instanceType].ports[tempCon->portName].type ==
              PortType::IN) {
            if (nodeIter->second.father->type == topModule) {
              if (suc.count(tempCon->instanceName) == 0) {
                suc.insert(tempCon->instanceName);
                nodeIter->second.suc.push_back(tempCon->instanceName);
              }
            } else {
              if (suc.count(nodeIter->second.father->hiraName + '/' +
                            tempCon->instanceName) == 0) {
                suc.insert(nodeIter->second.father->hiraName + '/' +
                           tempCon->instanceName);
                nodeIter->second.suc.push_back(
                    nodeIter->second.father->hiraName + '/' +
                    tempCon->instanceName);
              }
            }
          }
        }
      }
    }
    pre.clear();
    suc.clear();
    isoNode = false;
  }
}

void SetPortInfo(map<string, Module> &moduleMap) {
  PortInfo tPortInfo;
  map<string, Module>::iterator iModule;
  map<string, Port>::iterator iPort;
  map<string, Unit>::iterator iUnit;
  map<string, Wire>::iterator iWire;
  vector<WireConnectInfo>::iterator iWireCon;
  NodeDomain tempNodeDomain;

  for (nodeIter = nodeMap.begin(); nodeIter != nodeMap.end(); ++nodeIter) {
    if (nodeIter->second.type == topModule) {  // to skip the top node
      continue;
    }
    iModule = moduleMap.find(nodeIter->second.father->type);
    iUnit = iModule->second.units.find(nodeIter->second.name);
    for (iPort = iUnit->second.ports.begin();
         iPort != iUnit->second.ports.end(); ++iPort) {
      iWire = iModule->second.wires.find(iPort->second.connectWireName);
      for (iWireCon = iWire->second.conInfo.begin();
           iWireCon != iWire->second.conInfo.end(); ++iWireCon) {
        if (iWireCon->instanceType == nodeIter->second.father->type) {
          tPortInfo.conNode = nodeIter->second.father;
        } else if (iWireCon->instanceName == nodeIter->second.name) {
          continue;
        } else if (nodeIter->second.father->type != topModule) {
          tPortInfo.conNode = &nodeMap[nodeIter->second.father->hiraName + '/' +
                                       iWireCon->instanceName];
        } else {
          tPortInfo.conNode = &nodeMap[iWireCon->instanceName];
        }
        tPortInfo.conPortType = iWireCon->portType;
        tPortInfo.conPortName = iWireCon->portName;
        tPortInfo.wireName = iWire->second.wireName;
        if (iPort->second.type == PortType::IN) {
          if ((tPortInfo.conPortType == PortType::OUT) ||
              (tPortInfo.conNode == nodeIter->second.father &&
               tPortInfo.conPortType == PortType::IN)) {
            tempNodeDomain.node = tPortInfo.conNode->hiraName;
            if (tPortInfo.conNode->type == topModule) {
              tempNodeDomain.domain =
                  topPortDomain[tPortInfo.conPortName].domain;
            } else {
              tempNodeDomain.domain = tPortInfo.conNode->domain;
            }
            tempNodeDomain.selfPort = iPort->second.portName;
            tempNodeDomain.otherPort = tPortInfo.conPortName;
            nodeIter->second.source.push_back(tempNodeDomain);
            tempNodeDomain.clear();
          }
          nodeIter->second.inPort[iPort->second.portName].push_back(tPortInfo);
        } else if (iPort->second.type == PortType::OUT) {
          if ((tPortInfo.conPortType == PortType::IN) ||
              (tPortInfo.conNode == nodeIter->second.father &&
               tPortInfo.conPortType == PortType::OUT)) {
            tempNodeDomain.node = tPortInfo.conNode->hiraName;
            if (tPortInfo.conNode->type == topModule) {
              tempNodeDomain.domain =
                  topPortDomain[tPortInfo.conPortName].domain;
            } else {
              tempNodeDomain.domain = tPortInfo.conNode->domain;
            }
            tempNodeDomain.selfPort = iPort->second.portName;
            tempNodeDomain.otherPort = tPortInfo.conPortName;
            nodeIter->second.destination.push_back(tempNodeDomain);
            tempNodeDomain.clear();
          }
          nodeIter->second.outPort[iPort->second.portName].push_back(tPortInfo);
        }
        tPortInfo.clear();
      }
    }
  }
}
