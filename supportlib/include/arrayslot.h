/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Python Computer Graphics Kit.
 *
 * The Initial Developer of the Original Code is Matthias Baas.
 * Portions created by the Initial Developer are Copyright (C) 2004
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef ARRAYSLOT_H
#define ARRAYSLOT_H

/** \file arrayslot.h
 Array slot implementation.
 */

#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <typeinfo>
#include <exception>
#include "compile_switches.h"
#include "slot.h"
#include "common_exceptions.h"
#include <boost/shared_ptr.hpp>

#include "dependent.h"
#include "sizeconstraint.h"

namespace support3d {

/**
  A low level data container (only for internal use).

  This class implements a resizeable array of a template type T that
  also has a reference counter for shared access. When a new instance
  is created the reference counter is initialized with 0. It's up the
  caller to increase and decrease the reference count and to delete
  the instance when the reference count reaches zero.

  The copy constructor of this class actually copies the entire array.

  This class is used in the implementation of the SharedArray.

  \see SharedArray
 */
template<class T>
class DataContainer
{
  public:
  /// A pointer to the array data (can be 0)
  T* data_ptr;
  /// The number of allocated elements in data_ptr
  int data_size;
  /// The current reference count
  int ref_count;
  /// The multiplicity of one value (the value is actually an array of values)
  short multiplicity;

  public:
  /** Standard constructor. */
  DataContainer(short amultiplicity=1) 
    : data_ptr(0), data_size(0), ref_count(0), multiplicity(amultiplicity)
  {
  }
  /** Copy constructor. 
   
     The entire array is copied (deep copy).
   */
  DataContainer(const DataContainer& c)
    : data_ptr(0), data_size(0), ref_count(0)
  {
    multiplicity = c.multiplicity;
    resize(c.data_size);
    for(int i=0; i<data_size*multiplicity; i++)
    {
      data_ptr[i] = c.data_ptr[i];
    }
  }

  /** Destructor. 
    
    Deletes the array.
   */
  ~DataContainer()
  {
    if (data_ptr!=0)
    {
      delete [] data_ptr;
      data_ptr = 0;
    }
  }

  DataContainer& operator=(const DataContainer& dc)
  {
    std::cout<<"DataContainer::operator=()  ASSIGNMENT"<<std::endl;
  }
  

  /**
    Index operator.

    \pre \a index must not be out of range
   */
  T& operator[](int index) 
  {
    return data_ptr[multiplicity*index]; 
  }

  /** Return the current reference count. */
  int refCount() const { return ref_count; }
  /** Increase the reference count by one and return the new count. */
  int incRefCount() { ref_count++; return ref_count; }
  /** Decrease the reference count by one and return the new count. */
  int decRefCount() { ref_count--; return ref_count; }
  /** Return the size of the array. */
  int size() { return data_size; }
  /** Get a pointer to the raw array data (may be 0). */
  T* data() { return data_ptr; }

  /**
    Resize the array.

    The previous elements are copied into the new array. Additional
    new elements are initialized with their default value.

    If size is equal to the current size then the method returns
    immediately.
   */
  void resize(int size)
  {
    // Do we already have the correct size?
    if (size==data_size)
      return;

    T* p = 0;
    // Allocate a new array and copy the previous elements...
    if (size>0)
    {
      // m = min(data_size, size)
      // This is the number of elements that will be copied to the new array.
      int m = (data_size<size)? data_size : size;
      int i;
      // Allocate the new array
      p = new T[multiplicity*size];
      // Copy the previous array elements...
      // (m is 0 if no array was set)
      for(i=0; i<m*multiplicity; i++)
      {
        p[i] = data_ptr[i];
      }
      // Initialize the rest...
      for(i=m*multiplicity; i<size*multiplicity; i++)
      {
	//p[i] = T(0);
	p[i] = T();
      }
    }
    else
    {
      size = 0;
    }
    // Delete the old array
    if (data_ptr!=0)
    {
      delete [] data_ptr;
    }
    // Set the new array...
    data_ptr = p;
    data_size = size;
  }
};

/**
  An array class that shares its data among instances.

  This class represents an array that shares its data with copies of the
  array. Modifying the array data affects all shared arrays that use the same
  data. The actual data is stored by the DataContainer class.

  \image html "shared_arrays.png" Three shared arrays using the same array data.

  This class is used in the implementation of the ArraySlot class where an array slot
  actually uses the same array data than its controller.

  \see DataContainer, ArraySlot
 */
template<class T>
class SharedArray
{
  protected:
  /** Pointer to the actual data.
 
     The pointer is never 0 unless the container has been released
     (but only the destructor releases a container permanently).
   */
  DataContainer<T>* data_ctr;

