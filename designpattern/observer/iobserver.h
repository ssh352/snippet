#ifndef DESIGNPATTERN_OBSERVER_IOBSERVER_H_
#define DESIGNPATTERN_OBSERVER_IOBSERVER_H_

#include "common.h"
#include <string>

namespace DesignPatter
{

interface IObserver
{
    virtual bool Update(const std::string& data) = 0;
};
}

#endif

