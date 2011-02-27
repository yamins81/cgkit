.. % OBJ export


Wavefront OBJ export
--------------------

The OBJ export plugin writes Wavefront OBJ files (including material files).

The plugin supports the following options that can be passed to the :func:`save`
command:

+---------------+----------+-------------------------------+
| Option        | Default  | Description                   |
+===============+==========+===============================+
| ``root``      | ``None`` | Export only a subtree of the  |
|               |          | scene.                        |
+---------------+----------+-------------------------------+
| ``exportmtl`` | ``True`` | Determines whether a MTL file |
|               |          | will be written or not.       |
+---------------+----------+-------------------------------+
| ``mtlname``   | ``None`` | The name of the material file |
|               |          | that should be used.          |
+---------------+----------+-------------------------------+

No material information will be written into the OBJ file if *exportmtl* is
``False`` and *mtlname* is ``None``.

Currently, only the parameters of an :class:`OBJMaterial` object (see section
:ref:`objmaterial`) will be written into the MTL file.  If you want to convert
models from other formats into OBJ and preserve material information, you have
to convert the materials before saving the model.

