#ifndef SECONDSERVICEIMPL_H_
#define SECONDSERVICEIMPL_H_

#include <iostream>
#include "SecondService.h"

using namespace  ::thrift::multiplex::demo;

class SecondServiceHandler : virtual public SecondServiceIf 
{
public:
	SecondServiceHandler() 
	{
		// Your initialization goes here
	}
	
	void blahBlah() 
	{
		// Your implementation goes here
		std::cout << "I am service2" << std::endl;
	}
};

#endif
