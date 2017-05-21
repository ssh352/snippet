#ifndef DESIGNPATTERN_OBSERVER_OBSERVER_H
#define DESIGNPATTERN_OBSERVER_OBSERVER_H

#include "iobserver.h"
#include <iostream>

namespace DesignPatter 
{

class Observer : public IObserver
{
public:
	virtual bool Update(const std::string& data)
	{
		std::cout << "Update: " << data << std::endl;

		return true;
	}	
};

} // DesignPatter

#endif // DESIGNPATTERN_OBSERVER_OBSERVER_H
