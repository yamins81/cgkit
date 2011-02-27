# ***** BEGIN LICENSE BLOCK *****
# Version: MPL 1.1/GPL 2.0/LGPL 2.1
#
# The contents of this file are subject to the Mozilla Public License Version
# 1.1 (the "License"); you may not use this file except in compliance with
# the License. You may obtain a copy of the License at
# http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# The Original Code is the Python Computer Graphics Kit.
#
# The Initial Developer of the Original Code is Matthias Baas.
# Portions created by the Initial Developer are Copyright (C) 2004
# the Initial Developer. All Rights Reserved.
#
# Contributor(s):
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 2 or later (the "GPL"), or
# the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
# in which case the provisions of the GPL or the LGPL are applicable instead
# of those above. If you wish to allow use of your version of this file only
# under the terms of either the GPL or the LGPL, and not to allow others to
# use your version of this file under the terms of the MPL, indicate your
# decision by deleting the provisions above and replace them with the notice
# and other provisions required by the GPL or the LGPL. If you do not delete
# the provisions above, a recipient may use your version of this file under
# the terms of any one of the MPL, the GPL or the LGPL.
#
# ***** END LICENSE BLOCK *****
# $Id: ribexport.py,v 1.17 2006/04/27 16:56:46 mbaas Exp $

import sys, os, os.path, shutil, types
import protocols
import pluginmanager
import lightsource
#from cgkit import *
from cgtypes import *
from targetcamera import TargetCamera
from scene import getScene
from geomobject import *
from boxgeom import BoxGeom
from spheregeom import SphereGeom
from ccylindergeom import CCylinderGeom
from torusgeom import TorusGeom
from planegeom import PlaneGeom
from trimeshgeom import TriMeshGeom
from polyhedrongeom import PolyhedronGeom
from spotlight3ds import SpotLight3DS
from glpointlight import GLPointLight
from gltargetspotlight import GLTargetSpotLight
from glfreespotlight import GLFreeSpotLight
from gltargetdistantlight import GLTargetDistantLight
from glfreedistantlight import GLFreeDistantLight
from glmaterial import GLMaterial
from cgkit.Interfaces import *
from _OpenGL.GL import GL_REPLACE, GL_MODULATE, GL_DECAL, GL_BLEND
from _OpenGL.GL import *
import cmds
from ri import *
import sl
from riutil import *
from math import *

#class IObject(protocols.Interface): pass
class IGeometry(protocols.Interface): pass
class ILightSource(protocols.Interface): pass
class IMaterial(protocols.Interface): pass

# RIBImporter
class RIBExporter:
    """RIB export class.
    """

    _protocols = ["Export"]

    # extension
    def extension():
        return ["rib"]
    extension = staticmethod(extension)

    # description
    def description(self):
        """Return a short description for the file dialog."""
        return "RenderMan RIB file"
    description = staticmethod(description)

    # exportFile
    def exportFile(self,
                   filename,
                   camera = None,
                   output = None,
                   output_framebuffer = True,
                   bake = False,
                   bakemodel = None,
                   bakestvar = "st"):
        """Export a RIB file.

        \a camera is the camera object to use. If None is passed, the
        first camera found is used.
        \a output is the output file name or a list of output specifiers
                  (which contain the parameters for a RiDisplay() call). If
                  output is None, no RiDisplay() call is done.
        """

        scene = getScene()
        self.timestep = scene.timer().timestep
        frameNr = int(round(scene.timer().frame))
        self.bake = bake

        # A list with all light sources in the scene
        # (the items are the original lights, *not* the adapted lights)
        self.lights = []
        
        # A dictionary that stores lightsource shader names
        # Key is the light class, value is the shader name
        self.light_shader = {}

        # A dictionary that stores the passes created by light sources
        # Key is the original light instance, value is the passes list
        # Empty lists are not stored.
        self.light_passes = {}

        # A dictionary that stores material shader names
        # Key is the tuple (material class, surface shader name, displacement
        # shader name, interior name), value are the shader names (surface,
        # displacement, interior)
        # (the shader names in the key are not the true file names but
        # the names returned by the material)
        self.material_shaders = {}

        # A dictionary that stores the passes created by materials
        # Key is the original material instance, value is the passes list
        # Empty lists are not stored.
        self.material_passes = {}

        # A dictionary with map names used so far
        self.mapnames = {}

        # The shader names created so far (this dictionary is used to
        # create unique file names)
        self.shader_names = {}

        # Key: (geom, matid)  Value: File name
        self.geom_file = {}
        
        # The geom file names creates so far
        # (this dictionary is used to create unique file names)
        self.geom_names = {}

        # Directory names
        self.shader_path = "shaders"
        self.map_path = "maps"
        self.geom_path = "geoms"

        # The list with render passes
        passes = []

        # Search for a camera and collect the passes...
        cam = camera
        self.preprocess_called = {}
        for obj in scene.walkWorld():
            if cam==None and ICamera in obj.protocols():
                cam = obj

            # Check if the object is a light source
            try:
                explgt = protocols.adapt(obj, ILightSource)
                self.lights.append(obj)
                # Create the RenderPass objects required for this light
                lgtpasses = explgt.createPasses()
                lgtpasses = self.makePassOutputUnique(lgtpasses)
                if lgtpasses!=[]:
                    self.light_passes[obj] = lgtpasses
                    passes += lgtpasses
            except NotImplementedError:
                # TODO: Use protocols instead of isinstance()
                if isinstance(obj, lightsource.LightSource):
                    print 'Unknown light source: "%s"'%obj.name

            if obj.geom==None:
                continue

            # Collect the material passes
            if obj.visible:
                for i in range(obj.getNumMaterials()):
                    mat = obj.getMaterial(i)
                    if mat not in self.material_passes:
                        try:
                            expmat = protocols.adapt(mat, IMaterial)
                            # Create the RenderPass objects required for the material
                            ps = expmat.createPasses()
                            ps = self.makePassOutputUnique(ps)
                            if ps!=[]:
                                self.material_passes[mat] = ps
                                passes += ps
                            if mat not in self.preprocess_called:
                                expmat.preProcess(self)
                                self.preprocess_called[mat] = 1
                        except NotImplementedError:
                            pass

        if cam==None:
            cam = TargetCamera()

        # Store the camera so that Pass objects can access it
        self.cam = cam

        # Add the map path to the map names
        # (on win32, a backslash is replaced by two backslashes because
        # a renderer might interpret a single backslash as an escape character
        # (such as 3Delight))
        for ps in passes:
            tab = ps.getFilenameTable()
            for vname in tab:
                rname = tab[vname]
                rname = os.path.join(self.map_path, rname)
                if sys.platform=="win32":
                    rname = rname.replace('\\', "\\\\")
                tab[vname] = rname
            ps.setFilenameTable(tab)

        if len(passes)>0:
            self.checkMapPath()

        # Add the final image pass as last pass
#        if output==None:
#            output = scene.getGlobal("output", "out.tif")
        if bake:
            defaultdisplaymode = RI_RGBA
        else:
            defaultdisplaymode = RI_RGB
        mode = scene.getGlobal("displaymode", defaultdisplaymode)
        dispType = scene.getGlobal("displaytype", RI_FILE)
        out = self.outputSpec(output, mode=mode, output_framebuffer=output_framebuffer, display=dispType)
        if bake:
            bakemodel = cmds.worldObject(bakemodel)
            bakepass = BakePass(output=out, bakemodel=bakemodel, bakestvar=bakestvar)
            passes.append(bakepass)
        else:
            imgpass = ImagePass(output=out, cam=cam)
            passes.append(imgpass)

        # Initialize the exporter attribute in the passes...
        for p in passes:
            p.exporter = self

        # Start export...
        RiBegin(filename)

        RiOption("searchpath", "shader", "%s:&"%self.shader_path)
        RiOption("searchpath", "archive", "%s:&"%self.geom_path)
        RiOption("searchpath", "texture", "%s:&"%self.map_path)
        RiHider(RI_HIDDEN, "string depthfilter", "midpoint")
        globalrib = scene.getGlobal("rib", None)
        if globalrib!=None:
            RiArchiveRecord(RI_VERBATIM, globalrib+"\n")

        # Do render passes...
        print len(passes),"passes..."
