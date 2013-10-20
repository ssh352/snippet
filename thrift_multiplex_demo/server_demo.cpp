#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/processor/TMultiplexedProcessor.h>

#include "FirstServiceImpl.h"
#include "SecondServiceImpl.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::thrift::multiplex::demo;

int main(int argc, const char* argv[])
{
	int port = 9090;
	shared_ptr<TProcessor> processor1(new FirstServiceProcessor(shared_ptr<FirstServiceHandler>(new FirstServiceHandler())));
	shared_ptr<TProcessor> processor2(new SecondServiceProcessor(shared_ptr<SecondServiceHandler>(new SecondServiceHandler())));
	//使用MultiplexedProcessor
    shared_ptr<TMultiplexedProcessor> processor(new TMultiplexedProcessor());
	
	//注册各自的Processor
	processor->registerProcessor("FirstService", processor1);
	processor->registerProcessor("SecondService", processor2);
	
    shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
    shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
    TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
    server.serve();
	
    return 0;
}
