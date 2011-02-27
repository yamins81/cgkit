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

#ifndef PROCEDURALSLOT_H
#define PROCEDURALSLOT_H

/** \file proceduralslot.h
 Procedural slot implementation.
 */

#include "slot.h"
#include "component.h"

namespace support3d {

/**
  A generic procedural slot.

  A procedural slot is one that \em computes its value instead of taking 
  it as input from another slot. This means a procedural slot can never
  take input connections but of course, they can be dependent on other slots.

  The procedure that computes the output value is \em not implemented right
  in this class, but is implemented in some other Component class (usually
  the component that contains the procedural slot). The pointers to the
  member functions are then passed to the procedural slot via the setProcedure()
  methods.

  You must always provide the component whose methods are to be called and
  a pointer to a method that computes the output value. That method must take
  one argument which is a reference to the result value. Optionally, you can
  pass pointers to the onValueChanged() and onResize() callback methods.

  The %ProceduralSlot class takes two template parameters, the first is the
  type of the output value and the second is the Component class whose methods
  will be called.

  Example:

  \code
  // A component that has a procedural slot that computes a double value
  class MyComponent : public Component
  {
    ProceduralSlot<double, MyComponent> myslot;
    ...
    ...
    void computeValue(double& result);
  }

  // Constructor
  MyComponent::MyComponent()
    : myslot()
  {
    // Initialize the procedural slot
    myslot.setProcedure(this, &MyComponent::computeValue);
    addSlot("myslot", myslot);
  }
  \endcode

 */
template<class T, class C>
class ProceduralSlot : public Slot<T>
{
  public:
  /// The type of the method that computes the output value
  typedef void (C::* ComputeValuePtr)(T&);
  /// The type of the onValueChanged callback method
  typedef void (C::* OnValueChangedPtr)();
  /// The type of the onValueChanged callback method for arrays
  typedef void (C::* OnValueChangedArrayPtr)(int, int);
  /// The type of the onResize callback method for arrays
  typedef void (C::* OnResizeArrayPtr)(int);
  /// The type of the onControllerDeleted callback method
  typedef void (C::* OnControllerDeletedPtr)(int);

  protected:
  /// The component whose methods should be called.
  C* component;
  /// Pointer to the method that computes the output value.
  ComputeValuePtr computeValuePtr;
  /// Pointer to the onValueChanged() callback method 
  OnValueChangedPtr onValueChangedPtr;
  /// Pointer to the onValueChanged() callback method for arrays
  OnValueChangedArrayPtr onValueChangedArrayPtr;
  /// Pointer to the onResize() callback method for arrays
  OnResizeArrayPtr onResizeArrayPtr;

  public:
  /**
     Constructor.
   */
  ProceduralSlot() 
    : Slot<T>(Slot<T>::NO_INPUT_CONNECTIONS), component(0), 
      computeValuePtr(0), onValueChangedPtr(0), onValueChangedArrayPtr(0), onResizeArrayPtr(0) {}

  /**
    Set the procedure that computes the output value.

    \param acomp The component that implements the methods
    \param proc The method that computes the output value
    \param vc The onValueChanged callback method
   */
  void setProcedure(C* acomp, ComputeValuePtr proc, OnValueChangedPtr vc=0)
  {
    component = acomp;
    computeValuePtr = proc;
    onValueChangedPtr = vc;
  }

  /**
    Set the procedure that computes the output value.

    \param acomp The component that implements the methods
    \param proc The method that computes the output value
    \param vca The onValueChanged callback method for arrays
    \param ra The onResize callback method
   */
  void setProcedure(C* acomp, ComputeValuePtr proc, OnValueChangedArrayPtr vca, OnResizeArrayPtr ra)
  {
    component = acomp;
    computeValuePtr = proc;
    onValueChangedArrayPtr = vca;
    onResizeArrayPtr = ra;
  }

  
  virtual void onValueChanged() 
  { 
    Slot<T>::onValueChanged(); 
    if (onValueChangedPtr!=0)
      (component->*onValueChangedPtr)(); 
  }

  virtual void onValueChanged(int start, int end) 
  { 
    Slot<T>::onValueChanged(start, end); 
    if (onValueChangedArrayPtr!=0)
      (component->*onValueChangedArrayPtr)(start, end); 
  }

  virtual void onResize(int newsize) 
  { 
    Slot<T>::onResize(newsize); 
    if (onResizeArrayPtr!=0)
      (component->*onResizeArrayPtr)(newsize);
  }

  protected:
  virtual void computeValue() { (component->*computeValuePtr)(Slot<T>::value); }
};


}  // end of namespace

#endif