#        nr = 0
        # Tex passes first
        for rpass in passes:
            if isinstance(rpass, TexPass):
#                nr+=1
#                print "\015Pass %d..."%nr,
                rpass.doPass(frameNr)
                rpass._done = True

        # Other passes...
        for rpass in passes:
            if isinstance(rpass, TexPass):
                continue
#            nr+=1
#            print "\015Pass %d..."%nr,
            rpass.doPass(frameNr)
            rpass._done = True

        RiEnd()

    ## protected:

    # outputSpec
    def outputSpec(self, output, mode=RI_RGB, output_framebuffer=False, display=RI_FILE):
        """Return the output specification for the final image pass.

        output is the parameter passed to exportFile().
        mode and output_framebuffer is only used when output is a string.
        display is the the display type.
        The return value is a list of output specs which can be
        passed as output parameter to the ImagePass.
        """
        # No output?
        if output==None:
            return []
        # Is output a string? (i.e. the output file name)
        elif isinstance(output, types.StringTypes):
            out = [(output, display, mode, {})]
            if output_framebuffer:
                out += [(output, RI_FRAMEBUFFER, mode, {})]
            return out
        # User specified output specs? (output must already be a list
        # of specs)
        else:
            return output
        

    # isExportable
    def isExportable(self, wobj):
        """Check if a WorldObject can be exported or not.

        ??? Geometry?
        """

        geom = wobj.geom
        if geom==None:
            return False
        
        try:
            expgeom = protocols.adapt(geom, IGeometry)
            return True
        except NotImplementedError:
            print '"%s" has unknown geometry.'%wobj.name
            return False
        
    # applyViewTransform
    def applyViewTransform(self, V):
        """Apply the view transformation V.

        This method corresponds to a RiConcatTransform() call.
        It outputs the view transformation V as a RenderMan transformation.
        The handedness of the scene is taken into account.

        \param V (\c mat4) View transformation (world -> camera)
        """
        scene = getScene()
        if scene.handedness=='r':
            RiScale(-1,1,1)                
        RiConcatTransform(V.toList())

    # applyLightSource
    def applyLightSource(self, lgt):
        """Apply the light source lgt.

        This method corresponds to a RiLightSource() call.

        \param lgt (\c WorldObject) Light source
        \return Light id or None.
        """

#        try:
        explgt = protocols.adapt(lgt, ILightSource)
#        except NotImplementedError:
#            return

        # Save the light shader if it wasn't already saved before...
        cls = "%s_%s"%(lgt.__class__, explgt.shaderName())
        if cls not in self.light_shader:
            shadername = self.writeShader(explgt.shaderName(),
                                          explgt.shaderSource())
            self.light_shader[cls] = shadername

        # Get the shader name
        shadername = self.light_shader[cls]
        if shadername==None:
            return None

        # Get the parameters for the shader
        passes = self.light_passes.get(lgt, [])
        params = explgt.shaderParams(passes)

        lid = RiLightSource(shadername, params)
        return lid

    # applyTransformation
    def applyTransformation(self, T, linearvel=None, angularvel=None):
        """Apply the transformation T.

        This method corresponds to a RiConcatTransform() call.
        It outputs T as a RenderMan transformation.

        \param T (\c mat4) Transformation
        """
        RiConcatTransform(T.toList())
        if linearvel!=None and angularvel!=None:
            try:
                axis = angularvel.normalize()
            except:
                axis = vec3(1,0,0)
            dw = 0.5*self.timestep*degrees(angularvel.length())
            dr = 0.5*self.timestep*linearvel
            if abs(dw)>1E-6:
                RiMotionBegin(0, 1)
                RiRotate(-dw, axis)
                RiRotate( dw, axis)
                RiMotionEnd()
            if dr!=vec3(0):
                RiMotionBegin(0,1)
                RiTranslate(-dr)
                RiTranslate( dr)
                RiMotionEnd()

    # applyMaterial
    def applyMaterial(self, mat):
        """Apply a material.

        This method corresponds to the calls RiColor(), RiOpacity(),
        RiSurface(), RiDisplacement() and RiInterior().

        Evtl. Flags als Parameter, die bestimmten welche der Shader
        wirklich ausgegeben werden sollen (Surface, Displacement, Volume).
        Z.B. koennte Shadow-Pass auf Surface verzichten, evtl. aber nicht
        auf Displacement.

        \param mat (\c Material) Material
        """
        if mat==None:
            return

#        try:
        expmat = protocols.adapt(mat, IMaterial)
