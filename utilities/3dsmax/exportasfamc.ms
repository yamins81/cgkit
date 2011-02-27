-------------------------------------------------------------------------
-- Export a Biped into the Acclaim skeleton (ASF) and motion file (AMC).
-- (C) 2005 Matthias Baas (baas@ira.uka.de)
-- http://cgkit.sourceforge.net
--
-- Usage: After executing the script, there's a new utility "ASF/AMC Exporter"
-- in the MAXScript rollout.
-- To export a biped motion you first have to export the skeleton as ASF.
-- This can be done by selecting the biped and clicking the "Export 
-- skeleton (ASF)" button. The current pose of the biped is used as default
-- pose so it's a good idea to temporarily switch to figure mode so that the
-- biped is in its default pose. You can specify a scaling factor that applies
-- to the entire skeleton (however, the actual lengths and coordinates 
-- exported won't change, the scaling factor will just appear in the units
-- section of the file).
-- Once a skeleton was exported the "Export motion (AMC)" button is enabled
-- and you can export any number of motions. Only the motion within the
-- current time slider range is exported. Selecting the biped is not required
-- anymore as this was already done when exporting the skeleton. If you
-- want to export motions from another biped you have to export the skeleton
-- of that biped first.
------------------------------------------------------------------------


-- Store the Biped root object
global asf_root = undefined
-- Store the bone arrays
global asf_bones = #()
-- Store the ASF file name
global asf_filename = ""

-------------------------------------------------------------------
-- Filter an object name so that its valid for the output file.
-- (i.e. blanks are replaced with underscores)
-- The return value is the filtered name.
--
-- name: Object name
-------------------------------------------------------------------
function filterName name =
(
  res = ""
  for i in 1 to name.count do
  (
    if name[i]==" " then
	  res = res + "_"
	else
	  res = res + name[i]
  )
	
  return res
)

-------------------------------------------------------------------
-- Return the local transform of a joint.
-------------------------------------------------------------------
function localTransform obj =
(
  if obj.parent!=undefined then
    M = inverse obj.parent.transform
  else
    M = matrix3(1)
  return obj.transform*M
)

-------------------------------------------------------------------
-- Return a list of bones that begin at a particular joint.
-- It is assumed that each children creates one bone, i.e. the bone
-- is implicitly given between obj and the children.
-- Each bone descripion is again an array with the following values:
--
-- bone[1]: Bone name (filtered). This is derived from the name of obj.
-- bone[2]: Start object (joint). This is the passed argument obj.
-- bone[3]: End object (next joint). This is a children of obj.
-- bone[4]: Normalized direction (from start to end)
-- bone[5]: Length
-- bone[6]: Axis (orientation as euler angles)
-- bone[7]: Inverse zero transform 
-------------------------------------------------------------------
function getBones obj =
(
  res = #()
  i = 1
  
  -- Create a list of children that does not contain the "Steps" object
  children = #()
  for c in obj.children do
  (
    if (findString c.name "Schritte")!=undefined do
	  continue
    if (findString c.name "Steps")!=undefined do
	  continue
	 append children c
  )
  
  for c in children do
  (
    if children.count==1 then
	   name = obj.name
	else
	   name = obj.name + "_" + (i as string)
	   
	p1 = obj.transform[4]
    p2 = c.transform[4]
	d = p2-p1
	axis = (obj.transform as eulerAngles)
	append res #(filterName name, obj, c, normalize d, length d, axis, (inverse (localTransform obj)))
	i = i+1
  ) 
  return res
)

-------------------------------------------------------------------
-- Recursive helper function for the writeHierarchySection() function.
-- Each invocation of the function writes one line containing the links.
--
-- obj: Current parent object
-- bonename: Bone name of the bone corresponding to obj
-- out: Output stream (ASF)
-------------------------------------------------------------------
function writeHierarchyRec obj bonename out =
(
  bones = getBones obj
  if bones.count>0 do
  (
    -- Output a line containing the parent and its children  
    format "    %" bonename to:out
	  
    for b in bones do
    (
      format " %" b[1] to:out
    )
    format "\n" to:out

    -- Recursively descend the hierarchy  
    for b in bones do
    (
      writeHierarchyRec b[3] b[1] out
    )  
  ) 
)

