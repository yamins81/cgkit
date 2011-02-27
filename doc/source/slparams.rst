.. _slparams:

:mod:`slparams` --- Extracting RenderMan Shader parameters
==========================================================

.. module:: cgkit.slparams
   :synopsis: Extracting RenderMan Shader parameters


This module can be used to extract the parameters of a RenderMan shader either
from the shader source file or from the compiled shader. To read parameters from
a compiled shader it is necessary that the renderer package is installed that
was used to compile the shader. Currently, the following renderers are
supported:

* 3Delight
* Aqsis
* Pixie
* PRMan

Other renderers can be added at runtime (see the :mod:`sloargs<cgkit.sloargs>` module).


.. autofunction:: cgkit.slparams.slparams(slfile=None, cpp=None, cpperrstream=sys.stderr, includedirs=None, defines=None)


.. autofunction:: cgkit.slparams.convertdefault


The module defines the following exception classes:

.. exception:: SLParamsError

   Base class for the exceptions in this module. This class is derived from the
   Exception class.


.. exception:: PreprocessorNotFound

   This exception is thrown when calling the preprocessor didn't produce any data.


.. exception:: SyntaxError

   This exception is thrown when a syntax error is found in any function or shader
   definition. The exception has the following attributes:

   * *filename*: The filename where the error was found
   * *lineno*: The line number where the error was found
   * *line*: The entire line that contains the error
   * *errpos*: The character position where the error token starts

.. exception:: NoMoreTokens

   This exception is thrown when the parser runs out of tokens.

