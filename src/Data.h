#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

#ifndef DATA
#define DATA

enum PortType { IN, OUT, INOUT };

enum Location { FROM, TO, EITHER };

enum IsoGateType { AND, OR, NAND, NOR, XOR, XNOR };

struct WireConnectInfo {
  string instanceType;
  string instanceName;
  string portName;
  PortType portType;
  int portOrder;

  WireConnectInfo()
      : instanceType(),
        instanceName(),
        portName(),
        portType(IN),
        portOrder(-1) {}
  void clear() {
    this->instanceType.clear();
    this->instanceName.clear();
    this->portName.clear();
    this->portOrder = -1;
  }
};

struct Wire {
  string wireName;
  bool multiBits;
  vector<WireConnectInfo> conInfo;
  int from;
  int to;
  Wire() {
    this->multiBits = false;
    this->from = false;
    this->to = false;
  }
  void clear() {
    this->wireName.clear();
    this->multiBits = false;
    this->conInfo.clear();
    this->from = -1;
    this->to = -1;
  }
};

struct Port {
  string portName;
  int portOrder;
  PortType type;
  string connectWireName;
  bool multiBits;
  bool inverted;  // is there a simbol '~' in front of the connect-wire name
  int from;
  int to;

  Port()
      : portName(),
        portOrder(-1),
        type(IN),
        connectWireName(),
        multiBits(false),
        inverted(false),
        from(-1),
        to(-1) {}

  void clear() {
    this->portName.clear();
    this->connectWireName.clear();
    this->portOrder = -1;
    this->multiBits = false;
    this->inverted = false;
    this->from = -1;
    this->to = -1;
  }
};

struct Unit {
  string unitType;  // module
  string unitName;  // instance name
  map<string, Port> ports;

  void clear() {
    this->unitType.clear();
    this->unitName.clear();
    this->ports.clear();
  }
};

struct Module {             // to record the information of a module
  string moduleName;        // name of the module
  map<string, Wire> wires;  // wires of the module
  map<string, Port> ports;  // ports of the module
  map<string, Unit> units;  // instances of the module
  bool top;
  void clear() {
    this->moduleName.clear();
    this->wires.clear();
    this->ports.clear();
    this->units.clear();
  }
};

struct PortInfo;
struct NodeDomain;

typedef struct Node *NodePointer;

struct Node {
  string hiraName;  // the hirarchy name of the instance
  string type;      // the gate type of the instance
  string name;      // the name of the instance
  NodePointer father;
  vector<NodePointer> children;
  string domain;   // to specify which domain the instance is in
  bool domainDef;  // had the domain been defined
  vector<string> pre;
  vector<string> suc;
  map<string, vector<PortInfo> > inPort;
  map<string, vector<PortInfo> > outPort;
  vector<NodeDomain> source;
  vector<NodeDomain> destination;
};

struct C_P_D {  // create_power_domain
  string name;
  vector<string> instances;
  vector<string> ports;
  bool defDomain;  // is the power domain a default power domain
  bool alwaysOn;   // is the power domain a always on power domain

  C_P_D() : name(), instances(), ports(), defDomain(false), alwaysOn(false) {}
};

struct C_I_R {  // create_isolation_rule
  string name;  // the name of the isolation rule
  string from;  // source domain
  string to;    // destination domain
  string condition;
  bool negation;  // is there a '!' symbol in front of the condition, true
                  // denote as yes
  bool output;    // false denote as low, true denote as high

  C_I_R() : name(), from(), to(), condition(), negation(false), output(false) {}
};

typedef struct D_I_C *D_I_C_pointer;

struct D_I_C {  // define_isolation_cell
  string cell;
  string enable;
  string location;
  IsoGateType gateType;

  D_I_C() : cell(), enable(), location(), gateType(AND) {}
};

struct C_L_S_R {  // create_level_shifter_rule
  string name;
  string from;
  string to;

  C_L_S_R() : name(), from(), to() {}
};

typedef struct D_L_S_C *D_L_S_C_pointer;

struct D_L_S_C {  // define_level_shifter_cell
  string cell;
  string location;
  string direction;
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
  string name;
  float voltage;

  C_N_C() : name(), voltage(0.0) {}
};

struct C_P_M {  // create_power_mode
  string name;
  map<string, string> condition;
  bool defMode;

  C_P_M() : name(), condition(), defMode(false) {}
};

struct PortDomain {
  string portName;
  string domain;

  PortDomain() : portName(), domain() {}
};

struct PortInfo {
  PortType conPortType;
  string conPortName;
  NodePointer conNode;
  string wireName;

  void clear() {
    conPortType = PortType::IN;
    conPortName.clear();
    conNode = nullptr;
    wireName.clear();
  }
};

struct NodeDomain {
  string node;  // hirarchy name of the node
  string domain;
  string selfPort;
  string otherPort;

  void clear() {
    node.clear();
    domain.clear();
    selfPort.clear();
    otherPort.clear();
  }
};

#endif
