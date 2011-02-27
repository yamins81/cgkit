#!/usr/bin/env python
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

from distutils.core import setup, Extension
from distutils.sysconfig import get_python_lib, get_config_vars
import sys, shutil, os, os.path, time, glob, stat

########################## Functions #################################

# isNewer
def isNewer(file1, file2):
    """Check if file1 is newer than file2.

    Returns True if file1 is newer than file2 or if file2 doesn't exist.
    file1 must be an existing file.
    """
    if not os.path.exists(file2):
        return True
    return os.stat(file1)[stat.ST_MTIME]>os.stat(file2)[stat.ST_MTIME]

# findFile
def findFile(filename, dirs):
    """Search a file in a list of directories.

    Returns the full file name or throws an exception.
    """
    for dir in dirs:
        fullname = os.path.abspath(os.path.join(dir, filename))
        if os.path.isfile(fullname):
            return fullname

    msg = 'Error: File "%s" not found in:\n'%filename
    for dir in dirs:
        msg += '  %s\n'%dir
    print (msg)
    sys.exit(1)

# packageDLL
def packageDLL(dll):
    """Include a DLL into the package.

    The DLL is copied and will be made available in the package directory.

    dll: Full name of the DLL.
    """
    global data_files
    dllname = os.path.basename(dll)
    # Copy the DLL (only if the source is newer than the dest)...
    dstdll = os.path.join(PACKAGE_NAME, dllname)
    if (not os.path.exists(dstdll)) or (os.path.getmtime(dstdll)<os.path.getmtime(dll)):
        print ('Copying DLL %s...'%dll)
        shutil.copyfile(dll, dstdll)
    # Add it to the list of data files...
    dest = os.path.join(get_python_lib(plat_specific=True), PACKAGE_NAME)
    n = len(os.path.commonprefix([sys.exec_prefix, dest]))
    dest = dest[n+1:]
    data_files += [(dest, [dll])]

# enabledStr
def enabledStr(flag):
    if flag:
        return "enabled"
    else:
        return "disabled"

# updateInfoModule
def updateInfoModule(cgkit_light):
    """Updates the module cgkitinfo.

    Updates the info module so that the version string contains the
    current date and time.
    """

    print ("Updating version in the cgkitinfo module...")
    template = os.path.join("cgkit", "cgkitinfo_template.txt")
    infomod = os.path.join("cgkit", "cgkitinfo.py")

    # Read the cgkitinfo template module
    try:
        f = open(template, "rt")
        lines = f.readlines()
        f.close()
    except:
        print ('Error: Could not read file "%s"'%template)
        sys.exit(1)

    # Replace the version string...
    version_info = None
    for i in range(len(lines)):
        s = lines[i]
        f = s.split(" ")
        if len(f)==0:
            continue
        if f[0]=="version_info":
            try:
                ns = {}
                exec(s.strip(), ns)
                version_info = ns.get("version_info")
                if type(version_info)!=tuple:
                    raise Exception
                if len(version_info)!=4:
                    raise Exception
                major,minor,micro,rlevel = version_info
                if type(major)!=int or type(minor)!=int or type(micro)!=int:
                    raise Exception
                if type(rlevel)!=str:
                    raise Exception
            except:
                print ('Invalid version tuple in file "%s": %s'%(template,s.strip()))
                raise
                sys.exit(1)
        if f[0]=="version":
            if version_info==None:
                print ('%s: version_info must occur before version'%template)
                sys.exit(1)
            major,minor,micro,rlevel = version_info
            v = "%d.%d.%d"%(major,minor,micro)
            if rlevel!="final":
                v+=rlevel
            if cgkit_light:
                v += " 'light'"
            v+=" (%s)"%time.strftime("%b %d %Y, %H:%M")
            lines[i] = 'version = "%s"\n'%v
            print ("Version: %s"%v)

    lines.append("\ncgkit_light = %s"%cgkit_light)

    # Save the new content to the actual module file...
    try:
        f = open(infomod, "wt")
        f.writelines(lines)
        f.close()
    except:
        print ('Could not write file "%s"'%infomod)
        sys.exit(1)