  public:
  /** Standard constructor. 

    Creates a new instance of a DataContainer and sets its reference count
    to 1.
   */
  SharedArray(short amultiplicity=1)
  {
    data_ctr = new DataContainer<T>(amultiplicity);
    data_ctr->incRefCount();
  }
  /** Copy constructor. 
   
     This only makes a shallow copy. Both SharedArrays will use the same
     data container.
   */
  SharedArray(const SharedArray& a)
  {
    data_ctr = a.data_ctr;
    data_ctr->incRefCount();
  }
  /** Assignment operator. 

    No copying involved, both arrays will use the same data.
   */
  SharedArray& operator=(const SharedArray& a)
  {
    if (&a==this)
      return *this;

    releaseContainer();
    data_ctr = a.data_ctr;
    data_ctr->incRefCount();
    return *this;
  } 

  /** Destructor. */
  ~SharedArray() 
  {
    releaseContainer();
  }

  /** Return the multiplicity of a data item. */
  short multiplicity() const { return data_ctr->multiplicity; }

  /** Return the current reference count of the data container. */
  int refCount() const { return data_ctr->refCount(); }

  /** Return the array size. */
  int size() const 
  { 
    return data_ctr->size();
  }

  /**
     Return the raw pointer to the array data.

     \return A pointer to the array data (might be 0 if there is no data set).
   */
  T* dataPtr()
  {
    return data_ctr->data_ptr;
  }

  /** Index operator.
    
     \pre \a index must not be out of range
   */
  T& operator[](int index) { return (*data_ctr)[index]; }
  /** Index operator.
    
     \pre \a index must not be out of range
   */
  const T& operator[](int index) const { return (*data_ctr)[index]; }

  /** Resize the array. 

    This influences every other shared array that uses the same data container.
   */
  void resize(int size) { data_ctr->resize(size); }

  /**
    Make a copy of the data if someone is using the data as well.

    After calling this method it is guaranteed that the data is only used
    by this shared array.
   */
  void makeUnique()
  {
    // Are we already the only one using the data?
    if (data_ctr->refCount()==1)
      return;

    // Make a copy of the data...
    DataContainer<T>* p = new DataContainer<T>(*data_ctr);
    p->incRefCount();
    releaseContainer();
    data_ctr = p;
  }

  private:
  /** Decrease the container's reference count and destroy the container if the count becomes zero. */
  void releaseContainer()
  {
    if (data_ctr->decRefCount()==0)
    {
      delete data_ctr;
      data_ctr = 0;
    }
  }
};

/**
  Array slot interface.
 */
class IArraySlot : public ISlot
{
  public:
  /** Return the number of items in the array. 

  \return Number of items.
  */
  virtual int size() const = 0;

  /**
     Check if this slot can currently be resized to a particular size.

     This method can be used to check if calling resize() with
     \a size as argument would succeed or not.

     The method returns \c false if either the slot itself is size
     constrained or it is connected to a size constrained slot. In
     the latter case, the function will return \c true again if the
     connection is removed. The method will also return \c true if 
     \a size matches the constraint size.
     
     The usual way to implement this method is to ask all dependents
     if a resize operation would be ok (this is done via the 
     queryResizeVeto() method).

     \param size This is the size that would be passed to resize()
     \param ignorelocalconstraint If True the method computes the result as if there wouldn't be a constraint in this slot
     \return True if a resize() operation would succeed
     \see resize
   */
  virtual bool isResizable(int size, bool ignorelocalconstraint=false) const = 0;
  
  /**
     Set a new size for the array.

     The values in the array remain (unless they have been cut away).
     An exception is thrown if the slot cannot be resized (because
     of size constraints of the slot itself or dependent slots).

     \param size The new array size
     \see isResizable
   */
  virtual void resize(int size) = 0;

  /** Return the multiplicity of a data item. 

    \return Multiplicity
    */
  virtual short multiplicity() const = 0;

