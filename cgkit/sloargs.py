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
# Portions created by the Initial Developer are Copyright (C) 2008
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

"""This module allows using a RenderMan sloargs library.
"""

import sys, os, os.path
import ctypes
import ctypes.util
from _slreturntypes import _ShaderInfo, _ShaderParam
import rmanlibutil

# This is the name of the C library that gets loaded by the PRMan SloArgs
# implementation to obtain a function pointer to the free() function
# (which is only required for reading meta data).
# As of Python 2.6 this name should work cross-platform. In previous versions,
# the caller could modify this name to pick up the correct version (such
# as "msvcrt"). If the free() function couldn't be obtained, reading meta
# data is disabled.
# The variable can also be set to None to disable reading meta data altogether
# (in case there is a problem with it).
_libc_name = "c"

#################### ctypes type declarations #######################

class _POINT(ctypes.Structure):
    _fields_ = [("xval", ctypes.c_float),
                ("yval", ctypes.c_float),
                ("zval", ctypes.c_float)]

class _DEFAULTVAL(ctypes.Union):
    _fields_ = [("pointval", ctypes.POINTER(_POINT)),
                ("scalarval", ctypes.POINTER(ctypes.c_float)),
                ("stringval", ctypes.c_char_p),
                ("matrixval", ctypes.POINTER(16*ctypes.c_float))
               ]

# PRMan 14
class _VISSYMDEF_prman(ctypes.Structure):
    _fields_ = [("name", ctypes.c_char_p),
                ("type", ctypes.c_int),
                ("storage", ctypes.c_int),
                ("detail", ctypes.c_int),
                ("spacename", ctypes.c_char_p),
                ("default", _DEFAULTVAL),
                ("defaultval", _POINT),
                ("valisvalid", ctypes.c_uint),
                ("arraylen", ctypes.c_int),
               ]

# 3Delight 7.0
class _VISSYMDEF_3delight(ctypes.Structure):
    _fields_ = [("name", ctypes.c_char_p),
                ("type", ctypes.c_int),
                ("storage", ctypes.c_int),
                ("detail", ctypes.c_int),
                ("spacename", ctypes.c_char_p),
                ("default", _DEFAULTVAL),
                ("valisvalid", ctypes.c_uint),
                ("arraylen", ctypes.c_int),
               ]

# Aqsis
class _VISSYMDEF_aqsis(ctypes.Structure):
    _fields_ = [("name", ctypes.c_char_p),
                ("type", ctypes.c_int),
                ("storage", ctypes.c_int),
                ("detail", ctypes.c_int),
                ("spacename", ctypes.c_char_p),
                ("arraylen", ctypes.c_int),
                ("default", _DEFAULTVAL),
               ]

######################## Base class ##############################

