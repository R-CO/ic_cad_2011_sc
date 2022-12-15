#include "checker.hpp"

// C++ standard library
#include <iostream>
using std::cout;
using std::endl;
#include <sstream>
using std::stringstream;
#include <string>
using std::string;

// C++ STL container
#include <map>
using std::map;

#include "data_structure_define.hpp"
#include "power_intent_parser.hpp"
#include "verilog_parser.hpp"

string g_source;
string g_destination;

void Checker(map<string, Module> &moduleMap) {
  for (unsigned int i = 0; i < isoNodes.size(); i++) {
    CheckISO_1(isoNodes[i], moduleMap);

    if (!CheckISO_2_3(isoNodes[i], moduleMap)) continue;

    if (!CheckISO_5(isoNodes[i], moduleMap)) continue;

    if (!CheckISO_6(isoNodes[i], moduleMap)) continue;
  }

  for (unsigned int i = 0; i < levShNodes.size(); i++)
    CheckLS_1_2_4(levShNodes[i], moduleMap);

  CheckISO_4_LS_3(moduleMap);
}

bool CheckISO_2_3(NodePointer ISONodes, map<string, Module> &moduleMap) {
  NodePointer temp = ISONodes;

  g_source.clear();
  g_destination.clear();

  if (temp->pre.size() != 1) {
    return false;
  }
  while (temp->pre.back() == temp->father->hiraName) {
    temp = temp->father;
    if (temp->pre.size() != 1) {
      return false;
    }
  }

  if (temp->father->hiraName != topModule) {
    g_source = nodeMap[temp->pre.back()].domain;
  } else {
    return false;
  }

  temp = ISONodes;
  if (temp->suc.size() != 1) {
    return false;
  }

  while (temp->suc.back() == temp->father->hiraName) {
    temp = temp->father;
    if (temp->pre.size() != 1) {
      return false;
    }
  }

  if (temp->father->hiraName != topModule) {
    g_destination = nodeMap[temp->suc.back()].domain;
  } else {
    return false;
  }

  // Check ISO_2
  if (g_source == g_destination) {
    string s;

    s = "[Instance: " + ISONodes->hiraName + " (" + ISONodes->type +
        ") Source and g_destination power domain: " + g_source + "]";
    g_report[1].str.push_back(s);

    return false;
  }

  string locate = defineIsoCell[ISONodes->type].location;

  if (locate == "from") {
    if (ISONodes->domain == g_destination) {
      string s;

      s = "['" + ISONodes->hiraName + "' (" + ISONodes->type +
          ") is not allowed in g_source domain\n";
      s += "\t\t\t Applied rule: " +
           createIsoRule[g_source + g_destination].name + "]";
      g_report[2].str.push_back(s);

      return false;
    } else
      return true;
  } else if (locate == "to") {
    if (ISONodes->domain == g_source) {
      string s;

      s = "['" + ISONodes->hiraName + "' (" + ISONodes->type +
          ") is not allowed in g_source domain\n";
      s += "\t\t\t Applied rule: " +
           createIsoRule[g_source + g_destination].name + "]";
      g_report[2].str.push_back(s);

      return false;
    } else
      return true;
  } else {
    cout << "********  Unknown iso cell location for: " << ISONodes->hiraName
         << endl;
    cout << "Location: " << locate << endl;
    return false;
  }

  return true;
}

