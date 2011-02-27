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

#ifndef SIZECONSTRAINT_H
#define SIZECONSTRAINT_H

/** \file sizeconstraint.h
 Contains the SizeConstraint classes that can be used with ArraySlots.
 */

#include <vector>
#include <algorithm>
#include "common_exceptions.h"
#include "dependent.h"
#include "debuginfo.h"


namespace support3d {

// Forward declaration
class IArraySlot;

/**
  Size constraint base class.

  A size constraint object can be passed into the constructor of an
  array slot and constrains the size of the slot so that calling
  resize() manually will not work. This mechanism is used for primitive
  variables whose size depends on the geometry and cannot be arbitrarily
  set by the user.

  A SizeConstraint instance can be used for an arbitrary number of
  slots. Each slot whose size should be constrained by a
  SizeConstraint object has to register itself using the
  registerSlot() method and unregister using unregisterSlot(). The
  slot also has to keep a reference to the constraint so that it can
  check the valid size and throw an exception when someone attempts to
  resize the slot. Whenever the constraint receives a new size it
  calls the slot's resize() method which must proceed as normal if the
  new size matches the constraint size (which is the case when the call
  is made from the constraint).

  \see ArraySlot
 */
class SizeConstraintBase
{
  protected:
  /**
    A list of array slot pointer that are affected by this constraint.

    As a constraint is tied to an array slot the slot's entire life time,
    the pointer cannot be invalid (if the slot is deleted it unregisters
    itself from the constraint).
   */
  std::vector<IArraySlot*> slots;

  /// The current size for the slots.
  int size;

  public:
  SizeConstraintBase(int asize=0) : slots(), size(asize) {DEBUGINFO(this, "SizeConstraint::SizeConstraint()");}
  virtual ~SizeConstraintBase() 
  { 
    DEBUGINFO(this, "SizeConstraint::~SizeConstraint()");
    // Check if all slots have been unregistered. If not, then issue a warning
    // about the impending doom...
    if (slots.size()!=0)
      std::cerr<<"BUG-WARNING: A size constraint is deleted while there are still slots\nregistered, it is very likely that the program will crash soon.\nPlease delete the slots *before* the size constraints.\n";
    DEBUGINFO(this, "SizeConstraint::~SizeConstraint() --- end");
  }

  /**
    Return the current size of the constraint.
   */
  int getSize() const { return size; }

  /**
     Add another slot to the list of slots constrained by this object.

     This method is called by \a aslot when the constraint should be
     enabled.
     Registering the same slot twice generates an exception.

     \param aslot The array slot that should be constrained
     \see unregisterSlot
   */
  void registerSlot(IArraySlot& aslot);

  /**
     Remove a slot from the list of slots constrained by this object.

     This method is called by \a aslot when the constraint should be
     disabled.
     An exception is thrown if \a aslot was not previously registered.

     \param aslot The array slot that was constrained
     \see registerSlot
   */
  void unregisterSlot(IArraySlot& aslot);

  protected:
  void execSlotResize();
};

//////////////////////////////////////////////////////////////////////

/**
   SizeConstraint where the size is manually set.
 */
class UserSizeConstraint : public SizeConstraintBase
{
  public:
  UserSizeConstraint(int asize=0): SizeConstraintBase(asize) {};

  /**
    Set a new size for the constraint.

    All registered slots are resized accordingly.

    \param newsize The new slot size
   */
  void setSize(int newsize);
};

//////////////////////////////////////////////////////////////////////

/**
   SizeConstraint where the size depends linearly on the size of another slot.

   The constraint size is \em a* size + \em b  where \em size  is the
   size of another slot and \em a and \em b are two custom coefficients.
 */
class LinearSizeConstraint : public SizeConstraintBase, public Dependent
{
  protected:
  /// Coefficient 1.
  int a;
  /// Coefficient 2 (offset).
  int b;
  /// The slot on whose size this constraint depends.
  IArraySlot* ctrl;

  public:
  /**
     Constructor.

     \param actrl The slot on whose size this constraint should be based on.
     \param a Coefficient 1.
     \param b Coefficient 2 (offset).
   */
  LinearSizeConstraint(IArraySlot& actrl, int a=1, int b=0);
  virtual ~LinearSizeConstraint();

  /**
     Return the current coefficients.
   */
  void getCoeffs(int& aout, int& bout) { aout=a; bout=b; }

  /**
     Set new coefficients.

     The operation might raise an exception if the new coefficients aren't
     compatible with any other constraint. In this case, the old coefficients
     remain active.

     \param newa Coefficient 1.
     \param newb Coefficient 2 (offset).
   */
  void setCoeffs(int newa, int newb);

  virtual void onResize(int newsize);
  virtual bool queryResizeVeto(int size);
  virtual void onControllerDeleted() 
  { 
    DEBUGINFO1(this, "LinearSizeConstraint::onControllerDeleted()  (ctrl: 0x%lx)", long(ctrl));
    ctrl=0; 
  }
};


}  // end of namespace

#endif
