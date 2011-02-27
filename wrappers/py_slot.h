/*
 Slots.
 */

#ifndef PYSLOT_H
#define PYSLOT_H

#include <boost/python.hpp>
#include <exception>
#include <string>
#include "slot.h"
#include "arrayslot.h"
#include "vec3.h"
#include "vec4.h"
#include "mat3.h"
#include "mat4.h"
#include "quat.h"
#include "py_exceptions.h"

using namespace boost::python;
using namespace support3d;
using std::string;


// This macro can be used to create a new Python slot type
#define SLOT(sname,stype)  class_<Slot<stype>, \
                                  SlotWrapper<stype>, \
                                  bases<ISlot>, \
                                  boost::noncopyable>(sname) \
    .def(init<int>()) \
    .def(init<stype,int>()) \
    .def_readwrite("_value", &SlotWrapper<stype>::value) \
    .def("getValue", &SlotWrapper<stype>::base_getValue, \
	 return_value_policy<copy_const_reference>()) \
    .def("setValue", &SlotWrapper<stype>::base_setValue) \
    .def("computeValue", &SlotWrapper<stype>::base_computeValue) \
    .def("__str__", &SlotWrapper<stype>::__str__)

//    .def("onValueChanged", &SlotWrapper<stype>::base_onValueChanged)
//    .def("onValueChanged", &Slot<double>::onValueChanged, &SlotWrapper<double>::base_onValueChanged) 

// This macro can be used to create a new Python array slot type
#define ARRAYSLOT(sname,stype) class_<_ArraySlotIterator<stype> >("_"sname"_Iterator", init<ArraySlot<stype>&>()) \
    .def("__iter__", &_ArraySlotIterator<stype>::__iter__) \
    .def("next", &_ArraySlotIterator<stype>::next) \
  ; \
  class_<ArraySlot<stype>, \
                               ArraySlotWrapper<stype>, \
                               bases<IArraySlot>, \
                               boost::noncopyable>(sname) \
    .def(init<short>()) \
    .def(init<short, boost::shared_ptr<SizeConstraintBase> >()) \
    .def("__len__", &ArraySlotWrapper<stype>::size) \
    .def("__getitem__", &ArraySlotWrapper<stype>::__getitem__) \
    .def("__setitem__", &ArraySlotWrapper<stype>::__setitem__) \
    .def("getValue", &ArraySlotWrapper<stype>::__getitem__) \
    .def("setValue", &ArraySlotWrapper<stype>::__setitem__) \
    .def("__str__", &ArraySlotWrapper<stype>::__str__) \
    .def("__iter__", &ArraySlotWrapper<stype>::__iter__, return_value_policy<manage_new_object>())
//    .def("onValueChanged", &ArraySlotWrapper<stype>::base_onValueChanged) 
//    .def("getValue", &ArraySlotWrapper<stype>::getValue, 
//    	 return_value_policy<copy_const_reference>()) 
//    .def("setValue", &ArraySlotWrapper<stype>::setValue) 
//    .def("getValues", &ArraySlotWrapper<stype>::getValues_wrap) 
//    .def("setValues", &ArraySlotWrapper<stype>::setValues_wrap) 


// The ArraySlot iterator
template<class T>
class _ArraySlotIterator
{
  private:
  ArraySlot<T>& arrslot;
  int idx;

  public:
  _ArraySlotIterator(ArraySlot<T>& as);

  object __iter__() { return object(this); }
  object next();
};

template<class T>
_ArraySlotIterator<T>::_ArraySlotIterator(ArraySlot<T>& as)
  : arrslot(as), idx(0)
{
}

template<class T>
object _ArraySlotIterator<T>::next()
{
  if (idx>=arrslot.size())
  {
    throw StopIteration();
  }
  else
  {
    if (arrslot.multiplicity()==1)
    {
      T res = arrslot.getValue(idx);
      idx+=1;
      return object(res);
    }
    else
    {
      const T* a = arrslot.getValues(idx);
      list lst;
      for(int i=0; i<arrslot.multiplicity(); i++)
      {
	lst.append(a[i]);
      }
      idx+=1;
      return tuple(lst);
    }
  }
}


// Wrapper class for the Dependent class
class DependentWrapper : public Dependent
{
  protected:
  PyObject* self;

