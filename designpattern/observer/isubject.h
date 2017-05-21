#ifndef DESIGNPATTERN_OBSERVER_ISUBJECT_H
#define DESIGNPATTERN_OBSERVER_ISUBJECT_H

#include "common.h"

namespace DesignPatter
{

interface IObserver;
interface  ISubject
{
    virtual bool Register(IObserver*) = 0;
    virtual bool Notify() const = 0;
};
}

#endif