def pyx2c(pyxName, cName):
    """Run pyrex on a pyx file.
    
    pyxName is the input pyx file, cName the output C file.
    """
    cmd = "pyrexc -o %s %s"%(cName, pyxName)
    print (cmd)
    res = os.system(cmd)
    if res!=0:
        sys.stderr.write("Error running pyrexc\n")
        sys.exit()
    
def convertPyxFiles():
    """Convert all pyx files in pyrex/pyx.
    """
    pyxFiles = glob.glob("pyrex/pyx/*.pyx")
    for pyxFile in pyxFiles:
        cFile = os.path.splitext(os.path.basename(pyxFile))[0]+".c"
        cFile = os.path.join("pyrex", "c", cFile)
        if isNewer(pyxFile, cFile):
            pyx2c(pyxFile, cFile)
        else:
            print ("%s is up-to-date"%cFile)

######################################################################
######################################################################
######################################################################

# Name of the entire package (you can't use just this variable to rename
# the package, all the directories and source files have to be updated
# accordingly)
PACKAGE_NAME = "cgkit"

# The following variables are used to customize the compile process

INC_DIRS = []
LIB_DIRS = []
LIBS     = []
CC_ARGS  = []
LINK_ARGS = []
MACROS    = [("BOOST_PYTHON_MAX_ARITY", 18)]
data_files = []
scripts = []

# Default values
INSTALL_CGKIT_LIGHT = False
LIB3DS_AVAILABLE = False
CYBERX3D_AVAILABLE = False
OGRE_AVAILABLE = False
IRR_AVAILABLE = False
OSG_AVAILABLE = False
THREEDXWARE_AVAILABLE = False
WINTAB_AVAILABLE = False
GLOVESDK_AVAILABLE = False
GLOVESDK_BASE = None
BOOST_BASE = None
BOOST_LIB = "boost_python"
BOOST_DLL = "boost_python.dll"
USING_STLPORT = False
STLPORT_BASE = ""
STLPORT_DLL = "stlport_vc646.dll"
LIB3DS_PATH = None
LIB3DS_LIBNAME = "3ds"
CYBERX3D_PATH = None
CYBERX3D_LIB = None   # Default: "CyberX3D" (Windows) / "cx3d" (other)
CYBERX3D_XERCES_LIB = None  # Default: "xerces-c_2" (Windows) / "xerces-c" (other)
CYBERX3D_XERCES_DLL = "xerces-c_2_5_0.dll"
XERCES_PATH = None

# Remove the debug option
if sys.platform!="win32":
    vars = get_config_vars()
    vars["OPT"] = vars["OPT"].replace("-g", "")

# Read the general config file
configName = "config.cfg"
if os.path.exists(configName):
    print ("Reading %s..."%configName)
    exec(open(configName).read())
else:
    print ("Config file (%s) not available"%configName)

# Read the version-specific config file
vi = sys.version_info
configName = "config_py%s%s.cfg"%vi[:2]
if os.path.exists(configName):
    print ("Reading %s..."%configName)
    exec(open(configName).read())
else:
    print ("Version-specific config file (%s) not available"%configName)

# Disable all optional libs, STL and Boost for the light version...
if INSTALL_CGKIT_LIGHT:
    LIB3DS_AVAILABLE = False
    CYBERX3D_AVAILABLE = False
    OGRE_AVAILABLE = False
    IRR_AVAILABLE = False
    OSG_AVAILABLE = False
    THREEDXWARE_AVAILABLE = False
    WINTAB_AVAILABLE = False
    GLOVESDK_AVAILABLE = False
    USING_STLPORT = False
    BOOST_BASE = None

# Normalize the paths specified by the config file
if BOOST_BASE!=None:
    BOOST_BASE = os.path.normpath(BOOST_BASE)
if LIB3DS_PATH!=None:
    LIB3DS_PATH = os.path.normpath(LIB3DS_PATH)
if CYBERX3D_PATH!=None:
    CYBERX3D_PATH = os.path.normpath(CYBERX3D_PATH)
