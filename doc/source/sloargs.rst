:mod:`sloargs` --- Interrogate shader parameters from compiled RenderMan shaders
================================================================================

.. module:: cgkit.sloargs
   :synopsis: Interrogate shader parameters from compiled RenderMan shaders

This module can be used to query shader parameters from compiled shaders. This
is done by using the shared libraries that ship with the RenderMan renderers
which means you can only parse shaders if you have the respective renderer
installed on your system. The module has built-in support for shaders compiled
by PRMan, 3Delight, Aqsis and Pixie. The module can be extended to support other
renderers as well.

Usually, you don't have to use this module explicitly as it is used internally
by the :mod:`slparams<cgkit.slparams>` module which is the main module for
querying shaders.

.. autofunction:: cgkit.sloargs.slparams

.. autofunction:: cgkit.sloargs.getSloLib

.. autofunction:: cgkit.sloargs.setSloLib

.. autofunction:: cgkit.sloargs.registerSloArgs