#        except NotImplementedError:
#            return

        # Save the shaders if they weren't already saved before...
        cls = (mat.__class__,
               expmat.surfaceShaderName(),
               expmat.displacementShaderName(),
               expmat.interiorShaderName())
        if cls not in self.material_shaders:
            # Prepare the material shaders (save shader source)
            srfshader = self.writeShader(expmat.surfaceShaderName(),
                                         expmat.surfaceShaderSource())
            dispshader = self.writeShader(expmat.displacementShaderName(),
                                          expmat.displacementShaderSource())
            intrshader = self.writeShader(expmat.interiorShaderName(),
                                          expmat.interiorShaderSource())
            self.material_shaders[cls] = (srfshader, dispshader, intrshader)

        # Get the shader names
        srfshader, dispshader, intrshader = self.material_shaders[cls]

        # Get the parameters for the shader
        passes = self.material_passes.get(mat, [])
        srfparams = expmat.surfaceShaderParams(passes)
        dispparams = expmat.displacementShaderParams(passes)
        interiorparams = expmat.interiorShaderParams(passes)

        # Get the shader transforms
        srftransform = expmat.surfaceShaderTransform()
        disptransform = expmat.displacementShaderTransform()
        interiortransform = expmat.interiorShaderTransform()

        color = expmat.color()
        opacity = expmat.opacity()

        if color!=None:
            RiColor(color)
        if opacity!=None:
            RiOpacity(opacity)
        if srfshader!=None:
            if srftransform!=mat4(1):
                RiTransformBegin()
                RiConcatTransform(srftransform.toList())
            RiSurface(srfshader, srfparams)
            if srftransform!=mat4(1):
                RiTransformEnd()
        if dispshader!=None:
            if disptransform!=mat4(1):
                RiTransformBegin()
                RiConcatTransform(disptransform.toList())
            coordsys, bound = expmat.displacementBound()
            RiAttribute("displacementbound",
                        "string coordinatesystem", coordsys,
                        "float sphere", bound)
            RiDisplacement(dispshader, dispparams)
            if disptransform!=mat4(1):
                RiTransformEnd()
        if intrshader!=None:
            if interiortransform!=mat4(1):
                RiTransformBegin()
                RiConcatTransform(interiortransform.toList())
            RiInterior(intrshader, interiorparams)
            if interiortransform!=mat4(1):
                RiTransformEnd()
 

    # applyGeometry
    def applyGeometry(self, geom, matid=0):
        """Apply a geometry object.

        \param geom (\c GeomObject) Geometry or None
        """
        if geom==None:
            return

        # Was the geom already exported?
        if (geom, matid) in self.geom_file:
            filename = self.geom_file[geom, matid]
            RiReadArchive(filename)
            return

        # The geom was not exported, so do it now...

        try:
            expgeom = protocols.adapt(geom, IGeometry)
        except NotImplementedError:
            print 'Warning: Unknown geometry: "%s" (%s)'%(geom.name, geom.__class__.__name__)
            return

        # Create the geoms directory if it doesn't exist
        if not os.path.exists(self.geom_path):
            os.mkdir(self.geom_path)

        # Determine the output file name (without path)
        n = "%s_id%d_%s.rib"%(geom.__class__.__name__, matid, geom.name)
        filename = self.makeFilenameUnique(n, self.geom_names)
        self.geom_names[filename] = 1
        self.geom_file[geom, matid] = filename

        ctx = RiGetContext()
        RiBegin(os.path.join(self.geom_path, filename))
        RiOption(RI_RIBOUTPUT, RI_VERSION, 0)
        expgeom.render(matid)
        RiEnd()
        RiContext(ctx)
        RiReadArchive(filename)        


    # writeShader
    def writeShader(self, name, source):
        """Write a shader source file.

        \param name (\c str) Initial shader name or None
        \param source (\c str) Shader source code or None
        \return Shader name
        """
        # No source given? Then just use the specified shader name
        if source==None:
            return name

        if name==None:
            name = "shader"

        shadername = self.makeFilenameUnique(name, self.shader_names)
        self.shader_names[shadername]=1

        # Create the shaders directory if it doesn't exist
        if not os.path.exists(self.shader_path):
            os.mkdir(self.shader_path)

        filename = os.path.join(self.shader_path, shadername+".sl")
        f = file(filename, "wt")
        # Add the macros that are used for baking...
        if self.bake:
            f.write("#define BAKE_PASS\n")
            begin = 'varying point _P_bake = transform("world", "current", transform("camera", "object", Pref));\\\n  varying point _P_orig = P;\\\n  P = _P_bake;'
            end = 'P = _P_orig;'
            normal = 'normalize(n);'
        else:
            begin = ""
            end = ""
            normal = 'faceforward(normalize(n),I);'
        f.write("#define BAKE_BEGIN %s\n"%begin)
        f.write("#define BAKE_END %s\n"%end)
        f.write("#define BAKE_NORMAL(n) %s\n\n"%normal)
            
        f.write(source.replace("$SHADERNAME", shadername))
        f.close()
        return shadername

    # checkMapPath
    def checkMapPath(self):
        """Create the map directory if it doesn't already exist.
        """
        if not os.path.exists(self.map_path):
            os.mkdir(self.map_path)


    # makePassOutputUnique
    def makePassOutputUnique(self, passes):
        res = []
        for rpass in passes:
            tab = rpass.getFilenameTable()
            for vname in tab:
                rname = self.makeFilenameUnique(tab[vname], self.mapnames)
                tab[vname] = rname
                self.mapnames[rname] = 1
#            print tab
            rpass.setFilenameTable(tab)
            res.append(rpass)
            
        return res

    # makeFilenameUnique
    def makeFilenameUnique(self, name, names):
        """Modify a file name so that it is unique.

        If \a name is already among \a names it is modified by increasing
        a trailing number so that it is unique.

        \param name (\c str) File name
        \param names A list or dictionary with existing file names
        \return Unique file name
        """
        base, ext = os.path.splitext(name)
        # Read any trailing number
        # -> base: name without number (and extension) / num: Number
        i = 1
        while i<=len(base) and base[-i:].isdigit():
            i+=1
        i-=1
        if i==0:
            num = 1
        else:
            num = int(base[-i:])
            base = base[:-i]
        
        while name in names:
            name = "%s%d%s"%(base, num, ext)
            num += 1
            
        return name
        
        

######################################################################

# RenderPass
class RenderPass:
    """RenderPass.

    A pass object can access the exporter (RIBExport) via self.exporter.
    """
    def __init__(self, output, owner=None):
        """Constructor.
        
        output is a list of output specifications that contain the
        output file name, the output type and the output mode (RI_RGB,
        RI_RGBA,...) and optional extra parameters. It's all the
        information that's necessary for a RiDisplay() call.
			
        \param owner (\c Component) The object that requires the result of this pass
        """
        self.owner = owner
        self.output = output
        # A reference to the exporter class (this is initialized in the
        # exporter after the Passes were created)
        self.exporter = None

        # done flag (this is set to True in the exporter)
        self._done = False

        # Filename table. Key: Logical file name / Value: Real filename
        self.filenametab = {}
        for name,type,mode,params in output:
            self.filenametab[name] = name

    # done
    def done(self):
        """Check if this pass is already done or not.

        This method is used when shader parameters have to be determined.
        The output of a pass may only be used once the pass is done and
        the output really exists (for example, you can't use a shadow map
        until it was created).

        \return True if the pass was already rendered (i.e. the output images exist).
        """
        return self._done

    # realFilename
    def realFilename(self, filename):
        """Translate a logical file name into a real file name.

        \param filename (\c str) Logical file name
        """
        if filename not in self.filenametab:
            raise ValueError('File name "%s" is not an output file name.'%filename)

        return self.filenametab[filename]

    # getFilenameTable
    def getFilenameTable(self):
        """Return the filename translation table.

        \return The filename translation dictionary.
        """
        return self.filenametab

    # setFilenameTable
    def setFilenameTable(self, tab):
        """Set an updated filename table.

        \param tab (\c dict) The new filename translation dictionary.
        """
        self.filenametab = tab

    # doPass
    def doPass(self, framenr):
        """Write a Frame block.

        \param framenr (\c int) Frame number to use
        """
        pass

    # initDisplays
    def initDisplays(self):
        """Call RiDisplay() for all outputs stored in the class."""
        # Do RiDisplay() calls...
        append_plus = False
        for name,type,mode,params in self.output:
            name = self.realFilename(name)
            if append_plus:
                name = "+"+name
            RiDisplay(name, type, mode, params)
            append_plus = True

        

# ImagePass
class ImagePass(RenderPass):
    def __init__(self, output, cam):
        RenderPass.__init__(self, output, None)
        self.cam = cam

    # doPass
    def doPass(self, framenr):
        scene = getScene()

        RiArchiveRecord(RI_COMMENT, "")
        RiArchiveRecord(RI_COMMENT, "Image pass")
        RiArchiveRecord(RI_COMMENT, "")
        RiFrameBegin(framenr)

        i,j = scene.getGlobal("pixelsamples", (2,2))
        RiPixelSamples(i,j)
        res = scene.getGlobal("resolution", (640,480))
        try:
            if len(res)==2:
                w,h = res
                aspect = 1
            elif len(res)==3:
                w,h,aspect = res
            else:
                raise Exception
        except:
            print >>sys.stderr, "Error: Invalid resolution setting:",res
            w,h,aspect = 640,480,1
        RiFormat(w,h,aspect)
        RiShadingRate(scene.getGlobal("shadingrate", 1.0))
        
        filterdata = scene.getGlobal("pixelfilter", None)
        if filterdata!=None:
            filter, (xwidth, ywidth) = filterdata
            RiPixelFilter(filter, xwidth, ywidth)

        # Do RiDisplay() calls...
        self.initDisplays()

        # Camera...
        cam = self.cam
        fstop = getattr(cam, "fstop", 0)
        focallength = getattr(cam, "focallength", 0)
        RiProjection(RI_PERSPECTIVE, fov=cam.fov)
        if fstop!=0 and focallength!=0:
            # XXX: The following line only works with a TargetCamera!
            focaldistance = (cam.target-cam.pos).length()
            RiDepthOfField(fstop, focallength, focaldistance)
        self.exporter.applyViewTransform(cam.viewTransformation())
        RiShutter(0,1)

        RiWorldBegin()

        # Set light sources...
        for lgt in self.exporter.lights:
            RiAttributeBegin()
            RiAttribute("identifier", "name", lgt.name)
            RiConcatTransform(lgt.worldtransform.toList())
            # Custom RIB
            rib = getattr(lgt, "rib", None)
            if rib is not None:
                RiArchiveRecord(RI_VERBATIM, rib+"\n")
            lid = self.exporter.applyLightSource(lgt)
            RiAttributeEnd()
            if lid!=None:
                RiIlluminate(lid, lgt.enabled)
            
