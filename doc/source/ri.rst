
:mod:`ri` --- Generic RenderMan binding to produce RIB
======================================================

.. module:: cgkit.ri
   :synopsis: Generic RenderMan binding to produce RIB


The RenderMan interface is an API that is used to communicate a 3D scene
description (which includes 3D geometry, light sources, a camera description,
etc) to a renderer which will produce a 2D image of that scene. The API itself
is independent of a particular renderer and can be used for any renderer that
adheres to the RenderMan standard. There are some excellent renderers freely
available, such as `3Delight <http://www.3delight.com/>`_ or the Open Source
renderer  `Aqsis <http://aqsis.sourceforge.net/>`_ or `Pixie
<http://pixie.sourceforge.net/>`_. On the commercial side, the most popular
renderers are Pixar's `Photorealistic RenderMan <http://renderman.pixar.com/>`_
(PRMan),  `RenderDotC <http://www.dotcsw.com/>`_ and  `AIR
<http://www.sitexgraphics.com/>`_.  The RenderMan interface was created by Pixar
and the official specification can be downloaded from their site.

This document is not an introduction to the RenderMan interface itself, it just
explains the usage of this particular Python binding. The binding was written to
be compliant to v3.2 of Pixar's RenderMan Interface specification. However, it
also supports features that were introduced after v3.2.

There is another RenderMan module called :mod:`cri` that interfaces a renderer
directly. Almost everything that is said in this section applies to the
:mod:`cri` module as well.


Using the API
-------------

It is safe to import the module using  ::

   from cgkit.ri import * 

All the functions that get imported start with the prefix ``Ri``, all constants
start with ``RI_`` or ``RIE_``, so you probably won't get into a naming
conflict.

After importing the module this way you can use the functions just as you're
used to from the C API (well, almost). ::

   from cgkit.ri import *

   RiBegin(RI_NULL)
   RiWorldBegin()
   RiSurface("plastic")
   RiSphere(1,-1,1,360)
   RiWorldEnd()
   RiEnd()

The parameter to :func:`RiBegin` determines where the output is directed to. You
can pass one of the following:

* ``RI_NULL`` or an empty string: The RIB stream will be written to  stdout.

* A name that contains the suffix ``".rib"`` (case insensitive):  A file with
  the given name is created and the RIB stream is written into it.

* A name that contains the suffix ``".rib.gz"`` (case insensitive): Same as
  before, but the stream is compressed. The result is the same as if you would
  output a RIB file and then compress it using gzip.

* A name without the suffix ``".rib"`` or ``".rib.gz"``: The name is supposed to
  be an external program that reads RIB from stdin.  The program is launched and
  the RIB stream is piped into it.

Note: When using the :mod:`cri` module you first have to load a library and
invoke the functions on the returned handle (see the section on the :mod:`cri`
module for more information about that). The interpretation of the argument to
:func:`RiBegin` is then dependent on the renderer you are using.

.. % -----------


Online documentation
--------------------

Every function has an associated doc string that includes a short description of
the function, some information about what parameters the function expects and an
example how the function is called.

Example (inside an interactive Python session)::

   >>> from ri import *
   >>> help(RiPatch)
   RiPatch(type, paramlist)

       type is one of RI_BILINEAR (4 vertices) or RI_BICUBIC (16 vertices).

       Number of array elements for primitive variables:
       -------------------------------------------------
       constant: 1              varying: 4
       uniform:  1              vertex:  4/16 (depends on type)

       Example: RiPatch(RI_BILINEAR, [0,0,0, 1,0,0, 0,1,0, 1,1,0])

or from the shell (outside the Python shell)::

   > pydoc ri.RiCropWindow

   Python Library Documentation: function RiCropWindow in ri

   RiCropWindow(left, right, bottom, top)
       Specify a subwindow to render.

       The values each lie between 0 and 1.

       Example: RiCropWindow(0.0, 1.0 , 0.0, 1.0)  (renders the entire frame)
                RiCropWindow(0.5, 1.0 , 0.0, 0.5)  (renders the top right quarter)

.. % -----------


Differences between the C and Python API
----------------------------------------

The Python RenderMan binding is rather close to the C API, however there are
some minor differences you should know about.

**Types**

In this binding typing is not as strict as in the C API. For compatibility
reasons, the RenderMan types (RtBoolean, RtInt, RtFloat, etc.) do exist but they
are just aliases to the corresponding built-in Python types and you never have
to use them explicitly. In the ctypes-based :mod:`cri` module, the types refer
to the respective ctypes types and you may want to use them occasionally to
construct arrays.

Wherever the API expects vector types (RtPoint, RtMatrix, RtBound, RtBasis) you
can use any value that can be interpreted as a sequence of the corresponding
number of scalar values. These can be lists, tuples or your own class that can
be used as a sequence.

It is also possible to use nested sequences instead of flat ones. For example,
you can specify a matrix as a list of 16 values or as a list of four 4-tuples.
The following two calls are identical::

   RiConcatTransform([2,0,0,0, 0,2,0,0, 0,0,2,0, 0,0,0,1]) 

   RiConcatTransform([[2,0,0,0], [0,2,0,0], [0,0,2,0], [0,0,0,1]])

**Parameter lists**

When passing parameter lists you have to know the following points:

* In C parameter lists have to be terminated with ``RI_NULL``. In Python this is
  not necessary, the functions can determine the number of arguments themselves.
  However, adding ``RI_NULL`` at the end of the list will not generate an error.
  For example, if you are porting C code to Python you don't have to change those
  calls. So the following two calls are both valid::

     RiSurface("plastic", "kd", 0.6, "ks", 0.4)
     RiSurface("plastic", "kd", 0.6, "ks", 0.4, RI_NULL) 

