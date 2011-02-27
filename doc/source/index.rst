.. Python Computer Graphics Kit documentation master file, created by
   sphinx-quickstart on Sat Jul 11 19:14:33 2009.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to the Python Computer Graphics Kit documentation
=========================================================

The Python Computer Graphics Kit is a collection of utilities and Python
modules that simplify working with 3D data of any kind. The provided functionality
can be used in domains such as scientific visualization, Virtual Reality,
VFX, animation or even games.
Currently, the package is implemented in Python and C++ where the C++ part
is wrapped and made available in Python.

Broadly speaking, the modules inside the Python package can be divided into
two categories: generic self-contained modules that add 3D-related functionality
to Python (for example, the vector, matrix and quaternion types, the RenderMan
bindings, noise functions, etc.) and modules that provide scene management
functionality where a full 3D scene is kept in memory and can be processed
or visualized. The modules in the first category can be used in any environment
where Python is available such as, for example, in Maya, Houdini, Nuke, Blender
or any other application that can be extended using Python. The scene-specific
modules are typically only used in stand-alone Python programs as the
aforementioned applications have their own scene management.

.. raw:: html

   <p>Contents:</p>

.. toctree::
   :maxdepth: 2
   :numbered:

   introduction
   building
   tools
   generic_modules
   scene_specific_modules
   components
   worldobjects
   geomobjects
   materials
   importplugins
   exportplugins
   license

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

