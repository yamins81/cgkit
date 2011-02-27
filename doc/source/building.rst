***********************
Building and Installing
***********************

This chapter provides information about how to build the kit yourself from the
sources. You can skip this chapter if the kit is already installed on your
system or if you are installing binary versions.

.. % Building and Installing

.. % ----------------------------------------------------------------------


Building from the sources
=========================

The following instructions will build and install the full version of cgkit. You
can also install the 'light' version by setting the variable
``INSTALL_CGKIT_LIGHT`` to ``True`` in your config file (just rename the file
``config_template.cfg`` to ``config.cfg`` and uncomment the corresponding line
at the top of the file). Except for Python itself,  installing the light version
does not require any additional compiler, tool or library, you simply have to
call  ::

   > python setup.py install

while being in the root directory of the cgkit sources. For installing the light
version you can skip the remainder of this section, but read on if you actually
want to install the full version.

The steps required to build the full version of the kit are the same on every
platform.  The build process assumes you have the following tools/libraries
installed on your system:

* `Boost <http://www.boost.org/>`_ - Make sure that Boost is installed
  including Boost.Python (e.g. Mac users who are using MacPorts have to install
  the "+python" variant).

* `SCons <http://www.scons.org/>`_

* `lib3ds <http://lib3ds.sourceforge.net/>`_ (v1.x) - Optional. You need this
  if you want to import/export 3DS files.

* `CyberX3D <http://www.cybergarage.org/vrml/cx3d/cx3dcc/index.html>`_ -
  Optional. You need this if you want to import/export VRML or X3D files.

* `3DxWare SDK <http://www.3dconnexion.com/sdk.htm>`_ - Optional (currently
  Windows only). You need this if you want to use a SpaceMouse or SpaceBall.

* `Wintab Programmer's Kit <http://www.wacomeng.com/devsupport/index.html>`_ - Optional (Windows
  only). You need this if you want to use a tablet (or similar device).

* `GloveSDK <http://www.5dt.com/>`_ - Optional. You need this if you want to use
  a data glove. Note: Download the version 2 SDK from the Ultra series, this can
  also be used with older gloves. You could also use version 1, but some
  functionality won't be available then.

If you have successfully installed the above tools and libraries you can proceed
with building the kit. The first step is to get the cgkit sources. You can either
download the source archive or check out the latest version from svn. The
following build instructions apply to both versions.

The package consists of three parts:

#. A pure C++ library that is independent of Python. This library is located in
   the :file:`supportlib` directory.

#. Python wrappers around the above C++ library (using Boost.Python).  These
   wrappers are located in the :file:`wrappers` directory.

#. The actual Python package using the wrappers. The package is located in the
   :file:`cgkit` directory. The command line tools are in the main directory
   and the :file:utilities` directory.

Part 2) and 3) is built and installed via the Python distutils (i.e. the
:file:`setup.py` script). But before you can do so, you have to build the C++
library manually using SCons.

Building the C++ support library:

The C++ library is located in the directory :file:`supportlib`. The library uses
SCons as its build system. If you have to customize the build process you can
create a file :file:`cpp_config.cfg` where you can set some build variables
(e.g. adding search paths for include files). There is a template file
:file:`cpp_config_template.cfg` which you can use to create the actual config
file. However, if you have installed every library in standard directories it
may well be that you don't need any config file at all. Eventually you can build
the library by calling :program:`scons`::

   > cd supportlib
   > # ...create & modify cpp_config.cpp if necessary...
   > scons

If everything went fine you can see the result in the :file:`lib` subdirectory
(:file:`libcore.a` on Linux and OSX or :file:`core.lib` on Windows).

Building the Python wrappers:

The next step is to build and install the Python package. The package uses the
distutils, so you will find the familiar :file:`setup.py` script in the main
directory. Customization of the build process can again be done in a file called
:file:`config.cfg` and :file:`config_pyXY.cfg` where ``XY`` is the Python version
you want to build against. The former file is always read whereas the latter
is only read when building against that particular version of Python.
There is a template
file :file:`config_template.cfg` which you can use to create your own config
file. After setting up your configuration you can install the package with the
usual procedure::

   > cd ..  # if you were still in the supportlib directory
   > python setup.py install

(see the manual Installing Python Modules in your Python documentation for more
details about the distutils and how to proceed if you have any special
requirements)

Please also have a look at section :ref:`externaldeps` for a list of third-party
packages you might have to install before you can use cgkit. You can check cgkit
and its dependencies with the script  :file:`utilities/checkenv.py` that is part
of the source archive. For a more thorough test you can run the unit tests in
the :file:`unittests` directory.

A note to developers: You can build the package inplace by calling ::

   > python setup.py build_ext --inplace

In this case, the resulting binaries will be placed directly in the
:file:`cgkit` directory which will then contain the entire package. Use the
:envvar:`PYTHONPATH` variable and add the path to the main directory if you want
to use the inplace version from any other directory.

