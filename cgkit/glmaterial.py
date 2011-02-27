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
# $Id: glmaterial.py,v 1.6 2006/04/05 08:17:32 mbaas Exp $

## \file glmaterial.py
## Contains the GLMaterial class.

import _core
from cgtypes import *
from _OpenGL.GL import *
from _OpenGL.GLU import *
import math
import sys, os, os.path
import _Image as Image
import glslangparams
from slots import *

GLSLANG_VERTEX = _core.GLShader.ShaderType.VERTEX
GLSLANG_FRAGMENT = _core.GLShader.ShaderType.FRAGMENT

# GLTexture
class GLTexture(_core.GLTexture):
    def __init__(self,
                 imagename = "",
                 image = None,
                 mode = GL_DECAL,
                 mipmap = True,
                 mag_filter = GL_LINEAR,
                 min_filter = GL_LINEAR,
                 wrap_s = GL_REPEAT,
                 wrap_t = GL_REPEAT,
                 internalformat = GL_RGB,
                 texenvcolor = vec4(1),
                 transform = mat4(1),
                 size = None,
                 environment_map = False):
        """Constructor.
        """
        
        _core.GLTexture.__init__(self)
        self.imagename = imagename
        self.mode = mode
        self.mag_filter = mag_filter
        if mipmap and min_filter==GL_LINEAR:
            min_filter = GL_LINEAR_MIPMAP_LINEAR
        self.min_filter = min_filter
        self.wrap_s = wrap_s
        self.wrap_t = wrap_t
        self.texenvcolor = vec4(texenvcolor)
        self.transform = transform
        self.environment_map = environment_map
        self.mipmap = mipmap
        self.internalformat = internalformat
        self.size = size

        self._image = image
        
        # Remember the current working directory
        self._path = os.getcwd()

    # loadTexData
    def loadTexData(self):
        """Load and apply texture file.
        """
        # No image data set? Then use the file name and load the image...
        if self.image==None:
            if self.imagename=="":
                return
            print 'Loading "%s"...'%self.imagename,
            fullname = os.path.join(self._path, self.imagename)
            try:
                img = Image.open(fullname)
                self._passPILImage(img)
            except IOError, e:
                print "failed"
                print e
                return
        else:
            data = self.image
            if type(data)==str:
                # Raw image data as string
                w,h = self.size
                format = self.internalformat
                self.texData(w, h, format, GL_UNSIGNED_BYTE, data)
            else:
                # PIL image
                self._passPILImage(data)


    ## protected:

    def _passPILImage(self, img):
        """Pass a PIL image back to OpenGL.

        img must be a PIL image.
        The image is scaled to a power of 2 if necessary.
        """
        img = self._fitPILImage(img)
        
        w,h = img.size
        
        if img.mode=="RGB":
            format = GL_RGB
        elif img.mode=="RGBA":
            format = GL_RGBA
        else:
            raise RuntimeError, "Unsupported texture format (%s)"%img.mode
        
        self.texData(w, h, format, GL_UNSIGNED_BYTE, img.tostring())
        

    def _fitPILImage(self, img):
        """Scale the image to a power of 2 resolution.

        The image is scale to the user specified resolution. If no
        resolution was set, the next higher power of 2 resolution is used.
        
        If the image resolution is already a power of 2 then
        the input image is returned.
        """
        w,h = img.size
        if self.size==None:
            w2 = 2**int(math.ceil(math.log(w)/math.log(2)))
            h2 = 2**int(math.ceil(math.log(h)/math.log(2)))
        else:
            w2,h2 = self.size
            
        if w2!=w or h2!=h:
            img = img.resize((w2,h2), Image.BICUBIC)

        return img
        
        
    # "image" property...
    
    def _getImage(self):
        """Return the current image.

        This method is used for retrieving the \a image property.

        \return PIL Image (or None)
        """
        return self._image

    def _setImage(self, img):
        """Set a new texture image.

        This method is used for setting the \a image property.

        \param image PIL image or None
        """
        self._image = img
        # Assign the name again so that the image data will be reloaded
        self.imagename = self.imagename

    image = property(_getImage, _setImage, None, "Texture image")

