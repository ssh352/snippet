/*************************************************************************
	> File Name: hello.cpp
	> Author: billowqiu
	> Mail: billowqiu@billowqiu.com 
	> Created Time: 2017-11-13 22:12:59
	> Last Changed: 2017-11-13 22:12:59
*************************************************************************/
#include <boost/python.hpp>

const char* greet()
{
    return "hello,world";
}

BOOST_PYTHON_MODULE(hello_ext)
{
    boost::python::def("greet", greet);
}