#            shader,params,transform,name = exporter.light_shaders[lgt]
#            lid = RiLightSource(shader, params)

        self.renderChilds(scene.worldRoot())
        
        RiWorldEnd()     
        RiFrameEnd()

    def renderChilds(self, obj):
        exporter = self.exporter
        for child in obj.iterChilds():
            if not child.visible:
                continue
            # No geometry and no children? Then ignore this node
            if child.geom==None and child.lenChilds()==0:
                continue
#            if not exporter.isExportable(child):
#                continue

            RiAttributeBegin()
            
            # Store the name of the object
            RiAttribute("identifier", "name", child.name)
            # Transform the object
            exporter.applyTransformation(child.localTransform(), child.linearvel, child.angularvel)

            for i in range(child.getNumMaterials()):
                # Set shaders...
                exporter.applyMaterial(child.getMaterial(i))

                # Custom RIB
                rib = getattr(child, "rib", None)
                if rib!=None:
                    RiArchiveRecord(RI_VERBATIM, rib+"\n")

                # Output geometry
                exporter.applyGeometry(child.geom, i)
            
            # Recursively render all childs
            self.renderChilds(child)
            
            RiAttributeEnd()


# ShadowPass
class ShadowPass(RenderPass):
    def __init__(self, output, light, fov, resolution, orientoffset=mat4(1)):
        RenderPass.__init__(self, output, None)
        self.light = light
        self.fov = fov
        self.resolution = resolution
        self.orientoffset = orientoffset

    # doPass
    def doPass(self, framenr):
        scene = getScene()

        RiArchiveRecord(RI_COMMENT, "")
        RiArchiveRecord(RI_COMMENT, "Shadow pass")
        RiArchiveRecord(RI_COMMENT, "")
        RiFrameBegin(framenr)

        RiPixelSamples(1,1)
        RiPixelFilter(RiBoxFilter, 1, 1)
        RiFormat(self.resolution, self.resolution, 1)
        RiShadingRate(2)

        # Do RiDisplay() calls...
        self.initDisplays()

        # Camera...
        RiProjection(RI_PERSPECTIVE, fov=self.fov)
        V = (self.light.transform*self.orientoffset).inverse()
        self.exporter.applyViewTransform(V)
        RiShutter(0,1)

        RiWorldBegin()

        self.renderChilds(scene.worldRoot())
        
        RiWorldEnd()     
        RiFrameEnd()
        zname = self.realFilename(self.output[0][0])
        mapname = os.path.splitext(zname)[0]+".map"
        RiMakeShadow(zname, mapname)

    def renderChilds(self, obj):
        exporter = self.exporter
        for child in obj.iterChilds():
            if not child.visible:
                continue
            # No geometry and no children? Then ignore this node
            if child.geom==None and child.lenChilds()==0:
                continue
#            if not exporter.isExportable(child):
#                continue

            RiAttributeBegin()

            # Store the name of the object
            RiAttribute("identifier", "name", child.name)
            # Transform the object
            exporter.applyTransformation(child.localTransform())
            for i in range(child.getNumMaterials()):
                # Set shaders...
                exporter.applyMaterial(child.getMaterial(i))

                # Custom RIB
                rib = getattr(child, "rib", None)
                if rib!=None:
                    RiArchiveRecord(RI_VERBATIM, rib+"\n")

                # Output geometry
                exporter.applyGeometry(child.geom, i)
            
            # Recursively render all childs
            self.renderChilds(child)

            RiAttributeEnd()

# FlatReflectionPass
class FlatReflectionPass(RenderPass):
    def __init__(self, output, mirrorobj=None):
        RenderPass.__init__(self, output, None)
        self.mirrorobj = mirrorobj

    # doPass
    def doPass(self, framenr):
        scene = getScene()

        RiArchiveRecord(RI_COMMENT, "(Flat) Reflection pass")
        RiFrameBegin(framenr)

        RiPixelSamples(2,2)
        RiFormat(640,480,1)
        RiShadingRate(1)

        # Do RiDisplay() calls...
        self.initDisplays()

        # Camera...
        cam = self.exporter.cam
        RiProjection(RI_PERSPECTIVE, fov=cam.fov)
        self.exporter.applyViewTransform(cam.viewTransformation())
        RiScale(1,1,-1)
        RiShutter(0,1)
#        RiTranslate(0,0,1)

        RiWorldBegin()

        # Set light sources...
        for lgt in self.exporter.lights:
            RiAttributeBegin()
            RiAttribute("identifier", "name", lgt.name)
            RiConcatTransform(lgt.worldtransform.toList())
            lid = self.exporter.applyLightSource(lgt)
            RiAttributeEnd()
            if lid!=None:
                RiIlluminate(lid, RI_TRUE)
            
#            shader,params,transform,name = exporter.light_shaders[lgt]
#            lid = RiLightSource(shader, params)

        self.renderChilds(scene.worldRoot())
        
        RiWorldEnd()     
        RiFrameEnd()
        tifname = self.realFilename(self.output[0][0])
        texname = os.path.splitext(tifname)[0]+".tex"
        RiMakeTexture(tifname, texname, RI_CLAMP, RI_CLAMP, RiGaussianFilter, 1, 1)


    def renderChilds(self, obj):
        exporter = self.exporter
        for child in obj.iterChilds():
            if child==self.mirrorobj:
                continue
            if not child.visible:
                continue
#            if not exporter.isExportable(child):
#                continue

            RiAttributeBegin()
            
            # Store the name of the object
            RiAttribute("identifier", "name", child.name)
            # Transform the object
            exporter.applyTransformation(child.localTransform(), child.linearvel, child.angularvel)
            for i in range(child.getNumMaterials()):
                # Set shaders...
                exporter.applyMaterial(child.getMaterial(i))

                # Custom RIB
                rib = getattr(child, "rib", None)
                if rib!=None:
                    RiArchiveRecord(RI_VERBATIM, rib+"\n")

                # Output geometry
                exporter.applyGeometry(child.geom, i)
            
            # Recursively render all childs
            self.renderChilds(child)
            
            RiAttributeEnd()