if XERCES_PATH!=None:
    XERCES_PATH = os.path.normpath(XERCES_PATH)

# Settings common for all platforms

# Locations of include files
INC_DIRS = [os.path.join("supportlib","include")]+INC_DIRS
 
# Locations of library files
LIB_DIRS = [os.path.join("supportlib","lib")] + LIB_DIRS

# Libraries to link with
LIBS += [BOOST_LIB]
LIBS += ["core"]

# Scripts
scripts = ["viewer.py",
           "render.py",
           "utilities/postbake.py",
           "utilities/info3d.py",
           "utilities/convert3d.py",
           "utilities/seqls.py",
           "utilities/seqmv.py",
           "utilities/seqrm.py",
           "utilities/seqcp.py"]

# Lib3ds
if LIB3DS_AVAILABLE:
#    INC_DIRS += [LIB3DS_PATH]
#    LIB_DIRS += [os.path.join(LIB3DS_PATH, "msvc", "Build", "Release")]
    LIBS += [LIB3DS_LIBNAME]
    MACROS += [("LIB3DS_AVAILABLE", None)]

# CyberX3D
if CYBERX3D_AVAILABLE:
    MACROS += [("CYBERX3D_AVAILABLE", None)]

# Ogre3D
if OGRE_AVAILABLE:
    LIBS += ["OgreMain"]
    MACROS.append(("OGRE_AVAILABLE", None))
    scripts += ["viewerOgre.py"]
    
# Irrlicht
if IRR_AVAILABLE:
    LIBS += ["Irrlicht"]
    MACROS.append(("IRR_AVAILABLE", None))
    scripts += ["viewerIrr.py"]

# osg
if OSG_AVAILABLE:
    LIBS += ["OpenThreadsWin32", "opengl32", "Producer", "osg", "osgProducer", "osgUtil", "osgDB", "osgGA", "osgText", "osgSim", "osgToy", "osgIntrospection"]
    MACROS.append(("OSG_AVAILABLE", None))
    scripts += ["viewerOsg.py"]


######################################################################
# Do some checks...

# Check if the supportlib has already been compiled...
if sys.platform=="win32":
    libcore = "core.lib"
else:
    libcore = "libcore.a"
libcore = os.path.join("supportlib", "lib", libcore)
if not os.path.exists(libcore) and not INSTALL_CGKIT_LIGHT:
    print (70*"-")
    print ("The library %s does not exist."%libcore)
    print ("You have to compile the C++ support library first. Please go to the")
    print ("subdirectory 'supportlib' and invoke scons:\n")
    print (" > cd supportlib")
    print (" > scons\n")
    print ("After that, you can proceed with calling the setup script. Please")
    print ("see the file readme.txt or the section about compiling the package")
    print ("in the manual if you encounter any further problems.")
    print ("Note: You can read the manual online at http://cgkit.sf.net")
    print (70*"-")
    sys.exit(1)

##### Windows specific stuff #####
if sys.platform=="win32":

    if not INSTALL_CGKIT_LIGHT:
        MACROS += [("WIN32", None)]
    
        # Boost
        if BOOST_BASE==None:
            BOOST_BIN = None
        else:
            BOOST_BIN = os.path.join(BOOST_BASE, "libs", "python", "build", "bin-stage")
            INC_DIRS = [BOOST_BASE]+INC_DIRS
            LIB_DIRS = [BOOST_BIN] + LIB_DIRS
    
        if BOOST_DLL!=None:
            # Search for the Boost DLL
            boost_dll = findFile(BOOST_DLL, LIB_DIRS)
            # Add the boost dll as additional file
            # (it is supposed to go into the root package directory)
            packageDLL(boost_dll)
    
        # OpenGL
        LIBS += ["opengl32", "glu32"]
    
        # Additional compiler options
        CC_ARGS  += ["/GR", "/Zm700", "/EHsc"]
        # Additional linker options
        LINK_ARGS += ["/NODEFAULTLIB:LIBCMT"]
    
        # Add the stlport dll as additional file
        # (it is supposed to go into the root package directory)
        if USING_STLPORT:
            if os.path.isabs(STLPORT_DLL):
                stlport_dll = STLPORT_DLL
            else:
                stlport_dll = os.path.join(STLPORT_BASE, "lib", STLPORT_DLL)
            packageDLL(stlport_dll)
    
        # Lib3ds
        if LIB3DS_AVAILABLE:
            INC_DIRS += [LIB3DS_PATH]
            LIB_DIRS += [os.path.join(LIB3DS_PATH, "msvc", "Build", "Release")]
