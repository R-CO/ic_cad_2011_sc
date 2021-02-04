
#include "Data.h"

#ifndef VERILOG_TESTING_H
#define VERILOG_TESTING_H

void VerilogParserTesting(map<string, Module> &);

void VerilogParserTesting(map<string, Module> &moduleMap)
{
	map<string, Module>::iterator modIter;
	map<string, Port>::iterator portIter;
    map<string, Wire>::iterator wireIter; 
    map<string, Unit>::iterator unitIter;
	vector<WireConnectInfo>::iterator conIter;

	for ( modIter = moduleMap.begin(); modIter != moduleMap.end(); ++modIter ) {
        cout << boolalpha << "Module:" << modIter->second.moduleName << "***" << endl
            << "Top:" << modIter->second.top << "***" << endl
            << "Ports:" << endl;
        for ( portIter = modIter->second.ports.begin(); portIter != modIter->second.ports.end(); ++portIter ) {
            cout << "Port name:" << portIter->second.portName << "***" << endl
                << "Port order:" << portIter->second.portOrder << "***" << endl
                << "Port type:";
            switch ( portIter->second.type ) {
                case IN: cout << "IN***" << endl; break;
                case OUT: cout << "OUT***" << endl; break;
                default: break;
            }
            cout << boolalpha << "Multi-bits:" << portIter->second.multiBits << "***" << endl
                << "Connected-wire name:" << portIter->second.connectWireName << "***" << endl
                << "Inverted:" << portIter->second.inverted << "***" << endl
                << "To:" << portIter->second.to << "***From:" << portIter->second.from << endl;
        }
        cout << endl << "Wires:" << endl;
        for ( wireIter = modIter->second.wires.begin(); wireIter != modIter->second.wires.end(); ++wireIter ) {
            cout << "Wire name:" << wireIter->second.wireName << "***" << endl
                << "Multi-Bits:" << boolalpha << wireIter->second.multiBits << "***" << endl
                << "To:" << wireIter->second.to << "***From:" << wireIter->second.from << endl
                << "Connected-information:" << endl;
            for ( conIter = wireIter->second.conInfo.begin(); conIter != wireIter->second.conInfo.end(); ++conIter ) {
                cout << "Instance type:" << conIter->instanceType << "***" << endl
                    << "Instance name:" << conIter->instanceName << "***" << endl
                    << "Port name:" << conIter->portName << "***" << endl
                    << "Port type:";
                switch ( conIter->portType ) {
                    case IN: cout << "IN***" << endl; break;
                    case OUT: cout << "OUT***" << endl; break;
                    default: break;
                }
                cout << "Port Order:" << conIter->portOrder << "***" << endl;
            }
        }
        cout << endl << "Instances:" << endl;
        for ( unitIter = modIter->second.units.begin(); unitIter != modIter->second.units.end(); ++unitIter ) {
            cout << "Instance type:" << unitIter->second.unitType << "***" << endl
                << "Instance name:" << unitIter->second.unitName << "***" << endl
                << "Ports:" << endl;
            for ( portIter = unitIter->second.ports.begin(); portIter != unitIter->second.ports.end(); ++portIter ) {
                cout << "Port name:" << portIter->second.portName << "***" << endl
                    << "Port order:" << portIter->second.portOrder << "***" << endl
                    << "Port type:";
                switch ( portIter->second.type ) {
                    case IN: cout << "IN***" << endl; break;
                    case OUT: cout << "OUT***" << endl; break;
                    default: break;
                }
                cout << boolalpha << "Multi-bits:" << portIter->second.multiBits << "***" << endl
                    << "Connected-wire name:" << portIter->second.connectWireName << "***" << endl
                    << "Inverted:" << portIter->second.inverted << "***" << endl
                    << "To:" << portIter->second.to << "***From:" << portIter->second.from << endl;
            }
        }
        cout << endl << "********************" << endl << endl;
    }       
}                

#endif
