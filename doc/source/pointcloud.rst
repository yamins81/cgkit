.. _pointcloud:

:mod:`pointcloud` --- Read and write RenderMan point cloud files
================================================================

.. module:: cgkit.pointcloud
   :synopsis: Read and write RenderMan point cloud files


This module allows reading and writing RenderMan point cloud files. The module
relies on an external shared library that implements the actual low-level access
to the point cloud file. This library is not part of cgkit but must be provided
by the renderer package that you are using (for example, PRMan or 3Delight).
Without such a library you won't be able to read or write any point cloud file
using this module.

The module provides one single function :func:`open` which opens a point cloud
file for reading or writing.

.. autofunction:: open(fileName, mode="r", libName=None, ...)


PtcReader object
----------------

A :class:`PtcReader` object as returned by the :func:`open` function handles
reading point cloud files. The object has the following attributes and methods:

.. class:: PtcReader()

   .. attribute:: PtcReader.name

      The file name.


   .. attribute:: PtcReader.variables

      A list of tuples (*type*, *name*) that specifies what additional variables are
      stored in the file. The order of the variables in the file is that of the list.


   .. attribute:: PtcReader.npoints

      The number of points in the file.


   .. attribute:: PtcReader.datasize

      The size of the data part per point (in number of floats).


   .. attribute:: PtcReader.bbox

      A list of 6 floats specifying the bounding box of the point cloud.


   .. attribute:: PtcReader.format

      The format tuple (*xres*, *yres*, *aspect*).


   .. attribute:: PtcReader.world2eye

      The world-to-eye matrix as a list of 16 floats in row-major order.


   .. attribute:: PtcReader.world2ndc

      The world-to-ndc matrix as a list of 16 floats in row-major order.


   .. method:: PtcReader.close()

      Close the point cloud file.


   .. method:: PtcReader.iterAttrs()

      Iterate over all attributes defined in the file. Yields tuples (*name*,
      *value*).


   .. method:: PtcReader.readDataPoint()

      Read the next data point. Returns a tuple (*pos*, *normal*, *radius*,
      *dataDict*) where *pos* and *normal* are 3-tuples of floats, *radius* is a
      single float and *dataDict* a dictionary with the extra variables that are
      attached to the point. If no more point is available an :exc:`EOFError`
      exception is thrown. An :exc:`IOErrror` exception is thrown when an error occurs
      during reading or when the file has already been closed.

      Note that reading a large file using this method will be slow because every
      single point has to be read by a dedicated Python call. If you can process the
      points in batches, you should rather use the :meth:`readDataPoints` method which
      will be a lot faster because a single Python call will read an entire sequence
      of points at once.


   .. method:: PtcReader.readDataPoints(numPoints, buffer)

      Read a sequence of data points. *numPoints* is the number of points to read.
      *buffer* is either a single buffer that will receive all values or a tuple
      (*pointbuf*, *normalbuf*, *radiusbuf*, *databuf*) that contains the individual
      buffers for the respective values. A buffer must always be large enough to hold
      *numPoints* values. The function accepts :mod:`ctypes` arrays as buffers or any
      sequence object that supports the array interface (such as *numpy* arrays).

      The return value is the number of points that have actually been read
      (additional items in the buffers remain at their previous value). When 0 is
      returned, the end of the file has been reached


   .. method:: PtcReader.iterPoints()

      Iterate over all the points in the file. Yields tuples (*point*, *normal*,
      *radius*, *data*) for every point in the file. This is equivalent to calling
      :meth:`readDataPoint` repeatedly.


   .. method:: PtcReader.iterBatches(batchSize=1000, combinedBuffer=False, numpyArray=False)

      Iterate over point batches. Reads *batchSize* points at once and yields one or
      more buffers containing the data. *combinedBuffer* determines whether all data
      is written into one single buffer or if there is an individual buffer for the
      point, normal, radius and data. *numpyArray* determines whether the buffers are
      created as :mod:`numpy` arrays or *ctypes* arrays. The buffers will always
      contain *batchSize* elements unless it is the last buffer returned which may
      have a smaller size.

.. % ----------------------------------------------------------------


PtcWriter object
----------------

A :class:`PtcWriter` object as returned by the :func:`open` function handles
writing point cloude files. The object has the following attributes and methods:

.. class:: PtcWriter()


   .. attribute:: PtcWriter.name

      The file name.


   .. attribute:: PtcWriter.datasize

      The size of the data part per point (in number of floats).


   .. method:: PtcWriter.close()

      Close the point cloud file.


   .. method:: PtcWriter.writeDataPoint(point, normal, radius, data)

      Write a point into the point cloud file. *point* and *normal* are vectors (any
      3-sequence of floats) and *radius* a float. *data* is a dictionary that contains
      the extra variables that must have been declared when the file was opened.
      Undeclared values are ignored, missing declared values are set to 0.


   .. method:: PtcWriter.writeDataPoints(numPoints, buffer)

      Write a sequence of data points. *numPoints* is the number of points to write.
      *buffer* is either a single buffer that contains all values or a tuple
      (*pointbuf*, *normalbuf*, *radiusbuf*, *databuf*) that each contains the
      respective value. The buffers must contain at least *numPoints* items. The
      function accepts :mod:`ctypes` arrays as buffers or any sequence object that
      supports the array interface (such as :mod:`numpy` arrays).