#            LIBS += ["lib3ds-120s"]
#            MACROS += [("LIB3DS_AVAILABLE", None)]
    
        # CyberX3D
        if CYBERX3D_AVAILABLE:
            INC_DIRS += [os.path.join(CYBERX3D_PATH, "include")]
            LIB_DIRS += [os.path.join(CYBERX3D_PATH, "lib")]
            LIB_DIRS += [os.path.join(XERCES_PATH, "lib")]
            if CYBERX3D_LIB==None:
                CYBERX3D_LIB = "CyberX3D"
            if CYBERX3D_XERCES_LIB==None:
                CYBERX3D_XERCES_LIB = "xerces-c_2"
                LIBS += [CYBERX3D_LIB, CYBERX3D_XERCES_LIB, "gdi32"]
                if os.path.isabs(CYBERX3D_XERCES_DLL):
                    packageDLL(CYBERX3D_XERCES_DLL)
                else:
                    packageDLL(os.path.join(XERCES_PATH, "bin", CYBERX3D_XERCES_DLL))

##### Mac OS X (Darwin) #####
elif sys.platform=="darwin":
    # OpenGL 
    INC_DIRS += ["/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers/"]
    LINK_ARGS += ["-framework AGL -framework OpenGL"]

    LIB_DIRS += ["/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries"]
    LIBS += ["GL", "GLU"]

    # CyberX3D
    if CYBERX3D_AVAILABLE:
        if CYBERX3D_LIB==None:
            CYBERX3D_LIB = "cx3d"
        if CYBERX3D_XERCES_LIB==None:
            CYBERX3D_XERCES_LIB = "xerces-c"
        LIBS += [CYBERX3D_LIB, CYBERX3D_XERCES_LIB]

    # "Disable" debug symbols
#    CC_ARGS += ["-g0"]
   
##### Linux (and other) #####
else:
    # OpenGL 
    LIBS += ["GL", "GLU"]

    # CyberX3D
    if CYBERX3D_AVAILABLE:
        if CYBERX3D_LIB==None:
            CYBERX3D_LIB = "cx3d"
        if CYBERX3D_XERCES_LIB==None:
            CYBERX3D_XERCES_LIB = "xerces-c"
        LIBS += [CYBERX3D_LIB, CYBERX3D_XERCES_LIB]

    # "Disable" debug symbols
#    CC_ARGS += ["-g0"]

    
######################################################################

core_src_files = ["wrappers/py_wrapper.cpp",
                  "wrappers/py_noise.cpp",
                  "wrappers/py_vec3.cpp",
                  "wrappers/py_vec4.cpp",
                  "wrappers/py_mat3.cpp",
                  "wrappers/py_mat4.cpp",
                  "wrappers/py_quat.cpp",
                  "wrappers/py_slots1.cpp",
                  "wrappers/py_slots2.cpp",
                  "wrappers/py_slots3.cpp",
                  "wrappers/py_slots4.cpp",
                  "wrappers/py_arrayslots1.cpp",
                  "wrappers/py_arrayslots2.cpp",
                  "wrappers/py_arrayslot_mat3.cpp",
                  "wrappers/py_arrayslot_mat4.cpp",
                  "wrappers/py_arrayslots4.cpp",
                  "wrappers/py_component.cpp",
                  "wrappers/py_boundingbox.cpp",
                  "wrappers/py_worldobject.cpp",
                  "wrappers/py_worldobject2.cpp",
                  "wrappers/py_worldobject3.cpp",
                  "wrappers/py_material.cpp",
                  "wrappers/py_glmaterial.cpp",
                  "wrappers/py_geoms1.cpp",
                  "wrappers/py_spheregeom.cpp",
                  "wrappers/py_ccylindergeom.cpp",
                  "wrappers/py_torusgeom.cpp",
                  "wrappers/py_boxgeom.cpp",
                  "wrappers/py_planegeom.cpp",
                  "wrappers/py_trimeshgeom.cpp",
                  "wrappers/py_polyhedrongeom.cpp",
                  "wrappers/py_drawgeom.cpp",
                  "wrappers/py_lightsource.cpp",
                  "wrappers/py_glpointlight.cpp",
                  "wrappers/py_glspotlight.cpp",
                  "wrappers/py_gldistantlight.cpp",
                  "wrappers/py_glrenderer.cpp",
                  "wrappers/py_massproperties.cpp",
                  "wrappers/rply/rply/rply.c",
                  "wrappers/rply/py_rply_read.cpp",
                  "wrappers/rply/py_rply_write.cpp"]