bool CheckLS_1_2_4(NodePointer LSNodes, map<string, Module> &moduleMap) {
  unsigned int i;
  string source, destination;
  string soutce_name, destination_name;
  NodePointer temp = LSNodes;

  if (temp->pre.size() != 1) {
    return false;
  }
  while (temp->pre.back() == temp->father->hiraName) {
    temp = temp->father;
    if (temp->pre.size() != 1) {
      return false;
    }
  }

  if (temp->father->hiraName != topModule) {
    source = nodeMap[temp->pre.back()].domain;
    soutce_name = nodeMap[temp->pre.back()].hiraName;
  } else {
    return false;
  }

  temp = LSNodes;
  if (temp->suc.size() != 1) {
    return false;
  }

  while (temp->suc.back() == temp->father->hiraName) {
    temp = temp->father;
    if (temp->pre.size() != 1) {
      return false;
    }
  }

  if (temp->father->hiraName != topModule) {
    destination = nodeMap[temp->suc.back()].domain;
    destination_name = nodeMap[temp->suc.back()].hiraName;
  } else {
    return false;
  }

  // find legal voltage
  map<string, D_L_S_C>::iterator itter;
  float inLow, inHigh, outLow, outHigh;
  for (itter = defineLevShCell.begin(); itter != defineLevShCell.end();
       ++itter) {
    if (itter->second.cell == temp->type) {
      inLow = itter->second.inLow;
      inHigh = itter->second.inHigh;
      outLow = itter->second.outLow;
      outHigh = itter->second.outHigh;
      break;
    }
  }

  // Check LS4
  if ((LSNodes->pre.size() != 0) && (LSNodes->suc.size() != 0)) {
    if (source == destination) {
      string s;
      string finalport;
      string finalsource =
          findS(moduleMap, LSNodes->hiraName, PortType::IN, finalport);
      string finalport2;
      string finalsource2 =
          findS(moduleMap, LSNodes->hiraName, PortType::OUT, finalport2);

      s = "[Level shifter '" + LSNodes->hiraName + "' (" + LSNodes->type +
          ") has the same input/output voltage domain in all power modes\n";
      s += "\t\t\t Input pin driver: " + soutce_name + "/" + finalsource + "/" +
           finalport + "\n";
      s += "\t\t\t Output pin load: " + destination_name + "/" + finalsource2 +
           "/" + finalport2 + "\n";

      g_report[9].str.push_back(s);
      return false;
    }
  }

  // Check LS1 LS2
  bool chLS1 = true;
  bool chLS2 = true;

  for (i = 0; i < createPowMode.size(); i++) {
    // Has some problem in here
    if (createPowMode[i].defaultMode == false) continue;

    string source_name = CPM_DomainToNom(createPowMode[i], source);
    string dest_name = CPM_DomainToNom(createPowMode[i], destination);
    float source_vol = CNC_NameToVoltage(source_name);
    float dest_vol = CNC_NameToVoltage(dest_name);

    if ((LSNodes->pre.size() != 0) && chLS1) {
      if (source_vol > 0) {
        if ((source_vol < inLow) || (source_vol > inHigh)) {
          string s;

          s = "[Pin '" +
              findPort(LSNodes->hiraName, PortType::IN, moduleMap, "-1") +
              "' of '" + LSNodes->hiraName + "' (module " + LSNodes->type +
              " ) is connected to domain " + source + " (Power Voltage " +
              float2str(source_vol) + "V)\n";
          s += "\t\t\t Expected voltage domain is ";

          if (inLow == inHigh)
            s += float2str(inLow) + "V]";
          else
            s += float2str(inLow) + "V:" + float2str(inHigh) + "V]";

          g_report[6].str.push_back(s);
          chLS1 = false;
        }
      }
    }

    if ((LSNodes->suc.size() != 0) && chLS2) {
      if (dest_vol > 0) {
        if ((dest_vol < outLow) || (dest_vol > outHigh)) {
          string s;

          s = "[Pin '" +
              findPort(LSNodes->hiraName, PortType::OUT, moduleMap, "-1") +
              "' of '" + LSNodes->hiraName + "' (module " + LSNodes->type +
              " ) is connected to domain " + destination + " (Power Voltage " +
              float2str(dest_vol) + "V)\n";
          s += "\t\t\t Expected voltage domain is ";

          if (outLow == outHigh)
            s += float2str(outLow) + "V]";
          else
            s += float2str(outLow) + "V:" + float2str(outHigh) + "V]";

          g_report[7].str.push_back(s);
          chLS2 = false;
        }
      }
    }
  }

  return true;
}

