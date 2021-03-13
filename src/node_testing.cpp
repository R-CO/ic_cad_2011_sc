#include "node_testing.hpp"

#include <iostream>
using std::cin;
using std::cout;
using std::endl;
#include <string>
using std::string;

#include <map>
using std::map;
#include <vector>
using std::vector;

#include "data_structure_define.hpp"
#include "verilog_parser.hpp"

void NodeTesting() {
  string nodeHiraName;
  vector<NodePointer>::iterator iChild;
  vector<string>::iterator iPre, iSuc;
  map<string, vector<PortInfo> >::iterator iInPort, iOutPort;
  vector<PortInfo>::iterator iPortInfo;
  vector<NodeDomain>::iterator iSource, iDest;

  do {
    cout << "Please key in a node's hirarchy name(enter: \"over\" to exit):"
         << endl;
    cin >> nodeHiraName;
    if (nodeHiraName == "over" || nodeHiraName.find("over ") != string::npos) {
      cout << "End of node testing." << endl;
      break;
    }
    nodeIter = nodeMap.find(nodeHiraName);
    if (nodeIter == nodeMap.end()) {
      continue;
    }
    cout << "Here is the node's (" << nodeHiraName << ") information: " << endl;
    cout << "Hirarchy name: " << nodeIter->second.hiraName << "***" << endl;
    cout << "Type: " << nodeIter->second.type << "***" << endl;
    cout << "Name: " << nodeIter->second.name << "***" << endl;
    if (nodeIter->second.father != nullptr) {
      cout << "Father's hirarchy name: " << nodeIter->second.father->hiraName
           << "***" << endl;
    }
    cout << "Children's hirachy name: ";
    for (iChild = nodeIter->second.children.begin();
         iChild != nodeIter->second.children.end(); ++iChild) {
      cout << (*iChild)->hiraName << "*** ";
    }
    cout << endl;
    cout << "Node's domain: " << nodeIter->second.domain << "***" << endl;
    cout << "Predecessors' hirarchy name: ";
    for (iPre = nodeIter->second.pre.begin();
         iPre != nodeIter->second.pre.end(); ++iPre) {
      cout << *iPre << "*** ";
    }
    cout << endl;
    cout << "Successors' hirachy name: ";
    for (iSuc = nodeIter->second.suc.begin();
         iSuc != nodeIter->second.suc.end(); ++iSuc) {
      cout << *iSuc << "*** ";
    }
    cout << endl;
    for (iInPort = nodeIter->second.inPort.begin();
         iInPort != nodeIter->second.inPort.end(); ++iInPort) {
      for (iPortInfo = iInPort->second.begin();
           iPortInfo != iInPort->second.end(); ++iPortInfo) {
        cout << "Input port: " << iInPort->first << "***" << endl;
        cout << "Connect to node: " << iPortInfo->conNode->hiraName << "***"
             << endl;
        cout << "Connect to port: " << iPortInfo->conPortName << "***" << endl;
        cout << "Type: " << static_cast<int>(iPortInfo->conPortType) << "***"
             << endl;
        cout << "By wire: " << (*iPortInfo).wireName << "***" << endl;
      }
    }
    for (iOutPort = nodeIter->second.outPort.begin();
         iOutPort != nodeIter->second.outPort.end(); ++iOutPort) {
      for (iPortInfo = iOutPort->second.begin();
           iPortInfo != iOutPort->second.end(); ++iPortInfo) {
        cout << "Output port: " << iOutPort->first << "***" << endl;
        cout << "Connect to node: " << (*iPortInfo).conNode->hiraName << "***"
             << endl;
        cout << "Connect to port: " << (*iPortInfo).conPortName << "***"
             << endl;
        cout << "Type: " << static_cast<int>(iPortInfo->conPortType) << "***"
             << endl;
        cout << "By wire: " << iPortInfo->wireName << "***" << endl;
      }
    }
    for (iSource = nodeIter->second.source.begin();
         iSource != nodeIter->second.source.end(); ++iSource) {
      cout << "Source node: " << (*iSource).node << "***" << endl;
      cout << "Source domain: " << (*iSource).domain << "***" << endl;
      cout << "Self port: " << (*iSource).selfPort << "***" << endl;
      cout << "Other port: " << (*iSource).otherPort << "***" << endl;
    }
    for (iDest = nodeIter->second.destination.begin();
         iDest != nodeIter->second.destination.end(); ++iDest) {
      cout << "Destination node: " << (*iDest).node << "***" << endl;
      cout << "Destination domain: " << (*iDest).domain << "***" << endl;
      cout << "Self port: " << (*iDest).selfPort << "***" << endl;
      cout << "Other port: " << (*iDest).otherPort << "***" << endl;
    }
  } while (true);
}