# Shall the lib3ds wrappers be included?
if LIB3DS_AVAILABLE:
    core_src_files += ["wrappers/lib3ds/common_funcs.cpp",
                       "wrappers/lib3ds/nodedata.cpp",
                       "wrappers/lib3ds/light.cpp",
                       "wrappers/lib3ds/mesh.cpp",
                       "wrappers/lib3ds/material.cpp",
                       "wrappers/lib3ds/lib3ds.cpp"]

# Shall the CyberX3D wrappers be included?
if CYBERX3D_AVAILABLE:
    core_src_files += ["wrappers/cyberx3d/cyberx3d.cpp"]

# Shall the ogre wrappers be included?
if OGRE_AVAILABLE:
    core_src_files += ["wrappers/ogre/OgreCore.cpp",
                       "wrappers/ogre/OgreWrap.cpp"]

# IRRLICHT?
if IRR_AVAILABLE:
    core_src_files += ["wrappers/irr/IrrCore.cpp",
    		       "wrappers/irr/IrrWrap.cpp"]

# OSG?
if OSG_AVAILABLE:
    core_src_files += ["wrappers/osg/osgcore.cpp",
                       "wrappers/osg/core.cpp",
    		       "wrappers/osg/osgwrap.cpp"]		       


# Extension modules:

# Core library
ext_modules = [Extension("_core", core_src_files
                         ,extra_link_args=LINK_ARGS
                         ,libraries=LIBS
                         ,include_dirs=INC_DIRS
                         ,library_dirs=LIB_DIRS
                         ,extra_compile_args=CC_ARGS
                         ,define_macros=MACROS)]

# 3DXWare SpaceMouse/SpaceBall
if THREEDXWARE_AVAILABLE:
    ext_modules += [Extension("_spacedevice", ["wrappers/spacedevice/py_spacedevice.cpp"]
                             ,libraries=LIBS
                             ,include_dirs=INC_DIRS
                             ,library_dirs=LIB_DIRS
                             ,extra_compile_args=CC_ARGS
                             ,extra_link_args=LINK_ARGS+["/NODEFAULTLIB:LIBC"]
                             ,define_macros=MACROS)]

# Wintab
if WINTAB_AVAILABLE:
    ext_modules += [Extension("_wintab", ["wrappers/wintab/py_wintab.cpp"]
                             ,libraries=LIBS
                             ,include_dirs=INC_DIRS
                             ,library_dirs=LIB_DIRS
                             ,extra_compile_args=CC_ARGS
                             ,extra_link_args=LINK_ARGS
                             ,define_macros=MACROS)]

# Data glove
if GLOVESDK_AVAILABLE:
    if GLOVESDK_BASE!=None:
        INC_DIRS += [os.path.join(GLOVESDK_BASE, "inc")]
        LIB_DIRS += [os.path.join(GLOVESDK_BASE, "lib")]

    if sys.platform=="win32":
        MACROS += [("GLOVESDK_2_0", None)]
        if os.path.isabs(GLOVESDK_DLL):
            packageDLL(GLOVESDK_DLL)
        else:
            packageDLL(os.path.join(GLOVESDK_BASE, "dll",  GLOVESDK_DLL))
        
    ext_modules += [Extension("_glove", ["wrappers/glove/py_glove.cpp"]
                             ,libraries=LIBS
                             ,include_dirs=INC_DIRS
                             ,library_dirs=LIB_DIRS
                             ,extra_compile_args=CC_ARGS
                             ,extra_link_args=LINK_ARGS
                             ,define_macros=MACROS)]