  /** Copy one or more values into another %ArraySlot.

    Copies the slice [\a begin : \a end] into position \a index of the slot 
    \a target. The semantics of the slice is just as in Python, i.e. if
    you want to copy one single value you have to pass [n, n+1]. It's also
    possible to pass negative values.

    This method can be used without knowing the exact type of the slot
    (which would be the case if you wanted to copy by retrieving the value
    and setting the value again). For the copy operation to succeed, \a target
    has to be of the same type than this slot.

    If any index is out of range, an exception is thrown.

    Note: If this method is used to copy values within the same slot then
    the target range shouldn't currently overlap with the source slice.

    \param begin Begin of the slice in this array
    \param end  End of the slice in this array
    \param target Target slot (may be *this)
    \param index Begin of the slice in the target ArraySlot
   */
  virtual void copyValues(int begin, int end, IArraySlot& target, int index) = 0;
};


/**
  Default slot implementation for arrays.

  This template class can be used to store an array of any fixed type. 
  In addition to the ISlot interface it implements the getValue() and 
  setValue() methods.

  \see ISlot, Slot, SharedArray, SizeConstraintBase
 */
template<class T>
class ArraySlot : public IArraySlot
{
  protected:
  /// Slots that depend on this slot
  std::vector<Dependent*> dependents;

  /// Controlling slot
  ArraySlot<T>* controller;

  /// The shared array that contains the actual data
  SharedArray<T>  values;

  /// Size constraint (or 0)
  boost::shared_ptr<SizeConstraintBase> constraint;

  public:
  // Constructor
  ArraySlot(short amultiplicity=1, 
	    boost::shared_ptr<SizeConstraintBase> aconstraint=boost::shared_ptr<SizeConstraintBase>()) 
    : dependents(), controller(0), values(amultiplicity), constraint(aconstraint) 
  {
    DEBUGINFO3(this, "ArraySlot::ArraySlot(mult=%d, constraint=0x%lx)  (T: %s)", amultiplicity, long(aconstraint.get()), typeid(T).name());
    if (constraint.get()!=0)
      constraint->registerSlot(*this);
  }
  // Destructor
  virtual ~ArraySlot() 
  {
    DEBUGINFO(this, "ArraySlot::~ArraySlot()");
    if (constraint.get()!=0)
      constraint->unregisterSlot(*this);

    // Disconnect from the controller...
    if (controller!=0)
    {
      controller->removeDependent(this);
      controller = 0;
    }

    // Remove all existing dependencies...
    while(dependents.begin()!=dependents.end())
    {
      Dependent* d = *dependents.begin();
      d->onControllerDeleted();
      // Is the dependency still there? Then remove it...
      if (std::find(dependents.begin(), dependents.end(), d)!=dependents.end())
      {
        removeDependent(d);
      }
    }

    DEBUGINFO(this, "ArraySlot::~ArraySlot() --- end");
  }
  
  virtual bool isSlotCompatible(const ISlot* slot) const
  {
    const ArraySlot<T>* as = dynamic_cast<const ArraySlot<T>* >(slot);
    if (as==0)
      return false;
    return values.multiplicity()==as->values.multiplicity();
  }

  virtual const char* typeName() const { return typeid(ArraySlot<T>).name(); }
  
  
  virtual int size() const { return values.size(); }

  // isResizable
  virtual bool isResizable(int size, bool ignorelocalconstraint=false) const
  {
    if (this->size()==size)
      return true;

    // Check if there is a constraint that has to be satisfied...
    if (!ignorelocalconstraint && (constraint.get()!=0) && (size!=constraint->getSize()))
	return false;

    // Check if a dependent will veto the operation...
    std::vector<Dependent*>::const_iterator it;
    for(it=dependents.begin(); it!=dependents.end(); it++)
    {
      if ((*it)->queryResizeVeto(size))
	return false;
    }

    return true;
  };

  // resize
  virtual void resize(int size) 
  { 
    DEBUGINFO1(this, "ArraySlot::resize(%d)", size);

    // If the size does not change then return immediately...
    if (this->size()==size)
      return;

    // Is resizing the slot forbidden?
    if (!isResizable(size))
    {
      // Check if the problem was that the slot itself is size constrained
      // (to issue a different error message)
      if ((constraint.get()!=0) && (size!=constraint->getSize()))
	throw EValueError("Size constrained array slots cannot be resized manually.");
      
      throw EValueError("Resize operation rejected (disconnect any size constrained slots).");
    }

    // Do resize...
    if (controller==0)
    {
      values.resize(size); 
      notifyDependentsResize(size);
    }
    else
    {
      controller->resize(size);
    }
  }

  virtual short multiplicity() const { return values.multiplicity(); }

  virtual void copyValues(int begin, int end, IArraySlot& target, int index);

  virtual const T& getValue(int index);
  virtual void setValue(int index, const T& val);

