
#ifndef CUSTOM_PARENT_VECTOT_H
#define CUSTOM_PARENT_VECTOT_H

#include "ns3/node.h"

#include <iostream>
#include <sstream>

namespace ns3
{

class CustomParentVector
{
  public:
    CustomParentVector();

    ~CustomParentVector();

    std::vector<int> ParseStringToVector(std::string m_input);

    std::vector<Ptr<Node>> CreateCustomParentVector(std::string m_input) ;

    std::string m_input; 

};

} // namespace ns3

#endif
