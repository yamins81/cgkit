/*
  Test
 */

#include <boost/python.hpp>
#include "vec3.h"
#include <iostream.h>

using namespace boost::python;

typedef vec3<double> vec3d;

void myfunc(object a)
{
  vec3d v;
  extract<vec3d> x(a);
  if (x.check())
  {
    v=x();
    cout<<"vec3 = "<<v.x<<" "<<v.y<<" "<<v.z<<endl;
  }
  else
  {
    cout<<"Argument ist kein vec3"<<endl;
  }
}

//////////////////////////////////////////////////////////////////////

BOOST_PYTHON_MODULE(foo)
{
  def("myfunc", &myfunc);
}