  virtual const T* getValues(int index);
  virtual void setValues(int index, const T* vals);
  
  /**
     Return the raw pointer to the array data.

     The first n elements in the array are the first value of the array (n is
     the multiplicity of the array values), the second n elements are the second
     array value and so on. The pointer might be 0 if there is no data set.

     You may only modify the data if the slot does \b not have a controller set,
     otherwise the controller should modify the data so that really everyone is
     notified who depends on the data.
     When you do modify the data you have to call notifyDependentsValue() afterwards.

     \return A pointer to the array data (might be 0 if there is no data set).
   */
  T* dataPtr()
  {
    return values.dataPtr();
  }

  virtual ISlot* getController() const;
  virtual void setController(ISlot* ctrl);
  
  void addDependent(Dependent* d) 
  { 
    dependents.push_back(d); 
    d->onResize(size());
    d->onValueChanged(0, size());
  }

  void removeDependent(Dependent* d)
  {
    std::vector<Dependent*>::iterator res = std::remove(dependents.begin(), dependents.end(), d);
    dependents.erase(res, dependents.end());
  }

  virtual void onValueChanged(int start, int end) { notifyDependentsValue(start, end); }
  virtual void onResize(int newsize) { notifyDependentsResize(newsize); }
  virtual bool queryResizeVeto(int size) 
  { 
    return !isResizable(size);
  };
  virtual void onControllerDeleted() { setController(0); };
  void notifyDependents() { if (values.size()>0) notifyDependentsValue(0, values.size()); }
  void notifyDependentsValue(int start, int end);
  void notifyDependentsResize(int size);