# BakePass
class BakePass(RenderPass):
    """Create a bake pass.

    The class uses the technique described in the "Stupid RAT Tricks 2001":
    "The RenderMan EasyBake Oven" by Jonathan Litt and Dan Goldman.
    http://www.cs.washington.edu/homes/dgoldman/ezbake/EZ_Bake_Oven.htm

    Additionally, the original geometry is also stored so that raytracing
    can also be used.
    """
    
    def __init__(self, output, bakemodel, bakestvar="st"):
        """
        bakemodel is the WorldObject whose texture should be baked.
        bakestvar is the name of the variable that holds the texture
        coordinates for baking.
        """
        RenderPass.__init__(self, output, None)
        self.bakemodel = bakemodel
        self.bakestvar = bakestvar

    # doPass
    def doPass(self, framenr):
        scene = getScene()

        RiArchiveRecord(RI_COMMENT, "")
        RiArchiveRecord(RI_COMMENT, "Bake pass")
        RiArchiveRecord(RI_COMMENT, "")
        RiFrameBegin(framenr)

        i,j = scene.getGlobal("pixelsamples", (2,2))
        RiPixelSamples(i,j)
        res = scene.getGlobal("resolution", (256,256))
        try:
            if len(res)==2:
                w,h = res
                aspect = 1
            elif len(res)==3:
                w,h,aspect = res
            else:
                raise Exception
        except:
            print >>sys.stderr, "Error: Invalid resolution setting:",res
            w,h,aspect = 256,256,1
        RiFormat(w,h,aspect)
        RiShadingRate(scene.getGlobal("shadingrate", 1.0))

        # Do RiDisplay() calls...
        self.initDisplays()

        # Camera...
        RiProjection(RI_ORTHOGRAPHIC)
        bb = scene.boundingBox()
        bmin, bmax = bb.getBounds()
        RiScale(2,2,2)
        RiTranslate(-0.5, -0.5, -bmax.z-1)

        RiWorldBegin()

        # Set light sources...
        for lgt in self.exporter.lights:
            RiAttributeBegin()
            RiAttribute("identifier", "name", lgt.name)
            RiConcatTransform(lgt.worldtransform.toList())
            lid = self.exporter.applyLightSource(lgt)
            RiAttributeEnd()
            if lid!=None:
                RiIlluminate(lid, lgt.enabled)

        # Flattened geometry
        obj = self.bakemodel
        RiAttributeBegin()
        self.exporter.applyMaterial(obj.getMaterial(0))
#        RiCoordSysTransform("camera")
#        RiScale(2,2,2)
#        RiTranslate(-0.5,-0.5,1)
        RiTranslate(0,0, bmax.z+1.5)
        self.bakeModel(obj, self.bakestvar)
        RiAttributeEnd()

        # 3Delight attribute...
        RiAttribute("visibility", "string transmission", "opaque")
        # Pixie attribute...
        RiAttribute("visibility", "int trace", 1)
        
        self.renderChilds(scene.worldRoot())

        RiWorldEnd()     
        RiFrameEnd()

    def bakeModel(self, obj, stvarname="st"):
        """
        obj is the model that should be baked. stvarname is the name
        of the variable that holds the texture coordinates for baking.
        """

        # Check texture coordinates...
        info = obj.geom.findVariable(stvarname)
        if info==None:
            raise RuntimeError, "No variable '%s' found"%stvarname
            
        name, storage, type, mult = info

        if type!=FLOAT or mult!=2:
            raise TypeError, "variable '%s' is of wrong type"%stvarname

        if storage==FACEVARYING:
            geom = cmds.convFaceVarToVar(obj.geom)
        elif storage==VARYING:
            geom = obj.geom
        else:
            raise TypeError, "'%s' storage class %s not supported for baking"%(stvarname,storage)

        # Convert the model into a trimesh...
        if not isinstance(geom, TriMeshGeom):
            tm = TriMeshGeom()
            geom.convert(tm)
            geom = tm
        
        # Convert the tex coords into vec3s...
        st = geom.slot(stvarname)
#        print "sizes:",geom.verts.size(), st.size()
        stcoords = map(lambda x: vec3(x), st)

        # Transform the verts...
#        verts = []
#        W = obj.worldtransform
#        for i,j,k in geom.faces:
#            verts.append(W*geom.verts[i])
#            verts.append(W*geom.verts[j])
#            verts.append(W*geom.verts[k])

#        verts = 156*[(0,0,0)]

        # Create parameter list...
        W = obj.worldtransform
        params = {"P":stcoords,
                  "Pref":map(lambda x: W*x, geom.verts)}
        RiDeclare("Pref", "vertex point")
        clss = ["constant", "uniform", "varying", "vertex", "facevarying", "facevertex", "user"]
        typs = ["integer", "float", "string", "color", "point", "vector",
                "normal", "matrix", "hpoint"]
        for name, storage, type, multiplicity in geom.iterVariables():
            cls = clss[abs(storage)]
            typ = typs[abs(type)]
            if cls!="user":
                s = geom.slot(name)
                params[name] = list(s)
                if multiplicity==1:
                    decl = "%s %s"%(cls, typ)
                else:
                    decl = "%s %s[%d]"%(cls, typ, multiplicity)
                RiDeclare(name, decl)

#        RiCoordSysTransform("camera")
        RiPointsPolygons(len(geom.faces)*[3], list(geom.faces), params)

    def renderChilds(self, obj):
        exporter = self.exporter
        for child in obj.iterChilds():
            if not child.visible:
                continue
            # No geometry and no children? Then ignore this node
            if child.geom==None and child.lenChilds()==0:
                continue
#            if not exporter.isExportable(child):
#                continue

            RiAttributeBegin()
            
            # Store the name of the object
            RiAttribute("identifier", "name", child.name)
            # Transform the object
            exporter.applyTransformation(child.localTransform(), child.linearvel, child.angularvel)

            for i in range(child.getNumMaterials()):
                # Set shaders...
                exporter.applyMaterial(child.getMaterial(i))

                # Custom RIB
                rib = getattr(child, "rib", None)
                if rib!=None:
                    RiArchiveRecord(RI_VERBATIM, rib+"\n")

                # Output geometry
                exporter.applyGeometry(child.geom, i)
            
            # Recursively render all childs
            self.renderChilds(child)
            
            RiAttributeEnd()


# TexPass
class TexPass(RenderPass):
    def __init__(self, maps, output=[]):
        """Constructor.

        The map definition list contains one tuple for every map.
        The tuple is a 7-tuple with the following entries:

        - Map name (incl. path)
        - swrap ("periodic", "clamp", "black")
        - twrap ("periodic", "clamp", "black")
        - filterfunc ("gaussian", "box", "triangle", "sinc", "catmullrom", ...)
        - swidth
        - twidth
        - params - A dictionary with additional parameters

        These are the parameters for the \c RiMakeTexture() call.
        The output file name is generated from the input map name by
        replacing the suffix with "*.tex".
        
        \param maps A list with map defintions
        """
        RenderPass.__init__(self, output)
        self.maps = maps

    def doPass(self, framenr):
        
        for map,swrap,twrap,filterfunc,swidth,twidth,params in self.maps:

            # Copy the original image into the map directory and convert
            # to tif if necessary
            self.copyImageMap(map)

            # Call RiMakeTexture() to convert the .tif into .tex
            mapbase = os.path.basename(map)
            name, ext = os.path.splitext(mapbase)
            tifname = os.path.join(self.exporter.map_path, name)+".tif"
            texname = os.path.join(self.exporter.map_path, name)+".tex"
            if sys.platform=="win32":
                tifname = tifname.replace("\\", "\\\\")
                texname = texname.replace("\\", "\\\\")
            RiMakeTexture(tifname, texname,
                          swrap, twrap,
                          filterfunc, swidth, twidth, **params)

    ## protected:

    def copyImageMap(self, texmap):
        """Copy the texture map image into the map folder.

        \param texmap (\c str) Texture map name (incl. path)
        """

        self.exporter.checkMapPath()
        texname = os.path.basename(texmap)
        name, ext = os.path.splitext(texname)
        if ext.lower()!=".tif":
            print 'Converting "%s"'%texmap
            tifname = os.path.join(self.exporter.map_path, name+".tif")
            # Read original map
            try:
                img = Image.open(texmap)
            except IOError, e:
                print e
                return
            # Save map as TIF file
            img.save(tifname)
        else:
            print 'Copying "%s"'%texmap
            dest = os.path.join(self.exporter.map_path, os.path.basename(texmap))
            shutil.copyfile(texmap, dest)


