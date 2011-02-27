######################################################################
Python Computer Graphics Kit v2.0.0
Copyright (C) 2002,2004 Matthias Baas (see license.txt)
######################################################################

Building the package
====================

The following instructions will build and install the full version of
cgkit. You can also install the 'light' version by setting the
variable INSTALL_CGKIT_LIGHT to True in your config file (just rename
the file config_template.cfg to config.cfg and uncomment the
corresponding line at the top of the file). Except for Python itself,
installing the light version does not require any additional compiler,
tool or library, you simply have to call

> python setup.py install    # that's all for the light version

while being in the root directory of the cgkit sources. For installing
the light version you can skip the remainder of this text, but read
on if you actually want to install the full version.

The following notes apply to all platforms (tested on Windows and
Linux).

Requirements
------------

- Boost (v1.32)
- SCons (v0.96.1)

The following libraries are optional:

- lib3ds (v1.3.0): You need this if you want to import/export 3DS files. 
- CyberX3D: You need this if you want to import/export VRML or X3D files. 
- OGRE: You need this if you want to use the OGRE-Viewer. 
- STLport: Only required on Windows when you want to use OGRE. 
- 3DxWare SDK: (currently Windows only) You need this if you want to use a 
  SpaceMouse or SpaceBall. 
- Wintab Programmer's Kit: (Windows only) You need this if you want to use 
  a tablet (or similar device). 
- GloveSDK: You need this if you want to use a data glove. Note: Download 
  the version 2 SDK from the Ultra series, this can also be used with older 
  gloves. You could also use version 1, but some functionality won't be 
  available then.

Building & Installing
---------------------

The package consists of three parts:

1) a pure C++ library that is independent of Python  
2) Python wrappers around the above C++ library (using Boost.Python)
3) the actual Python package using the wrappers

Part 2) and 3) is built and installed via the Python distutils (i.e.
the setup.py script). But before you can do so, you have to build the
C++ library manually using SCons.

The C++ library is located in the directory supportlib. The library
uses SCons as its build system. If you have to customize the build
process you can create a file cpp_config.cfg where you can set some
build variables (e.g. adding search paths for include files). There is
a template file cpp_config_template.cfg which you can use to create
the actual config file. However, if you have installed every library
in standard directories it may well be that you don't need any config
file at all. Eventually you can build the library by calling scons:

  $ cd supportlib
  $ # ...create & modify cpp_config.cpp if necessary...
  $ scons

If everything went fine you can see the result in the "lib" subdirectory
(libcore.a on Linux or core.lib on Windows).

The next step is to build and install the Python package. The package
uses the distutils, so you will find the familiar setup.py script in
the main directory. Customization of the build process can be done
in a file called "config.cfg" which is executed by the setup script.
There is a template file "config_template.cfg" which you can use to
create your own config file. After setting up your configuration you
can install the package with the usual procedure:

  $ cd ..  # if you were still in the supportlib directory
  $ python setup.py install

(see the manual "Installing Python Modules" in your Python documentation
for more details about the distutils and how to proceed if you have
any special requirements)

Please also have a look at the user manual for a list of third-party
packages you might have to install before you can use cgkit. You can
check cgkit and its dependencies with the script utilities/checkenv.py
that is part of the source archive. For a more thorough test you can
run the unit tests in the unittests directory.

----------------------------------------------------------------------
Developers note: You can build the package inplace by calling

  $ python setup.py build_ext --inplace

In this case, the resulting binaries will be placed directly in the
"cgkit" directory which will then contain the entire package. Use
the PYTHONPATH variable and add the path to the main directory if you 
want to use the inplace version from any other directory.
----------------------------------------------------------------------

