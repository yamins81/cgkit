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

#ifndef SLOT_H
#define SLOT_H

/** \file slot.h
 Slot implementation.
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
#include "common_exceptions.h"
#include "debuginfo.h"

#include "dependent.h"


// Define the CGKIT_SHARED variable
#ifdef DLL_EXPORT_SLOT
  #include "shared_export.h"
#else
  #include "shared.h"
#endif

namespace support3d {


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

/**
  Exception class.
 */
class ENonCopyable : public std::exception
{
  public:
  ENonCopyable()
  {
  }
  ~ENonCopyable() throw() {}

  /// Return exception message.
  const char* what() const throw()
  {
    return "Slots cannot be copied.";
  }
};

template<class T>
class CGKIT_SHARED Slot;

/**
  Generic slot interface.

  A slot is a container for a value of a particular type. They are
  used to store animatable attributes of arbitrary components.
  Slots can be connected to each other to propagate their value along
  a chain. A slot A that forwards its value to another slot B is called
  the \em controller of that slot. %Slot B is then a \em dependent of
  slot A.

  \image html "slot_controller.png" "Slot 1 is the controller of slot 2. Both slots will always carry the same value."

  This abstract base class contains the basic interface of a slot.
  Every slot must derive from this class and implement the methods.

  \see Slot, SlotValueInterface
*/
class CGKIT_SHARED ISlot : public Dependent
{
  public:
  static long _slot_counter;

  public:
  ISlot();
  virtual ~ISlot();

  /**
    Check if another slot is compatible with this slot.

    Two slots are compatible if they hold the same value. Only
    compatible slots can be connected to each other.
    But note, that even when this method returns true, it can
    happen that the slots can \em not be connected. This is
    the case whenever the target just doesn't accept input 
    connections (for example, because it's computing its output 
    value procedurally).
    
    \return True if compatible, false otherwise.
   */
  virtual bool isSlotCompatible(const ISlot* slot) const = 0;

  /**
     Return the name of the stored type.

     This name can be used to display the type of the slot to the user.
     The name should be chosen so that if two slots cannot be connected
     to each other then they should also return different names
     (however, if two slots have different names it's still possible
     that they can be connected to each other).

     \return Type name.
  */
  virtual const char* typeName() const = 0;

  /**
     Connect the slot with another slot.

     This method establishes a connection between \a this and \a target
     so that \a target will be notified whenever the value of \a this
     changes. 

     The method has no effect if \a target is 0.

     \param target The target slot that should be connected to
   */
  virtual void connect(ISlot* target)
  {
    if (target!=0)
      target->setController(this);
  }

  /**
     Disconnect the connection to another slot.

     This method breaks an existing connection to another slot. An
     \c EValueError exception is thrown if a connection between \a this
     and \a target does not exist.

     The method has no effect if \a target is 0.

     \param target The target slot that should be disconnected from
   */
  virtual void disconnect(ISlot* target)
  {
    if (target!=0)
    {
      if (target->getController()!=this)
	throw EValueError("attempt to disconnect a non-existing connection");
      target->setController(0);
    }
  }
  
  /**
   Establish a dependency between this and another object.

   The argument \a d is added to the list of dependent objects. This
   means that \a d (or the slot it stands for) somehow depends on the 
	 value of \a this and that \a d has to be notified whenever the 
	 value of \a this changes.

	 Usually \a d is a pointer to the actual slot whose value depends
	 on the value of \a this. However, it is possible to pass another
	 object on behalf of the slot object. This is useful if the slot
	 value depends on more than just one other slot and the it wants
	 to keep track of which slot has actually changed.

   The actual notification is done by calling notifyDependents() which 
	 in turn calls onValueChanged() on all dependent objects.

   \warning Whenever you establish a dependency using this method you
            have to make sure that the dependency is removed when \a d
            is deleted. Otherwise there will be a dangling pointer in
            this object which will lead to a crash when \a this is
            deleted (because it tries to notify the non-existent \a d
            about its deletion).

   \param d The object that contains the notification callback.
   \see removeDependent, notifyDependents, onValueChanged
	 \todo What happens if d is already in the list?
  */
  virtual void addDependent(Dependent* d) = 0;
  