void CheckISO_1(NodePointer ISONode, map<string, Module> &moduleMap) {
  string signal = moduleMap[ISONode->father->type]
                      .units[ISONode->name]
                      .ports[defineIsoCell[ISONode->type].enable]
                      .connectWireName;

  if (topPortDomain.count(signal) == 0) {
    return;
  }

  for (size_t i = 0; i < createPowMode.size(); i++) {
    if (createNomCondition[createPowMode[i]
                               .condition[topPortDomain[signal].domain]]
                .voltage == 0 &&
        createNomCondition[createPowMode[i].condition[ISONode->domain]]
                .voltage != 0.0) {
      string s;

      s = "[Pin: '" + signal +
          "' in control path can be OFF when the isolation control pin '" +
          ISONode->hiraName + "/" + defineIsoCell[ISONode->type].enable +
          "' is ON]";

      g_report[0].str.push_back(s);

      // cout << "Error: Pin: \'" << signal << "\' in control path can be OFF
      // when the isolation control pin \'" << ISONode->hiraName << "/" <<
      // defineIsoCell[ISONode->type].enable << "\'" << endl;
      break;
    }
  }

  return;
}

// modify here
bool CheckISO_4_LS_3(map<string, Module> &moduleMap) {
  unsigned int i, j, k;
  // string suc_hirname;
  map<string, Node>::iterator sucIter;
  // string destination;

  // Traversal All Map
  for (nodeIter = nodeMap.begin(); nodeIter != nodeMap.end(); ++nodeIter) {
    if (isSpeciallInstance(
            nodeIter->second.type,
            nodeIter->second.hiraName))  // This instance is special case
      continue;

    for (i = 0; i < nodeIter->second.suc.size(); i++) {
      sucIter = nodeMap.find(nodeIter->second.suc[i]);  // find suc instance

      if (isSpeciallInstance(
              sucIter->second.type,
              sucIter->second.hiraName))  // suc instance is special case
        continue;

      bool isIgnore = false;
      for (k = 0; k < sucIter->second.children.size(); k++) {
        if (isSpeciallInstance(sucIter->second.children[k]->type,
                               sucIter->second.children[k]->name)) {
          isIgnore = true;
          break;
        }
      }
      if (isIgnore) continue;

      // Check LS 3
      if (createLevShRule.find(nodeIter->second.domain +
                               sucIter->second.domain) !=
          createLevShRule.end()) {
        float source_vol = 0;
        float dest_vol = 0;

        for (unsigned int k = 0; k < createPowMode.size(); k++) {
          if (createPowMode[k].defaultMode == false) continue;

          string source_name =
              CPM_DomainToNom(createPowMode[k], nodeIter->second.domain);
          string dest_name =
              CPM_DomainToNom(createPowMode[k], sucIter->second.domain);
          source_vol = CNC_NameToVoltage(source_name);
          dest_vol = CNC_NameToVoltage(dest_name);
        }

        string s;

        string finalport;
        string finalsource = findS(moduleMap, nodeIter->second.hiraName,
                                   PortType::OUT, finalport);

        s = "[Driver Pin: '" + nodeIter->second.hiraName + "/" + finalsource +
            "/" + finalport + "'(voltage: " + float2str(source_vol) +
            "V) to Load Pin: '" + sucIter->second.hiraName + "/" +
            findPort(sucIter->second.hiraName, PortType::IN, moduleMap, "-1") +
            "'(voltage: " + float2str(dest_vol) +
            "V) does not have level shifter cell]";

        g_report[8].str.push_back(s);
      }

      // Check ISO 4
      for (j = 0; j < createPowMode.size(); j++) {
        string source_name =
            CPM_DomainToNom(createPowMode[j], nodeIter->second.domain);
        string dest_name =
            CPM_DomainToNom(createPowMode[j], sucIter->second.domain);
        float source_vol = CNC_NameToVoltage(source_name);
        float dest_vol = CNC_NameToVoltage(dest_name);

        C_I_R *c =
            &createIsoRule[nodeIter->second.domain + sucIter->second.domain];

        if ((source_vol - 0.0 > 0.1) && (dest_vol - 0.0 < 0.0001)) {  // On->off
          string s;

          s = "[Pin: '" + nodeIter->second.hiraName + "/" +
              findPort(nodeIter->second.hiraName, PortType::IN, moduleMap,
                       "-1") +
              "' in rule '" + c->name + "' does not have isolation cell]";

          g_report[3].str.push_back(s);
        } else if ((source_vol - 0.0 < 0.0001) &&
                   (dest_vol - 0.0 > 0.1)) {  // Off->on
          string s;

          s = "[Pin: '" + nodeIter->second.hiraName + "/" +
              findPort(nodeIter->second.hiraName, PortType::IN, moduleMap,
                       "-1") +
              "' in rule '" + c->name + "' does not have isolation cell]";

          g_report[3].str.push_back(s);
        } else {  // On->on or On->off
          ;       // do nothing
        }
      }
    }
  }

  return true;
}