  public:
  DependentWrapper(PyObject* self_)
    : Dependent(), self(self_)
  { 
  }

  //////// onValueChanged ////////

  // this method is called when onValueChanged() is called from C++ code
  void onValueChanged()
  {
    Dependent::onValueChanged();
    //    cout<<"Dependent::onValueChanged()"<<endl;
    call_method<void>(self, "onValueChanged");
  }

  // this method is called when onValueChanged() is called from Python code
  void base_onValueChanged()
  {
    //    cout<<"Dependent::base_onValueChanged()"<<endl;
    Dependent::onValueChanged();
  }

  //////// onValueChanged ////////

  // this method is called when onValueChanged() is called from C++ code
  void onValueChanged(int start, int end)
  {
    Dependent::onValueChanged(start, end);
    //    cout<<"Dependent::onValueChanged(start, end)"<<endl;
    call_method<void>(self, "onValueChanged", start, end);
  }

  // this method is called when onValueChanged() is called from Python code
  void base_onValueChanged_Array(int start, int end)
  {
    //    cout<<"Dependent::base_onValueChanged_Array(start, end)"<<endl;
    Dependent::onValueChanged(start, end);
  }

  //////// onResize ////////

  // this method is called when onResize() is called from C++ code
  void onResize(int size)
  {
    Dependent::onResize(size);
    //    cout<<"Dependent::onResize(size)"<<endl;
    call_method<void>(self, "onResize", size);
  }

  // this method is called when onResize() is called from Python code
  void base_onResize_Array(int size)
  {
    //    cout<<"Dependent::base_onResize_Array(size)"<<endl;
    Dependent::onResize(size);
  }

  //////// queryResizeVeto ////////

  // this method is called when queryResizeVeto() is called from C++ code
  bool queryResizeVeto(int size)
  {
    // Dependent::queryResizeVeto(size);
    return call_method<bool>(self, "queryResizeVeto", size);
  }

  // this method is called when queryResizeVeto() is called from Python code
  bool base_queryResizeVeto(int size)
  {
    return Dependent::queryResizeVeto(size);
  }

};


/**
  Slot wrapper class.

  \todo Sollten noch mehr Methoden so wie onValueChanged() verpackt werden?
 */
template<class T>
class SlotWrapper : public Slot<T>
{
  protected:

  PyObject* self;

  public:
  SlotWrapper(PyObject* self_, int aflags=0)
    : Slot<T>(aflags), self(self_)
  { 
  }

  SlotWrapper(PyObject* self_, const T& initial, int aflags)
    : Slot<T>(initial, aflags), self(self_)
  { 
  }

  // This class has a dedicated __str__() method, so that no copy of the
  // slot is made.
  // Using .def(::str(self)) creates a copy.
  static boost::python::str __str__(Slot<T>* self)
  {
    std::ostringstream os;
    os<<*self;
    return boost::python::str(os.str());
  }


  //////// getValue ////////

  // this method is called when getValue() is called from C++ code
  const T& getValue()
  {
    //    cout<<"SlotWrapper<T>::getValue()"<<endl;
    Slot<T>::value = call_method<T>(self, "getValue");
    return Slot<T>::value;
  }

  // this method is called when getValue() is called from Python code
  const T& base_getValue()
  {
    //    cout<<"SlotWrapper<T>::base_getValue()"<<endl;
    return Slot<T>::getValue();
  }

  //////// setValue ////////

  // this method is called when setValue() is called from C++ code
  void setValue(const T& val)
  {
    //    cout<<"SlotWrapper<T>::setValue()"<<endl;
    call_method<void>(self, "setValue", val);
  }

  // this method is called when setValue() is called from Python code
  void base_setValue(const T& val)
  {
    //    cout<<"SlotWrapper<T>::base_setValue()"<<endl;
    Slot<T>::setValue(val);
  }

  //////// onValueChanged ////////

  // this method is called when onValueChanged() is called from C++ code
  /*  void onValueChanged()
  {
    //    cout<<"SlotWrapper<T>::onValueChanged()"<<endl;
    call_method<void>(self, "onValueChanged");
  }

  // this method is called when onValueChanged() is called from Python code
  void base_onValueChanged()
  {
    //    cout<<"SlotWrapper<T>::base_onValueChanged"<<endl;
    Slot<T>::onValueChanged();
    }*/

  //////// computeValue ////////

