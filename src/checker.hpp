#ifndef IC_CAD_2011_SC_CHECKER_HPP
#define IC_CAD_2011_SC_CHECKER_HPP

// C++ standard library
#include <sstream>
#include <string>

// C++ STL container
#include <map>

#include "data_structure_define.hpp"
#include "report.hpp"

void Checker(std::map<std::string, Module> &);
bool CheckLS_1_2_4(NodePointer LSNodes, std::map<std::string, Module> &);
bool CheckISO_4_LS_3(std::map<std::string, Module> &);
bool CheckISO_2_3(NodePointer, std::map<std::string, Module> &);
bool CheckISO_5(NodePointer, std::map<std::string, Module> &);
bool CheckISO_6(NodePointer, std::map<std::string, Module> &);
void CheckISO_1(NodePointer, std::map<std::string, Module> &);
std::string CPM_DomainToNom(C_P_M, std::string &);
float CNC_NameToVoltage(std::string);
bool isSpeciallInstance(std::string &, std::string &);
std::string float2str(float &i);
std::string findPort(std::string hirname, PortType type,
                     std::map<std::string, Module> &moduleMap,
                     const std::string &notsame);
std::string findControl(std::string type);
std::string findS(std::map<std::string, Module> &moduleMap, std::string hirname,
                  PortType type, std::string &finalport);

extern std::string g_source;
extern std::string g_destination;

#endif  // end of define IC_CAD_2011_SC_CHECKER_HPP
