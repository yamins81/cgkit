:mod:`riutil` --- RenderMan utilities
=====================================

.. module:: cgkit.riutil
   :synopsis: RenderMan utility functions



.. function:: RiuArrow(height=1.0, thickness=0.1, headheight=0.2, headscale=1.7)


.. function:: RiuCoordSystem(thickness=0.06, shader="matte")


.. function:: RiuDefaultHeader()

   Outputs a default header into the RIB stream. This function can be called right
   after :func:`RiBegin` to write the following information into the RIB stream::

      ##RenderMan RIB-Structure 1.1
      ##Creator <Filename>
      ##CreationDate <Date>
      ##For <User>

   The ``"For"`` information is left out if the user name cannot be determined.


.. function:: RiuGrid(thickness=0.02, cells=6, shader="matte", color=(0.9, 0.9, 0.9))

   Outputs a grid primitive. *thickness* determines the thickness of the grid lines
   and *cells* the number of gridlines. The grid lies on the XY plane and is
   centered at the origin. The grid spacing is 1 unit. The grid uses the given
   shader and color.

   .. image:: pics/grid.*
      :width: 6cm
      :align: center
