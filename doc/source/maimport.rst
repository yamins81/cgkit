.. % MA import


Maya ASCII (MA) import
----------------------

The MA import plugin reads Maya ASCII files. The plugin does not (and never
will) support all nodes that can be stored in a Maya ASCII file as this would
mean the entire Maya functionality had to be implemented in cgkit.  So only
those nodes are imported that have an equivalent component in cgkit.  Currently,
these are cameras, light sources, raw geometry (polys, NURBS (wip)) and
skeletons. Polygonal spheres, cubes and planes will be imported as true spheres,
cubes, planes if their "creator" node is maintained. In all other cases, you
should delete history on your objects in order to see the geometry in cgkit. You
can also create  active and passive rigid bodies in Maya which will prepare
those objects for simulation with ODE. However, a solver component is not
created automatically (this means you have to create an :class:`ODEDynamics`
object manually).

The plugin supports the following options that can be passed to the :func:`load`
command:

+------------+----------+------------------------------+
| Option     | Default  | Description                  |
+============+==========+==============================+
| ``parent`` | ``None`` | Parent object to be used for |
|            |          | the entire scene.            |
+------------+----------+------------------------------+

Note: The MA importer is still work in progress.

