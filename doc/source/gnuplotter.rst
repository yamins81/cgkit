.. % GnuPlotter component


:class:`GnuPlotter` --- Plot values using Gnuplot
=================================================

The :class:`GnuPlotter` class can be used to plot the graph of a floating point
slot. To do so, connect any :class:`DoubleSlot` to an input slot of the plotter.
The input slots are called ``input<n>_slot`` where ``<n>`` is the number of the
slot.


.. class:: GnuPlotter(name = "GnuPlotter",  title = None,  xlabel = None,  ylabel = None,  xrange = None,  yrange = None,  inputs = 1,  plottitles = [],  starttime = 0.0,  endtime = 99999.0,  enabled = True,  auto_insert = True)

   *title* is a string containing the title of the entire plot.

   *xlabel*, *ylabel* are strings containing the labels for the X and Y axis.

   *xrange*, *yrange* is each a 2-tuple (*start*, *end*)  containing the range of
   the X axis resp. Y axis.

   *inputs* is the number of input slots that should be created (i.e. the number of
   curves you want to plot).

   *plottitles* is a list of strings containing the name of the respective curve.

   *starttime* and *endtime* defines the range in which values are received and
   plotted. The times are given in seconds.

   *enabled* is a flag that can be used to disable the plotter.

A :class:`GnuPlotter` object has the following slots:

+-----------------+-------+--------+--------------+
| Slot            | Type  | Access | Description  |
+=================+=======+========+==============+
| ``input1_slot`` | float | rw     | First curve  |
+-----------------+-------+--------+--------------+
| ``input2_slot`` | float | rw     | Second curve |
+-----------------+-------+--------+--------------+
| ``...``         | ...   | ...    | ...          |
+-----------------+-------+--------+--------------+

.. % --------------------

.. note::

   To use the :class:`GnuPlotter` component the `Gnuplot.py <http://gnuplot-
   py.sourceforge.net/>`_ module has to be  installed on your system (and of
   course,  `gnuplot <http://www.gnuplot.info/>`_ itself as well).