  /**
   Remove the dependency between this and another object.

   \param d  The dependent object that is to be removed.     
   \see addDependent, notifyDependents, onValueChanged
	 \todo What happens if d is not in the list?
   */
  virtual void removeDependent(Dependent* d) = 0;

  /**
     Notify all dependent slots about a value change.

     Calls the onValueChanged() method of all slots whose value
     depends on the value of this slot.

     \see addDependent, removeDependent, onValueChanged
   */
  virtual void notifyDependents() = 0;


  /**
    Helper method for extracting the value of a slot.

    The actual slot object (this) must be derived from Slot and the
    type of \a target must be the same than the type of the slot value.
    If the types do not match \a target will be left unchanged and false is
    returned.

    \param[out] target This variables receives the slot value
    \return True if the value could be retrieved, false otherwise.
  */
  template<class T>
  bool getVal(T& target)
  {
    Slot<T>* slot = dynamic_cast<Slot<T>*>(this);
    if (slot!=0)
    {
       target = slot->getValue();
       return true;
    }
    else
      return false;
  }

  /**
    Helper method for setting a new slot value.

    The actual slot object (this) must be derived from Slot and the
    type of \a val must be the same than the type of the slot value.
    If the types do not match no value is set and false is returned.

    \param val New value to set.
    \return True if the value could be set, false otherwise.
  */
  template<class T>
  bool setVal(const T& val)
  {
    Slot<T>* slot = dynamic_cast<Slot<T>*>(this);
    if (slot!=0)
    {
       slot->setValue(val);
       return true;
    }
    else
      return false;
  }

  /**
     Get the associated controller.

     \return Controlling slot or 0 if there is no controller.
  */
  virtual ISlot* getController() const = 0;

  /**
     Set a controller for this slot.

     Replaces the current controller with \a ctrl which may be 0 to
     remove the current controller only.
     If the controller is not compatible with this slot an
     EIncompatibleSlotTypes exception is thrown.
     This method also calls addDependent(this) on \a ctrl to add this
     slot to the list of dependent slots of \a ctrl.

     \param ctrl Controlling slot or 0 to disconnect any existing controller.
  */
  virtual void setController(ISlot* ctrl) = 0;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

/**
  Exception class.
 */
class EIncompatibleSlotTypes : public std::exception
{
  public:
  std::string msg;
  ISlot* slot1;
  ISlot* slot2;

  /// Constructor.
  EIncompatibleSlotTypes(ISlot* _slot1, ISlot* _slot2)
    : exception(), msg(), slot1(_slot1), slot2(_slot2)
  {
    std::ostringstream os;
    os<<"Incompatible slot types: "<<slot1->typeName()<<" and "<<slot2->typeName();
    msg = os.str();
  }
  ~EIncompatibleSlotTypes() throw() {}

  /// Return exception message.
  const char* what() const throw()
  {
    return msg.c_str();
  }
};

/**
  Exception class.
 */
class ENoInputConnectionsAllowed : public std::exception
{
  public:
  ENoInputConnectionsAllowed()
  {
  }
  ~ENoInputConnectionsAllowed() throw() {}

  /// Return exception message.
  const char* what() const throw()
  {
    return "No input connections allowed.";
  }
};


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

/**
  Default slot implementation.

  This template class can be used to store any fixed type. In addition
  to the ISlot interface it implements the getValue() and setValue()
  methods.

  \see ISlot, ArraySlot
  \todo Sub slots müssen sich noch beim master slot bekannt machen können
 */
template<class T>
class CGKIT_SHARED Slot : public ISlot
{
  protected:
  /// Dependent objects (each object usually represents another slot).
  std::vector<Dependent*> dependents;

  public:
  /// Controlling slot
  Slot<T>* controller;

  /// Flags
  unsigned short flags;

  /// Cache value
  T value;

  /// Flag identifiers
  enum Flags { CACHE_VALID          = 0x01, 
               NO_INPUT_CONNECTIONS = 0x02 };

