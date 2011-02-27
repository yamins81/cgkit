
:mod:`mayaascii` --- Reading Maya ASCII files
=============================================

.. module:: cgkit.mayaascii
   :synopsis: Reading Maya ASCII files


This module contains the :class:`MAReader` class which can be used as a base
class for reading Maya ASCII (\*.ma) files. The class reads the file and invokes
callback methods with the corresponding data in the file. Derived classes have
to implement those callback methods and process the data as appropriate.

The module also contains a couple of helper classes that may be used by a
derived reader class to facilitate the processing of the data.


.. function:: splitDAGPath(path)

   Split a Maya DAG path into its components. The path is given as a string that
   may have the form <namespace>:<path> where <path> is a sequence of strings
   separated by '\|'.  The return value is a 2-tuple (*namespace*, *names*) where
   *namespace* is ``None``  if the path did not contain a ':' character. *names* is
   a list  of individual path names.


.. function:: stripQuotes(s)

   Remove surrounding quotes if there are any. The function returns the string *s*
   without surrounding quotes (i.e. ``"foo" -> foo``).  If there are no quotes the
   string is returned unchanged.

.. % ----------------------------------------------------------------


MAReader class
--------------

The :class:`MAReader` class reads Maya ASCII files and calls appropriate methods
which have to be implemented in a derived class. The content of the file is
actually a subset of the Maya Embedded Language (MEL) which is the scripting
language implemented inside Maya.  The :class:`MAReader` parses the file, breaks
down the content of the file in commands and their arguments and options
(expressions are not evaluated). Each MEL command will then trigger a callback
method that has to execute the command.  These callback methods have to be
implemented in a derived class.

