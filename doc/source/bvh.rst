:mod:`bvh` --- Reading Biovision Hierarchical (BVH) motion capture files
========================================================================

.. module:: cgkit.bvh
   :synopsis: Reading Biovision Hierarchical (BVH) motion capture files


This module contains the :class:`BVHReader` class which can be used as a base
class for reading Biovision Hierarchical (BVH) files. The class reads the file
and invokes callback methods with the corresponding data in the file. Derived
classes have to implement those callback methods and process the data as
appropriate.


.. class:: BVHReader(filename)

   *filename* is the name of the BVH file that should be read.


.. method:: BVHReader.read()

   Read the entire file.


.. method:: BVHReader.onHierarchy(root)

   This method is called after the joint hierarchy was read. The entire  hierarchy
   is passed in the argument *root* which is a :class:`Node` object.


.. method:: BVHReader.onMotion(frames, dt)

   This method is called when the motion data begins. *frames* is the number of
   motion samples that follow and *dt* is the time interval that corresponds to one
   frame.


.. method:: BVHReader.onFrame(values)

   This method is called for each motion sample (frame) in the file. *values* is a
   list of floats that contains the position and angles of the entire skeleton. The
   order is the same than when traversing the joint hierarchy in a depth-first
   manner.

.. % ------------------------------------


Node objects
------------

The :meth:`onHierarchy` method of the :class:`BVHReader` class takes the joint
hierarchy of the skeleton as input. Each node in this hierarchy is represented
by a :class:`Node` object that contains all information stored in the BVH file.


.. class:: Node

   A :class:`Node` object represents one joint in the hierarchy.


.. attribute:: Node.name

   This is the name of the joint (or the root).


.. attribute:: Node.channels

   This is a list of channel names that are associated with this joint. This list
   determines how many values are stored in the motion section and how they are to
   be interpreted. Each channel name can be one of ``Xposition``, ``Yposition``,
   ``Zposition``, ``Xrotation``, ``Yrotation``, ``Zrotation``.


.. attribute:: Node.offset

   This is a 3-tuple of floats containing the offset position of this joint
   relative to the parent joint.


.. attribute:: Node.children

   This is a list of children joints (which are again described by :class:`Node`
   objects).


.. method:: Node.isRoot()

   Returns ``True`` if the node is the root node.


.. method:: Node.isEndSite()

   Returns ``True`` if the node is a leaf.