class _SloArgs:
    """Provides functionality to read shader parameters using the C sloargs interface.

    Because of differences between renderers, the sloargs interface is not
    exposed directly. Instead, all information about a shader is returned
    by the getShaderInfo() method.
    
    This is the base class for the renderer-specific versions.
    """
    
    def __init__(self, libName, VISSYMDEF):
        """Constructor.
        
        libName is the name of the library that implements the sloargs interface.
        The name can either be just the base lib name or an absolute file name.
        VISSYMDEF is a ctype Structure object that describes the VISSYMDEF
        struct for this particular renderer.
        """
        
        self._VISSYMDEF = VISSYMDEF

        libName = rmanlibutil.resolveRManLib(libName)
        self.libName = libName
        
        # Load the library...
        try:
            self._sloargs = self._loadLibrary(libName)
        except:
            raise ValueError('Failed to load library "%s": %s'%(libName, sys.exc_info()[1]))
        try:
            self._declareFunctions(self._sloargs)
        except:
            raise ValueError('Library "%s" does not implement the sloargs interface: %s'%(libName, sys.exc_info()[1]))

    @staticmethod
    def defaultLibName():
        raise NotImplementedError("defaultLibName() must be implemented by derived classes")

    def _loadLibrary(self, libName):
        """Load the library providing the sloargs interface.
        
        libName is the (resolved) file name of the library to load.
        The return value is the ctypes library that must contain the
        sloargs functions as defined by PRMan.
        """
        sloargs = ctypes.cdll.LoadLibrary(libName)
        return sloargs
    
    def _declareFunctions(self, sloargs):
        """Declare the arguments and return types of the library functions.
        
        sloargs is the ctypes library as returned by LoadLibrary().
        This method can be overridden by derived classes to add renderer-specific
        functions. The derived class must call the inherited function.
        """
        VISSYMDEF = self._VISSYMDEF
        sloargs.Slo_SetPath.argtypes = [ctypes.c_char_p]
        sloargs.Slo_SetPath.restype = ctypes.c_int
        sloargs.Slo_SetShader.argtypes = [ctypes.c_char_p]
        sloargs.Slo_SetShader.restype = ctypes.c_int
        sloargs.Slo_GetName.argtypes = []
        sloargs.Slo_GetName.restype = ctypes.c_char_p
        sloargs.Slo_GetType.argtypes = []
        sloargs.Slo_GetType.restype = ctypes.c_int
        sloargs.Slo_GetNArgs.argtypes = []
        sloargs.Slo_GetNArgs.restype = ctypes.c_int
        sloargs.Slo_GetArgById.argtypes = [ctypes.c_int]
        sloargs.Slo_GetArgById.restype = ctypes.POINTER(VISSYMDEF)
        sloargs.Slo_GetArgByName.argtypes = [ctypes.c_char_p]
        sloargs.Slo_GetArgByName.restype = ctypes.POINTER(VISSYMDEF)
        sloargs.Slo_GetArrayArgElement.argtypes = [ctypes.POINTER(VISSYMDEF), ctypes.c_int]
        sloargs.Slo_GetArrayArgElement.restype = ctypes.POINTER(VISSYMDEF)
        sloargs.Slo_EndShader.argtypes = []
        sloargs.Slo_EndShader.restype = None
        sloargs.Slo_TypetoStr.argtypes = [ctypes.c_int]
        sloargs.Slo_TypetoStr.restype = ctypes.c_char_p
        sloargs.Slo_StortoStr.argtypes = [ctypes.c_int]
        sloargs.Slo_StortoStr.restype = ctypes.c_char_p
        sloargs.Slo_DetailtoStr.argtypes = [ctypes.c_int]
        sloargs.Slo_DetailtoStr.restype = ctypes.c_char_p
        
    def _getMetaData(self):
        """Return the meta data for the current shader.
        
        The function is called after Slo_SetShader() but before
        Slo_EndShader() has been called. The return value should
        be the meta data for the shader (preferably as a dict).
        """
        return None

    def getShaderInfo(self, shader):
        """Read the shader parameters from a given shader.
        """
        sloargs = self._sloargs
        
        # Split path and file name
        shaderPath,shaderFileName = os.path.split(shader)
        if shaderPath=="":
            shaderPath = "."
        # Remove the extension
        shader = os.path.splitext(shaderFileName)[0]

        sloargs.Slo_SetPath(shaderPath)
        # Try the shader name with extension first...
        if sloargs.Slo_SetShader(shaderFileName)!=0:
            # If the above failed, try the shader name without extension (prman)...
            if sloargs.Slo_SetShader(shader)!=0:
                raise IOError('Failed to open shader "%s"'%shader)
        
        shaderName = sloargs.Slo_GetName()
        shaderType = sloargs.Slo_TypetoStr(sloargs.Slo_GetType())
        metaData = self._getMetaData()
        params = []
        numParams = sloargs.Slo_GetNArgs()
        for i in range(1, numParams+1):
            res = sloargs.Slo_GetArgById(i)
            symdef = res.contents
            
            if sloargs.Slo_StortoStr(symdef.storage).startswith("output"):
                output = "output"
            else:
                output = ""
            storage = sloargs.Slo_DetailtoStr(symdef.detail)
            paramType = sloargs.Slo_TypetoStr(symdef.type)
            arrLen = symdef.arraylen
            if arrLen==0:
                arrLen = None
            name = symdef.name
            space = self._getSpace(symdef)
            if space=="":
                space = None
            defaultVal = self._getDefaultVal(symdef)
            params.append(_ShaderParam(output,storage,paramType,arrLen,name,space,defaultVal))
            
        sloargs.Slo_EndShader()
        return [_ShaderInfo(type=shaderType, name=shaderName, params=params, meta=metaData)]
        
    def _getSpace(self, symdef):
        if symdef.arraylen>0:
            res = []
            for i in range(symdef.arraylen):
                psd = self._sloargs.Slo_GetArrayArgElement(symdef, i)
                if bool(psd):
                    space = self._getSpace(psd.contents)
                    if space=="":
                        space = None
                else:
                    print >>sys.stderr, "Warning: Slo_GetArrayArgElement(%s) returned a NULL pointer"%i
                    space = None
                res.append(space)
            # Did we only get a list of None objects?
            # Then just return None (as this has probably been a type
            # that has no spaces associated with it)
            if res==len(res)*[None]:
                res = None
            return res
        else:
            return symdef.spacename
        
    def _getDefaultVal(self, symdef):
        if symdef.arraylen>0:
            res = []
            for i in range(symdef.arraylen):
                psd = self._sloargs.Slo_GetArrayArgElement(symdef, i)
                if bool(psd):
                    res.append(self._getDefaultVal(psd.contents))
                else:
                    print >>sys.stderr, "Warning: Slo_GetArrayArgElement(%s) returned a NULL pointer"%i
                    res.append(None)
            return res
        
        typ = self._sloargs.Slo_TypetoStr(symdef.type)
        # Scalar?
        if typ=="float":
            return symdef.default.scalarval.contents.value
        # Color, Point, Normal, Vector?
        elif typ in ["color", "point", "vector", "normal"]:
            v = symdef.default.pointval.contents
            return (v.xval, v.yval, v.zval)
        # String?
        elif typ=="string":
            return symdef.default.stringval
        # Matrix?
        elif typ=="matrix":
            return tuple(symdef.default.matrixval.contents)
        
        return None
        

