.. % MayaSpotLight

:class:`MayaSpotLight` --- Maya spot light
==========================================


.. class:: MayaSpotLight(name = "MayaSpotLight", enabled = True, color = (1,1,1), intensity = 1.0, decayRate = 0, coneAngle = 40, penumbraAngle = 40, dropoff = 0, useDepthMapShadows = False, dmapResolution = 512, useMidDistDmap = True, useDmapAutoFocus = True, dmapFocus = 90.0, dmapFilterSize = 1, dmapBias = 0.001, target = (0,0,0))

   *enabled* is a boolean flag that can be used to turn the light source  on or
   off.
   
   *intensity* is the overall intensity of the light source. The higher
   the value, the brighter the light.
   
   *color* defines the color of the light
   source. It must be a sequence  of 3 floats containing RGB values.
   
   ...
   
   *target* is the target point that the light source aims at.