######################################################################
####################### Geometry adapters ############################
######################################################################

class BoxAdapter:
    protocols.advise(instancesProvide=[IGeometry], asAdapterForTypes=[BoxGeom])
    
    def __init__(self, boxgeom, proto):
        self.geom = boxgeom

    def render(self, matid):
        if matid!=0:
            return
        
        lx2 = self.geom.lx/2
        ly2 = self.geom.ly/2
        lz2 = self.geom.lz/2
        A = vec3(-lx2,-ly2,-lz2)
        B = vec3( lx2,-ly2,-lz2)
        C = vec3( lx2, ly2,-lz2)
        D = vec3(-lx2, ly2,-lz2)
        E = vec3(-lx2,-ly2, lz2)
        F = vec3( lx2,-ly2, lz2)
        G = vec3( lx2, ly2, lz2)
        H = vec3(-lx2, ly2, lz2)
        RiPatch(RI_BILINEAR, P=[D,C,A,B])
        RiPatch(RI_BILINEAR, P=[A,B,E,F])
        RiPatch(RI_BILINEAR, P=[D,A,H,E])
        RiPatch(RI_BILINEAR, P=[C,D,G,H])
        RiPatch(RI_BILINEAR, P=[B,C,F,G])
        RiPatch(RI_BILINEAR, P=[E,F,H,G])


class SphereAdapter:
    protocols.advise(instancesProvide=[IGeometry], asAdapterForTypes=[SphereGeom])
    
    def __init__(self, spheregeom, proto):
        self.geom = spheregeom

    def render(self, matid):
        if matid==0:
            r = self.geom.radius
            RiSphere(r, -r, r, 360)
        

class CCylinderAdapter:
    protocols.advise(instancesProvide=[IGeometry], asAdapterForTypes=[CCylinderGeom])
    
    def __init__(self, ccylgeom, proto):
        self.geom = ccylgeom

    def render(self, matid):
        if matid!=0:
            return
        
        r = self.geom.radius
        l = self.geom.length
        l2 = l/2
        silhouette_len = pi*r+l
        cap_len = pi/2*r;
        v1 = cap_len/silhouette_len
        v2 = v1+l/silhouette_len
        
        RiCylinder(r, -l2, l2, 360, st=[0,v1, 1,v1, 0,v2, 1,v2])
        RiTransformBegin()
        RiTranslate(0,0,l2)
        RiSphere(r, 0, r, 360, st=[0,v2, 1,v2, 0,1, 1,1])
        RiTransformEnd()
        RiTransformBegin()
        RiTranslate(0,0,-l2)
        RiSphere(r, -r, 0, 360, st=[0,0, 1,0, 0,v1, 1,v1])
        RiTransformEnd()

class TorusAdapter:
    protocols.advise(instancesProvide=[IGeometry], asAdapterForTypes=[TorusGeom])
    
    def __init__(self, torusgeom, proto):
        self.geom = torusgeom

    def render(self, matid):
        if matid==0:
            RiTorus(self.geom.major, self.geom.minor, 0, 360, 360)

class PlaneAdapter:
    protocols.advise(instancesProvide=[IGeometry], asAdapterForTypes=[PlaneGeom])
    
    def __init__(self, planegeom, proto):
        self.geom = planegeom

    def render(self, matid):
        if matid!=0:
            return
        
        lx2 = self.geom.lx/2
        ly2 = self.geom.ly/2
        A = vec3(-lx2,-ly2,0)
        B = vec3( lx2,-ly2,0)
        C = vec3( lx2, ly2,0)
        D = vec3(-lx2, ly2,0)
        RiPatch(RI_BILINEAR, P=[A,B,D,C])

class TriMeshAdapter:
    protocols.advise(instancesProvide=[IGeometry], asAdapterForTypes=[TriMeshGeom])
    
    def __init__(self, meshgeom, proto):
        self.geom = meshgeom

    def render(self, matid):
        if self.geom.findVariable("matid")!=None:
            tm = cmds.extractUniform(self.geom, "matid", matid)
            tm = cmds.removeIsolatedVertices(tm)
        else:
            if matid!=0:
                return
            tm = self.geom
                    
        if len(tm.faces)==0:
            return

        if tm.findVariable("N")!=None and tm.findVariable("Nfaces")!=None:
            tm = cmds.convMeshAttr(tm, "N", "Nfaces")

        # Create parameter list...
        params = {"P":list(tm.verts)}
        clss = ["constant", "uniform", "varying", "vertex", "facevarying", "facevertex", "user"]
        typs = ["integer", "float", "string", "color", "point", "vector",
               "normal", "matrix", "hpoint"]
        for name, storage, type, multiplicity in tm.iterVariables():
            cls = clss[abs(storage)]
            typ = typs[abs(type)]
            if cls!="user":
                s = tm.slot(name)
                params[name] = list(s)
                if multiplicity==1:
                    decl = "%s %s"%(cls, typ)
                else:
                    decl = "%s %s[%d]"%(cls, typ, multiplicity)
                RiDeclare(name, decl)
                
        if hasattr(self.geom, "subdiv"):
            RiSubdivisionMesh("catmull-clark", len(tm.faces)*[3],
                              list(tm.faces), [],0,[],[], params)
        else:
            RiPointsPolygons(len(tm.faces)*[3], list(tm.faces), params)

class PolyhedronAdapter:
    protocols.advise(instancesProvide=[IGeometry], asAdapterForTypes=[PolyhedronGeom])
    
    def __init__(self, polyhedrongeom, proto):
        self.geom = polyhedrongeom

    def render(self, matid):
        if matid!=0:
            return

        pg = self.geom

        # Create parameter list...
        params = {"P":list(pg.verts)}
        clss = ["constant", "uniform", "varying", "vertex", "facevarying", "facevertex", "user"]
        typs = ["integer", "float", "string", "color", "point", "vector",
               "normal", "matrix", "hpoint"]
        for name, storage, type, multiplicity in pg.iterVariables():
            cls = clss[abs(storage)]
            typ = typs[abs(type)]
            if cls!="user":
                s = pg.slot(name)
                params[name] = list(s)
                if multiplicity==1:
                    decl = "%s %s"%(cls, typ)
                else:
                    decl = "%s %s[%d]"%(cls, typ, multiplicity)
                RiDeclare(name, decl)

        nloops = []
        nverts = []
        vertids = []
        for i in range(pg.getNumPolys()):
            poly = pg.getPoly(i)
            nloops.append(len(poly))
            for loop in poly:
                nverts.append(len(loop))
                vertids.append(loop)

        if hasattr(self.geom, "subdiv"):
            if nloops==len(nloops)*[1]:
                RiSubdivisionMesh("catmull-clark", nverts, vertids,
                                  [], 0, [], [], params)
            else:
                print >>sys.stderr, "Warning: Polyhedron with holes in its polys can't be used as subdiv"
        else:
            RiPointsGeneralPolygons(nloops, nverts, vertids, params)


######################################################################
######################### Light adapters #############################
######################################################################