  public:
  Slot(int aflags=0);
  Slot(const T& initialvalue, int aflags);
  Slot(const Slot<T>& s);
  virtual ~Slot();
  
  Slot<T>& operator=(const Slot<T>& s) { throw ENonCopyable(); }

  virtual bool isSlotCompatible(const ISlot* slot) const;
  virtual const char* typeName() const { return typeid(T).name(); }
  
  virtual const T& getValue();
  virtual void setValue(const T& val);
  
  virtual ISlot* getController() const;
  virtual void setController(ISlot* ctrl);
  
  void addDependent(Dependent* d);
  void removeDependent(Dependent* d);

  virtual void onValueChanged();
  virtual void onValueChanged(int start, int end);
  virtual void onResize(int newsize);
  virtual void onControllerDeleted() { setController(0); };
  void notifyDependents();
  
  protected:
  /**
    This method has to compute a new value based on other input values.

    This method is used to compute a new value whenever getValue() is 
    called and the cache is invalid.
   */
  virtual void computeValue() {}

  /**
    Set the state of one or more flags.

    \param aflags Flags to set
	\param value Boolean value to which the flags fill be set
   */
  void setFlags(int aflags, bool value)
  {
    if (value)
	{
	  flags |= aflags;
	}
	else
	{
	  flags &= ~aflags;
	}
  }

  public:
  #ifdef MSVC6_TEMPLATES
    // Declaration for MSVC6
    friend std::ostream& operator<<(std::ostream& os, const Slot<T>& slot);
  #else
    // Declaration for gcc 3.3
    template<class T2>
    friend std::ostream& operator<<(std::ostream& os, const Slot<T2>& slot);
  #endif
};

//////////////////////////////////////////////////////////////////////

/**
  Get and set slot values through a raw ISlot pointer.

  This class can be used to get and set the slot value of a raw ISlot pointer.
  When declaring an object of this class you have to provide the expected slot
  value type as template parameter. By default, the class assumes that the slot
  is implemented by the Slot class. You can provide a different class as second
  template parameter.

  Example:

  \code
  // Retrieve a slot pointer from somewhere that's supposed to hold 
  // the radius of a sphere using a double.
  ISlot* radius_slot = ...;

  // Create an interface to a double value
  SlotValueInterface<double> radius(radius_slot);

  // Read the value of the radius slot
  double rad = radius();
  
  // Set a new radius value
  radius(1.5);
  
  \endcode

  \see ISlot, Slot
 */
template<class T, class S = Slot<T> >
class SlotValueInterface
{
  protected:
  ISlot* slot;

  public:
  /**
    Constructor.

   \param aslot  The slot whose value should be read or written.
  */
  SlotValueInterface(ISlot* aslot) : slot(aslot) {}
 
  /**
    Get the slot value.

    An exception is thrown if the type of the return value is not compatible with 
    the slot.

    \return A reference to the slot value.
    \todo Richtige Exception werfen
   */
  T& operator()()
  {
    S* s = dynamic_cast<S*>(slot);
    if (s!=0)
    {
      return s->getValue();
    }
    else
    {
      throw 1;
    }
  }

