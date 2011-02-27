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

#ifndef DEPENDENT_H
#define DEPENDENT_H

/** \file dependent.h
 Contains the Dependent base class and the NotificationForwarder.
 */

// Define the CGKIT_SHARED variable
#ifdef DLL_EXPORT_DEPENDENT
  #include "shared_export.h"
#else
  #include "shared.h"
#endif

namespace support3d {

/**
  Base class for everyone who wants to be notified by slots.

 */
class CGKIT_SHARED Dependent
{
  public:
  Dependent() {}
  virtual ~Dependent() {}
  /**
     Notification callback method.

     This method is called by a controlling slot whenever the value of the
     slot has changed. 

     The usual procedure in derived classes is to mark the local cache
     as invalid so that next time getValue() is called the updated value
     is retrieved from the controlling slot. All dependent slots also
     have to be notified.
   */
  virtual void onValueChanged() {};
  virtual void onValueChanged(int start, int end) {};
  /**
     Notification callback method.
     
     This method is called by a controlling array slot whenever the size
     of the slot has changed.

     \param newsize The new size of the array slot.
   */
  virtual void onResize(int newsize) {};
  /**
     Check if a dependent vetoes a resize operation.

     This method is called by a controlling slot whenever the slot size
     should be changed. If the method returns true the resize operation
     is rejected.
     The default implementation returns false. Only slots with size 
     constraints should return true.

     \param size The new size for the slot
     \return True if the resize operation has to be rejected
   */
  virtual bool queryResizeVeto(int size) { return false; };

  /**
     This callback is called when the controller is being deleted.

     When the controlling object is deleted it calls onControllerDeleted() on
     all dependent objects. The controller still exists and can be used. It is
     allowed to call removeDependent() on the controller, but it's not necessary
     as it is done by the controller automatically.
   */
  virtual void onControllerDeleted() {};
};

/** 
  Forward slot events to arbitrary class methods.

  This class can be used to forward slot events from one particular 
  slot (onValueChanged(), onResize(), ...) to an arbitrary method.
  The template parameter is the class of the object that will receive
  the events.

  \b Usage: Initialize the notification forwarder by calling an init() method
  which tells the forwarder which methods it should call when an event
  occurs. Then connect the slot you depend on with the forwarder instance.
  
 */
template<class T>
class NotificationForwarder : public Dependent
{
  public:
  typedef void (T::* OnValueChangedPtr)();
  typedef void (T::* OnValueChangedArrayPtr)(int start, int end);
  typedef void (T::* OnResizeArrayPtr)(int size);
  
  protected:
  T* _dest;
  OnValueChangedPtr _onValueChanged;
  OnValueChangedArrayPtr _onValueChangedArray;
  OnResizeArrayPtr _onResizeArray;

  public:
  NotificationForwarder()
  {
  }

  void init(T* dest, OnValueChangedPtr valchanged)
  {
    _dest = dest;
    _onValueChanged = valchanged;
    _onValueChangedArray = 0;
    _onResizeArray = 0;
  }

  void init(T* dest, OnValueChangedArrayPtr valchanged, OnResizeArrayPtr resize)
  {
    _dest = dest;
    _onValueChanged = 0;
    _onValueChangedArray = valchanged;
    _onResizeArray = resize;
  }

  void onValueChanged() 
  { 
    if (_onValueChanged!=0)
    {
      (_dest->*_onValueChanged)();
    }
  }
  void onValueChanged(int start, int end)
  {
    if (_onValueChangedArray!=0)
    {
      (_dest->*_onValueChangedArray)(start, end);
    }
  }
  void onResize(int newsize) 
  {
    if (_onResizeArray!=0)
    {
      (_dest->*_onResizeArray)(newsize);
    }
  }
};


}  // end of namespace

#endif