############################# PRMan ##################################

class _SloArgs_PRMan(_SloArgs):
    def __init__(self, libName):
        global _libc_name
        
        _SloArgs.__init__(self, libName=libName, VISSYMDEF=_VISSYMDEF_prman)
        
        # Try to get a pointer to the C free() function (required for freeing
        # the result returned by getting the shader meta data).
        # We just try to load the "c" library (as of Python 2.6 this also works
        # on Windows).
        self._free = None
        if _libc_name is not None:
            libcName = ctypes.util.find_library(str(_libc_name))
            try:
                lib = ctypes.cdll.LoadLibrary(libcName)
                self._free = lib.free
                self._free.argtypes = [ctypes.c_void_p]
                self._free.restype = None
            except:
                pass

    @staticmethod
    def defaultLibName():
        return "prman"

    def _declareFunctions(self, sloargs):
        _SloArgs._declareFunctions(self, sloargs)
        # PRMan-specific functions
        sloargs.Slo_GetAllMetaData.argtypes = []
        sloargs.Slo_GetAllMetaData.restype = ctypes.POINTER(ctypes.c_char_p*1000)

    def _getMetaData(self):
        """Return the shader meta data.
        """
        # If we couldn't obtain the free() function in the constructor we
        # just return an empty dict (otherwise we would have to create a memory
        # leak further down).
        if self._free is None:
            return {}
        
        res = {}
        data = self._sloargs.Slo_GetAllMetaData()
        i = 0
        while data.contents[i]!=None:
            key = data.contents[i]
            val = data.contents[i+1]
            res[key] = val
            i += 2

        self._free(data)
        return res

############################# 3Delight ##################################

class _SloArgs_3Delight(_SloArgs):
    def __init__(self, libName):
        _SloArgs.__init__(self, libName=libName, VISSYMDEF=_VISSYMDEF_3delight)

    @staticmethod
    def defaultLibName():
        return "3delight"

    def _declareFunctions(self, sloargs):
        _SloArgs._declareFunctions(self, sloargs)
        # 3Delight-specific functions
        sloargs.Slo_GetNAnnotations.argtypes = []
        sloargs.Slo_GetNAnnotations.restype = ctypes.c_int
        sloargs.Slo_GetAnnotationKeyById.argtypes = [ctypes.c_int]
        sloargs.Slo_GetAnnotationKeyById.restype = ctypes.c_char_p
        sloargs.Slo_GetAnnotationByKey.argtypes = [ctypes.c_char_p]
        sloargs.Slo_GetAnnotationByKey.restype = ctypes.c_char_p
    
    def _getMetaData(self):
        res = {}
        n = self._sloargs.Slo_GetNAnnotations()
        for i in range(1, n+1):
            key = self._sloargs.Slo_GetAnnotationKeyById(i)
            if key is None:
                continue
            ann = self._sloargs.Slo_GetAnnotationByKey(key)
            if ann is None:
                continue
            res[key] = ann
        return res

