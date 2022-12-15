#ifndef IC_CAD_2011_SC_DATA_STRUCTRUE_DEFINE_HPP
#define IC_CAD_2011_SC_DATA_STRUCTRUE_DEFINE_HPP

// C++ standard library
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

// C++ STL container
#include <map>
#include <unordered_map>
#include <vector>

enum class PortType { IN, OUT, INOUT };

enum class Location { FROM, TO, EITHER };

enum class IsoGateType { AND, OR, NAND, NOR, XOR, XNOR };

struct WireConnectInfo {
  int portOrder;
  PortType portType;
  std::string instanceType;
  std::string instanceName;
  std::string portName;

  WireConnectInfo()
      : portOrder(-1),
        portType(PortType::IN),
        instanceType(),
        instanceName(),
        portName() {}

  void clear() {
    this->portOrder = -1;
    this->portType = PortType::IN;
    this->instanceType.clear();
    this->instanceName.clear();
    this->portName.clear();
  }

  friend bool operator==(const WireConnectInfo &lhs,
                         const WireConnectInfo &rhs) {
    if (lhs.portType == rhs.portType && lhs.portOrder == rhs.portOrder &&
        lhs.instanceType == rhs.instanceType &&
        lhs.instanceName == rhs.instanceName &&
        lhs.portOrder == rhs.portOrder) {
      return true;
    }

    return false;
  }
};

struct Wire {
  bool multiBits;
  int from;
  int to;
  std::string wireName;
  std::vector<WireConnectInfo> conInfo;

  Wire() : multiBits(false), from(-1), to(-1), wireName(), conInfo() {}

  void clear() {
    this->multiBits = false;
    this->from = -1;
    this->to = -1;
    this->wireName.clear();
    this->conInfo.clear();
  }

  friend bool operator==(const Wire &lhs, const Wire &rhs) {
    if (lhs.multiBits == rhs.multiBits && lhs.from == rhs.from &&
        lhs.to == rhs.to && lhs.wireName == rhs.wireName &&
        lhs.conInfo == rhs.conInfo) {
      return true;
    }

    return false;
  }
};

struct Port {
  bool multiBits;
  bool inverted;  // is there a simbol '~' in front of the connect-wire name
  int portOrder;
  int from;
  int to;
  PortType type;
  std::string portName;
  std::string connectWireName;

  Port()
      : multiBits(false),
        inverted(false),
        portOrder(-1),
        from(-1),
        to(-1),
        type(PortType::IN),
        portName(),
        connectWireName() {}

  void clear() {
    this->multiBits = false;
    this->inverted = false;
    this->portOrder = -1;
    this->from = -1;
    this->to = -1;
    this->type = PortType::IN;
    this->portName.clear();
    this->connectWireName.clear();
  }

  friend bool operator==(const Port &lhs, const Port &rhs) {
    if (lhs.multiBits == rhs.multiBits && lhs.inverted == rhs.inverted &&
        lhs.portOrder == rhs.portOrder && lhs.from == rhs.from &&
        lhs.to == rhs.to && lhs.portName == rhs.portName &&
        lhs.connectWireName == rhs.connectWireName) {
      return true;
    }

    return false;
  }
};

struct Unit {
  std::string unitType;  // module
  std::string unitName;  // instance name
  std::map<std::string, Port> ports;

  Unit() : unitType(), unitName(), ports() {}

  void clear() {
    this->unitType.clear();
    this->unitName.clear();
    this->ports.clear();
  }

  friend bool operator==(const Unit &lhs, const Unit &rhs) {
    if (lhs.unitType == rhs.unitType && lhs.unitName == rhs.unitName &&
        lhs.ports == rhs.ports) {
      return true;
    }

    return false;
  }
};

struct Module {                       // to record the information of a module
  bool top;                           // is this a top module
  std::string moduleName;             // name of the module
  std::map<std::string, Wire> wires;  // wires of the module
  std::map<std::string, Port> ports;  // ports of the module
  std::map<std::string, Unit> units;  // instances of the module

