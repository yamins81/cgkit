
.. _glslangparams:

:mod:`glslangparams` --- Extracting OpenGL 2 shader parameters
==============================================================

.. module:: cgkit.glslangparams
   :synopsis: Extracting OpenGL 2 shader parameters


This module can be used to extract the parameters of an OpenGL 2 shader from the
shader source file.


.. autofunction:: cgkit.glslangparams.glslangparams(shader=None, cpp=None, cpperrstream=sys.stderr)


The module defines the following exception class:

.. exception:: GLSLangParseError

   This exception is thrown when an error occurs during parsing the shader source
   file.