string CPM_DomainToNom(C_P_M mode, string &domain) {
  map<string, string>::iterator iter;

  iter = mode.condition.find(domain);

  if (iter != mode.condition.end())
    return iter->second;
  else
    cout << "Create_Power_Mode can't find " << domain << " in " << mode.name
         << endl;

  return nullptr;
}

float CNC_NameToVoltage(string name) {
  map<string, C_N_C>::iterator cnc;

  cnc = createNomCondition.find(name);

  if (cnc != createNomCondition.end())
    return cnc->second.voltage;
  else
    cout << "Create_nominal_condition can't find " << name << endl;

  return -1;
}

bool isSpeciallInstance(string &type, string &name) {
  if (defineIsoCell.find(type) != defineIsoCell.end()) return true;

  if (defineLevShCell.find(type) != defineLevShCell.end()) return true;

  if (type == topModule) return true;

  return false;
}

bool CheckISO_5(NodePointer ISONodes, map<string, Module> &moduleMap) {
  NodePointer father = ISONodes->father;

  if (moduleMap[father->type]
          .units[ISONodes->name]
          .ports[defineIsoCell[ISONodes->type].enable]
          .connectWireName !=
      createIsoRule[g_source + g_destination].condition) {
    string s;
    D_I_C *cell = &defineIsoCell[ISONodes->type];
    C_I_R *c = &createIsoRule[g_source + g_destination];

    s = "[The control pin '" + cell->enable + "' of '" + ISONodes->hiraName +
        "' is connected to a wrong pin. \n";
    s += "\t\t\t Rule '" + createIsoRule[g_source + g_destination].name +
         "' - specified control pin is ";

    if (c->negation)
      s += "!" + c->condition + "]";
    else
      s += c->condition + "]";

    g_report[4].str.push_back(s);

    return false;
  }

  return true;
}

