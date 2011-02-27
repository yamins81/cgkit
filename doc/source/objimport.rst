.. % OBJ import


Wavefront OBJ import
--------------------

The OBJ import plugin reads Wavefront OBJ files. Currently, only polygonal
models will be imported; freeform surfaces will be ignored. The plugin tries to
recreate the object hierarchy by inspecting the groups that each object belongs
to (this may fail if you have objects with the same name in several places in
the hierarchy). Normals and texture coordinates will be imported as facevarying
variables. The models will be imported as triangle meshes if they only consist
of triangles, otherwise they will be imported as polyhedron objects.

The plugin supports the following options that can be passed to the :func:`load`
command:

+------------+----------+------------------------------+
| Option     | Default  | Description                  |
+============+==========+==============================+
| ``parent`` | ``None`` | Parent object to be used for |
|            |          | the entire imported scene.   |
+------------+----------+------------------------------+