There are 12 MEL commands that can appear in a Maya ASCII  file [#]_:

* file

* requires

* fileInfo

* currentUnit

* createNode

* setAttr

* addAttr

* connectAttr

* disconnectAttr

* parent

* select

* lockNode

Each command has a number of arguments and can also take options. The callback
methods receive the arguments as regular arguments to the method and the options
as an additional argument ``opts`` which is a dictionary containing the options
that were specified in the file. The key is the long name of the option (without
leading dash) and the value is a list of strings containing the option values.
The number of values and how they have to be interpreted depend on the actual
option.


.. class:: MAReader()

   Creates an instance of the reader.


.. attribute:: MAReader.filename

   The file name (if it could be obtained). This may be used for generating warning
   or error messages.


.. attribute:: MAReader.linenr

   The current line number. This may be used for generating warning or error
   messages.


.. method:: MAReader.abort()

   Aborts reading the current file. This method can be called in handler methods to
   abort reading the file.


.. method:: MAReader.read(f)

   Read the content of a file. *f* is either a file like object that can be used to
   read the content of the file or the name of a file.


.. method:: MAReader.begin()

   Callback method that is called before the file is read.


.. method:: MAReader.end()

   Callback method that is called after the file was read.


.. method:: MAReader.onFile(filename, opts)

   Reference an external file.


.. method:: MAReader.onRequires(product, version)

   Specify a requirement that is needed to load the file properly. *product* is a
   string containing the required software component and *version* is a string
   containing the required version of that component.


.. method:: MAReader.onFileInfo(keyword, value, opts)

   Specifies information about the file. *keyword* and *value* are both strings.


.. method:: MAReader.onCurrentUnit(opts)

   Specify the units (linear, angular, time) used in the file.


.. method:: MAReader.onCreateNode(nodetype, opts)

   Create a new node. *nodetype* is a string specifying the type of node that is to
   be created. The new node will automatically be selected (i.e. subsequent setAttr
   commands refer to this node).


.. method:: MAReader.onSetAttr(attr, vals, opts)

   Set a node attribute. *attr* is a string containing the attribute to be set.
   *vals* is a list of values. The number of elements and the type of each element
   depends on the attribute.


.. method:: MAReader.onAddAttr(opts)

   Add a new attribute to the node.


.. method:: MAReader.onConnectAttr(srcattr, dstattr, opts)

   Connect two attributes. *srcattr* is a string specifiying the attribute that
   serves as a source and *dstattr* is the name of the attribute that will receive
   the value.


.. method:: MAReader.onDisconnectAttr(srcattr, dstattr, opts)

   Break the attribute connection between two attributes.


.. method:: MAReader.onParent(objects, parent, opts)

   Set the parent of one or more nodes. *objects* is a list of node names and
   *parent* the name of the parent.


.. method:: MAReader.onSelect(objects, opts)

   Select a node from a referenced file. *objects* is a list of strings containing
   the node names.


.. method:: MAReader.onLockNode(objects, opts)

   Lock/unlock a node. *objects* is a list of strings containing the node names
   (the list may be empty).

.. % ----------------------------------------------------------------


DefaultMAReader class
---------------------

This class is derived from the :class:`MAReader` class and implements  the
callback methods which build the graph in memory using the helper classes
(:class:`Node`, etc.). You may derive from this class and only implement the
:meth:`end` callback to process the graph. All nodes of the graph can be found
in the :attr:`nodelist` attribute.


.. class:: DefaultMAReader()

   Creates an instance of the reader.


.. attribute:: DefaultMAReader.nodelist

   This list will contain all :class:`Node` objects that have been created. The
   order is the same as they have been encountered in the file.


.. method:: DefaultMAReader.findNode(path, create=False)

   Return the :class:`Node` object corresponding to a particular path. *path* may
   also be ``None`` in which case :class:`None` is returned. If *create* is
   ``True``, any missing node is automatically created.

.. % ----------------------------------------------------------------


Node class
----------

The :class:`Node` class is a helper class which may be used in a concrete
implementation of the :class:`MAReader` class to represent  Maya nodes.

This class does not implement the actual functionality of a particular Maya
node, it just tracks attribute changes and connections which can later be
retrieved once the entire file was read. So this class can be used for all Maya
nodes in a file.


.. class:: Node(nodetype, opts, parent=None)

   *nodetype* and *opts* are the arguments of the  :meth:`onCreateNode` callback of
   the :class:`MAReader` class. *parent* is either None or another :class:`Node`
   object that will  be the parent of the node.


.. attribute:: Node.nodetype

   This is a string containing the type of the node (this is the value that was
   passed to the constructor).


.. attribute:: Node.opts

   This is the option dictionary that was passed to the constructor (i.e. that is
   used to create the node).


.. method:: Node.getName()

   Return the name of the node. If no node name was specified during the creation
   of the object, the dummy name 'MayaNode' is returned.


.. method:: Node.getParentName()

   Return the name of the parent node or ``None`` if no parent was specified.


.. method:: Node.getParent()

   Return the parent :class:`Node` object.


.. method:: Node.setParent(parent)

   *parent* is either ``None`` to remove the node from its parent or it is another
   :class:`Node` object that will be the new parent.


.. method:: Node.iterChildren()

   Return an iterator that yields all children :class:`Node` objects.


.. method:: Node.setAttr(attr, vals, opts)

   *attr*, *vals* and *opts* are the arguments of the  :meth:`onSetAttr` callback
   of the :class:`MAReader` class. The Python value of an attribute can be obtained
   by calling :meth:`getAttrValue`. The final Python value can be retrieved with
   the :meth:`getAttrValue` method.


.. method:: Node.getAttrValue(lname, sname, type, n=1, default=None)

   Get the Python value of an attribute. *lname* is the long name, *sname* the
   short name. *type* is the required type and *n* the required number of elements
   (see the :meth:`Attribute.getValue` method
   for more information on the type).  *type* and *n* may be None. The return value
   is either a normal Python type (int, float, sequence) or a
   :class:`MultiAttrStorage` object in cases where the ``setAttr`` command
   contained the index operator. When no attribute with the given long or short
   name could be found the provided default value is returned.


.. method:: Node.addAttr(opts)

   *opts* is the arguments of the :meth:`onAddAttr` callback of  the
   :class:`MAReader` class.


.. method:: Node.addInConnection(localattr, nodename, attrname)

   Specify an incoming connection. *nodename* is the name of a node and *attrname*
   the full  attribute name.


.. method:: Node.addOutConnection(localattr, node, nodename, attrname)

   Specify an outgoing connection. *node* is a :class:`Node` object, *nodename* the
   name of the  node and *attrname* the full attribute name.


.. method:: Node.getInNode(localattr_long, localattr_short)

   Return the node and attribute that serves as input for the local attribute with
   long name *localattr_long* and short name *localattr_short*. The return value is
   a 2-tuple (*nodename*, *attrname*) that specifies the input connection for the
   local attribute. (``None``, *None*) is returned if there is  no connection.


.. method:: Node.getOutNodes(localattr_long, localattr_short)

   Return the nodes and attributes that the specified local attribute connects to.
   *localattr_long* is the long name of the local attribute and *localattr_short*
   its short name. The return value is a list of 3-tuples (*node*, *nodename*,
   *attrname*) that specify the output connections for the local attribute. The
   tuple contains the values that were previously added with
   :meth:`addOutConnection`.


.. method:: Node.getOutAttr(lname, sname, dstnodetype)

   Check if a local attribute is connected to a particular type of node. Returns a
   tuple (*node*, *attrname*) where *node* is the :class:`Node` object of the
   destination node and *attrname* the name of  the destination attribute. If there
   is no connection with a node of type *dstnodetype*, the method returns
   (``None``, *None*).

   If the attribute is connected to more than one node with the given type or to
   several attributes of the same node then only the first connection encountered
   is returned.

.. % ----------------------------------------------------------------


Attribute class
---------------

The :class:`Attribute` class can be used to convert the value of an attribute
(as specified by the ``setAttr`` MEL command) into an appropriate Python value.

An :class:`Attribute` object is initialized with the arguments that were passed
to the :meth:`onSetAttr` callback of the reader class. The value can be
retrieved using the :meth:`getValue` method. Whenever you have prior knowledge
of the node you are  currently processing you should pass the expected type of
the attribute to the :meth:`getValue` method to prevent the method from having
to guess the type in case it is not specified in the ``setAttr`` call.


.. class:: Attribute(attr, vals, opts)

   *attr*, *vals* and *opts* are the arguments of the  :meth:`onSetAttr` callback
   of the :class:`MAReader` class.


.. method:: Attribute.getBaseName()

   Return the base name of the attribute. This is the first part of the attribute
   name (and may actually refer to another attribute). ::

      ".t"            -> "t"
      ".ed[0:11]"     -> "ed"
      ".uvst[0].uvsn" -> "uvst"


.. method:: Attribute.getFullName()

   Return the full attribute specifier.


.. method:: Attribute.getValue(type=None, n=None)

   .. _attributegetvalue:

   Return the value of the attribute as an appropriate Python value. *type* is a
   string containing the required type of the value. If ``None`` is passed, the
   method tries to retrieve the value from the attribute itself. If it fails, an
   exception is thrown. The following table lists the valid type strings and their
   corresponding Python type:

   +--------------------+----------------------------------------+
   | type               | Python type                            |
   +====================+========================================+
   | ``"bool"``         | bool                                   |
   +--------------------+----------------------------------------+
   | ``"int"``          | int                                    |
   +--------------------+----------------------------------------+
   | ``"float"``        | float                                  |
   +--------------------+----------------------------------------+
   | ``"string"``       | str                                    |
   +--------------------+----------------------------------------+
   | ``"short2"``       | (int, int)                             |
   +--------------------+----------------------------------------+
   | ``"short3"``       | (int, int, int)                        |
   +--------------------+----------------------------------------+
   | ``"long2"``        | (int, int)                             |
   +--------------------+----------------------------------------+
   | ``"long3"``        | (int, int, int)                        |
   +--------------------+----------------------------------------+
   | ``"int32Array"``   | [int, ...]                             |
   +--------------------+----------------------------------------+
   | ``"float2"``       | (float, float)                         |
   +--------------------+----------------------------------------+
   | ``"float3"``       | (float, float, float)                  |
   +--------------------+----------------------------------------+
   | ``"double2"``      | (float, float)                         |
   +--------------------+----------------------------------------+
   | ``"double3"``      | (float, float, float)                  |
   +--------------------+----------------------------------------+
   | ``"doubleArray"``  | [float, ...]                           |
   +--------------------+----------------------------------------+
   | ``"polyFaces"``    | PolyFace (see :ref:`polyface`)         |
   +--------------------+----------------------------------------+
   | ``"nurbsSurface"`` | NurbsSurface (see :ref:`nurbssurface`) |
   +--------------------+----------------------------------------+
   | ``"nurbsCurve"``   | NurbsCurve (see :ref:`nurbscurve`)     |
   +--------------------+----------------------------------------+

   The argument *n* specifies how many values are expected. An exception is thrown
   if the number of values that were set by the ``setAttr`` call doesn't match the
   required number. If ``None`` is passed, an arbitrary number of values is
   allowed. The value of *n* also influences the return type. If the value is 1 the
   method will return one of the types in the above table, otherwise it will return
   a list of the above types.

.. % ----------------------------------------------------------------


.. _polyface:

PolyFace class
--------------


.. class:: PolyFace

   This class stores the data of a polygonal face. :class:`PolyFace` objects are
   returned when the value of a ``polyFaces`` attribute is requested.

The class has the following data members:


.. attribute:: PolyFace.f

   This is a list of integers containing the edge indices of the edges  making up
   the face. If an index is negative the edge has to be reversed (the edge index
   then is -i-1).


.. attribute:: PolyFace.h

   This is a list of holes. Each hole is described by a list of integers
   containing the edge indices of the edges  making up the hole in the face. If an
   index is negative the edge has to be reversed (the edge index then is -i-1).


.. attribute:: PolyFace.mf

   This is a list of texture coordinate ids of the face. This data type is obsolete
   as of Maya version 3.0. It is replaced by "mu".


.. attribute:: PolyFace.mh

   This is a list of texture coordinate ids of the hole. This data type is obsolete
   as of Maya version 3.0. It is replaced by "mu".


.. attribute:: PolyFace.mu

   For each loop (i.e. outer loop or hole) this list contains a list of 2-tuples
   (*uvset*, *ids*) where *uvset* is the index of the UV set and *ids* the indices
   of the texture coordinates.


.. attribute:: PolyFace.fc

   For each loop (outer loop or hole) this list contains a list of color  index
   values.

.. % ----------------------------------------------------------------


.. _nurbssurface:

NurbsSurface class
------------------


.. class:: NurbsSurface

   This class stores the data of a NURBS surface. :class:`NurbsSurface` objects are
   returned when the value of a ``nurbsSurface`` attribute is requested.

The class has the following data members:


.. attribute:: NurbsSurface.udegree

   Degree in u direction.


.. attribute:: NurbsSurface.vdegree

   Degree in v direction.


.. attribute:: NurbsSurface.uform

   Form attribute for the u direction. The attribute can have one of the  following
   values:

   +-------+----------+
   | Value | Meaning  |
   +=======+==========+
   | ``0`` | Open     |
   +-------+----------+
   | ``1`` | Closed   |
   +-------+----------+
   | ``2`` | Periodic |
   +-------+----------+


.. attribute:: NurbsSurface.vform

   Form attribute for the v direction (see above).


.. attribute:: NurbsSurface.isrational

   This attribute is ``True`` if the surface contains a rational component. In this
   case, the control vertices are given as 4-tuples, otherwise as 3-tuples.


.. attribute:: NurbsSurface.uknots

   This is a list of floats containing the knot values for the u direction.


.. attribute:: NurbsSurface.vknots

   This is a list of floats containing the knot values for the v direction.


.. attribute:: NurbsSurface.cvs

   A list of control vertices. Each vertex is given either as a 3-tuple or 4-tuple
   of floats.

.. % ----------------------------------------------------------------


.. _nurbscurve:

NurbsCurve class
----------------


.. class:: NurbsCurve

   This class stores the data of a NURBS curve. :class:`NurbsCurve` objects are
   returned when the value of a ``nurbsCurve`` attribute is requested.

The class has the following data members:


.. attribute:: NurbsCurve.degree

   The degree of the curve.


.. attribute:: NurbsCurve.spans

   The number of spans.


.. attribute:: NurbsCurve.form

   Form attribute. The attribute can have one of the following values:

   +-------+----------+
   | Value | Meaning  |
   +=======+==========+
   | ``0`` | Open     |
   +-------+----------+
   | ``1`` | Closed   |
   +-------+----------+
   | ``2`` | Periodic |
   +-------+----------+


.. attribute:: NurbsCurve.isrational

   This attribute is ``True`` if the curve contains a rational component. In this
   case, the control vertices have one additional value.


.. attribute:: NurbsCurve.dimension

   The dimension of the curve (2 or 3).


.. attribute:: NurbsCurve.knots

   This is a list of floats containing the knot values.


.. attribute:: NurbsCurve.cvs

   A list of control vertices. Each vertex is a tuple of 2, 3 or 4 floats (the
   actual number depends on the *dimension* and *isrational* settings).

.. rubric:: Footnotes

.. [#] Actually, there could appear any MEL command, but at least Maya will only export
   files containing the above commands.

