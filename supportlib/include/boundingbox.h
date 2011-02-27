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

#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

/** \file boundingbox.h
 Contains the BoundingBox class.
 */

#include <iostream>
#include "vec3.h"
#include "mat4.h"

// Define the CGKIT_SHARED variable
#ifdef DLL_EXPORT_BOUNDINGBOX
  #include "shared_export.h"
#else
  #include "shared.h"
#endif


namespace support3d {


/**
  Axis aligned bounding box.

 */
class CGKIT_SHARED BoundingBox
{
  private:
  vec3d bmin;
  vec3d bmax;

  public:
  BoundingBox();
  BoundingBox(const vec3d& min, const vec3d& max);
  ~BoundingBox() {}

  void clear();
  /// Return true if the bounding box is empty.
  bool isEmpty() const { return bmin.x>bmax.x; }
  void getBounds(vec3d& min, vec3d& max) const;
  void getBounds(const vec3d& dir, vec3d& min, vec3d& max) const;
  void setBounds(const vec3d& min, const vec3d& max);
  vec3d center() const;
  void addPoint(const vec3d& p);
  void addBoundingBox(const BoundingBox& bb);
  void transform(const mat4d& M, BoundingBox& bb);
  vec3d clamp(const vec3d& p) const;
  void clamp(const vec3d& p, vec3d& target) const;
};

CGKIT_SHARED std::ostream& operator<<(std::ostream& os, const BoundingBox& bb);

}  // end of namespace

#endif
