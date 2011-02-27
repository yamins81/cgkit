
:mod:`stitch` --- Stitching together image tiles
================================================

.. module:: cgkit.stitch
   :synopsis: Stitching together image tiles


This module can be used to stitch together images that have been rendered in
tiles (using RenderMan's :func:`RiCropWindow()` functionality,  for example). If you are
using the render tool to create an image you can use the global value *tile* to
render the image in tiles (see section :ref:`ribexport`). The module can also be
used as a stand-alone command line utility.


.. function:: stitch(filename, removetiles=False, infostream=None)

   Stitches several image tiles together.    *filename* is the base name of the
   image that determines the file names of the tiles. *filename* is also the name
   of the output image.  If *removetiles* is ``True``, the individual image files
   will be deleted  after the image has been stitched. If *infostream* is set to a
   file like object it is used to output status information about the stitching
   process.

   The name of an image tile must contain the crop information that was used to
   create the image. For example, the name of a tile for an image ``"out.tif"`` could
   look like this: ``"out_0.0_0.5_0.75_1.0.tif"``.  The four values are the
   *x1*, *x2*, *y1*, *y2* values of the crop window. Those values together with
   the resolution of the tile determine the resolution of the entire image.
   The position of the tile within that image is given by *x1*, *y1*.