############################# Aqsis ##################################

class _SloArgs_Aqsis(_SloArgs):
    def __init__(self, libName):
        _SloArgs.__init__(self, libName=libName, VISSYMDEF=_VISSYMDEF_aqsis)

    @staticmethod
    def defaultLibName():
        return "slxargs"


############################# Pixie ##################################

class _UDefaultVal(ctypes.Union):
    pass
_UDefaultVal._fields_ = [("matrix", ctypes.POINTER(16*ctypes.c_float)),
                         ("vector", ctypes.POINTER(3*ctypes.c_float)),
                         ("scalar", ctypes.c_float),
                         ("string", ctypes.c_char_p),
                         ("array", ctypes.POINTER(_UDefaultVal))
                        ]

class _TSdrParameter(ctypes.Structure):
    pass
_TSdrParameter._fields_ = [("name", ctypes.c_char_p),
                           ("type", ctypes.c_int),
                           ("container", ctypes.c_int),
                           ("writable", ctypes.c_int),
                           ("numItems", ctypes.c_int),
                           ("space", ctypes.c_char_p),
                           ("defaultValue", _UDefaultVal),
                           ("next", ctypes.POINTER(_TSdrParameter))
                          ]

class _TSdrShader(ctypes.Structure):
    _fields_ = [("name", ctypes.c_char_p),
                ("type", ctypes.c_int),
                ("parameters", ctypes.POINTER(_TSdrParameter))
               ]

class _SloArgs_Pixie(_SloArgs):
    """Pixie version.
    
    This class does derive from _SloArgs but doesn't use the sloargs interface
    as Pixie has its own interface.
    """
    
    def __init__(self, libName):
        _SloArgs.__init__(self, libName=libName, VISSYMDEF=None)
        self._shaderType = {0:"surface", 1:"displacement", 2:"volume", 3:"light", 4:"imager"}
        self._container = {0:"constant", 1:"uniform", 2:"varying", 3:"vertex"}
        self._paramType = {0:"float", 1:"vector", 2:"normal", 3:"point", 4:"color", 5:"matrix", 6:"string"}

    @staticmethod
    def defaultLibName():
        return "sdr"

    def _declareFunctions(self, sloargs):
        # We don't call the inherited method as Pixie's library does not
        # provide the same interface than PRMan
        
        sloargs.sdrGet.argtypes = [ctypes.c_char_p]
        sloargs.sdrGet.restype = ctypes.POINTER(_TSdrShader)
        sloargs.sdrDelete.argtypes = [ctypes.POINTER(_TSdrShader)]

    def getShaderInfo(self, shader):
        sdr = self._sloargs
        shaderInfo = sdr.sdrGet(shader)
        if not shaderInfo:
            raise IOError('Failed to open shader "%s"'%shader)
        
        try:
            # Strip path and extension (as Pixie doesn't return the shader name but the file name)
            shaderName = os.path.splitext(os.path.split(shaderInfo.contents.name)[1])[0]
            shaderType = self._shaderType.get(shaderInfo.contents.type, "?")
            params = []
            param = shaderInfo.contents.parameters
            while param:
                param = param.contents
                
                if param.writable:
                    output = "output"
                else:
                    output = ""
                    
                storage = self._container.get(param.container, "?")
                paramType = self._paramType.get(param.type, "?")
                arrLen = param.numItems
                if arrLen==1:
                    arrLen = None
                name = param.name
                space = param.space
                # Set space to "current" if Pixie didn't return a space name for a type that requires a space
                if paramType in ["point", "vector", "normal", "color", "matrix"] and space is None:
                    space = "current"
                if arrLen is None:
                    defaultVal = self._getDefaultVal(paramType, param.defaultValue)
                else:
                    defaultVal = []
                    for i in range(arrLen):
                        defaultVal.append(self._getDefaultVal(paramType, param.defaultValue.array[i]))
                
                params.append((output,storage,paramType,arrLen,name,space,defaultVal))
                param = param.next
        finally:
            sdr.sdrDelete(shaderInfo)
          
        # Pixie reports the parameters in reverse order, so reverse the list
        params.reverse()
        return [(shaderType, shaderName, params)]  

    def _getDefaultVal(self, paramType, defval):
        if paramType=="float":
            return defval.scalar
        elif paramType in ["vector", "normal", "point", "color"]:
            return tuple(defval.vector.contents)
        elif paramType=="string":
            defaultVal = defval.string
        elif paramType=="matrix":
            return tuple(defval.matrix.contents)
        return None