---------------------------------------------------------------------------
-- Write the hierarchy section of the ASF file.
-- 
-- bip: Biped root
-- out: Output stream (ASF file)
---------------------------------------------------------------------------
function writeHierarchySection bip out =
(
  format ":hierarchy\n" to:out
  format "  begin\n" to:out
  writeHierarchyRec bip "root" out
  format "  end\n" to:out
)

---------------------------------------------------------------------------
-- Write information for one bone.
--
-- bone: Bone information array (see getBones())
-- out: Output stream (ASF file)
---------------------------------------------------------------------------
function writeBoneData bone out =
(
  dir = bone[4]
  axis = bone[6]
  format "  begin\n" to:out
  format "    name %\n" bone[1] to:out
  format "    direction % % %\n"  dir.x dir.y dir.z to:out
  format "    length %\n" bone[5] to:out
  format "    axis % % % XYZ\n" axis.x axis.y axis.z to:out
  format "    dof rx ry rz\n" to:out
  format "    limits (-inf inf)\n" to:out
  format "           (-inf inf)\n" to:out
  format "           (-inf inf)\n" to:out  
  format "  end\n" to:out
)

---------------------------------------------------------------------------
-- Write bone data recursively.
--
-- obj: Current "joint"
-- out: Output stream (ASF file)
---------------------------------------------------------------------------
function writeBoneDataRec obj out =
(
  global asf_bones
  
  bones = getBones obj
  for b in bones do
  (
    writeBoneData b out
	append asf_bones b
  )
  
  for b in bones do
  (
    writeBoneDataRec b[3] out
  )
)

---------------------------------------------------------------------------
-- Write the BoneData section of the ASF file.
--
-- bip: Biped root
-- out: Output stream (ASF file)
---------------------------------------------------------------------------
function writeBoneDataSection bip out =
(
  global asf_root
  global asf_bones

  asf_root = bip
  asf_bones = #()
  
  format ":bonedata\n" to:out
  
  writeBoneDataRec bip out
)

----------------------------------------------------------------------------
-- Main function to export the ASF file.
--
-- filename: Output file name
-- bip: Biped root
-- scale: Parameter which is written as units.length (the actual coordinates remain unchanged)
----------------------------------------------------------------------------
function exportASF filename bip scale =
(
  global asf_filename
  asf_filename = filename
  
  -- Open output file (ASF)
  asffile = createFile filename
  if (asffile==undefined) do
  (
    messagebox ("Could not create file \""+filename+"\".") title:"IO error"
	return false
  )
  
  -- Write header
  format "# BIP root: %\n" bip.name to:asffile
  format "# Creation time: %\n" localTime to:asffile
  format ":version 1.10\n" to:asffile
  format ":name %\n" bip.name to:asffile
  format ":units\n" to:asffile
  format "  mass 1.0\n" to:asffile
  format "  length %\n" scale to:asffile
  format "  angle deg\n" to:asffile

  format ":root\n" to:asffile
  format "  order TX TY TZ RX RY RZ\n" to:asffile
  format "  axis XYZ\n" to:asffile
  p = bip.transform[4]
  format "  position % % %\n" p.x p.y p.z to:asffile
  format "  orientation 0 0 0 \n" to:asffile
 
  writeBoneDataSection bip asffile
  writeHierarchySection bip asffile
 
  -- Close the output file
  close asffile
)

