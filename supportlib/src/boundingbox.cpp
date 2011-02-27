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

#define DLL_EXPORT_BOUNDINGBOX
#include "boundingbox.h"

template<class T>
const T& xmin(const T& x, const T& y)
{
  return (x<=y)? x : y;
}

template<class T>
const T& xmax(const T& x, const T& y)
{
  return (x>y)? x : y;
}


namespace support3d {

BoundingBox::BoundingBox()
  : bmin(0,0,0), bmax(-1,0,0)
{
}

BoundingBox::BoundingBox(const vec3d& min, const vec3d& max)
  : bmin(0,0,0), bmax(-1,0,0)
{
  setBounds(min, max);
}

/**
  Make the bounding box empty.  
 */
void BoundingBox::clear()
{
  bmin.set(0,0,0);
  bmax.set(-1,0,0);
}

/**
  Return the minimum and maximum bound.

  Returns two identical points if the bounding box is empty.

  \param[out] min Minimum bound.
  \param[out] max Maximum bound.
 */
void BoundingBox::getBounds(vec3d& min, vec3d& max) const 
{ 
  if (isEmpty())
  {
    min = min;
    max = min;
  }
  else
  {
    min = bmin;
    max = bmax; 
  }
}

/**
  Return the minimum and maximum bound along a direction.

  Returns two opposite corners of the bounding box. Which corners
  these actually are is defined by \a dir. \a min will always be
  before \a max when sorted along a ray with the given direction.
  If you negate \ dir the values of \a min and \a max will just be 
  swapped.

  Returns two identical points if the bounding box is empty.

  \param dir Direction
  \param[out] min Minimum bound along dir
  \param[out] max Maximum bound along dir
 */
void BoundingBox::getBounds(const vec3d& dir, vec3d& min, vec3d& max) const 
{ 
  if (isEmpty())
  {
    min = min;
    max = min;
  }
  else
  {
    if (dir.x<0)
    {
      min.x = bmax.x;
      max.x = bmin.x;
    }
    else
    {
      min.x = bmin.x;
      max.x = bmax.x;
    }

    if (dir.y<0)
    {
      min.y = bmax.y;
      max.y = bmin.y;
    }
    else
    {
      min.y = bmin.y;
      max.y = bmax.y;
    }

    if (dir.z<0)
    {
      min.z = bmax.z;
      max.z = bmin.z;
    }
    else
    {
      min.z = bmin.z;
      max.z = bmax.z;
    }
  }
}

/**
  Set new bounds for the bounding box. 
  
  The rectangle given by b1 and b2 defines the new bounding box.

  \param min 1st bound
  \param max 2nd bound
 */
void BoundingBox::setBounds(const vec3d& min, const vec3d& max)
{
  bmin.set(xmin(min.x, max.x), xmin(min.y, max.y), xmin(min.z, max.z));
  bmax.set(xmax(min.x, max.x), xmax(min.y, max.y), xmax(min.z, max.z));
}

/**
  Return the center of the bounding box.

  0 is returned if the bounding box is empty.

  \return Center of the bounding box, i.e. 0.5*(bmin+bmax)
 */
vec3d BoundingBox::center() const
{
  if (isEmpty())
    return vec3d(0);
  else
    return 0.5*(bmin+bmax);
}

/**
   Enlarge the bounding box so that the point p is enclosed in the box.

  \param p The point that should be enclosed in the box.
 */
void BoundingBox::addPoint(const vec3d& p)
{
  if (isEmpty())
  {
    bmin = p;
    bmax = p;
  }
  else
  {
    bmin.set(xmin(bmin.x, p.x), xmin(bmin.y, p.y), xmin(bmin.z, p.z));
    bmax.set(xmax(bmax.x, p.x), xmax(bmax.y, p.y), xmax(bmax.z, p.z));
  }
}

/**
  Enlarge the bounding box so that bb is enclosed in the box.

  \param bb The bounding box that should be enclosed in this box.
 */
void BoundingBox::addBoundingBox(const BoundingBox& bb)
{
  if (bb.isEmpty())
    return;

  addPoint(bb.bmin);
  addPoint(bb.bmax);
}

/**
	Returns a transformed bounding box. 
  
  The transformation is given by M. The result will still be axis aligned, 
  so the volume will not be preserved.

  The bounding box on which this method is called remains unchanged.
  It is allowed to pass \a *this as \a bb.

  \param M Transformation matrix
  \param[out] bb  Result.
 */
void BoundingBox::transform(const mat4d& M, BoundingBox& bb)
{
  double x1,y1,z1;
  double x2,y2,z2;
  if (!isEmpty())
  {
    bmin.get(x1, y1, z1);
    bmax.get(x2, y2, z2);
    bb.clear();
    bb.addPoint( M*vec3d(x1,y1,z1) );
    bb.addPoint( M*vec3d(x1,y1,z2) );
    bb.addPoint( M*vec3d(x1,y2,z1) );
    bb.addPoint( M*vec3d(x1,y2,z2) );
    bb.addPoint( M*vec3d(x2,y1,z1) );
    bb.addPoint( M*vec3d(x2,y1,z2) );
    bb.addPoint( M*vec3d(x2,y2,z1) );
    bb.addPoint( M*vec3d(x2,y2,z2) );
  }
  else
  {
    bb.clear();
  }
}

/**
   Clamp a point so that it lies within the bounding box.

   Each axis is clamped against the corresponding bounding box interval.

   \param p The point to clamp
   \return The clamped point
 */
vec3d BoundingBox::clamp(const vec3d& p) const
{
  vec3d res;
  clamp(p, res);
  return res;
}

/**
   Clamp a point so that it lies within the bounding box.

   Each axis is clamped against the corresponding bounding box interval.

   \param p The point to clamp
   \param[out] target The clamped point (may be the same reference as p)
 */
void BoundingBox::clamp(const vec3d& p, vec3d& target) const
{
  target.x = xmin(xmax(bmin.x, p.x), bmax.x);
  target.y = xmin(xmax(bmin.y, p.y), bmax.y);
  target.z = xmin(xmax(bmin.z, p.z), bmax.z);
}

std::ostream& operator<<(std::ostream& os, const BoundingBox& bb)
{
  os<<"<BoundingBox: ";
  if (bb.isEmpty())
  {
    os<<"empty>";
  }
  else
  {
    vec3d bmin, bmax;
    bb.getBounds(bmin, bmax);
    os<<bmin<<" - "<<bmax<<">";
  }
  return os;
}

}  // end of namespace