############################################################################

# The registered SloArgs classes.
# Key: Compiled Shader Suffix - Value: SloArgs class
_sloArgsClasses = {}

# The already instantiated SloArgs objects (so that they can be reused).
# Key: Compiled Shader Suffix - Value: SloArgs instance
_sloArgsInstances = {}

# The names of the libraries that should be used for reading the parameters
# Key: Compiled Shader Suffix - Value: libName
_sloLibNames = {}

def registerSloArgs(sloSuffix, sloArgsCls):
    """Register a :class:`SloArgs` class for a particular renderer.
    
    *sloSuffix* is the suffix (without dot) that is used for the compiled
    shaders of this renderer. *sloArgsCls* is the :class:`SloArgs` class that
    can read compiled shaders for this renderer.
    """
    global _sloArgsClasses
    
    sloSuffix = sloSuffix.lower()
    _sloArgsClasses[sloSuffix] = sloArgsCls

def getSloLib(sloSuffix):
    """Return the library name that manages a particular type of compiled shaders.
    
    *sloSuffix* is the suffix of the compiled shader.
    The return value is the library name that is used to read the parameters
    from compiled shaders of the given suffix.
    
    When called before a shader was parsed, the return value is the
    library name that will be used when the library is loaded. This value can be
    set by calling :func:`setSloLib()`. When the function is called after a
    shader was already parsed, the return value is the absolute path to the
    actual library that is used to read parameters.
    """
    global _sloArgsClasses, _sloArgsInstances
    
    sloSuffix = sloSuffix.lower()
    if sloSuffix in _sloArgsInstances:
        return _sloArgsInstances[sloSuffix].libName
    elif sloSuffix in _sloArgsClasses:
        return _sloArgsClasses[sloSuffix].defaultLibName()
    else:
        raise ValueError, "Unknown compiled shader extension: %s"%sloSuffix
    
def setSloLib(sloSuffix, libName):
    """Set the library name that should be used for reading shader parameters.
    
    Shaders with the suffix *sloSuffix* (case-insensitive) will be handled by
    the library *libName*. This function has no effect if a shader of the given
    suffix has already been read. You must call this function at the beginning
    of your application when :func:`slparams()` hasn't been called yet.
    """
    global _sloLibNames
    
    _sloLibNames[sloSuffix.lower()] = libName

def slparams(shader):
    """Read shader parameters.
    
    See :func:`slparams.slparams()<cgkit.slparams.slparams>` for more details.
    """
    global _sloArgsClasses, _sloArgsInstances, _sloLibNames
    
    if not os.path.exists(shader):
        raise IOError('Shader file "%s" does not exist'%shader)

    # Get the SloArgs class for the renderer that produced the compiled
    # shader (based on the shader suffix).
    shaderBase,ext = os.path.splitext(shader)
    ext = ext.lower()[1:]
    if ext in _sloArgsInstances:
        sloArgs = _sloArgsInstances[ext]
    elif ext in _sloArgsClasses:
        SloArgs = _sloArgsClasses[ext]
        libName = _sloLibNames.get(ext, SloArgs.defaultLibName())
        sloArgs = SloArgs(libName=libName)
        _sloArgsInstances[ext] = sloArgs
    else:
        raise ValueError, "Unknown compiled shader extension: %s"%shader
    
    return sloArgs.getShaderInfo(shader) 


# Register the built-in SloArgs classes
registerSloArgs("slo", _SloArgs_PRMan)
registerSloArgs("sdl", _SloArgs_3Delight)
registerSloArgs("slx", _SloArgs_Aqsis)
registerSloArgs("sdr", _SloArgs_Pixie)
        
### TEST ###
if __name__=="__main__":
    import cgkit.slparams
    
    if 0:
        info = cgkit.slparams.slparams("foo.sl")
        if len(info)>0:
            type,name,params = info[0]
            print "Shader:",name
            print "Type  :",type
            for param in params:
                print param, cgkit.slparams.convertdefault(param)
    
    info = slparams("foo.sdr")
    if len(info)>0:
        type,name,params = info[0]
        print "Shader:",name
        print "Type  :",type
        for param in params:
            print param