bool CheckISO_6(NodePointer ISONodes, map<string, Module> &moduleMap) {
  // Build Table
  int table[4][8];
  memset(table, 0, sizeof(int) * 4 * 8);

  // and
  table[0][0] = 1;
  table[0][1] = 1;
  table[0][2] = 1;
  table[0][3] = 1;
  table[0][4] = 0;
  table[0][5] = 0;
  table[0][6] = 0;
  table[0][7] = 0;

  // or
  table[1][0] = 0;
  table[1][1] = 0;
  table[1][2] = 0;
  table[1][3] = 0;
  table[1][4] = 1;
  table[1][5] = 1;
  table[1][6] = 1;
  table[1][7] = 1;

  // nand
  table[2][0] = 1;
  table[2][1] = 1;
  table[2][2] = 1;
  table[2][3] = 1;
  table[2][4] = 0;
  table[2][5] = 0;
  table[2][6] = 0;
  table[2][7] = 0;

  // nor
  table[3][0] = 0;
  table[3][1] = 0;
  table[3][2] = 0;
  table[3][3] = 0;
  table[3][4] = 1;
  table[3][5] = 1;
  table[3][6] = 1;
  table[3][7] = 1;

  C_I_R *c = &createIsoRule[g_source + g_destination];
  D_I_C *cell = &defineIsoCell[ISONodes->type];
  Module *module = &moduleMap[ISONodes->type];
  Module *father = &moduleMap[ISONodes->father->type];

  int EN;
  if (c->negation)
    EN = 0;
  else
    EN = 1;

  string control = cell->enable;
  string subType;
  int t = 0;

  // Find subModule
  map<string, Unit>::iterator iter_u;
  for (iter_u = module->units.begin(); iter_u != module->units.end();
       ++iter_u) {
    subType = iter_u->second.unitType;
  }

  // Find type
  if (subType == "and")
    t = 0;
  else if (subType == "or")
    t = 1;
  else if (subType == "nand")
    t = 2;
  else if (subType == "nor")
    t = 3;
  else if (subType == "xor")
    t = 4;
  else if (subType == "xnor")
    t = 5;
  else if (subType == "not")
    t = 6;
  else
    t = 7;

  // Module
  int A, B, C;
  map<string, Port>::iterator ports;
  for (ports = module->ports.begin(); ports != module->ports.end(); ++ports) {
    if (ports->second.portName == control) {
      if (ports->second.inverted)
        A = 0;
      else
        A = 1;
    } else if (ports->second.type == PortType::IN) {
      if (ports->second.inverted)
        B = 0;
      else
        B = 1;

    } else if (ports->second.type == PortType::OUT) {
      if (ports->second.inverted)
        C = 0;
      else
        C = 1;
    }
  }

  // ISO Cell
  for (iter_u = father->units.begin(); iter_u != father->units.end();
       ++iter_u) {
    if (iter_u->second.unitName == ISONodes->name) break;
  }

  for (ports = iter_u->second.ports.begin();
       ports != iter_u->second.ports.end(); ++ports) {
    if (ports->second.portName == control) {
      if (ports->second.inverted) {
        if (A == 0)
          A = 1;
        else
          A = 0;
      }
    } else if (ports->second.type == PortType::IN) {
      if (ports->second.inverted) {
        if (B == 0)
          B = 1;
        else
          B = 0;
      }
    } else if (ports->second.type == PortType::OUT) {
      if (ports->second.inverted) {
        if (C == 0)
          C = 1;
        else
          C = 0;
      }
    }
  }

  if (t < 4) {
    if (table[t][A * 4 + B * 2 + C * 1] != EN) {
      string s;

      s = "[The control pin '" + control + "' of '" + ISONodes->hiraName +
          "' is connected to the correct pin but with wrong polarity\n";
      s += "\t\t\t Rule '" + c->name + "' - specified control pin is ";

      if (c->negation)
        s += "!" + c->condition + "]";
      else
        s += c->condition + "]";

      g_report[5].str.push_back(s);

      return false;
    } else
      return true;
  } else
    return false;

  return true;
}

string float2str(float &i) {
  string s;
  stringstream ss(s);
  ss << i;

  return ss.str();
}

string findPort(string hirname, PortType type, map<string, Module> &moduleMap,
                const string &notsame) {
  NodePointer instance = &nodeMap[hirname];
  Module *mou = &moduleMap[instance->type];
  map<string, Port>::iterator p;

  for (p = mou->ports.begin(); p != mou->ports.end(); ++p) {
    if ((p->second.type == type) && (notsame != p->second.portName))
      return p->second.portName;
  }

  return "Y";
}

string findControl(string type) {
  D_I_C *d = &defineIsoCell[type];
  return d->enable;
}

