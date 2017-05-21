#ifndef DESIGNPATTERN_OBSERVER_SUBJECT_H_
#define DESIGNPATTERN_OBSERVER_SUBJECT_H_

#include <vector>
#include "isubject.h"

namespace DesignPatter
{

class Subject : public ISubject
{
public:
	virtual bool Register(IObserver*);
    virtual bool Notify() const;
private:
	typedef std::vector<IObserver*> ObserverVec;
	ObserverVec observers_;
};
}
#endif
