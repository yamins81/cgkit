.. % PLY export


Polygon (PLY) export
--------------------

The PLY format that this plugin exports is the format that was developed at
Stanford University and that stores a polygonal model together with arbitrary
user attributes.

The plugin supports the following options that can be passed to the :func:`save`
command:

+------------+-------------+----------------------+
| Option     | Default     | Description          |
+============+=============+======================+
| ``object`` | ``None``    | The object to export |
+------------+-------------+----------------------+
| ``mode``   | ``"ascii"`` | The output mode      |
+------------+-------------+----------------------+

*object* is the name of an object or the object itself that will be exported. If
no object is given, the plugin examines the scene and if there is only one
object this object is exported. If there are several objects, an exception is
thrown.

*mode* is a string specifying whether the output file will be ascii or binary.
It can be one of "ascii", "little_endian" or "big_endian".

.. note::

   The plugin uses the RPly library which is available at
   `<http://www.cs.princeton.edu/ diego/professional/rply/>`_. ::

      RPly 1.01 license
      Copyright © 2003-2005 Diego Nehab.

      Permission is hereby granted, free of charge, to any person obtaining a
      copy of this software and associated documentation files (the "Software"),
      to deal in the Software without restriction, including without limitation
      the rights to use, copy, modify, merge, publish, distribute, sublicense,
      and/or sell copies of the Software, and to permit persons to whom the
      Software is furnished to do so, subject to the following conditions:

      The above copyright notice and this permission notice shall be included in
      all copies or substantial portions of the Software.

      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
      IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
      FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
      AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
      LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
      FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
      DEALINGS IN THE SOFTWARE.

