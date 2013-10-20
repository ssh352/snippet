#ifndef FIRSTSERVICEIMPL_H_
#define FIRSTSERVICEIMPL_H_

#include <iostream>
#include "FirstService.h"

using namespace  ::thrift::multiplex::demo;

class FirstServiceHandler : virtual public FirstServiceIf 
{
public:
	FirstServiceHandler() 
	{
		// Your initialization goes here
	}
	
	void blahBlah() 
	{
		// Your implementation goes here
		std::cout << "I am service1" << std::endl;
	}
};

#endif
