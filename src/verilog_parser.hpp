#ifndef IC_CAD_2011_SC_VERILOG_PARSER_HPP
#define IC_CAD_2011_SC_VERILOG_PARSER_HPP

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "data_structure_define.hpp"
#include "verilog_testing.hpp"

/*** Global Variables ***/
extern bool secondTime;
extern std::map<std::string, int> primitiveGate;
extern bool ok;
extern size_t libStart;

/*** global variables ***/
extern std::map<std::string, std::string> libModule;
extern std::map<std::string, Node>
    nodeMap;  // using the hierarchy name to be the index
extern std::map<std::string, Node>::iterator nodeIter;

// std::map<std::string, D_L_S_C_pointer> levShCells;
extern std::vector<NodePointer> isoNodes;
extern std::vector<NodePointer> levShNodes;

/*** about top-module ***/
// to record the name of the top module in the design
extern std::string topModule;
// using the top level port name to be the index
extern std::map<std::string, PortDomain> topPortDomain;

/*** default power domain & default power mode ***/
extern std::map<std::string, bool>
    notYetSetDomain;  // using the hirarchy name to be the index

bool ReadVerilogFile(const char *file_name, std::vector<std::string> &, int &);
void StoreVerilogStatement(std::vector<std::string> &,
                           std::vector<std::string> &, const int);
void ParseVerilogFile(std::vector<std::string> &,
                      std::map<std::string, Module> &);
std::string ModuleHandle(
    const std::string &,
    std::map<std::string, Module> &);  // the return value is the current
                                       // parsing module name
void InputHandle(const std::string &, std::map<std::string, Module> &,
                 const std::string &);
void OutputHandle(const std::string &, std::map<std::string, Module> &,
                  const std::string &);
bool InstanceHandle(const std::string &, std::map<std::string, Module> &,
                    const std::string &);
void WireHandle(const std::string &, std::map<std::string, Module> &,
                const std::string &);
void ShowVerilogError(const std::string &);  // it won't show the line number
std::string SplitModuleName(const std::string &);
void CreatePrimitiveGate(void);
void FindTopModule(std::map<std::string, Module> &);

#endif  // end of define IC_CAD_2011_SC_VERILOG_PARSER_HPP