  public:
  #ifdef MSVC6_TEMPLATES
   // Declaration for MSVC6
   friend std::ostream& operator<<(std::ostream& os, const ArraySlot<T>& slot);
  #else
   // Declaration for gcc 3.3
   template<class T2>
   friend std::ostream& operator<<(std::ostream& os, const ArraySlot<T2>& slot);
  #endif
};

// Output an array slot
template<class T>
std::ostream& operator<<(std::ostream& os, const ArraySlot<T>& slot)
{
  long addr = (long)&slot;
  int i,j;
  os<<"ArraySlot at 0x"<<std::setw(8)<<std::setfill('0')<<std::hex<<addr<<": ";
  os<<"size:"<<std::dec<<slot.size()<<"  mult:"<<slot.values.multiplicity()<<"  refcount:"<<slot.values.refCount();
  if (slot.controller==0)
  {
    os<<"  no controller";
  }
  else
  {
    addr = (long)slot.controller;
    os<<"  controller:0x"<<std::setw(8)<<std::setfill('0')<<std::hex<<addr;
  }
  os<<std::endl<<"  values: ";
  for(i=0; i<slot.size(); i++)
  {
    const T* ptr = const_cast<ArraySlot<T>&>(slot).getValues(i);
    if (slot.values.multiplicity()==1)
    {
      os<<(*ptr);
    }
    else
    {
      os<<"[";
      for(j=0; j<slot.values.multiplicity(); j++)
      {
        if (j>0)
          os<<", ";
        os<<ptr[j];
      }
      os<<"]";
    }
    if (i<slot.size()-1)
      os<<", ";
    if (i==5 && i!=slot.size()-1)
    {
      os<<"...";
      break;
    }
  }
  os<<std::endl<<"  "<<std::dec<<slot.dependents.size()<<" dependents:";
  for(i=0; i<int(slot.dependents.size()); i++)
  {
    addr = (long)slot.dependents[i];
    os<<" 0x"<<std::setw(8)<<std::setfill('0')<<std::hex<<addr;
  }
  return os;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

template<class T>
void ArraySlot<T>::copyValues(int begin, int end, IArraySlot& target, int index)
{
  // Check if the type of target is ok...
  if (!isSlotCompatible(&target))
    throw EValueError("Cannot copy values between incompatible slots");

  // The following cast will work because of the above check...
  ArraySlot<T>* typedtarget = dynamic_cast<ArraySlot<T>* >(&target);

  // Adjust indices...
  if (begin<0)
    begin = size()+begin;
  if (end<0)
    end = size()+end;
  if (index<0)
    index = target.size()+index;

  //  short mult = multiplicity();

  // Copy the values...
  for(int i=begin; i<end; i++)
  {
    const T* vals = getValues(i);
    typedtarget->setValues(index, vals);
    index++;
  }
}

/**
  Return the value at a particular index.

  This method returns the value at position \a index. This method should
  only be used when multiplicity is 1. Use getValues() if the multiplicity
  is greater than 1.
  The index may be negative to count from the end of the array. If the
  index is out of range, an EIndexError exception is thrown.

  \param index Array index (0-based). May be negative.
  \return Value at position \a index.
 */
template<class T>
const T& ArraySlot<T>::getValue(int index)
{
  if (index<0)
    index = size()+index;
  if ((index<0) || (index>=size()))
    throw EIndexError();

  return values[index];
}

/**
  Set a new value a particular index.

  This method sets a new value at position \a index. It should
  only be used when multiplicity is 1. Use setValues() if the multiplicity
  is greater than 1.
  The index may be negative to count from the end of the array. If the
  index is out of range, an EIndexError exception is thrown.

  \param index Array index (0-based). May be negative.
  \param val New value
 */
template<class T>
void ArraySlot<T>::setValue(int index, const T& val)
{
  if (index<0)
    index = size()+index;
  if ((index<0) || (index>=size()))
    throw EIndexError();

  if (controller!=0)
  {
    controller->setValue(index, val);
  }
  else
  {
    values[index] = val;
    notifyDependentsValue(index, index+1);
  }
}

/**
  Return the array value at a particular index.

  This method returns a pointer to the value at position \a index and
  it is intended for array slots with a multiplicity that's greater than 1.
  The pointer actually points to the array at position \a index.
  The index negative to count from the end of the array. If the index is 
  out of range, an EIndexError exception is thrown.

  \param index Array index (0-based). May be negative.
  \return Pointer to the array at position \a index.
 */
template<class T>
const T* ArraySlot<T>::getValues(int index)
{
  if (index<0)
    index = size()+index;
  if ((index<0) || (index>=size()))
    throw EIndexError();
  return &(values[index]);
}

/**
  Set a new array value.

  This method sets a new value at position \a index and
  it is intended for array slots with a multiplicity that's greater than 1.
  The index negative to count from the end of the array. If the index is 
  out of range, an EIndexError exception is thrown.

  \param index Array index (0-based). May be negative.
  \param vals New value (array)
 */
template<class T>
void ArraySlot<T>::setValues(int index, const T* vals)
{
  if (controller!=0)
  {
    controller->setValues(index, vals);
  }
  else
  {
    if (index<0)
      index = size()+index;
    if ((index<0) || (index>=size()))
      throw EIndexError();

    T* ptr = &(values[index]);
    for(int i=0; i<values.multiplicity(); i++)
    {
      ptr[i] = vals[i];
    }
    notifyDependentsValue(index, index+1);
  }
}


template<class T>
ISlot* ArraySlot<T>::getController() const
{
  return controller;
}

/**
  \todo notifyDependents() aufrufen wenn Controller neu gesetzt wird
 */
template<class T>
void ArraySlot<T>::setController(ISlot* ctrl)
{
  DEBUGINFO1(this, "ArraySlot::setController(0x%lx)", long(ctrl));
  ArraySlot<T>* newctrl = 0;

  // If the new and old controller are the same, then there's nothing to do
  if (controller==ctrl)
    return;  

  // Check compatibility...
  if (ctrl!=0)
  {
    newctrl = dynamic_cast<ArraySlot<T>* >(ctrl);
    if (newctrl==0)
    {
      throw EIncompatibleSlotTypes(this, ctrl);
    }

    // Check if the size of the controller matches the constraint size
    // (if there is a constraint)...
    if ((constraint.get()!=0) && (newctrl->size()!=constraint->getSize()))
    {
      throw EValueError("Array slot size mismatch (the controller must match the constrained size).");
    }

    // Set the reference to the data container
    values = newctrl->values;
    
    // Connect to the new controller
    newctrl->addDependent(this);
    notifyDependentsResize(values.size());
  }
  else
  {
    values.makeUnique();
  }

  // Disconnect from old controller...
  if (controller!=0)
    controller->removeDependent(this);

  controller = newctrl;
}

template<class T>
void ArraySlot<T>::notifyDependentsValue(int start, int end)
{
  std::vector<Dependent*>::iterator it;
  for(it=dependents.begin(); it!=dependents.end(); it++)
  {
    (*it)->onValueChanged(start, end);
  }
}

template<class T>
void ArraySlot<T>::notifyDependentsResize(int size)
{
  std::vector<Dependent*>::iterator it;
  for(it=dependents.begin(); it!=dependents.end(); it++)
  {
    (*it)->onResize(size);
  }
}


}  // end of namespace

#endif
