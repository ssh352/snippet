#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TMultiplexedProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include "FirstService.h"
#include "SecondService.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

using boost::shared_ptr;

using namespace  ::thrift::multiplex::demo;

int main(int argc, const char* argv[])
{
	shared_ptr<TSocket> transport(new TSocket("localhost", 9090));
	transport->open();
	shared_ptr<TBinaryProtocol> protocol(new TBinaryProtocol(transport));
	
	shared_ptr<TMultiplexedProtocol> mp1(new TMultiplexedProtocol(protocol, "FirstService"));
	shared_ptr<FirstServiceClient> service1(new FirstServiceClient(mp1));
	
	shared_ptr<TMultiplexedProtocol> mp2(new TMultiplexedProtocol(protocol, "SecondService"));
	shared_ptr<SecondServiceClient> service2(new SecondServiceClient(mp2));
	
	service1->blahBlah();
	service2->blahBlah();
	
	return 0;
}
