#ifndef IC_CAD_2011_SC_VERILOG_PARSER_HPP
#define IC_CAD_2011_SC_VERILOG_PARSER_HPP

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
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

bool ReadVerilogFile(const char *filePath,
                     std::vector<std::string> &fileContent, int &fileCount);
void StoreVerilogStatement(std::vector<std::string> &fileContent,
                           std::vector<std::string> &veriStatements,
                           const int fileCount);
void ParseVerilogFile(std::vector<std::string> &veriStatements,
                      std::map<std::string, Module> &moduleMap);
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

namespace rco {

using ModuleName = std::string;
#ifdef USE_UNORDERED_MAP
using ModuleMap = std::unordered_map<ModuleName, Module>;
#else   // use std::map
using ModuleMap = std::map<ModuleName, Module>;
#endif  // end of ifdef USE_UNORDERED_MAP

class VerilogParser {
 public:
  VerilogParser();
  // VerilogParser(std::shared_ptr<ModuleMap> module_map_);
  ~VerilogParser();

  bool parseFile(const std::string &file_path, ModuleMap &module_map);

 protected:
  bool readFile(const std::string &file_path);
  bool parse(ModuleMap &module_map);

  std::vector<std::string> statements_;
  // std::shared_ptr<ModuleMap> module_map_;

 private:
};

}  // end of namespace rco

#endif  // end of define IC_CAD_2011_SC_VERILOG_PARSER_HPP
