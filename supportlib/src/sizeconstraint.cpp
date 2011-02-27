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

#include "sizeconstraint.h"
#include "arrayslot.h"

namespace support3d {

/**
  Calls resize() on all registered slots.

  This is called by actual constraint implementations whenever the size
  has changed. The size attribute must be set to the new size before
  calling this method.

  The method may throw an exception when a resize operation on a slot
  has failed. In this case the registered slots may have different sizes.
  In this case, the caller must call the method again after restoring the
  original size.
 */
void SizeConstraintBase::execSlotResize()
{
  std::vector<IArraySlot*>::iterator it;
  std::vector<IArraySlot*>::iterator it_end = slots.end();
  for(it=slots.begin(); it!=it_end; it++)
  {
    (*it)->resize(size);
  }
}

// registerSlot
void SizeConstraintBase::registerSlot(IArraySlot& aslot)
{
  DEBUGINFO1(this, "SizeConstraint::registerSlot(0x%x)", long(&aslot));

  // Check if the slot is already there...
  std::vector<IArraySlot*>::iterator it = std::find(slots.begin(), slots.end(), &aslot);
  if (it!=slots.end())
    throw EValueError("Slot is already registered with the size constraint.");

  // Add the slot...
  slots.push_back(&aslot);
  // ...and adjust the size
  aslot.resize(size);

  DEBUGINFO2(this, "SizeConstraint::registerSlot(0x%x) -- end (#reg slots: %d)", long(&aslot), slots.size());
}

// unregisterSlot
void SizeConstraintBase::unregisterSlot(IArraySlot& aslot)
{
  DEBUGINFO1(this, "SizeConstraint::unregisterSlot(0x%x)", long(&aslot));

  // Search for the slot...
  std::vector<IArraySlot*>::iterator it = std::find(slots.begin(), slots.end(), &aslot);
  // ...and remove it (if it was there)
  if (it!=slots.end())
  {
    slots.erase(it);
  }
  else
  {
    throw EValueError("Could not unregister slot from size constraint.");
  }

  DEBUGINFO2(this, "SizeConstraint::unregisterSlot(0x%x) -- end (#reg slots: %d)", long(&aslot), slots.size());
}

/////////////// UserSizeConstraint ////////////////////

void UserSizeConstraint::setSize(int newsize)
{ 
  // Store the previous size so it can be restored if a resize 
  // operation failed
  int prevsize = size;

  size = newsize;
  try
  {
    execSlotResize(); 
  }
  catch(...)
  {
    // A registered slot could not resize, so restore the previous size
    // (which ought to work as those that have already been resized actually
    // *are* resizeable and the other ones haven't been resized anyway)
    size = prevsize;
    execSlotResize();
    throw;
  }
}

/////////////// LinearSizeConstraint ////////////////////

LinearSizeConstraint::LinearSizeConstraint(IArraySlot& actrl, int a, int b)
  : SizeConstraintBase(), Dependent(), ctrl(&actrl), a(a), b(b)
{
  ctrl->addDependent(this);
}

LinearSizeConstraint::~LinearSizeConstraint()
{
  DEBUGINFO(this, "LinearSizeConstraint::~LinearSizeConstraint()");
  if (ctrl!=0)
  {
    DEBUGINFO1(this, "  0x%x->removeDependent()", long(ctrl));
    ctrl->removeDependent(this);
  }
  DEBUGINFO(this, "LinearSizeConstraint::~LinearSizeConstraint() --- end");
}

// setCoeffs
void LinearSizeConstraint::setCoeffs(int newa, int newb)
{
  int preva = a;
  int prevb = b;
  a = newa;
  b = newb;
  try
  {
    onResize(ctrl->size());
  }
  catch(...)
  {
    a = preva;
    b = prevb;
    throw;
  }
}

// onResize
void LinearSizeConstraint::onResize(int newsize)
{ 
  // Store the previous size so it can be restored if a resize 
  // operation failed
  int prevsize = size;

  size = a*newsize+b; 
  try
  {
    execSlotResize(); 
  }
  catch(...)
  {
    // A registered slot could not resize, so restore the previous size
    // (which ought to work as those that have already been resized actually
    // *are* resizeable and the other ones haven't been resized anyway)
    size = prevsize;
    execSlotResize();
    throw;
  }
}

// queryResizeVeto
bool LinearSizeConstraint::queryResizeVeto(int size)
{
  std::vector<IArraySlot*>::iterator it;
  std::vector<IArraySlot*>::iterator it_end = slots.end();
  // Issue a veto if one of the registered slots cannot be resized...
  for(it=slots.begin(); it!=it_end; it++)
  {
    if (!(*it)->isResizable(size, true))
      return true;
  }
  return false;
}


}  // end of namespace