# SpotLight3DSAdapter
class SpotLight3DSAdapter:
    protocols.advise(instancesProvide=[ILightSource], asAdapterForTypes=[SpotLight3DS])
    
    def __init__(self, lgt, proto):
        self.obj = lgt

    def createPasses(self):
        """Returns a list of RenderPass objects."""
        if self.obj.shadowed:
            shadpass = ShadowPass(output=[("shadow.z", "zfile", RI_Z, {})],
                                  light=self.obj, fov=self.obj.falloff,
                                  resolution = self.obj.shadow_size)
            return [shadpass]

        return []

    def shaderName(self):
        """Returns the name of the corresponding light shader or None.
        """
        return "spotlight3ds"

    def shaderSource(self):
        """Returns surface shader source code as a string or None.
        """
        return """// SpotLight3DS shader
        
light $SHADERNAME(float intensity = 1.0;
         uniform color lightcolor = color "rgb" (1, 1, 1);
         float falloff = 45.0;
         float hotspot = 43.0;
         float attenuation = 0;
         float inner_range = 0;
         float outer_range = 999;
         float overshoot = 0;
         float shadow_filter = 4.0;
         float shadow_bias = 0.01;
         float shadow_samples = 16;
         string shadowname = "")
{
  uniform float cosfalloff = cos(radians(0.5*falloff));
  uniform float coshotspot = cos(radians(0.5*hotspot));
  uniform vector axis = normalize(vector "shader" (0,0,1));
  float illuminate_angle;

  if (overshoot==0)
  {
    illuminate_angle = falloff*PI/360.0;
  }
  else
  {
    illuminate_angle = PI;
  }
  
  illuminate(point "shader" (0,0,0), axis, illuminate_angle)
  {
    vector L0 = normalize(L);
    float dist = length(L);
    float att = 1.0;

    // Attenuation due to hotspot/falloff
    if (overshoot==0)
    {
      float ca = L0.axis;
      att = smoothstep(cosfalloff, coshotspot, ca);
    }

    // Attenuation due to distance
    if (attenuation!=0)
      att *= 1.0-smoothstep(inner_range, outer_range, dist);
    
    Cl = att * intensity * lightcolor;

    if (shadowname!="")
      Cl *= 1 - shadow(shadowname, Ps, "width", shadow_filter, "bias", shadow_bias, "samples", shadow_samples);
    
  }
}        
"""

    def shaderParams(self, passes):
        """Return a dictionary with shader parameters and their values."""
        params = {"intensity":self.obj.intensity,
                "uniform color lightcolor":self.obj.color,
                "uniform float falloff":self.obj.falloff,
                "uniform float attenuation":self.obj.attenuation,
                "uniform float inner_range":self.obj.inner_range,
                "uniform float outer_range":self.obj.outer_range,
                "uniform float hotspot":self.obj.hotspot,
                "uniform float overshoot":int(self.obj.overshoot)
                  }
        if self.obj.shadowed and passes[0].done():
            zfile = passes[0].realFilename(passes[0].output[0][0])
            mapname = os.path.splitext(zfile)[0]+".map"
            params["uniform string shadowname"] = mapname
            params["uniform float shadow_filter"] = self.obj.shadow_filter
            params["uniform float shadow_bias"] = self.obj.shadow_bias
        return params



# GLPointLightAdapter
class GLPointLightAdapter:
    protocols.advise(instancesProvide=[ILightSource], asAdapterForTypes=[GLPointLight])
    
    def __init__(self, lgt, proto):
        self.obj = lgt

    def createPasses(self):
        """Returns a list of RenderPass objects."""
        return []

    def shaderName(self):
        """Returns the name of the corresponding light shader or None.
        """
        return "glpointlight"

    def shaderSource(self):
        """Returns surface shader source code as a string or None.
        """
        return """// GLPointLight shader
light $SHADERNAME(float intensity = 1.0;
         color diffuse = color "rgb" (1, 1, 1);
         color specular = color "rgb" (1, 1, 1);
         float constant_attenuation = 1;
         float linear_attenuation = 0;
         float quadratic_attenuation = 0)         
{
  illuminate(point "shader" (0,0,0))
  {
    float d = length(L);
    float att = 1.0/(constant_attenuation + linear_attenuation*d + quadratic_attenuation*d*d);
    
    Cl = att*intensity*color "rgb" (1,1,1);
  }
}        
"""

    def shaderParams(self, passes):
        """Return a dictionary with shader parameters and their values."""
        return {"intensity":self.obj.intensity,
                "uniform color diffuse":self.obj.diffuse,
                "uniform color specular":self.obj.specular,
                "uniform float constant_attenuation":self.obj.constant_attenuation,
                "uniform float linear_attenuation":self.obj.linear_attenuation,
                "uniform float quadratic_attenuation":self.obj.quadratic_attenuation}


# GLSpotLightAdapter
class GLSpotLightAdapter:
    protocols.advise(instancesProvide=[ILightSource], asAdapterForTypes=[GLTargetSpotLight, GLFreeSpotLight])
    
    def __init__(self, lgt, proto):
        self.obj = lgt

    def createPasses(self):
        """Returns a list of RenderPass objects."""
        if hasattr(self.obj, "shadowmap"):
            res,width,blur,bias,samples = self.obj.shadowmap
            shadpass = ShadowPass(output=[("shadow.z", "zfile", RI_Z, {})],
                                  light=self.obj, fov=self.obj.cutoff,
                                  resolution = res)
            return [shadpass]

        return []

    def shaderName(self):
        """Returns the name of the corresponding light shader or None.
        """
        return "glspotlight"

    def shaderSource(self):
        """Returns surface shader source code as a string or None.
        """
        return """// GLSpotLight shader
light $SHADERNAME(float intensity = 1.0;
         uniform color diffuse = color "rgb" (1, 1, 1);
         uniform color specular = color "rgb" (1, 1, 1);
         float constant_attenuation = 1;
         float linear_attenuation = 0;
         float quadratic_attenuation = 0;
         float exponent = 0.0;
         float cutoff = 45.0;
         string shadowname = "";
         float width = 1.0;
         float blur = 0.0;
         float bias = 0.1;
         float samples = 16;)
{
  float cutoff_rad = cutoff*PI/180.0;
  
  illuminate(point "shader" (0,0,0), vector "shader" (0,0,1), cutoff_rad)
  {
    float d = length(L);
    float att = 1.0/(constant_attenuation + linear_attenuation*d + quadratic_attenuation*d*d);
    
    vector Lshad = normalize(vtransform("shader", L));
    float sp = max(zcomp(Lshad), 0);
    float spot = 0;
    if (sp>=cos(cutoff_rad))
    {
      spot = pow(sp, exponent);
    }
    
    Cl = att * spot * intensity * color "rgb" (1,1,1);

    if (shadowname!="")
      Cl *= 1 - shadow(shadowname, Ps, "blur", blur, "width", width, "bias", bias, "samples", samples);
    
  }
}        
"""

    def shaderParams(self, passes):
        """Return a dictionary with shader parameters and their values."""
        params = {"intensity":self.obj.intensity,
                "uniform color diffuse":self.obj.diffuse,
                "uniform color specular":self.obj.specular,
                "uniform float constant_attenuation":self.obj.constant_attenuation,
                "uniform float linear_attenuation":self.obj.linear_attenuation,
                "uniform float quadratic_attenuation":self.obj.quadratic_attenuation,
                "uniform float exponent":self.obj.exponent,
                "uniform float cutoff":self.obj.cutoff}
        if hasattr(self.obj, "shadowmap") and passes[0].done():
            res,width,blur,bias,samples = self.obj.shadowmap
            zfile = passes[0].realFilename(passes[0].output[0][0])
            mapname = os.path.splitext(zfile)[0]+".map"
            params["uniform string shadowname"] = mapname
            params["uniform float width"] = width
            params["uniform float blur"] = blur
            params["uniform float bias"] = bias
            params["uniform float samples"] = samples
        return params

