#include "subject.h"
#include "observer.h"

int main(int argc, char const *argv[])
{
	DesignPatter::Subject s;
	for (int i = 0; i < 10; ++i)
	{
		DesignPatter::Observer* o = new DesignPatter::Observer;
		s.Register(o);
	}
	s.Notify();

	return 0;
}