# GLShader
class GLShader(_core.GLShader):
    """OpenGL 2 shader source file.
    
    """

    def __init__(self,
                 shadertype,
                 filename,
                 cpp = None,
                 cpperrstream = sys.stderr,
                 **shaderparams):
        """Constructor.
        """
        _core.GLShader.__init__(self, shadertype, filename)

        # Extract the shader parameters...
        uniform,attribute,varying = glslangparams.glslangparams(filename, cpp=cpp, cpperrstream=cpperrstream)

        self._uniform = {}
        for type,name,arraysize,structname,struct in uniform:
            if type=="struct":
                print "Warning: structs are not yet supported"
                continue
            self._uniform[name] = (type,name,arraysize,structname,struct)

        for paramname in shaderparams:
            setattr(self, paramname, shaderparams[paramname])

    def __str__(self):
        if self.type==GLSLANG_VERTEX:
            stype = "vertex"
        else:
            stype = "fragment"
        return "<%s shader '%s'>"%(stype, os.path.basename(self.filename))

    # getattr
    def __getattr__(self, attr):
        """Return slot value or None if the slot wasn't created yet.

        An attempt an unknwon attribute (i.e. no uniform var) results
        in an error.
        """
        slot = self.__dict__.get("%s_slot"%attr, None)
        if slot!=None:
            return slot.getValue()

        if attr in self._uniform:
            return None

        raise AttributeError, "shader '%s' has no parameter '%s'"%(self.filename, attr)

    # setattr
    def __setattr__(self, attr, value):
        if attr in ["_uniform"] or attr[-5:]=="_slot":
            _core.GLShader.__setattr__(self, attr, value)
            return

        # Is attr a slot variable?
        slot = self.__dict__.get("%s_slot"%attr, None)
        if slot!=None:
            slot.setValue(value)
            return 

        # Is it a parameter that has no value yet? then we must create
        # a new slot...
        uniform = getattr(self, "_uniform", [])
        if attr in uniform:
            slot = self._createSlot(attr, uniform[attr][0])
            slot.setValue(value)
        else:
            raise AttributeError, "shader '%s' has no parameter '%s'"%(self.filename, attr)

    # iterUniform
    def iterUniform(self):
        return self._uniform.itervalues()

    # _createSlot
    def _createSlot(self, param, type):
        """Create a slot for a parameter.

        param is the name of the parameter and type its OpenGL type (as str).
        """
        slot_lut = {"bool" : "BoolSlot",
                    "float" : "DoubleSlot",
                    "int" : "IntSlot",
                    "bvec2" : "Vec3Slot",
                    "bvec3" : "Vec3Slot",
                    "bvec4" : "Vec4Slot",
                    "ivec2" : "Vec3Slot",
                    "ivec3" : "Vec3Slot",
                    "bvec4" : "Vec4Slot",
                    "vec2" : "Vec3Slot",
                    "vec3" : "Vec3Slot",
                    "vec4" : "Vec4Slot",
                    "mat2" : "Mat3Slot",
                    "mat3" : "Mat3Slot",
                    "mat4" : "Mat4Slot",
                    "sampler1D" : "IntSlot",
                    "sampler2D" : "IntSlot",
                    "sampler3D" : "IntSlot",
                    "samplerCube" : "IntSlot",
                    "sampler1DShadow" : "IntSlot",
                    "sampler2DShadow" : "IntSlot"}
        slotname = slot_lut.get(type, None)
        if slotname==None:
            raise ValueError, "Invalid parameter type: %s"%type
        exec "slot = %s()"%slotname
        self.addSlot(param, slot)
        # Store the slot as <param>_slot 
        setattr(self, "%s_slot"%param, slot)
        return slot

        
# GLMaterial
class GLMaterial(_core.GLMaterial):

    def __init__(self,
                 name = "GLMaterial",
                 ambient = (0.2, 0.2, 0.2, 1),
                 diffuse = (0.7, 0.7, 0.7, 1),
                 specular = (0,0,0,1),
                 shininess = 0.0,
                 emission = (0,0,0,1),
                 blend_factors = None,
                 texture = None,
                 vertex_shader = None,
                 fragment_shader = None,
                 density=1.0):
        _core.GLMaterial.__init__(self, name, density)

        self.ambient = vec4(ambient)
        self.diffuse = vec4(diffuse)
        self.specular = vec4(specular)
        self.shininess = shininess
        self.emission = vec4(emission)
        if texture!=None:
            if isinstance(texture, _core.GLTexture):
                texture = [texture]
            self.setNumTextures(len(texture))
            for i,tex in enumerate(texture):
                self.setTexture(tex,i)
        if vertex_shader!=None:
            if isinstance(vertex_shader, _core.GLShader):
                vertex_shader = [vertex_shader]
            self.setNumVertexShaders(len(vertex_shader))
            for i,shd in enumerate(vertex_shader):
                self.setVertexShader(shd,i)
        if fragment_shader!=None:
            if isinstance(fragment_shader, _core.GLShader):
                fragment_shader = [fragment_shader]
            self.setNumFragmentShaders(len(fragment_shader))
            for i,shd in enumerate(fragment_shader):
                self.setFragmentShader(shd,i)
        if blend_factors!=None:
            self.blend_sfactor = blend_factors[0]
            self.blend_dfactor = blend_factors[1]
