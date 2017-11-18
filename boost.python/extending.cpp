// Copyright Ralf W. Grosse-Kunstleve 2002-2004. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <iostream>
#include <string>
#include <vector>

namespace { // Avoid cluttering the global namespace.

    typedef std::vector<int> tvi;
    struct Foo
    {
        int32_t a;
        std::string b;
        tvi vi;
        bool operator==(Foo const& f) const { return f.a == a; }
    };

    // A friendly class.
    typedef std::vector<Foo> VecFoo;
    class hello
    {
        public:
            hello(const std::string& country) { this->country = country; }
            std::string greet() const { return "Hello from " + country; }
            Foo get_foo()const
            {
                Foo foo;
                foo.a = 123;
                foo.b = country;
                return foo;
            }
        public:
            std::string country;
            VecFoo vf;
    };

    // A function taking a hello object as an argument.
    std::string invite(const hello& w) {
        return w.greet() + "! Please come soon!";
    }

    struct FooConvert
    {
        /*
           static PyObject* convert(Foo const & f)
           {
        //boost::python::object* obj = new boost::python::object(f);
        boost::python::list* pylist = new boost::python::list;
        pylist->append(f.a);
        pylist->append(f.b);

        return pylist->ptr();
        //return boost::python::incref(pylist.ptr());
        }

        static PyObject* convert(Foo const & f)
        {
        boost::python::dict * pydict = new boost::python::dict;
        pydict->setdefault("a", f.a);
        pydict->setdefault("b", f.b);

        return pydict->ptr();
        }
        */

        static PyObject* convert(Foo const & f)
        {
            boost::python::object * pyobj = new boost::python::object(f);

            return pyobj->ptr();
        }
    };


}

struct VecFooConvert
{
    static PyObject* convert(const VecFoo& vf)
    {
        boost::python::list pylist;
        for(auto& f : vf)
        {
            pylist.append(f);
        }

        return boost::python::incref(pylist.ptr());
    }
};

struct X // a container element
{
    std::string s;
    /*
    X():s("default") {}
    X(std::string s):s(s) {}
    std::string repr() const { return s; }
    void reset() { s = "reset"; }
    void foo() { s = "foo"; }
    bool operator!=(X const& x) const { return s != x.s; }
    */
    bool operator==(X const& x) const { return s == x.s; }
};

typedef std::vector<X> VecX;

VecFoo getVecFoo()
{
    VecFoo vf;
    Foo f;
    f.a = 1;
    f.b = "1";
    vf.push_back(f);

    f.a = 2;
    f.b = "1123";
    vf.push_back(f);

    return vf;
}

BOOST_PYTHON_MODULE(extending)
{
    using namespace boost::python;

    class_<tvi>("tvi").def(vector_indexing_suite<tvi>());

    //boost::python::to_python_converter<Foo, FooConvert>();
    class_<Foo>("Foo")
        .def_readwrite("a", &Foo::a)
        .def_readwrite("b", &Foo::b)
        .def_readwrite("vi", &Foo::vi);

    //class_<VecFoo>("VecFoo").def(vector_indexing_suite<VecFoo>());
    class_<VecX>("VecX").def(vector_indexing_suite<VecX>());
    boost::python::to_python_converter<VecFoo, VecFooConvert>();
    def("getVecFoo", getVecFoo);
    class_<hello>("hello", init<std::string>())
        // Add a regular member function.
        .def("greet", &hello::greet)
        // Add invite() as a member of hello!
        .def("invite", invite)
        .def("get_foo", &hello::get_foo)
        .def_readwrite("vf", &hello::vf);
        ;

    // Also add invite() as a regular function to the module.
    def("invite", invite);
}