----------------------------------------------------------------------------
-- Main function to export the AMC file.
-- This may only be called if exportASF was previously called (where the
-- global variables asf_root and asf_bones was initialized).
--
-- filename: Output file name
----------------------------------------------------------------------------
function exportAMC filename =
(
  global asf_root
  global asf_bones
  global asf_filename
  
  -- Open output file (AMC)
  amcfile = createFile filename
  if (amcfile==undefined) do
  (
    messagebox ("Could not create file \""+filename+"\".") title:"IO error"
	return false
  )

  a = filterString asf_filename "\\"
  asfname = a[a.count]

  -- Write header
  format "# BIP: %\n" asf_root.name to:amcfile
  format "# Creation time: %\n" localTime to:amcfile
  format "# ASF: %\n" asfname to:amcfile
  format ":fully-specified\n" to:amcfile
  format ":degrees\n" to:amcfile

  -- Export the angles for each frame of the timeline
  for t in animationRange.start to animationRange.end do
  (
    format "%\n" (t.frame as integer) to:amcfile
	at time t 
	(
	  -- Write the root position
	  -- (the orientation will be applied to the first bone)
	  p = asf_root.transform[4]
--      e = (asf_root.transform) as eulerAngles
      format "root % % % 0 0 0\n" p.x p.y p.z to:amcfile
	  -- Write the orientation for each bone
	  for b in asf_bones do
	  (
	    name = b[1]
		L = localTransform b[2]
	    e = (L*b[7]) as eulerAngles
	    format "% % % %\n" name e.x e.y e.z to:amcfile
	  )
	)
  )


  -- Close the output file
  close amcfile  
)


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
utility ASFAMCExport "ASF/AMC Exporter v1.0"
(  
  group "Biped export"
  (
    spinner scaleSpinner "Scale:" range:[0.0001,1000,1.0] type:#float 
    button exportASFBtn "Export skeleton (ASF)" width:190 enabled:((selection.count==1) and (classof selection[1]==Biped_Object))
    button exportAMCBtn "Export motion (AMC)" width:190 enabled:false
    label bipedLabel "Biped: <none>" 
  )
  label aboutLabel1 "ASF/AMC file exporter"
  label aboutLabel2 "(C) 2005 Matthias Baas"
  hyperLink link1 "http://cgkit.sourceforge.net" address:"http://cgkit.sourceforge.net/" color:(color 50 50 230) hoverColor:(color 200 60 130) align:#center
  
  function selectionChangedCallBack = 
  (
    if (selection.count==1) and (classof selection[1]==Biped_Object) then
    (
   	   exportASFBtn.enabled = true
    )
	else
	(
  	   exportASFBtn.enabled = false
	)
  )
  
  on ASFAMCExport open do
  (
	callbacks.removeScripts id:#mbASFAMCExport
	callbacks.addScript #selectionSetChanged  "ASFAMCExport.selectionChangedCallback()" id:#mbASFAMCExport
  )
  
  on ASFAMCExport close do
  (
	callbacks.removeScripts id:#mbASFAMCExport
  )

  -- Export ASF
  on exportASFBtn pressed do
  (
    asfname = getSaveFileName caption:"Export skeleton (ASF)" filename:"bip.asf" types:"ASF (*.asf)|*.asf|All files (*.*)|*.*"
	if (asfname!=undefined) do
	(
	  bip = selection[1]
	  -- Search root biped object
	  while (bip.parent!=undefined) and (classof bip.parent==Biped_Object) do
	    bip = bip.parent
		
	  bipedLabel.text = "Biped: "+bip.name
	  format "Exporting biped \"%\" into \"%\"..." (bip.name) asfname
      exportASF asfname bip scaleSpinner.value
	  format "OK\n"
      exportAMCBtn.enabled = true
	  when bip deleted do
	  (
        exportAMCBtn.enabled = false
        bipedLabel.text = "Biped: <none>"
	  )
	)
  )

  -- Export AMC
  on exportAMCBtn pressed do
  (
    amcname = getSaveFileName caption:"Export motion (AMC)" filename:"bip.amc" types:"AMC (*.amc)|*.amc|All files (*.*)|*.*"
	if (amcname!=undefined) do
	(
 	  format "Exporting motion into \"%\"..." amcname
      exportAMC amcname
	  format "OK\n"	  
	)
--    global asf_bones
--	format "------------------------\n"
--	for b in asf_bones do
--	  format "%\n" b[1]
  )
)

