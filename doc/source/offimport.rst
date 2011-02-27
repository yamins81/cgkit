.. % OFF import


Geomview Object File Format (OFF) import
----------------------------------------

The OFF import plugin reads `Geomview <http://www.geomview.org/>`_  Object File
Format (OFF) files. Currently, only ASCII files can be read whereas binary files
will not be recognized.

If present the imported model will have normals, colors and texture coordinates
which will all be of storage class *varying*. An exception is the color which
may also be stored as *uniform*. The color is ignored if it is specified via a
colormap index. An optional alpha value will also be ignored.

The plugin supports the following options that can be passed to the :func:`load`
command:

+-----------------+-----------+------------------------------+
| Option          | Default   | Description                  |
+=================+===========+==============================+
| ``invertfaces`` | ``False`` | Invert the face orientation. |
+-----------------+-----------+------------------------------+