  Module() : top(false), moduleName(), wires(), ports(), units() {}

  void clear() {
    this->top = false;
    this->moduleName.clear();
    this->wires.clear();
    this->ports.clear();
    this->units.clear();
  }

  friend bool operator==(const Module &lhs, const Module &rhs) {
    if (lhs.moduleName == rhs.moduleName && lhs.top == rhs.top &&
        lhs.wires == rhs.wires && lhs.ports == rhs.ports &&
        lhs.units == rhs.units) {
      return true;
    }

    return false;
  }
};

struct PortInfo;
struct NodeDomain;

typedef struct Node *NodePointer;

struct Node {
  std::string hiraName;  // the hirarchy name of the instance
  std::string type;      // the gate type of the instance
  std::string name;      // the name of the instance
  NodePointer father;
  std::vector<NodePointer> children;
  std::string domain;  // to specify which domain the instance is in
  bool domainDef;      // had the domain been defined
  std::vector<std::string> pre;
  std::vector<std::string> suc;
  std::map<std::string, std::vector<PortInfo> > inPort;
  std::map<std::string, std::vector<PortInfo> > outPort;
  std::vector<NodeDomain> source;
  std::vector<NodeDomain> destination;
};

struct C_P_D {  // create_power_domain
  std::string name;
  std::vector<std::string> instances;
  std::vector<std::string> ports;
  bool defDomain;  // is the power domain a default power domain
  bool alwaysOn;   // is the power domain a always on power domain

  C_P_D() : name(), instances(), ports(), defDomain(false), alwaysOn(false) {}
};

struct C_I_R {       // create_isolation_rule
  std::string name;  // the name of the isolation rule
  std::string from;  // source domain
  std::string to;    // destination domain
  std::string condition;
  bool negation;  // is there a '!' symbol in front of the condition, true
                  // denote as yes
  bool output;    // false denote as low, true denote as high

  C_I_R() : name(), from(), to(), condition(), negation(false), output(false) {}
};

typedef struct D_I_C *D_I_C_pointer;

struct D_I_C {  // define_isolation_cell
  std::string cell;
  std::string enable;
  std::string location;
  IsoGateType gateType;

  D_I_C() : cell(), enable(), location(), gateType(IsoGateType::AND) {}
};

struct C_L_S_R {  // create_level_shifter_rule
  std::string name;
  std::string from;
  std::string to;

  C_L_S_R() : name(), from(), to() {}
};

typedef struct D_L_S_C *D_L_S_C_pointer;

struct D_L_S_C {  // define_level_shifter_cell
  std::string cell;
  std::string location;
  std::string direction;
  float inLow;
  float inHigh;
  float outLow;
  float outHigh;

  D_L_S_C()
      : cell(),
        location(),
        direction(),
        inLow(0.0),
        inHigh(0.0),
        outLow(0.0),
        outHigh(0.0) {}
};

struct C_N_C {  // create_nominal_condition
  std::string name;
  float voltage;

  C_N_C() : name(), voltage(0.0) {}
};

struct C_P_M {  // create_power_mode
  std::string name;
  std::map<std::string, std::string> condition;
  bool defaultMode;  // is default mode

  C_P_M() : name(), condition(), defaultMode(false) {}
};

struct PortDomain {
  std::string portName;
  std::string domain;

  PortDomain() : portName(), domain() {}
};

struct PortInfo {
  PortType conPortType;
  std::string conPortName;
  NodePointer conNode;
  std::string wireName;

  void clear() {
    conPortType = PortType::IN;
    conPortName.clear();
    conNode = nullptr;
    wireName.clear();
  }
};

struct NodeDomain {
  std::string node;  // hirarchy name of the node
  std::string domain;
  std::string selfPort;
  std::string otherPort;

  void clear() {
    node.clear();
    domain.clear();
    selfPort.clear();
    otherPort.clear();
  }
};

#endif  // end of define IC_CAD_2011_SC_DATA_STRUCTRUE_DEFINE_HPP
