#ifndef IC_CAD_2011_SC_POWER_INTENT_PARSER_HPP
#define IC_CAD_2011_SC_POWER_INTENT_PARSER_HPP

#include <map>
#include <string>
#include <vector>

#include "data_structure_define.hpp"

bool ReadPowerFile(const char *, std::vector<std::string> &);
void ParsePowerFile(const std::vector<std::string> &,
                    std::map<std::string, Module> &);
void CPD_Handle(const std::string &);  // create_power_domain handle
void CIR_Handle(const std::string &);  // create_isolation_rule handle
void DIC_Handle(
    const std::string &,
    std::map<std::string, Module> &);   // define_isolation_cell handle
void CLSR_Handle(const std::string &);  // create_level_shifter_rule handle
void DLSC_Handle(const std::string &);  // define_level_shifter_cell handle
void CNC_Handle(const std::string &);   // create_nominal_condition handle
void CPM_Handle(const std::string &);   // create_power_mode handle

/*** global variables ***/
extern std::map<std::string, C_P_D>
    createPowDomain;  // using the power domain name to be the index
extern std::map<std::string, C_I_R>
    createIsoRule;  // using from + to to be the index
extern std::map<std::string, D_I_C>
    defineIsoCell;  // using the isolation cell name to be the index
extern std::map<std::string, C_L_S_R>
    createLevShRule;  // using from + to to be the index
extern std::map<std::string, D_L_S_C>
    defineLevShCell;  // using the level shifter cell name to be the index
extern std::map<std::string, C_N_C>
    createNomCondition;  // using the nominal condition name to be the index
extern std::vector<C_P_M> createPowMode;

/*** default power domain & default power mode ***/
extern std::string defaultDomain;
extern std::string defaultMode;

#endif  // end of define IC_CAD_2011_SC_POWER_INTENT_PARSER_HPP
