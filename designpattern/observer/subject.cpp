#include "subject.h"
#include <algorithm>
#include "iobserver.h"
namespace DesignPatter 
{
bool Subject::Register(IObserver* ob)
{
	observers_.push_back(ob);

	return true;
}

bool Subject::Notify() const
{
	std::for_each(observers_.begin(), observers_.end(), [](IObserver* ob) {ob->Update("data");});

	return true;
}

} // DesignPatter
