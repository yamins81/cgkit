.. % OFF export


Geomview Object File Format (OFF) export
----------------------------------------

The OFF export plugin writes `Geomview <http://www.geomview.org/>`_  Object File
Format (OFF) files. Currently, only ASCII files can be  exported.

The plugin will export normals, colors and texture coordinates if they are of
storage class varying (colors may also be uniform).

The plugin supports the following options that can be passed to the :func:`save`
command:

+----------+----------+------------------------------+
| Option   | Default  | Description                  |
+==========+==========+==============================+
| ``root`` | ``None`` | Export only a subtree of the |
|          |          | scene.                       |
+----------+----------+------------------------------+

