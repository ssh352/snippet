/**
 * \file module.cpp
 * \author Julien Kauffmann <julien.kauffmann@freelan.org>
 * \brief The Python module file.
 */

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/class.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <datetime.h>

boost::posix_time::ptime add_five_seconds(boost::posix_time::ptime ptime)
{
  if (!ptime.is_special())
  {
    return ptime + boost::posix_time::seconds(5);
  }

  return ptime;
}

struct date_to_python_converter
{
  static PyObject* convert(boost::posix_time::ptime value)
  {
    if (value.is_not_a_date_time())
      return Py_None;

    PyDateTime_IMPORT;

    return PyDateTime_FromDateAndTime(
        static_cast<int>(value.date().year()),
        static_cast<int>(value.date().month()),
        static_cast<int>(value.date().day()),
        static_cast<int>(value.time_of_day().hours()),
        static_cast<int>(value.time_of_day().minutes()),
        static_cast<int>(value.time_of_day().seconds()),
        static_cast<int>(value.time_of_day().total_microseconds() - value.time_of_day().total_seconds() * 1000000L)
        );
  }
};

struct date_from_python_converter
{
  static void* is_convertible(PyObject* obj_ptr)
  {
    assert(obj_ptr);

    if (obj_ptr == Py_None)
      return obj_ptr;

    PyDateTime_IMPORT;

    if (PyDateTime_Check(obj_ptr))
      return obj_ptr;

    return NULL;
  }

  static void convert(PyObject* obj_ptr, boost::python::converter::rvalue_from_python_stage1_data* data)
  {
    assert(obj_ptr);

    void* const storage = reinterpret_cast<boost::python::converter::rvalue_from_python_storage<boost::posix_time::ptime>*>(data)->storage.bytes;

    if (obj_ptr == Py_None)
    {
      new (storage) boost::posix_time::ptime();
    } else
    {
      PyDateTime_IMPORT;
      PyDateTime_DateTime* dt_ptr = reinterpret_cast<PyDateTime_DateTime*>(obj_ptr);

      const int year = PyDateTime_GET_YEAR(dt_ptr);
      const int month = PyDateTime_GET_MONTH(dt_ptr);
      const int day = PyDateTime_GET_DAY(dt_ptr);
      const int hour  = PyDateTime_DATE_GET_HOUR(dt_ptr);
      const int minute  = PyDateTime_DATE_GET_MINUTE(dt_ptr);
      const int second = PyDateTime_DATE_GET_SECOND(dt_ptr);
      const int microsecond = PyDateTime_DATE_GET_MICROSECOND(dt_ptr);

      new (storage) boost::posix_time::ptime(boost::gregorian::date(year, month, day), boost::posix_time::time_duration(hour, minute, second, 0) + boost::posix_time::microseconds(microsecond));
    }

    data->convertible = storage;
  }
};

BOOST_PYTHON_MODULE(datetime_convert)
{
  boost::python::to_python_converter<boost::posix_time::ptime, date_to_python_converter>();
  boost::python::converter::registry::push_back(&date_from_python_converter::is_convertible, 
    &date_from_python_converter::convert, boost::python::type_id<boost::posix_time::ptime>());  
  boost::python::def("add_five_seconds", &add_five_seconds, boost::python::args("ptime"), "Add five seconds to a datetime then return the result");
}

