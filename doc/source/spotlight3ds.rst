.. % SpotLight3DS


:class:`SpotLight3DS` --- 3DS spot light
========================================

The :class:`SpotLight3DS` class stores all parameters of the spot lights as they
are stored in 3DS files. The main purpose of this class is to ensure that data
doesn't get lost during file conversions.

This light sources can also be used for RenderMan renderings. However, not all
of the parameters are currently taken into account in the corresponding
RenderMan shader (see below).


.. class:: SpotLight3DS(name = "SpotLight3DS",  enabled = True, intensity = 1.0, color = (1,1,1), see_cone = False, roll = 0.0, outer_range = 0, inner_range = 0, attenuation = 0, rectangular_spot = 0, shadowed = False, shadow_bias = 0, shadow_filter = 4.0, shadow_size = 256, spot_aspect = 0, use_projector = False, projector = 0, overshoot = False, ray_shadows = False, ray_bias = False, hotspot = 43, falloff = 45, target = (0,0,0) )

   *enabled* is a boolean flag that can be used to turn the light source on or off.

   *intensity* is the overall intensity of the light source. The higher the value,
   the brighter the light.

   *color* defines the color of the light source. It must be a sequence of 3 floats
   containing RGB values.

   *see_cone* ...

   *roll* ...

   *inner_range* and *outer_range* specify an intensity range based on distance.
   Any parts of the scene that are nearer to the light source than *inner_range*
   are fully illuminated. Within the range between *inner_range* and *outer_range*
   the brightness drops off to 0 and objects further than *outer_range* are not
   illuminated at all. These range values are only taken into account when
   *attenuation* is not 0.

   *rectangular_spot* ...

   If *shadowed* is set to ``True`` the light casts a shadow.

   *shadow_bias* is a small value greater than 0 that is used to prevent invalid
   self-shadowing (i.e. so that a surface element doesn't shadow itself).

   *shadow_filter* specifies the size of the filter when doing shadow map lookups.
   The higher the value the blurrier the shadow.

   *shadow_size* defines the size of the shadow map. The shadow map will always be
   square and have a width of *shadow_size* pixels.

   *spot_aspect* ...

   *use_projector* ...

   *projector* ...

   If *overshoot* is ``True`` the light virtually becomes a point light source,
   i.e. it also illuminates the parts of the scene that lie outside its cone but
   the shadow is still restricted to the cone.

   *ray_shadows* ...

   *ray_bias* ...

   *hotspot* This is the angle (in degrees) of the "inner" cone which is fully
   illuminated.

   *falloff* This is the angle (in degrees) of the "outer" cone. The light
   intensity between the inner and outer cone drops off to 0. The region outside
   the outer cone is not illuminated (unless *overshoot* is activated).

   *target* is the target point that the light source aims at.

The following parameters are used in the corresponding RenderMan shader, all
other parameters are currently ignored:

* intensity

* color

* falloff

* hotspot

* attenuation

* inner_range

* outer_range

* overshoot

* shadow_bias

* shadow_filter

* shadow_size

