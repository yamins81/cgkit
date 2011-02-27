--------------------------------------------------------------------------
-- cgkit export v1.0
--------------------------------------------------------------------------
-- Export objects as Python source code. The script exports the following 
-- object types:
--
-- * Spline shapes (only the first spline of a shape is exported)
-- * Cameras
--
-- Usage: After executing the script there is a new utility "cgkit export".
-- The utility has one button "Export selection..." that exports all
-- currently selected objects. Objects that are not supported are ignored.
--
-- (C) 2005 Matthias Baas (baas@ira.uka.de)
-- http://cgkit.sourceforge.net
--------------------------------------------------------------------------

-- Convert an object name to a valid variable name
function name2var name =
(
  toks = filterString name " "
  res = ""
  for i=1 to toks.count do
  (
    res += toks[i]
	if (i<toks.count) do
	  res += "_"
  )
  return res
)

-- Convert a boolean to a Python string
function pyBool b =
(
  if b then
    return "True"
  else
    return "False"
)

--------------------------------------------------------------------
-- Export a shape as cgkit BezierCurve
--------------------------------------------------------------------
function exportShape crv outfile =
(
  crvvar = name2var crv.name

  -- Test for NURBS curves
  if (classOf crv==NURBSCurveShape) do
  (
    format "NURBS curves cannot be exported yet (%).\n" crv.name
	return 0
  )

  -- Check if the shape is already SplineShape or line. If it is not
  -- then make a temporary copy, convert it to a spline shape and use
  -- that for export.
  if (classOf crv==SplineShape) or (classOf crv==line) then
  (
    tmpcrv = crv
    is_copy = false
  )
  else
  (
    -- Make a temporary copy and convert the shape to a spline shape
    tmpcrv = copy crv
	tmpcrv.name = "__tmp_"+tmpcrv.name
	convertToSplineShape tmpcrv
    is_copy = true	
  )

  -- Output the geom
  format "### %\n" crv.name to:outfile
  format "%_geom = BezierCurveGeom( pnts = [\n" crvvar to:outfile
  for i=1 to (numKnots tmpcrv 1) do
  (
    in coordsys local
	(
      p = getKnotPoint tmpcrv 1 i
	  invec = (getInVec tmpcrv 1 i) - p
	  outvec = (getOutVec tmpcrv 1 i) - p
	)
--    format "Pnt %: %  % %\n" i p invec outvec
    format "    BezierPoint((%, %, %)" p.x p.y p.z to:outfile
    format ", intangent=(%, %, %)" invec.x invec.y invec.z to:outfile
	format ", outtangent=(%, %, %)),\n" outvec.x outvec.y outvec.z to:outfile
  )  
  format("    ],\n") to:outfile
  if (isClosed tmpcrv 1) then
    format("    closed = True,\n") to:outfile
  else
    format("    closed = False,\n") to:outfile
  format("    subdiv = %\n") tmpcrv.steps to:outfile
  format(")\n\n") to:outfile
  
  -- Output the WorldObject
  format "% = BezierCurve(\n" crvvar to:outfile
  format "    name      = \"%\",\n" crv.name to:outfile
  format "    curvegeom = %_geom,\n" crvvar to:outfile
  format "    pos       = (%, %, %),\n" crv.pos.x crv.pos.y crv.pos.z to:outfile
  e = crv.rotation as eulerAngles
  format "    rot       = mat3().fromEulerXYZ(radians(%), radians(%), radians(%)),\n" -e.x -e.y -e.z to:outfile
  format "    scale     = (%, %, %)\n" crv.scale.x crv.scale.y crv.scale.z to:outfile
  format ")\n\n" to:outfile
  
  if (is_copy) do
    delete tmpcrv
)

------------------------------------------------------------------
-- Export a camera
------------------------------------------------------------------
function exportCamera cam outfile =
(
  camvar = name2var cam.name

  if cam.type==#free then
    camclass = "FreeCamera"
  else
    camclass = "TargetCamera"
	
  -- Convert fov from horizontal to vertical
  r_aspect = (renderWidth as float)/renderHeight 
  fov = 2.0*atan(tan(cam.fov/2.0)/r_aspect)
	
  format "### %\n" cam.name to:outfile
  format "% = %(\n" camvar camclass to:outfile
  format "    name         = \"%\",\n" cam.name to:outfile
  format "    pos          = (%, %, %),\n" cam.pos.x cam.pos.y cam.pos.z to:outfile
  if cam.type==#target then
    format "    target       = (%, %, %),\n" cam.target.pos.x cam.target.pos.y cam.target.pos.z to:outfile    
  else
  (
    -- Rotate the cam by 180 degrees as in cgkit the viewing axis is the positive z axis
	q = quat 180 [0,1,0]
    e = (cam.rotation*q) as eulerAngles
    format "    rot          = mat3().fromEulerXYZ(radians(%), radians(%), radians(%)),\n" -e.x -e.y -e.z to:outfile  
  )
  format "    fov          = %,\n" fov to:outfile
  format "    auto_nearfar = %,\n" (pyBool (not cam.clipManually)) to:outfile
  format "    nearplane    = %,\n" cam.nearclip to:outfile
  format "    farplane     = %\n" cam.farclip to:outfile
  format ")\n\n" to:outfile
    
)

------------------------------------------------------------------
-- Export all selected objects.
------------------------------------------------------------------
function exportSelection outfile =
(
  for obj in selection do
  (
    -- Is the object a shape?
    if (superClassOf obj==shape) do
	   exportShape obj outfile
	if (superClassOf obj==camera) do
	   exportCamera obj outfile
  )
)

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
utility CGKitExport "cgkit export"
(  
  button exportBtn "Export selection..." width:150 enabled:(selection.count>0)
  label aboutLabel1 "cgkit exporter"
  label aboutLabel2 "(C) 2005 Matthias Baas"
  hyperLink link1 "http://cgkit.sourceforge.net" address:"http://cgkit.sourceforge.net/" color:(color 50 50 230) hoverColor:(color 200 60 130) align:#center
  
  function selectionChangedCallBack = 
  (
    if (selection.count>0) then
    (
   	   exportBtn.enabled = true
    )
	else
	(
  	   exportBtn.enabled = false
	)
  )
  
  on CGKitExport open do
  (
	callbacks.removeScripts id:#mbCGKitExport
	callbacks.addScript #selectionSetChanged  "CGKitExport.selectionChangedCallback()" id:#mbCGKitExport
  )
  
  on CGKitExport close do
  (
	callbacks.removeScripts id:#mbCGKitExport
  )

  -- Export 
  on exportBtn pressed do
  (
    outname = getSaveFileName caption:"Export" filename:"objects.py" types:"Python (*.py)|*.py|All files (*.*)|*.*"
	if (outname!=undefined) do
	(
      -- Open output file 
      out = createFile outname
      if (out==undefined) then
      (
        messagebox ("Could not create file \""+outname+"\".") title:"IO error"
      )
	  else
	  (
	    format "# 3ds max objects\n" to:out
        format "# Creation time: %\n" localTime to:out
		format "\n" to:out
	    exportSelection out
		close out
	  )
	
	)
  )

)
