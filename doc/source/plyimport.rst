.. % PLY import


Polygon (PLY) import
--------------------

The plugin supports the following options that can be passed to the :func:`load`
command:

+-----------------+-----------+---------------------------------+
| Option          | Default   | Description                     |
+=================+===========+=================================+
| ``includevar``  | ``None``  | Only include the specified      |
|                 |           | variables                       |
+-----------------+-----------+---------------------------------+
| ``excludevar``  | ``None``  | Exclude the specified variables |
+-----------------+-----------+---------------------------------+
| ``invertfaces`` | ``False`` | Invert face orientations        |
+-----------------+-----------+---------------------------------+

*includevar* is a list of primitive variable names that should be  imported. All
other variables are ignored. By default, all variables  stored in the file are
imported. If you specify an empty list, no user variable will be created and you
will end up with the raw geometry.

*excludevar* is a list of primitive variable names that should not be imported.
Every user variable specified in this list is ignored.

*invertfaces* specifies whether the orientation of the polygons should be
inverted or not.

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

