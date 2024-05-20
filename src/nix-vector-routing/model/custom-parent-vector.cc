#include "custom-parent-vector.h"


#include "ns3/node.h"
#include "ns3/node-list.h"


#include <iostream>
#include <sstream>


namespace ns3
{

NS_LOG_COMPONENT_DEFINE("CustomParentVector");

CustomParentVector::CustomParentVector()
{
    NS_LOG_FUNCTION(this);
}

CustomParentVector::~CustomParentVector()
{
    NS_LOG_FUNCTION(this);
}

std::vector<int> 
CustomParentVector::ParseStringToVector(std::string input) {
    std::vector<int> result;
    std::stringstream ss(input);
    std::string token;
    while (std::getline(ss, token, ',')) {
        result.push_back(std::stoi(token));
    }
    return result;
}


std::vector<Ptr<Node>> 
CustomParentVector::CreateCustomParentVector(std::string input) {
    std::vector<int> intVector = ParseStringToVector(input);
    std::vector<Ptr<Node>> CustomParentVector;

    for(long unsigned int i =0; i < intVector.size(); i++){
        Ptr<Node> node = NodeList::GetNode(intVector[i]);
        CustomParentVector.push_back(node);
    }
    return CustomParentVector;
}
} // Namespace ns3