string findS(map<string, Module> &moduleMap, string hirname,
             PortType source_type, string &finalport) {
  // LS Cell
  NodePointer nowNode = &nodeMap[hirname];  // LS
  // Module* nowModule = &moduleMap[nowNode->type];		// LS
  NodePointer father = nowNode->father;                 // Father
  Module *fatherModule = &moduleMap[father->type];      // Father
  Unit *nowUnit = &fatherModule->units[nowNode->name];  // LS
  map<string, Port>::iterator now_port_iter;
  string nowWireName;

  for (now_port_iter = nowUnit->ports.begin();
       now_port_iter != nowUnit->ports.end(); ++now_port_iter) {
    if (source_type == now_port_iter->second.type) {
      nowWireName = now_port_iter->second.connectWireName;
      break;
    }
  }

  Wire *nowWire = &fatherModule->wires[nowWireName];
  string nextInstanceName;
  string nextPortName;

  for (unsigned int i = 0; i < nowWire->conInfo.size(); i++) {
    if (nowWire->conInfo[i].instanceName != hirname) {
      nextInstanceName = nowWire->conInfo[i].instanceName;
      nextPortName = nowWire->conInfo[i].portName;
      break;
    }
  }

  Unit *nextUnit = &fatherModule->units[nextInstanceName];  // Next
  Module *nextModule = &moduleMap[nextUnit->unitType];      // Next
  Port *nextPort = &nextModule->ports[nextPortName];        // Next
  string nextWireName = nextPort->connectWireName;          // Next
  Wire *nextWire = &nextModule->wires[nextWireName];        // Next
  string finalInstanceName;
  string finalPortName;

  // cout << "nextUnit: " << nextUnit->unitType << endl;
  // cout << "nextInstanceName: " << nextInstanceName << endl;
  // cout << "nextModule: " << nextModule->moduleName << endl;
  // cout << "nextPort: " << nextPort->portName << endl;
  // cout << "nextWireName: " << nextWireName << endl;
  // cout << "Size: " << nextWire->conInfo.size() << endl;
  for (unsigned int i = 0; i < nextWire->conInfo.size(); i++) {
    // cout << "Name: " << nextWire->conInfo[i].instanceName << endl;
    if (nextWire->conInfo[i].instanceType != nextModule->moduleName) {
      finalInstanceName = nextWire->conInfo[i].instanceName;
      finalPortName = nextWire->conInfo[i].portName;
      break;
    }
  }

  finalport = finalPortName;

  return finalInstanceName;
}

string findD(map<string, Module> &moduleMap, string hirname,
             PortType source_type, string &finalport) {
  // LS Cell
  NodePointer nowNode = &nodeMap[hirname];  // LS
  // Module* nowModule = &moduleMap[nowNode->type];		// LS
  NodePointer father = nowNode->father;                 // Father
  Module *fatherModule = &moduleMap[father->type];      // Father
  Unit *nowUnit = &fatherModule->units[nowNode->name];  // LS
  map<string, Port>::iterator now_port_iter;
  string nowWireName;

  for (now_port_iter = nowUnit->ports.begin();
       now_port_iter != nowUnit->ports.end(); ++now_port_iter) {
    if (source_type == now_port_iter->second.type) {
      nowWireName = now_port_iter->second.connectWireName;
      break;
    }
  }

  Wire *nowWire = &fatherModule->wires[nowWireName];
  string nextInstanceName;
  string nextPortName;

  for (unsigned int i = 0; i < nowWire->conInfo.size(); i++) {
    if (nowWire->conInfo[i].instanceName != hirname) {
      nextInstanceName = nowWire->conInfo[i].instanceName;
      nextPortName = nowWire->conInfo[i].portName;
      break;
    }
  }

  Unit *nextUnit = &fatherModule->units[nextInstanceName];  // Next
  Module *nextModule = &moduleMap[nextUnit->unitType];      // Next
  Port *nextPort = &nextModule->ports[nextPortName];        // Next
  string nextWireName = nextPort->connectWireName;          // Next
  Wire *nextWire = &nextModule->wires[nextWireName];        // Next
  string finalInstanceName;
  string finalPortName;

  for (unsigned int i = 0; i < nextWire->conInfo.size(); i++) {
    if (nextWire->conInfo[i].instanceType != nextModule->moduleName) {
      finalInstanceName = nextWire->conInfo[i].instanceName;
      finalPortName = nextWire->conInfo[i].portName;
      break;
    }
  }

  finalport = finalPortName;

  return finalInstanceName;
}