.. % ----------------------------------------------------------------


Examples
--------

You create a new point cloud file like this::

   >>> from cgkit.cgtypes import *
   >>> from cgkit import pointcloud
   >>> ptc = pointcloud.open("cloud.ptc", "w", "3delight", vars=[("float", "foo")],
                             world2eye=mat4(1), world2ndc=mat4(1), format=(640,480,1))
   >>> ptc.name
   'cloud.ptc'
   >>> ptc.datasize
   1

The data size of 1 means there is one additional float attached to each point
which is the extra variable ``foo``. Once the file is open you can write
individual points like this::

   >>> ptc.writeDataPoint((0.5,0.3,0.9), (0,0,1), 1.0, {"foo":0.75})

Writing each point individually will be slow though. If possible you should try
to gather a larger number of points and write them with a single call like this
(for the sake of the example, we just create three points manually)::

   >>> pnts = (9*ctypes.c_float)(1.5,0.5,0.8, 0.8,0.9,1.0, 0.1,0.2,0.3)
   >>> normals = (9*ctypes.c_float)(0,0,1, 1,0,0, 0,1,0)
   >>> rads = (3*ctypes.c_float)(0.6, 0.7, 0.8)
   >>> datas = (3*ptc.datasize*ctypes.c_float)(0.4, 0.3, 0.2)
   >>> ptc.writeDataPoints(3, (pnts,normals,rads,datas))
   >>> ptc.close()

The file can then be read again as follows::

   >>> ptc = pointcloud.open("cloud.ptc", "r", "3delight")
   >>> ptc.name
   'cloud.ptc'
   >>> ptc.npoints
   4
   >>> ptc.datasize
   1
   >>> ptc.variables
   [('float', 'foo')]
   >>> ptc.world2eye
   [1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0]
   >>> ptc.bbox
   [-1.3333333730697632, -1.3333333730697632, -1.3333333730697632, 2.6666667461395264, 2.6666667461395264, 2.6666667461395264]
   >>> ptc.format
   (640.0, 480.0, 1.0)

Once the file is open, the data can be read in various ways. If you want to
process each point individually in Python, you could either use the
:meth:`readDataPoint` method or just iterate over all the points::

   >>> for p in ptc.iterPoints(): print p
   ... 
   ((0.5, 0.30000001192092896, 0.89999997615814209), (0.0, 0.0, 1.0), 1.0, {'foo': 0.75})
   ((1.5, 0.5, 0.80000001192092896), (0.0, 0.0, 1.0), 0.60000002384185791, {'foo': 0.40000000596046448})
   ((0.80000001192092896, 0.89999997615814209, 1.0), (1.0, 0.0, 0.0), 0.69999998807907104, {'foo': 0.30000001192092896})
   ((0.10000000149011612, 0.20000000298023224, 0.30000001192092896), (0.0, 1.0, 0.0), 0.80000001192092896, {'foo': 0.20000000298023224})

Again, this will be slow for large point cloud files. If your application allows
batch processing the points or you interface with C code, it will be faster to
read several points at once (this time we use :mod:`numpy` instead of *ctypes*
for creating a buffer)::

   >>> import numpy
   >>> buf = numpy.zeros(shape=(5, 7+ptc.datasize), dtype=numpy.float32)
   >>> ptc.readDataPoints(5, buf)
   4
   >>> print buf
   [[ 0.5         0.30000001  0.89999998  0.          0.          1.          1.
      0.75      ]
    [ 1.5         0.5         0.80000001  0.          0.          1.
      0.60000002  0.40000001]
    [ 0.80000001  0.89999998  1.          1.          0.          0.
      0.69999999  0.30000001]
    [ 0.1         0.2         0.30000001  0.          1.          0.
      0.80000001  0.2       ]
    [ 0.          0.          0.          0.          0.          0.          0.
      0.        ]]
   >>> ptc.readDataPoints(5, buf)
   0

Instead of a single buffer, you could again pass four individual buffers just
like above when we were writing the file.

You could also leave the buffer creation up to the point cloud handle and just
iterate over buffers (output slightly reformatted for better readability)::

   >>> for buffers in ptc.iterBatches(numpyArray=True): print buffers
   ... 
   (array([[ 0.5       ,  0.30000001,  0.89999998],
          [ 1.5       ,  0.5       ,  0.80000001],
          [ 0.80000001,  0.89999998,  1.        ],
          [ 0.1       ,  0.2       ,  0.30000001]],dtype=float32),
   array([[ 0.,  0.,  1.],
          [ 0.,  0.,  1.],
          [ 1.,  0.,  0.],
          [ 0.,  1.,  0.]], dtype=float32),
   array([ 1.        ,  0.60000002,  0.69999999,  0.80000001], dtype=float32),
   array([[ 0.75      ],
          [ 0.40000001],
          [ 0.30000001],
          [ 0.2       ]], dtype=float32))

