.. % OBJMaterial


.. _objmaterial:

:class:`OBJMaterial` --- OBJ material
=====================================

The :class:`OBJMaterial` class stores the parameters of materials as specified
in a MTL file (which is the material file that accompanies  a Wavefront OBJ
file).


.. class:: OBJMaterial(name = "OBJMaterial",  illum = 2, Ka = (0.2, 0.2, 0.2), Kd = (0.8, 0.8, 0.8), Ks = (0.0, 0.0, 0.0), Ke = (0.0, 0.0, 0.0), Ns = 0.0, Ni = 1.0, d = 1.0, Tr = 1.0, Tf = (1.0, 1.0, 1.0), sharpness = 0.0, map_Ka = None, map_Kd = None, map_Ks = None, map_Ke = None, map_Ns = None, map_d = None, map_Bump = None, density = 1.0 )

   *illum* sepcifies the illumination mode (0=constant, 1=diffuse,
   2=diffuse+specular, ...).

   *Ka* is the ambient color.

   *Kd* is the diffuse color.

   *Ks* is the specular color.

   *Ke* is the emissive color.

   *Ns* is the specular exponent (i.e. the shininess).

   *Ni* is the index of reflection.

   *d*, *Tr* and *Tf* are all transparency values. *d* and *Tr* are given as floats
   and *Tf* as a color.

   *sharpness*...

   The *map_* arguments specify a texture map for the corresponding attribute. The
   value must be a :class:`OBJTextureMap` object.

.. % -----------------------


:class:`OBJTextureMap` --- Texture map definition for the OBJ material
----------------------------------------------------------------------

The :class:`OBJTextureMap` class stores the parameters of texture maps as
specified in an MTL file.


.. class:: OBJTextureMap(filename, offset = (0,0,0), scale = (1,1,1), turb = (0,0,0), mm = (0.0, 1.0), clamp = False, blendu = True, blendv = True, bumpsize = None, refltype = None )

   *filename* is the name of the image map to use.

   *offset* contains three floats that are added to the u, v and w texture
   coordinates, i.e. the map is translated.

   *scale* contains three floats that are used to scale the u, v and w  texture
   coordinates, i.e. to scale the texture map.

   *turb* specifies turbulence values.

   *mm* is a 2-tuple (*bias*, *gain*) that is used to modify the color values of
   the map.

   *clamp*...

   *blendu* and *blendv*...

   *bumpsize* is a float that determines the size of the bumps in a bump map.

   *refltype* is a string that is only used in combination with reflection maps.
   The string determines how the image should be interpreted  ("sphere", ...).

