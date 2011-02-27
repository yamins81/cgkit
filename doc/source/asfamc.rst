
:mod:`asfamc` --- Acclaim skeleton and motion file (ASF/AMC) reader
===================================================================

.. module:: cgkit.asfamc
   :synopsis: Acclaim skeleton and motion file (ASF/AMC) reader

This module provides base classes to read Acclaim Skeleton and Motion Files (ASF/AMC)
containing the definition of a character and its motion. The classes parse
the files and call handler methods that can be implemented in a derived class.

ASFReader class
---------------

The :class:`ASFReader` class reads Acclaim Skeleton Files (ASF) and calls
appropriate methods which have to be implemented in a derived class.


.. class:: ASFReader(filename)

   *filename* is the name of the ASF file that should be read.


.. method:: ASFReader.read()

   Read the entire file.


.. method:: ASFReader.onVersion(version)

   This method is called when the file format version is encountered. *version* is
   a float containing the version number.


.. method:: ASFReader.onName(name)

   This method is called when the skeleton name is read.


.. method:: ASFReader.onUnits(units)

   This method is called when the units section was read. *units* is a dictionary
   that contains all definitions that were present in the units section of the
   input file. The key is the unit name (such as ``mass``, ``length`` and
   ``angle``) and the value is the  corresponding value. If possible the value was
   cast to float, otherwise it's still a string.


.. method:: ASFReader.onDocumentation(doc)

   This method is called when the file documentation was read. *doc* contains the
   documentation (which may contain several lines).


.. method:: ASFReader.onRoot(data)

   This method is called when the root section was read. This section contains
   information about the root of the skeleton. *data* is a dictionary that contains
   all the key-value pairs in the root section. The value is always a tuple (even
   when it's only one single value).


.. method:: ASFReader.onBonedata(bones)

   This method is called after the entire bone data was read. *bones* is a list of
   bone definitions. Each definition is a data dictionary containing the key-value
   pairs in the respective bone section. All values are tuples (even when it's only
   one single value). An exception to this is the ``limits`` attribute which is a
   list of (*min*, *max*) tuples that contain the minumum and maximum limits as
   floats (or as the special strings ``"-inf"`` and ``"inf"``).


.. method:: ASFReader.onHierarchy(links)

   This method is called after the hierarchy section was read. *links* is a list of
   2-tuples (*parent*, *children*) where *parent* is the name of the parent bone
   and *children* is a list of children bone names.

.. % ----------------------------------------------------------------------


AMCReader class
---------------

The :class:`AMCReader` class reads Acclaim Motion Capture Data (AMC) files and
calls :meth:`onFrame` for every motion sample in the file.


.. class:: AMCReader(filename)

   *filename* is the name of the AMC file that should be read.


.. method:: AMCReader.read()

   Read the entire file.


.. method:: AMCReader.onFrame(framenr, data)

   This method is called for every frame. *framenr* is the frame number  and *data*
   is a list of 2-tuples (*bone*, *values*) where *bone* is a bone name and
   *values* the corresponding  position/orientation for this frame. The number of
   values and the meaning of the values is defined in the corresponding ASF file.