* The tokens inside the parameter list have to be declared (either inline or
  using :func:`RiDeclare`), otherwise an error is generated. Standard tokens (like
  ``RI_P``, ``RI_CS``, ...) are already pre-declared.

* Parameter lists can be specified in several ways. The first way is the
  familiar one you already know from the C API, that is, the token and the value
  are each an individual parameter::

     RiSurface("plastic", "kd", 0.6, "ks", 0.4) 

  Alternatively, you can use keyword arguments::

     RiSurface("plastic", kd=0.6, ks=0.4) 

  But note that you can't use inline declarations using keyword arguments. Instead
  you have to previously declare those variables using :func:`RiDeclare`. Also,
  you can't use keyword arguments if the token is a reserved Python keyword (like
  the standard ``"from"`` parameter).  The third way to specify the parameter list
  is to provide a dictionary including the token/value pairs::

     RiSurface("plastic", {"kd":0.6, "ks":0.4}) 

  This is useful if you generate the parameter list on the fly in your program.

**Arrays**

In the C API functions that take arrays as arguments usually take the length of
the array as a parameter as well. This is not necessary in the Python binding.
You only have to provide the array, the length can be determined by the
function.

For example, in C you might write::

   RtPoint points[4] = {0,1,0, 0,1,1, 0,0,1, 0,0,0};
   RiPolygon(4, RI_P, (RtPointer)points, RI_NULL); 

The number of points has to be specified explicitly. In Python however, this
call could look like this::

   points = [0,1,0, 0,1,1, 0,0,1, 0,0,0]
   RiPolygon(RI_P, points) 

The functions that are affected by this rule are::

   RiBlobby()
   RiColorSamples()
   RiCurves()
   RiGeneralPolygon()
   RiMotionBegin()
   RiPoints()
   RiPointsGeneralPolygons()
   RiPointsPolygons()
   RiPolygon()
   RiSubdivisionMesh()
   RiTransformPoints()
   RiTrimCurve()

When using the :mod:`cri` module it is particularly advantageous to pass arrays
as ctypes arrays or numpy arrays. In this case, no data conversion is required
which makes the function call considerably faster (particularly for large
amounts of data). ::

   # Creating a ctypes array of floats
   points = (12*RtFloat)(0,1,0, 0,1,1, 0,0,1, 0,0,0)

   # Creating a numpy array of floats
   points = numpy.array([0,1,0, 0,1,1, 0,0,1, 0,0,0], dtype=numpy.float32)

**User defined functions**

Some RenderMan functions may take user defined functions as input which will be
used during rendering. When using the :mod:`cri` module to link to an actual
RenderMan library you can use Python functions in addition to the standard
functions. However, in the case of the generic (:mod:`ri`) module, you can only
use the predefined standard functions.

**Filter functions**

It is not possible to use your own filter functions in combination with the
:mod:`ri` module, you have to use one of the predefined filters:

* RiGaussianFilter

* RiBoxFilter

* RiTriangleFilter

* RiSincFilter

* RiCatmullRomFilter

**Procedurals**

It is not possible to use your own procedurals directly in the RIB generating
program, you can only use one of the predefined procedural primitives:

* RiProcDelayedReadArchive

* RiProcRunProgram

* RiProcDynamicLoad

However, this is not really a restriction since you always can use
RiProcRunProgram to invoke your Python program that generates geometry.

**Extended transformation functions**

The transformation functions :func:`RiTranslate`, :func:`RiRotate`,
:func:`RiScale` and :func:`RiSkew` have been extended in a way  that is not part
of the official spec. Each of these functions takes one or two vectors as input
which usually are provided as 3 separate scalar values, like the axis of a
rotation for example::

   RiRotate(45, 0,0,1) 

Now in this implementation you can choose to provide such vectors as sequences
of 3 scalar values::

   RiRotate(45, [0,0,1]) 

   axis = vec3(0,0,1)
   RiRotate(45, axis)

**Empty stubs**

In the :mod:`ri` module, the function RiTransformPoints() always returns None
and never transforms points (as the module just outputs RIB and does not
maintain transformations matrices). In the :mod:`cri` module, on the other hand,
the function is available and can be used to transform points.

.. % -----------


Implementation specific options
-------------------------------

There is currently one option that is specific to this RenderMan binding and
that won't produce any RIB call but will control what gets written to the output
stream:


.. function:: RiOption(RI_RIBOUTPUT, RI_VERSION, 0)

   If this option is set to 0 directly after :func:`RiBegin` is called, then no
   ``"version"`` call will be generated in the RIB stream (default is 1).
   --- New in version 1.1
   (as of cgkit 2.0.0alpha9, the ``version`` call has been disabled)

.. % -----------


Error handling
--------------

Besides the three standard error handlers RiErrorIgnore, RiErrorPrint (default)
and RiErrorAbort the module provides an additional error handler called
RiErrorException. Whenever an error occurs RiErrorException raises the exception
:exc:`RIException`.

If you install a new error handler with :func:`RiErrorHandler` only the three
standard error handlers will produce an output in the RIB stream, if you install
RiErrorException or your own handler then the handler is installed but no RIB
output is produced.

The module does some error checking, however there are still quite a bit of
possible error cases that are not reported. For example, the module checks if
parameters are declared, but it is not checked if you provide the correct number
of values. In general, the module also does not check if a function call is
valid in a given state (e.g. the module won't generate an error if you call
:func:`RiFormat` inside a world block).