ext_modules += [Extension("_pointcloud", ["pyrex/c/_pointcloud.c"]
                         ,extra_compile_args=CC_ARGS
                         ,extra_link_args=LINK_ARGS
                         ,define_macros=MACROS)]


# Infos...
updateInfoModule(INSTALL_CGKIT_LIGHT)
print (70*"=")
print ("3DS importer:      %s"%(enabledStr(LIB3DS_AVAILABLE)))
print ("VRML/X3D importer: %s"%(enabledStr(CYBERX3D_AVAILABLE)))
print ("OpenSceneGraph:    %s"%(enabledStr(OSG_AVAILABLE)))
print ("OGRE:              %s"%(enabledStr(OGRE_AVAILABLE)))
print ("3DXWare:           %s"%(enabledStr(THREEDXWARE_AVAILABLE)))
print ("Wintab:            %s"%(enabledStr(WINTAB_AVAILABLE)))
print ("Glove module:      %s"%(enabledStr(GLOVESDK_AVAILABLE)))
print (70*"=")

print ("Include paths (INC_DIRS):\n")
for p in INC_DIRS:
    print (" %s"%p)
    
print ("\nLibrary paths (LIB_DIRS):\n")
for p in LIB_DIRS:
    print (" %s"%p)

print ("\nLibraries to link with (LIBS):\n")
print (" %s"%(", ".join(LIBS)))

print ("")

# Check if the boost/python.hpp header can be found...
# [disabled the test because it doesn't take the built-in search paths
# (such as /usr/include) into account]
#if not INSTALL_CGKIT_LIGHT:
#    for p in INC_DIRS:
#        if os.path.exists(os.path.join(p, "boost", "python.hpp")):
#            print "Boost.Python header found in %s"%p
#            break
#    else:
#        print 70*"-"
#        print "Error: The header <boost/python.hpp> is not available."
#        print 70*"-"
#        sys.exit(1)

# Check if config.cpp is newer than py_wrapper.cpp
# If it is, "touch" py_wrapper.cpp so that it will be compiled again
# (because optional libs might have been included or excluded)
if os.path.exists("config.cfg") and not INSTALL_CGKIT_LIGHT:
    if isNewer("config.cfg", "wrappers/py_wrapper.cpp"):
        print ("Forcing compilation of py_wrapper.cpp")
        os.utime("wrappers/py_wrapper.cpp", None)

print (70*"=")

# Test (to enable light version)
if INSTALL_CGKIT_LIGHT:
    ext_modules = []

convertPyxFiles()

setup(name = PACKAGE_NAME,
      version = "2.0.0alpha8cvs",
      description = "Python Computer Graphics Kit",
      author = "Matthias Baas",
      author_email = "mbaas@users.sourceforge.net",
      url = "http://cgkit.sourceforge.net",
      license = "MPL/GPL/LGPL",

      # Process all pure Python modules in the cgkit directory
      packages = [PACKAGE_NAME,
                  PACKAGE_NAME+"._OpenGL",
                  PACKAGE_NAME+".all",
                  PACKAGE_NAME+".light",
                  PACKAGE_NAME+".light.cgtypes",
                  PACKAGE_NAME+".GUI",
                  PACKAGE_NAME+".Interfaces",
                  PACKAGE_NAME+".wintab",
                  PACKAGE_NAME+".spacedevice",
                  PACKAGE_NAME+".glove",
                  PACKAGE_NAME+".jobqueue",
                  PACKAGE_NAME+".jobqueue.defaultprocs",
                  ],

      # Stick all extension modules into the main package
      ext_package = PACKAGE_NAME,
      ext_modules = ext_modules,
      scripts = scripts,
      data_files = data_files
      )

print ("... finished setup")
