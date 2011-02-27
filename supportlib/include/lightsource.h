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

#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

/** \file lightsource.h
 Contains the LightSource base class.
 */

#include "worldobject.h"

namespace support3d {

/**
  This is the base class for all light sources.

 */
class LightSource : public WorldObject
{
  public:
  /// Enabled state
  Slot<bool> enabled;

  /** Enable or disable shadows.

     It's up to the actual light source implementation and the renderer
     whether the light source can cast shadows at all. So this flag might 
     get ignored by an actual implementation.
   */
  Slot<bool> cast_shadow;

  /// Light intensity
  Slot<double> intensity;

  public:
  LightSource() : WorldObject(), enabled(true, 0), cast_shadow(false, 0), intensity(1.0, 0)
  {
    addSlot("enabled", enabled);
    addSlot("cast_shadow", cast_shadow);
    addSlot("intensity", intensity);
  }
  LightSource(string aname) : WorldObject(aname), enabled(true, 0), cast_shadow(false, 0), intensity(1.0, 0)
  {
    addSlot("enabled", enabled);
    addSlot("cast_shadow", cast_shadow);
    addSlot("intensity", intensity);
  }

  /**
    Check if a particular object casts shadows.

    \return True if \a obj is supposed to block light from this light source.
   */
  bool isShadowCaster(boost::shared_ptr<WorldObject> obj) const
  { 
    // Dummy implementation that checks if obj has a bool slot "cast_shadows"
    // and returns the value of that slot. If no valid slot is available
    // the function returns true.
    try
    {
      ISlot* slot = &(obj->slot("cast_shadows"));
      Slot<bool>* bslot = dynamic_cast<Slot<bool>*>(slot);
      if (bslot!=0)
      {
	return bslot->getValue();
      }
      else
      {
	return true;
      }
    }
    catch(EKeyError&)
    {
      return true;
    }
  }

  /**
    Check if a particular object receives shadows.

    \return True if \a obj is supposed to receive shadows from this light source.
   */
  bool isShadowReceiver(boost::shared_ptr<WorldObject> obj) const 
  { 
    // Dummy implementation that checks if obj has a bool slot "receive_shadows"
    // and returns the value of that slot. If no valid slot is available
    // the function returns true.
    try
    {
      ISlot* slot = &(obj->slot("receive_shadows"));
      Slot<bool>* bslot = dynamic_cast<Slot<bool>*>(slot);
      if (bslot!=0)
      {
	return bslot->getValue();
      }
      else
      {
	return true;
      }
    }
    catch(EKeyError&)
    {
      return true;
    }
  }
};


}  // end of namespace

#endif