  /**
    Set the slot value.

    An exception is thrown if the type of \a val is not compatible with the slot.

    \param val New value.
    \todo Richtige Exception werfen
   */
  void operator()(const T& val)
  {
    S* s = dynamic_cast<S*>(slot);
    if (s!=0)
    {
       s->setValue(val);
    }
    else
    {
      throw 1;
    }
  }
};

//////////////////////////////////////////////////////////////////////

template<class T>
std::ostream& operator<<(std::ostream& os, const Slot<T>& slot)
{
  long addr = (long)&slot;
  os<<"Slot at 0x"<<std::setw(8)<<std::setfill('0')<<std::hex<<addr<<": value:";
  os<<slot.value<<" ("<<typeid(T).name()<<") flags:"<<slot.flags;
  if (slot.controller==0)
  {
    os<<"  no controller";
  }
  else
  {
    addr = (long)slot.controller;
    os<<"  controller:0x"<<std::setw(8)<<std::setfill('0')<<std::hex<<addr;
  }
  os<<std::endl<<"  "<<std::dec<<slot.dependents.size()<<" dependents:";
  for(int i=0; i<int(slot.dependents.size()); i++)
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
Slot<T>::Slot(int aflags)
  : dependents(), controller(0), flags(aflags), value(T())
{
  DEBUGINFO2(this, "Slot<T>::Slot(aflags=%d)  (%s)", aflags, typeid(T).name());
  
  // Set the CACHE_VALID flag to the inverted state ot the NO_INPUT_CONNECTIONS flag
  setFlags(CACHE_VALID, !(flags & NO_INPUT_CONNECTIONS));
}

template<class T>
Slot<T>::Slot(const T& initialvalue, int aflags)
 : dependents(), controller(0), flags(aflags), value(initialvalue)
{
  DEBUGINFO2(this, "Slot<T>::Slot(initialvalue, aflags=%d)  (%s)", aflags, typeid(T).name());

  // Set the CACHE_VALID flag to the inverted state ot the NO_INPUT_CONNECTIONS flag
  setFlags(CACHE_VALID, !(flags & NO_INPUT_CONNECTIONS));
}

/**
  Copy constructor.

  The copy only gets the cache value (which may not be the true value) and the flags.
  It has no controller set and there are no dependencies.
 */
template<class T>
Slot<T>::Slot(const Slot<T>& s)
: dependents(), controller(0), flags(s.flags), value(s.value)
{
  DEBUGINFO1(this, "Slot<T>::Slot(const Slot<T>&) (copy constructor) (%s)", typeid(T).name());
}

template<class T>
Slot<T>::~Slot()
{
  DEBUGINFO1(this, "Slot<T>::~Slot()  (%s)", typeid(T).name());
  
  // Disconnect from the controller...
  //  DEBUGINFO(this, "  disconnect");
  if (controller!=0)
  {
    controller->removeDependent(this);
    controller = 0;
  }

  // Remove all existing dependencies...
  //  DEBUGINFO(this, "  remove deps");
  while(dependents.begin()!=dependents.end())
  {
    Dependent* d = *dependents.begin();
    //    DEBUGINFO1(this, "    %#lx->onControllerDeleted()", d);
    d->onControllerDeleted();
    // Is the dependency still there? Then remove it...
    //    DEBUGINFO(this, "    check");
    if (std::find(dependents.begin(), dependents.end(), d)!=dependents.end())
    {
      //      DEBUGINFO1(this, "    remove(0x%d)", long(d));
      removeDependent(d);
    }
  }

  DEBUGINFO1(this, "Slot<T>::~Slot()  (%s) --- end", typeid(T).name());
}

template<class T>
bool Slot<T>::isSlotCompatible(const ISlot* slot) const
{
  return dynamic_cast<const Slot<T>* >(slot) != 0;
}


/**
   Return a reference to the value of this slot.

   This method updates the value if necessary and returns a reference to the
   current value. The method works according to the following flow chart:

   \image html "getvalue_diagram.png"

  \return Slot value.
 */
template<class T>
const T& Slot<T>::getValue()
{
  DEBUGINFO(this, "Slot<T>::getValue()");

  // Is the value in the cache still valid?
  if (flags & CACHE_VALID)
    return value; 

  // Obtain a new value and store it in the cache
  if (controller!=0)
  {
    value = controller->getValue();
  }
  else
  {
    computeValue();
  }

  flags |= CACHE_VALID;
  return value;
}

/**
   Set a new slot value.

   Replaces the current value with a copy of \a val. 
   If the slot has a controller the new value is propagated up to
   the controller. It is then up to the controller how this new value
   influences the actual output value. So it is not guaranteed that
   the output of the slot is really the new value.
   When the value really changes this method triggers the call of
   onValueChanged() on all dependent objects.

   The method works according to the following flow chart:

   \image html "setvalue_diagram.png"

   \param val New value
 */
template<class T>
void Slot<T>::setValue(const T& val)
{
  DEBUGINFO(this, "Slot<T>::setValue(val)");

  // Is this a procedural slot? then the value cannot be set
  if (flags & NO_INPUT_CONNECTIONS)
    return;

  // Check if the new value is really a *new* value
  if ((flags & CACHE_VALID) && value==val)
    return;

  if (controller!=0)
  {
    flags &= ~CACHE_VALID;
    controller->setValue(val);
  }
  else
  {
    flags |= CACHE_VALID;
    value = val;
    notifyDependents();
  }
}
  
template<class T>
ISlot* Slot<T>::getController() const
{
  return controller;
}

/*
  throws EIncompatibleSlotTypes exception if ctrl is not compatible
 */
template<class T>
void Slot<T>::setController(ISlot* ctrl)
{
  DEBUGINFO1(this, "Slot<T>::setController(%p)", ctrl);
  // Check if this slot can take input connections (but allow setting 0
  // as this can happen during destruction)
  if ((flags & NO_INPUT_CONNECTIONS) && ctrl!=0)
	  throw ENoInputConnectionsAllowed();

  // If the new and old controller are the same, then there's nothing to do
  if (controller==ctrl)
  {
    DEBUGINFO1(this, "Slot<T>::setController(%p) --- end  (new==old)", ctrl);
    return;
  }

  // Is the new ctrl 0? then disconnect only...
  // Note: If ctrl==0 then controller!=0, otherwise we wouldn't be here
  if (ctrl==0)
  {
    // Update the value before disconnecting
    // (it's not necessary to notify the dependents because the value
    // doesn't actually change. It just might be that it wasn't stored
    // here)
    value = controller->getValue();
    // Now disconnect
    controller->removeDependent(this);
    controller = 0;
    flags |= CACHE_VALID;
  }
  else
  {
    Slot<T>* newctrl = dynamic_cast<Slot<T>* >(ctrl);
    if (newctrl==0)
    {
      throw EIncompatibleSlotTypes(this, ctrl);
    }    

    // Disconnect from old controller...
    if (controller!=0)
      controller->removeDependent(this);

    // ...and connect to new one
    controller = newctrl;
    flags &= ~CACHE_VALID;
    ctrl->addDependent(this);
    notifyDependents();
  }
  DEBUGINFO1(this, "Slot<T>::setController(%p) --- end", ctrl);
}

template<class T>
void Slot<T>::addDependent(Dependent* d)
{
  DEBUGINFO1(this, "Slot<T>::addDependent(%p)", d);

  // Do nothing if the dependent was already added before
  if (std::find(dependents.begin(), dependents.end(), d)!=dependents.end())
    return;

  dependents.push_back(d);
  d->onValueChanged();
}

template<class T>
void Slot<T>::removeDependent(Dependent* d)
{
  DEBUGINFO1(this, "Slot<T>::removeDependent(%p)", d);

  // Remove the element (it is moved to the back)
  std::vector<Dependent*>::iterator res = std::remove(dependents.begin(), dependents.end(), d);
  // Is d not in the dependent list?
  if (res==dependents.end())
  {
    throw EValueError("Attempt to remove a non-existent slot dependency.");
  }
  // Erase the element from the vector
  dependents.erase(res, dependents.end());
}


template<class T>
void Slot<T>::notifyDependents()
{
  std::vector<Dependent*>::iterator it;
  for(it=dependents.begin(); it!=dependents.end(); it++)
  {
    (*it)->onValueChanged();
  }
}

/**
  Notification callback method

  The cache is marked as invalid and the dependent objects are notified
  about the value change.

  \image html "onvaluechanged_diagram.png"
*/
template<class T>
void Slot<T>::onValueChanged()
{
  DEBUGINFO(this, "Slot<T>::onValueChanged()");

  // Only notify when the cache was still valid
  // (otherwise the dependents have already been notified before
  // and didn't yet request a new value).
  // This also breaks cyclic dependencies.
  if (flags & CACHE_VALID)
  {
    flags &= ~CACHE_VALID;
    notifyDependents();
  }
}

template<class T>
void Slot<T>::onValueChanged(int start, int end)
{
  onValueChanged();
}

template<class T>
void Slot<T>::onResize(int newsize)
{
  onValueChanged();
}

}  // end of namespace

#endif