  // this method is called when computeValue() is called from C++ code
  void computeValue()
  {
    //    cout<<"SlotWrapper<T>::computeValue()"<<endl;
    call_method<void>(self, "computeValue");
  }

  // this method is called when computeValue() is called from Python code
  void base_computeValue()
  {
    //    cout<<"SlotWrapper<T>::base_computeValue"<<endl;
    Slot<T>::computeValue();
  }

};

/**
  ArraySlot wrapper class.

  \todo Sollten noch mehr Methoden so wie onValueChanged() verpackt werden?
 */
template<class T>
class ArraySlotWrapper : public ArraySlot<T>
{
  protected:

  PyObject* self;

  public:
  ArraySlotWrapper(PyObject* self_, 
		   short multiplicity=1, 
		   boost::shared_ptr<SizeConstraintBase> aconstraint=boost::shared_ptr<SizeConstraintBase>())
    : ArraySlot<T>(multiplicity, aconstraint), self(self_)
  { 
  }

  //  __iter__ method()
  static _ArraySlotIterator<T>* __iter__(ArraySlot<T>* self)
  {
    return new _ArraySlotIterator<T>(*self);
  }

  static object __getitem__(ArraySlot<T>* self, int index)
  {
    if (self->multiplicity()==1)
    {
      return object(self->getValue(index));
    }
    else
    {
      return getValues_py(self, index);
    }
  }

  static void __setitem__(ArraySlot<T>* self, int index, object value)
  {
    if (self->multiplicity()==1)
    {
      T v = extract<T>(value);
      self->setValue(index, v);
    }
    else
    {
      setValues_py(self, index, value);
    }
  }


  /* wrapper for the getValues() method. The return value is a list of values

    The methods are static so that they can also be used on ArraySlot
    classes instead of ArraySlotWrapper classes. That's why they take
    the ArraySlot object as first argument.
    If this was a regular method it could happen that they can't be
    called in Python (for example, if you want to call them on 
    TriMeshGeom.faces, because in this case the ArraySlot wasn't created
    from Python and is actually not an  ArraySlotWrapper class).
   */
  static object getValues_py(ArraySlot<T>* self, int index)
  {
    list res;
    const T* ptr = self->getValues(index);
    for(int i=0; i<self->multiplicity(); i++)
    {
      res.append(ptr[i]);
    }
    return tuple(res);
  }

  static void setValues_py(ArraySlot<T>* self, int index, object pyvalues)
  {
    // Convert the input for setValues() from Python to C++...
    T* vals = new T[self->multiplicity()];
    for(int i=0; i<self->multiplicity(); i++)
    {
      object py;

      // Get the i. Python value...
      try
      {
        py = pyvalues.attr("__getitem__")(i);
      }
      catch(error_already_set&)
      {
	delete [] vals;
	// Clear the already generated exception (which is an AttributeError
	// because __getitem__ is not available or an IndexError because i
	// is out of range)
	PyErr_Clear();
	// Throw a more descriptive ValueError
	boost::python::object msg = "The values have to be given as a %d-sequence."%make_tuple(self->multiplicity());
	throw EValueError(PyString_AsString(msg.ptr()));
      }
      // ...and convert it to the corresponding C++ value
      try
      {
        T v = extract<T>(py);
	vals[i] = v;
      }
      catch(error_already_set&)
      {
	delete [] vals;
	throw_error_already_set();
      }
    }
    // Call the C++ setValues() method
    self->setValues(index, vals);
    delete [] vals;
  }

  // this method is called when onValueChanged() is called from C++ code
  /*  void onValueChanged(int start, int end)
  {
    //    cout<<"SlotWrapper<T>::onValueChanged()"<<endl;
    call_method<void>(self, "onValueChanged", start, end);
  }

  // this method is called when onValueChanged() is called from Python code
  void base_onValueChanged(int start, int end)
  {
    //    cout<<"SlotWrapper<T>::base_onValueChanged"<<endl;
    ArraySlot<T>::onValueChanged(start, end);
    }*/

  // This class has a dedicated __str__() method, so that no copy of the
  // slot is made.
  // Using .def(::str(self)) creates a copy.
  static boost::python::str __str__(ArraySlot<T>* self)
  {
    std::ostringstream os;
    os<<*self;
    return boost::python::str(os.str());
  }

};

#endif