# GLDistantLightAdapter
class GLDistantLightAdapter:
    protocols.advise(instancesProvide=[ILightSource], asAdapterForTypes=[GLTargetDistantLight, GLFreeDistantLight])
    
    def __init__(self, lgt, proto):
        self.obj = lgt

    def createPasses(self):
        """Returns a list of RenderPass objects."""
        return []

    def shaderName(self):
        """Returns the name of the corresponding light shader or None.
        """
        return "gldistantlight"

    def shaderSource(self):
        """Returns surface shader source code as a string or None.
        """
        return """// GLDistantLight shader
light $SHADERNAME(float intensity = 1.0;
         uniform color diffuse = color "rgb" (1, 1, 1);
         uniform color specular = color "rgb" (1, 1, 1))
{
  solar(vector "shader" (0,0,1), PI/2)
  {
    Cl = intensity * color "rgb" (1,1,1);
  }
}        
"""

    def shaderParams(self, passes):
        """Return a dictionary with shader parameters and their values."""
        return {"intensity":self.obj.intensity,
                "uniform color diffuse":self.obj.diffuse,
                "uniform color specular":self.obj.specular}


######################################################################
####################### Material adapters ############################
######################################################################


# GLMaterialAdapter
class GLMaterialAdapter:
    """
    """

    protocols.advise(instancesProvide=[IMaterial], asAdapterForTypes=[GLMaterial])

    def __init__(self, material, proto):
        self.mat = material

    def createPasses(self):
        """Returns a list of RenderPass objects."""
        if self.mat.getTexture(0)==None:
            return []
        
        tex = self.mat.getTexture(0)
        return [TexPass(maps=[(tex.imagename, "periodic", "periodic", "gaussian", 1, 1, {})])]

    def preProcess(self, exporter):
        """Preprocessing method.

        This method is called before the image is rendered and can be used
        to create or copy image maps.
        """
        pass

    def color(self):
        """Return the color for the RiColor() call or None.
        """
        return self.mat.diffuse

    def opacity(self):
        """Return the opacity for the RiOpacity() call or None.
        """
        return None
#        a = self.mat.diffuse.w
#        return (a,a,a)

    def surfaceShaderName(self):
        """Returns the name of the corresponding surface shader or None.
        """
        return "glmaterial"

    def surfaceShaderSource(self):
        """Returns surface shader source code as a string or None.

        If the return value is None, then shaderName() must return
        the name of the shader to use.
        """
        ambient = self.mat.ambient
        emission = self.mat.emission
        return """// GLMaterial shader
surface $SHADERNAME(color ambient = color "rgb" (0.2, 0.2, 0.2);
           color specular = color "rgb" (0, 0, 0);
           float shininess = 0.0;
           color emission = color "rgb" (0, 0, 0);
           float diffuse_alpha = 1.0;
           string texname = "";
           float T[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
           float mode = 0;
           color texenvcolor = color "rgb" (1,1,1);
           float texenvcolor_alpha = 1;
           float blend_sfactor = -1;
           float blend_dfactor = -1;
           varying point Pref = point(0,0,0);
           )
{
  BAKE_BEGIN
  normal Nf = BAKE_NORMAL(N);
  vector V = normalize(-I);
  color diffuse = Cs;

  Ci = emission + 0*ambient;
  illuminance(P, Nf, PI/2)
  {
    vector L0 = normalize(L);
    vector S = normalize(L0+V);
    float NL = max(Nf.L0, 0);
    float fi = (NL!=0)? 1 : 0;

    color diffuse_lgt = color "rgb" (1,1,1);
    color specular_lgt = color "rgb" (1,1,1);
    lightsource("diffuse", diffuse_lgt);
    lightsource("specular", specular_lgt);
    
    Ci += Cl*(diffuse*diffuse_lgt*NL +
              fi*specular*specular_lgt*pow(max(Nf.S, 0), shininess) );
  }

  if (texname!="")
  {
    float ss = T[0]*s + T[1]*t + T[3];
    float tt = T[4]*s + T[5]*t + T[7];
    float dn = T[12]*s + T[13]*t + T[15];
    color texcolor = texture(texname, ss/dn, tt/dn);
    if (mode==0)  // GL_REPLACE
    {
      Ci = texcolor;
    }
    else if (mode==1)  // GL_MODULATE
    {
      Ci = texcolor*Ci;
    }
    else if (mode==2)  // GL_DECAL
    {
      Ci = texcolor;
    }
    else  // GL_BLEND
    {
      Ci = Ci*(color "rgb" (1,1,1)-texcolor) + texenvcolor*texcolor;
    }
  }

  Oi = 1;

  if (blend_sfactor!=-1 && blend_dfactor!=-1)
  {
    color S = 0;
    color D = 0;

    if (blend_sfactor==1)
      S = 1;
    else if (blend_sfactor==3)
      S = Ci;
    else if (blend_sfactor==5)
      S = 1-Ci;
    else if (blend_sfactor==6)
      S = diffuse_alpha;
    else if (blend_sfactor==7)
      S = 1-diffuse_alpha;

    if (blend_dfactor==1)
      D = 1;
    else if (blend_dfactor==3)
      D = Ci;
    else if (blend_dfactor==5)
      D = 1-Ci;
    else if (blend_dfactor==6)
      D = diffuse_alpha;
    else if (blend_dfactor==7)
      D = 1-diffuse_alpha;
    
    Ci = S*Ci;
    Oi = 1-D;
  }
  BAKE_END
}        
"""

    def surfaceShaderParams(self, passes):
        """Return a dictionary with shader parameters and their values."""
        blend_dict = { GL_ZERO : 0,
                       GL_ONE : 1,
                       GL_DST_COLOR : 2,
                       GL_SRC_COLOR : 3,
                       GL_ONE_MINUS_DST_COLOR : 4,
                       GL_ONE_MINUS_SRC_COLOR : 5,
                       GL_SRC_ALPHA : 6,
                       GL_ONE_MINUS_SRC_ALPHA : 7,
                       GL_DST_ALPHA : 8,
                       GL_ONE_MINUS_DST_ALPHA : 9,
                       GL_SRC_ALPHA_SATURATE : 10
                     }
        sfactor = blend_dict.get(self.mat.blend_sfactor, -1)
        dfactor = blend_dict.get(self.mat.blend_dfactor, -1)
        res = {"uniform float shininess" : self.mat.shininess,
               "uniform color specular" : tuple(self.mat.specular)[:3],
               "uniform color ambient" : tuple(self.mat.ambient)[:3],
               "uniform color emission" : tuple(self.mat.emission)[:3],
               "uniform float blend_sfactor" : sfactor,
               "uniform float blend_dfactor" : dfactor,
               "uniform float diffuse_alpha" : self.mat.diffuse.w
               }

        if self.mat.getTexture(0)!=None:
            tex = self.mat.getTexture(0)
            # Image name
            n = os.path.basename(tex.imagename)
            name,ext = os.path.splitext(n)
            res["uniform string texname"] = name+".tex"
            # Mode
            try:
                res["uniform float mode"] = [GL_REPLACE, GL_MODULATE, GL_DECAL, GL_BLEND].index(tex.mode)
            except:
                pass
            res["uniform color texenvcolor"] = tuple(tex.texenvcolor)[:3]
            res["uniform float texenvcolor_alpha"] = tex.texenvcolor.w
            res["uniform float [16] T"] = tex.transform.transpose()
      
        return res

    def surfaceShaderTransform(self):
        return mat4(1)

    def displacementShaderName(self):
        return None
    
    def displacementShaderSource(self):
        return None
    
    def displacementShaderParams(self, passes):
        return {}

    def displacementShaderTransform(self):
        return mat4(1)

    def displacementBound(self):
        return "current", 0
	
    def interiorShaderName(self):
        return None
    
    def interiorShaderSource(self):
        return None
    
    def interiorShaderParams(self, passes):
        return {}

    def interiorShaderTransform(self):
        return mat4(1)


######################################################################

# Register the OffImporter class as a plugin class
pluginmanager.register(RIBExporter)
