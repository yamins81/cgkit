#!/usr/bin/env python
######################################################################
# Check if everything is installed to run cgkit
######################################################################

import sys

VER_PLATFORM_WIN32s = 0
VER_PLATFORM_WIN32_WINDOWS = 1
VER_PLATFORM_WIN32_NT = 2
VER_PLATFORM_WIN32_CE = 3

print 70*"-"
print "Python",sys.version

# Print platform information
if sys.platform=="win32":
    try:
        major, minor, build, platform, text = sys.getwindowsversion()
        if platform==VER_PLATFORM_WIN32s:
            plat = "Windows 3.1"
        elif platform==VER_PLATFORM_WIN32_WINDOWS:
            plat = "Windows 95/98/ME"
        elif platform==VER_PLATFORM_WIN32_NT:
            plat = "Windows NT/2000/XP"
        elif platform==VER_PLATFORM_WIN32_CE:
            plat = "Windows CE"
        else:
            plat = str(platform)
    except:
        plat = None

    print "Platform: %s"%sys.platform,
    if plat!=None:
        print "(%s)"%plat
        print "          Windows %d.%d (Build %d) %s"%(major, minor, build, text)
    else:
        print ""
else:
    print "Platform:",sys.platform

print 70*"-"

def chkmsg(txt):
    dots = (30-len(txt))*"."
    print "%s%s"%(txt, dots),

# Check Python version
vi = sys.version.split()[0].split(".")
try:
    major = int(vi[0])
    minor = int(vi[1])
except:
    major = 0
    minor = 0
chkmsg("Python version: %d.%d"%(major, minor))
if (major>2) or (major==2 and minor>=3):
    print "OK"
else:
    print "unsupported"
    print """
You are using an unsupported version of Python. It is recommended that
you upgrade to a more recent version. Some basic modules might still
work but it is quite likely that there are other parts that will not
work as expected.
"""

# Check PyProtocols
chkmsg("PyProtocols")
try:
    import protocols
    print "is installed"
except:
    print "missing"
    print """
PyProtocols is not installed. This package is required as soon as you are
using the cgkit scene management stuff (for example, as soon as you want
to use the viewer or render tool). You can get PyProtocols at this site:
http://peak.telecommunity.com/PyProtocols.html
"""

# Check PyOpenGL
chkmsg("PyOpenGL")
try:
    import OpenGL.GL
    print "is installed"
except:
    print "missing"
    print """
PyOpenGL is not installed. You will still be able to use the viewer tool
as most geometric objects are implemented in C++. Some objects that are
implemented in Python may raise an exception if you are attempting to
visualize them. You can obtain PyOpenGL at http://pyopengl.sourceforge.net/
"""

# Check PIL
chkmsg("PIL")
try:
    import Image
    print "is installed"
except:
    print "missing"
    print """
The Python Imaging Library (PIL) is missing. This package is required
whenever images have to be processed (for example, when using texture
maps or taking screenshots). You can get PIL at
http://www.pythonware.com/products/pil/index.htm
"""
    
# Check numarray
#try:
#    import numarray
#    numarray_installed = True
#except:
#    numarray_installed = False

# Check pygame
chkmsg("pygame")
try:
    import pygame
    print "is installed"
except:
    print "missing"
    print """
pygame is not installed. You can use cgkit in your own applications but
you will not be able to use the viewer tool. You can get pygame at 
http://www.pygame.org/
"""

# Check PyODE
chkmsg("PyODE")
try:
    import ode
    print "is installed"
except:
    print "missing"
    print """
The Python bindings of the Open Dynamics Engine (ODE) are not installed.
You will not be able to use the ODEDynamics component to do rigid body
simulations. You can get PyODE at http://pyode.sourceforge.net/
"""

# Check PySerial
chkmsg("PySerial")
try:
    import serial
    print "is installed"
except:
    print "missing"
    print """
PySerial is not installed. This module is only required for accessing
an Ascension Flock Of Bird motion tracker. You can get the module at
http://pyserial.sourceforge.net/
"""

# cgkit
chkmsg("cgkit (base)")
try:
    import cgkit.cgkitinfo
    print cgkit.cgkitinfo.version
except:
    print "missing"

chkmsg("cgkit (C++ lib)")
try:
    import cgkit._core
    print "OK"
except:
    print "failed"
    print """
The cgkit supportlib could not be imported. One possible reason for that
is that shared libraries (such as the boost_python runtime or OpenGL)
could not be found.
"""

chkmsg("cgkit (all)")
try:
    import cgkit.all
    print "OK"
except:
    print "failed"
