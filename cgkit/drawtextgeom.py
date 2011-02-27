# ***** BEGIN LICENSE BLOCK *****
# Version: MPL 1.1/GPL 2.0/LGPL 2.1
#
# The contents of this file are subject to the Mozilla Public License Version
# 1.1 (the "License"); you may not use this file except in compliance with
# the License. You may obtain a copy of the License at
# http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# The Original Code is the Python Computer Graphics Kit.
#
# The Initial Developer of the Original Code is Matthias Baas.
# Portions created by the Initial Developer are Copyright (C) 2004
# the Initial Developer. All Rights Reserved.
#
# Contributor(s):
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 2 or later (the "GPL"), or
# the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
# in which case the provisions of the GPL or the LGPL are applicable instead
# of those above. If you wish to allow use of your version of this file only
# under the terms of either the GPL or the LGPL, and not to allow others to
# use your version of this file under the terms of the MPL, indicate your
# decision by deleting the provisions above and replace them with the notice
# and other provisions required by the GPL or the LGPL. If you do not delete
# the provisions above, a recipient may use your version of this file under
# the terms of any one of the MPL, the GPL or the LGPL.
#
# ***** END LICENSE BLOCK *****
# $Id: drawtextgeom.py,v 1.2 2005/08/28 19:42:43 mbaas Exp $

import sys
from cgkit.geomobject import GeomObject
from cgkit.boundingbox import BoundingBox
from cgkit.cgtypes import *
from _OpenGL.GL import *
try:
    from OpenGL.GLUT import *
    has_glut = True
except:
    has_glut = False
    

# DrawTextGeom
class DrawTextGeom(GeomObject):
    """Geometry object that draw text strings.

    This is similar to the C++ Draw class, except that it only draws
    texts using GLUT. This functionality isn't incorporated into the
    C++ class to prevent a dependency from GLUT. Whereas using Python
    you can still use everything else from cgkit if GLUT isn't available
    (in this case, a warning will be printed and the texts are simply not
    drawn).
    """

    def __init__(self):
        """Constructor.
        """
        global has_glut
        GeomObject.__init__(self)
        
        self.text = []
        self.boundingbox = BoundingBox()

        if not has_glut:
            print >>sys.stderr, "WARNING: Cannot draw texts. GLUT is not available."

    def uniformCount(self):
        return 0

    def varyingCount(self):
        return 0

    def vertexCount(self):
        return 0

    def boundingBox(self):
        return self.boundingbox

    def drawGL(self):
        global has_glut
        if not has_glut:
            return
        
        glPushAttrib(GL_ALL_ATTRIB_BITS)
        glDisable(GL_LIGHTING)
#        glDisable(GL_DEPTH_TEST)

        for pos,txt,font,col in self.text:
            x,y,z = pos
            r,g,b = col
            glColor3d(r,g,b)
            glRasterPos3d(x,y,z)
            self._drawText(txt, font)

        glPopAttrib()

    # clear
    def clear(self):
        """Clear all texts."""
        self.text = []
        self.boundingbox = BoundingBox()
        
    # addText
    def addText(self, pos, txt, font=None, color=(1,1,1)):
        """Add a text string.

        pos contains the 3D position of the string, txt is a string
        containing the actual text. font can be one of the constants
        defined in GLUT:

        - GLUT_BITMAP_8_BY_13
        - GLUT_BITMAP_9_BY_15
        - GLUT_BITMAP_TIMES_ROMAN_10
        - GLUT_BITMAP_TIMES_ROMAN_24
        - GLUT_BITMAP_HELVETICA_10
        - GLUT_BITMAP_HELVETICA_12
        - GLUT_BITMAP_HELVETICA_18

        color is the color of the text.
        """
        if font==None:
            try:
                font = GLUT_BITMAP_9_BY_15
            except:
                font = 0
        self.text.append((pos,txt,font,color))
        self.boundingbox.addPoint(vec3(pos))


    def _drawText(self, txt, font):
        for c in txt:
            glutBitmapCharacter(font, ord(c))
