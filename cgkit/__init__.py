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
# $Id: __init__.py,v 1.19 2006/02/14 19:29:39 mbaas Exp $

"""Python Computer Graphics Kit
============================

The Python Computer Graphics Kit is a generic 3D package that can be
useful in any domain where you have to deal with 3D data of any kind,
be it for visualization, creating photorealistic images, Virtual
Reality or even games.

At its lowest level, the package provides the basic functionality that
is useful for writing your own tools that process 3D data. For
example, the cgtypes module defines the fundamental types for computer
graphics such as vectors and matrices, the ri module contains the
complete RenderMan API to create RIB files, and so on.

Using these relatively low level modules the second level provides the
functionality to store a 3D scene in memory. This is the major part of
the package and this actually turns the general purpose language
Python into a specialized scripting language like MEL or MaxScript,
for example.

Eventually, the package provides small tools that let you actually see
your 3D scene. The two standard tools are for interactive rendering
using OpenGL and for offline rendering using a RenderMan renderer.

See the manual and tutorials at: http://cgkit.sourceforge.net/
"""

import os.path, cgkitinfo

# Is this cgkit light? Then modify the subpackage searchpath so that the
# special 'light' modules are found instead of the normal ones.
if cgkitinfo.cgkit_light:
    # Package's main folder
    dirname = __path__[0]
    # Put the 'light' subpackage in front so that its contents shadows
    # the normal extension modules...
    __path__.insert(0, os.path.join(dirname, "light"))


#__all__ = ["pluginmanager", "cmds", "preferences",
#           "scene", "component", "slots",
#           "worldobject",
#           "GUI"]

#import _core

#_core._set_debug_flag(True)

##import pluginmanager, cmds, preferences, eventmanager
##from eventmanager import eventManager
##from events import *
##from keydefs import *
##from application import getApp

##from cgtypes import vec3, vec4, mat3, mat4, quat, getEpsilon, setEpsilon, slerp, squad
##from scene import Scene, getScene
##from sceneglobals import Globals
##from worldobject import WorldObject
##from material import Material
##from glmaterial import GLMaterial, GLTexture, GL_DECAL, GL_REPLACE, GL_BLEND, GL_MODULATE, GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR, GL_CLAMP, GL_REPEAT, GL_RGB, GL_RGBA
##from lightsource import LightSource
##from component import Component, createFunctionComponent
##from slots import DoubleSlot, BoolSlot, IntSlot, Vec3Slot, Vec4Slot, Mat3Slot, Mat4Slot, QuatSlot, PySlot, slotPropertyCode, ProceduralIntSlot, ProceduralDoubleSlot, ProceduralVec3Slot, ProceduralVec4Slot, ProceduralMat3Slot, ProceduralMat4Slot, ProceduralQuatSlot, NotificationForwarder, UserSizeConstraint, LinearSizeConstraint
##from slots import Dependent
##from boundingbox import BoundingBox

### Geom objects:
##from spheregeom import SphereGeom
##from ccylindergeom import CCylinderGeom
##from torusgeom import TorusGeom
##from boxgeom import BoxGeom
##from planegeom import PlaneGeom
##from trimeshgeom import TriMeshGeom
##from polyhedrongeom import PolyhedronGeom
##from drawgeom import DrawGeom
##from beziercurvegeom import BezierCurveGeom, BezierPoint

### Geometry world objects:
##from quadrics import Sphere
##from ccylinder import CCylinder
##from torus import Torus
##from box import Box
##from plane import Plane
##from trimesh import TriMesh
##from polyhedron import Polyhedron
##from draw import Draw
##from ribarchive import RIBArchive

##from joint import Joint

### Dynamics world objects
##from odedynamics import ODEDynamics, ODEContactProperties, ODEBallJoint, ODEHingeJoint, ODESliderJoint, ODEHinge2Joint
##from joints import HingeJoint

### Camera/light
##from targetcamera import TargetCamera
##from freecamera import FreeCamera
##from lookat import LookAt
##from glpointlight import GLPointLight
##from glfreespotlight import GLFreeSpotLight
##from gltargetspotlight import GLTargetSpotLight
##from glfreedistantlight import GLFreeDistantLight
##from gltargetdistantlight import GLTargetDistantLight

##from spotlight3ds import SpotLight3DS
##from material3ds import Material3DS, TextureMap3DS
##from objmaterial import OBJMaterial, OBJTextureMap

##from camcontrol import CameraControl

##from group import Group
##from tunnel import Tunnel
##from flockofbirds import FlockOfBirds
##from valuetable import ValueTable
##from expression import Expression
##from euleradapter import EulerAdapter
##from pidcontroller import PIDController
##from gnuplotter import GnuPlotter
##from slideshow import SlideShow, Slide, XFade, XCube

##from glrenderer import GLRenderInstance

##from joystick import Joystick

##CONSTANT = _core.VarStorage.CONSTANT
##UNIFORM = _core.VarStorage.UNIFORM
##VARYING = _core.VarStorage.VARYING
##VERTEX = _core.VarStorage.VERTEX
##FACEVARYING = _core.VarStorage.FACEVARYING
##FACEVERTEX = _core.VarStorage.FACEVERTEX
##USER = _core.VarStorage.USER

##INT = _core.VarType.INT
##FLOAT = _core.VarType.FLOAT
##STRING = _core.VarType.STRING
##COLOR = _core.VarType.COLOR
##POINT = _core.VarType.POINT
##VECTOR = _core.VarType.VECTOR
##NORMAL = _core.VarType.NORMAL
##MATRIX = _core.VarType.MATRIX
##HPOINT = _core.VarType.HPOINT

### Importer
##import offimport
##import pyimport
##import dddsimport
##import x3dimport
##import ifsimport
##import objimport
##import stlimport
##import asfamcimport
##import bvhimport
##import maimport
##import plyimport
### Exporter
##import ribexport
##import offexport
##import objexport
##import plyexport

##from rmshader import RMMaterial, RMLightSource, RMShader

