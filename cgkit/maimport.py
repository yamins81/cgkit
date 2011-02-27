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
# $Id: maimport.py,v 1.12 2005/06/15 19:20:29 mbaas Exp $

import sys
import mayaascii, cmds
import pluginmanager
import freecamera
import quadrics, box, plane, polyhedron, joint, trimesh, trimeshgeom
import glpointlight, glfreespotlight, glfreedistantlight
import mayaspotlight
from geomobject import *
from cgtypes import *
from worldobject import WorldObject
from sl import *
from math import *
            
                

# MAImporter
class MAImporter(mayaascii.DefaultMAReader):
    """MA import.
    """

    _protocols = ["Import"]

    def __init__(self):
        mayaascii.DefaultMAReader.__init__(self)

    # extension
    def extension():
        """Return the file extensions for this format."""
        return ["ma"]
    extension = staticmethod(extension)

    # description
    def description(self):
        """Return a short description for the file dialog."""
        return "Maya ASCII file"
    description = staticmethod(description)

    # importFile
    def importFile(self, filename, parent=None):
        """Import a MA file."""

        self.root_parent = parent

        f = file(filename)
        self.read(f)

    # begin
    def begin(self):
        mayaascii.DefaultMAReader.begin(self)
        # A dict with created WorldObjects (key = name)
        self.worldobjects = {}

    # end
    def end(self):
        mayaascii.DefaultMAReader.end(self)
        
        issued_warnings = {}
        # Process the nodes by calling an appropriate handler method
        # onNode<Type>(node) for each node type...
        for node in self.nodelist:
            s = node.nodetype
            handlername = "onNode%s%s"%(s[0].upper(), s[1:])
            handler = getattr(self, handlername, None)
            if handler!=None:
                handler(node)
            else:
                if node.nodetype not in issued_warnings:
                    print >>sys.stderr, "WARNING: %s nodes are ignored."%node.nodetype
                    issued_warnings[node.nodetype] = 1

    #################################################

    ### High level callbacks:
    
    # Each node callback takes the current Node object as argument.
    # Whenever a new WorldObject is created, the method has to call
    # the newWorldObject() method to tell the importer about the new object
    # (so that it is known when someone else uses it as parent).

    def ignoreWarning(self, node):
        """(this is just a dummy callback so that no warning is issued)"""
        pass
    onNodeTransform = ignoreWarning
    onNodePolySphere = ignoreWarning
    onNodePolyCube = ignoreWarning
    onNodePolyPlane = ignoreWarning
    onNodeRigidBody = ignoreWarning    

    def onNodePointLight(self, node):
        """PointLight node.
        """
        args = self.fillWorldObjectArgs(node)
        
        cl = vec3(node.getAttrValue("color", "cl", "float3", 1, vec3(1)))
        intensity = node.getAttrValue("intensity", "in", "float", 1, 1.0)
        de = node.getAttrValue("decayRate", "de", "int", 1, 0)
        if de==0:
            catt = 1.0
            latt = 0.0
            qatt = 0.0
        elif de==1:
            catt = 0.0
            latt = 1.0
            qatt = 0.0
        else:
            catt = 0.0
            latt = 0.0
            qatt = 1.0

        lgt = glpointlight.GLPointLight(diffuse = cl,
                                        intensity = intensity,
                                        constant_attenuation = catt,
                                        linear_attenuation = latt,
                                        quadratic_attenuation = qatt,
                                        enabled = args["visible"],
                                        **args)
        self.newWorldObject(lgt, node.getParentName())

    def onNodeSpotLight(self, node):
        """SpotLight node.
        """
        args = self.fillWorldObjectArgs(node)
        
        cl = vec3(node.getAttrValue("color", "cl", "float3", 1, vec3(1)))
        intensity = node.getAttrValue("intensity", "in", "float", 1, 1.0)
        de = node.getAttrValue("decayRate", "de", "int", 1, 0)
        ca = node.getAttrValue("coneAngle", "ca", "float", 1, 40.0)
        pa = node.getAttrValue("penumbraAngle", "pa", "float", 1, 0.0)
        dro = node.getAttrValue("dropoff", "dro", "float", 1, 0.0)

        dms = node.getAttrValue("useDepthMapShadows", "dms", "bool", 1, False)
        dr = node.getAttrValue("dmapResolution", "dr", "int", 1, 512)
        md = node.getAttrValue("useMidDistDmap", "md", "bool", 1, True)
        af = node.getAttrValue("useDmapAutoFocus", "af", "bool", 1, True)
        df = node.getAttrValue("dmapFocus", "df", "float", 1, 90.0)
        fs = node.getAttrValue("dmapFilterSize", "fs", "int", 1, 1)
        db = node.getAttrValue("dmapBias", "db", "float", 1, 0.001)

        args["transform"] = args["transform"]

        lgt = mayaspotlight.MayaSpotLight(color = cl,
                                          intensity = intensity,
                                          decayRate = de,
                                          coneAngle = ca,
                                          penumbraAngle = pa,
                                          dropoff = dro,
                                          useDepthMapShadows = dms,
                                          dmapResolution = dr,
                                          useMidDistDmap = md,
                                          useDmapAutoFocus = af,
                                          dmapFocus = df,
                                          dmapFilterSize = fs,
                                          dmapBias = db,
                                          enabled = args["visible"],
                                          **args)
        self.newWorldObject(lgt, node.getParentName())

    def onNodeDirectionalLight(self, node):
        """DirectionalLight node.
        """
        args = self.fillWorldObjectArgs(node)
        
        cl = vec3(node.getAttrValue("color", "cl", "float3", 1, vec3(1)))
        intensity = node.getAttrValue("intensity", "in", "float", 1, 1.0)

        args["transform"] = args["transform"]*mat4(1).rotation(pi, vec3(1,0,0))

        lgt = glfreedistantlight.GLFreeDistantLight(diffuse = cl,
                                        intensity = intensity,
                                        enabled = args["visible"],
                                        **args)
        self.newWorldObject(lgt, node.getParentName())

    def onNodeJoint(self, node):
        """Joint node.
        """
        args = self.fillWorldObjectArgs(node)

        # Compute transform
        t = vec3(node.getAttrValue("translate", "t", "double3", 1, vec3(0)))
        r = vec3(node.getAttrValue("rotate", "r", "double3", 1, vec3(0)))
        s = vec3(node.getAttrValue("scale", "s", "double3", 1, vec3(1)))
        ra = vec3(node.getAttrValue("rotateAxis", "ra", "double3", 1, vec3(0)))
        roi = node.getAttrValue("rotationInterpolation", "roi", "int", 1, 1)
        ro = node.getAttrValue("rotateOrder", "ro", "int", 1, default=0)
        jo = vec3(node.getAttrValue("jointOrient", "jo", "double3", 1, vec3(0)))
        is_ = vec3(node.getAttrValue("inverseScale", "is", "double3", 1, vec3(1)))
        jot = node.getAttrValue("jointOrientType", "jot", "string", 1, default="xyz")

        S = mat4().scaling(s)
        T = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, t.x,t.y,t.z,1)
        IS = mat4().scaling(is_)

        sx = sin(radians(ra.x))
        cx = cos(radians(ra.x))
        sy = sin(radians(ra.y))
        cy = cos(radians(ra.y))
        sz = sin(radians(ra.z))
        cz = cos(radians(ra.z))
        AX = mat4(1,0,0,0, 0,cx,sx,0, 0,-sx,cx,0, 0,0,0,1)
        AY = mat4(cy,0,-sy,0, 0,1,0,0, sy,0,cy,0, 0,0,0,1)
        AZ = mat4(cz,sz,0,0, -sz,cz,0,0, 0,0,1,0, 0,0,0,1)
        RA = AX*AY*AZ

        # Euler-angle rotation (todo: quat)
        sx = sin(radians(r.x))
        cx = cos(radians(r.x))
        sy = sin(radians(r.y))
        cy = cos(radians(r.y))
        sz = sin(radians(r.z))
        cz = cos(radians(r.z))
        RX = mat4(1,0,0,0, 0,cx,sx,0, 0,-sx,cx,0, 0,0,0,1)
        RY = mat4(cy,0,-sy,0, 0,1,0,0, sy,0,cy,0, 0,0,0,1)
        RZ = mat4(cz,sz,0,0, -sz,cz,0,0, 0,0,1,0, 0,0,0,1)
        a,b,c = ["XYZ", "YZX", "ZXY", "XZY", "YXZ", "ZYX"][ro]
        exec "R=R%s*R%s*R%s"%(a,b,c)

        sx = sin(radians(jo.x))
        cx = cos(radians(jo.x))
        sy = sin(radians(jo.y))
        cy = cos(radians(jo.y))
        sz = sin(radians(jo.z))
        cz = cos(radians(jo.z))
        AX = mat4(1,0,0,0, 0,cx,sx,0, 0,-sx,cx,0, 0,0,0,1)
        AY = mat4(cy,0,-sy,0, 0,1,0,0, sy,0,cy,0, 0,0,0,1)
        AZ = mat4(cz,sz,0,0, -sz,cz,0,0, 0,0,1,0, 0,0,0,1)
        a,b,c = jot.upper()
        exec "JO=A%s*A%s*A%s"%(a,b,c)

        WT = S*RA*R*IS*T
           
#        WT = S*RA*R*IS*T
#        WT = T
        WT = WT.transpose()
        args["transform"] = WT

        jnt = joint.Joint(radius = 0.5*node.getAttrValue("radius", "radi", "float", 1, 1.0),
                          offsetTransform = JO.inverse(),
                          **args)
        jnt.freezePivot()
        # Reset offset transform (so that pivot frame and local frame coincide)
        jnt.setOffsetTransform(mat4(1))
        self.newWorldObject(jnt, node.getName())


    def onNodeCamera(self, node):
        """Camera node.
        """
        args = self.fillWorldObjectArgs(node)
        self.getCameraArgs(node, args)
        args["transform"] = args["transform"]*mat4(1).rotation(pi, vec3(0,1,0))
        cam = freecamera.FreeCamera(**args)
        self.newWorldObject(cam, node.getParentName())

    def onNodeMesh(self, node):
        args = self.fillWorldObjectArgs(node)
        parentnode = node.getParent()
        
        # Check if the mesh is just a regular mesh or if it's created by
        # a "creator" node (cube, sphere, ...). 
        n,a = node.getInNode("inMesh", "i")
        if n==None:
            meshtype = None
            verts = node.getAttrValue("vrts", "vt", "float3", None, [])
            edges = node.getAttrValue("edge", "ed", "long3", None, [])
            faces = node.getAttrValue("face", "fc", "polyFaces", None, [])
            polyobj = polyhedron.Polyhedron(verts=verts, **args)
            geom = polyobj.geom
            # Initialize the polygons...
            geom.setNumPolys(len(faces))
            valid_st = True
            for i, face in enumerate(faces):
                valid_st &= face.hasValidTexCoords()
                poly = []
                # Iterate over all "loop" (i.e. the outer loop and the holes)
                # and create the vertex id list for each loop
                for loop in [face.f]+face.h:
                    p = []
                    for e in loop:
                        if e>=0:
                            p.append(edges[e][0])
                        else:
                            p.append(edges[-e-1][1])
                    poly.append(p)
                geom.setPoly(i, poly)

            # Set texture coordinates...
            uvset = node.getAttrValue("uvSet", "uvst", type=None, n=None)
            if uvset!=None:
                stlst = list(uvset[0].uvsp)
                if valid_st and len(stlst)>0:
                    geom.newVariable("st", USER, FLOAT, 2, len(stlst))
                    st = geom.slot("st")
                    for i,stval in enumerate(stlst):
                        st[i] = stval
                    # Set the tex coord faces...
                    geom.newVariable("stfaces", FACEVARYING, INT)
                    stfaces = geom.slot("stfaces")
                    i = 0
                    for face in faces:
                        for mulst in face.mu:
                            for id in mulst[0][1]:
                                stfaces[i] = id
                                i+=1

            # Set colors...
            colattr = node.getAttrValue("colors", "clr", type="float4", n=None)
            if colattr!=None:
                collst = list(colattr)
                geom.newVariable("Cs", USER, COLOR, 1, len(collst))
                Cs = geom.slot("Cs")
                for i,Csval in enumerate(collst):
                    Cs[i] = vec3(Csval[:3])
                # Set the color faces...
                geom.newVariable("Csfaces", FACEVARYING, INT)
                Csfaces = geom.slot("Csfaces")
                i = 0
                for face in faces:
                    for fclst in face.fc:
                        for id in fclst:
                            Csfaces[i] = id
                            i+=1

            # Apply vertex tweaks...
            pnts = node.getAttrValue("pnts", "pt", "float3", None, [])
            pnts = list(pnts)
            for i,v in enumerate(geom.verts):
                if i>=len(pnts):
                    break
                p = pnts[i]
                if p!=None:
                    if type(p)==list:
                        p = p[0]
                    v += vec3(p)
                    geom.verts[i] = v

            # Convert the poly to a trimesh if it's a rigid body
            self.getRigidBodyArgs(parentnode, args)
            if args["dynamics"]:
                tm = trimeshgeom.TriMeshGeom()
                polyobj.geom.convert(tm)
                cmds.delete(polyobj)
                polyobj = trimesh.TriMesh(**args)
                polyobj.geom = tm

            self.newWorldObject(polyobj, node.getParentName())
        else:
            creator = self.findNode(n)
            meshtype = creator.nodetype
            # Sphere?
            if meshtype=="polySphere":
                self.createSphere(node, creator, args)
            # Cube?
            elif meshtype=="polyCube":
                self.createBox(node, creator, args)
            # Plane?
            elif meshtype=="polyPlane":
                self.createPlane(node, creator, args)

    def createSphere(self, mesh, creator, args):
        """Creates a true sphere from a polySphere node.

        mesh is the mesh node that takes its input from creator.
        creator is an import Node which must be of type polySphere.
        args is a dictionary that already contains the basic WorldObject
        arguments that can be passed to the constructor.
        """
        parentnode = mesh.getParent()
        nod,attr = parentnode.getOutAttr("worldMatrix", "wm", "rigidBody")
        dynamics = (nod!=None)
        r = creator.getAttrValue("radius", "r", "float", 1, 1.0)
        segsu = creator.getAttrValue("subdivisionAxis", "sa", "int", 1, 20)
        segsv = creator.getAttrValue("subdivisionHeight", "sh", "int", 1, 20)
        ax = vec3(creator.getAttrValue("axis", "ax", "double3", 1, vec3(0,1,0)))
#        args["transform"] *= self.axisToTransform(ax)
#        if ax==vec3(1,0,0):
#            args["transform"] *= mat4(1).rotation(pi/4, vec3(0,0,1))
        self.getRigidBodyArgs(parentnode, args)
        s = quadrics.Sphere(radius=r,
                            segmentsu=segsu, segmentsv=segsv,
                            **args)
        self.newWorldObject(s, parentnode.getName())

    def createBox(self, mesh, creator, args):
        """Creates a true box from a polyCube node.

        mesh is the mesh node that takes its input from creator.
        creator is an import Node which must be of type polyCube.
        args is a dictionary that already contains the basic WorldObject
        arguments that can be passed to the constructor.
        """
        parentnode = mesh.getParent()
        w = creator.getAttrValue("width", "w", "float", 1, 1.0)
        h = creator.getAttrValue("height", "h", "float", 1, 1.0)
        d = creator.getAttrValue("depth", "d", "float", 1, 1.0)
        sw = creator.getAttrValue("subdivisionWidth", "sw", "int", 1, 1)
        sh = creator.getAttrValue("subdivisionHeight", "sh", "int", 1, 1)
        sd = creator.getAttrValue("subdivisionDepth", "sd", "int", 1, 1)
        ax = vec3(creator.getAttrValue("axis", "ax", "double3", 1, vec3(0,1,0)))

        # "Rename" width/height/depth to match the axis setting...
        if ax==vec3(1,0,0):
            tmp = d
            d = w
            w = h
            h = tmp
            tmp = sd
            sd = sw
            sw = sh
            sh = tmp
        elif ax==vec3(0,1,0):
            tmp = d
            d = h
            h = tmp
            tmp = sd
            sd = sh
            sh = tmp
            
        self.getRigidBodyArgs(parentnode, args)
        b = box.Box(lx=w, ly=d, lz=h,
                    segmentsx=sw, segmentsy=sd, segmentsz=sh,
                    **args)
        self.newWorldObject(b, parentnode.getName())

    def createPlane(self, mesh, creator, args):
        """Creates a true plane from a polyPlane node.

        mesh is the mesh node that takes its input from creator.
        creator is an import Node which must be of type polyPlane.
        args is a dictionary that already contains the basic WorldObject
        arguments that can be passed to the constructor.
        """
        parentnode = mesh.getParent()
        w = creator.getAttrValue("width", "w", "float", 1, 1.0)
        h = creator.getAttrValue("height", "h", "float", 1, 1.0)
        sw = creator.getAttrValue("subdivisionWidth", "sw", "int", 1, 1)
        sh = creator.getAttrValue("subdivisionHeight", "sh", "int", 1, 1)
        ax = vec3(creator.getAttrValue("axis", "ax", "double3", 1, vec3(0,1,0)))

        if ax!=vec3(0,0,1):
            print "WARNING: Plane %s ignored because axis!=z"%node.getName()
            return

#        args["transform"] *= self.axisToTransform(ax)
        self.getRigidBodyArgs(parentnode, args)
        if "static" in args:
            del args["static"]
        p = plane.Plane(lx=w, ly=h,
                segmentsx=sw, segmentsy=sh,
                **args)
        self.newWorldObject(p, parentnode.getName())


    def getRigidBodyArgs(self, transform, res):
        """Retrieve rigid body information.

        transform is the transform node as NodeData object.
        """
        rbnode,attr = transform.getOutAttr("worldMatrix", "wm", "rigidBody")
        # No rigid body node? then don't add any arguments
        if rbnode==None:
            res["dynamics"] = False
            return res

        res["dynamics"] = True
        res["mass"] = rbnode.getAttrValue("mass", "mas", "float", 1, 1.0)
        res["static"] = not rbnode.getAttrValue("active", "act", "bool", 1, True)
        return res
        

#    def axisToTransform(self, axis):
#        if axis==vec3(1,0,0):
#            return mat4(1).rotation(pi/2, vec3(0,1,0))
#        elif axis==vec3(0,1,0):
#            return mat4(1).rotation(pi/2, vec3(1,0,0))
#        else:
#            return mat4(1)

    ### Helpers:

    def newWorldObject(self, obj, mayanodename):
        """Notify about the creation of a new world object.

        This method has to be called whenever a new WorldObject is created.
        This is needed to later find the object when it is used as parent.
        """
        if mayanodename in self.worldobjects:
            print "WARNING: Duplicate node name '%s'"%mayanodename
            
        self.worldobjects[mayanodename] = obj

    # getCameraArgs
    def getCameraArgs(self, node, res):
        """Create the argument dict for the WorldObject constructor.

        node must contain the data from a Maya transform node. 
        """

        fl = node.getAttrValue("focalLength", "fl", "float", 1, 35.0)
        res["focallength"] = fl
        cap = node.getAttrValue("cameraAperture", "cap", "double2", 1, (1.41732, 0.94488))
        filmFit = node.getAttrValue("filmFit", "ff", "int", 1, 1)

        xfov = degrees(2*atan((cap[0]*25.4)/(2*fl)))
        yfov = degrees(2*atan((cap[1]*25.4)/(2*fl)))
        
        # Horizontal
        if filmFit==1:
            # Convert the FOV from horizontal to vertical direction
            # (Todo: What aspect ratio to use?)
            yfov = degrees(atan(480/640.0*tan(radians(xfov/2.0))))*2.0
            yfov = xfov
        # Vertical
        elif filmFit==2:
            pass
        # Fill and overscan not supported yet
        else:
            pass
        
        res["fov"] = yfov
        res["fstop"] = node.getAttrValue("fStop", "fs", "float", 1, 5.6)
        return res

    # fillWorldObjectArgs
    def fillWorldObjectArgs(self, node, args=None):
        """Fill the argument dict for the WorldObject constructor.

        node must be a Node object containing either the transform node
        or the shape node (whose parent must then be a transform node).
        args must be a dictionary which is filled with the arguments
        for a WorldObject constructor. It can also be None in which case
        a new dictionary is created and returned.
        The following attributes are written into args:

        - name: The name of the transform node
        - parent: The parent WorldObject (or self.root_parent)
        - transform: The transform matrix (pos rot may come from a rigidBody node)

        The return value is the args dictionary or the newly created dict.
        """
        # Obtain the transform node -> tnode
        if node.nodetype in ["transform", "joint"]:
            tnode = node
        else:
            tnode = node.getParent()
            if tnode==None or tnode.nodetype!="transform":
                raise ValueError, "transform node not found for node %s."%node.getFullName()

        if args==None:
            args = {}

        # Name
        args["name"] = tnode.getName()

        # Parent
        parentnode = tnode.getParent()
        if parentnode!=None:
            parentname = parentnode.getFullName()
            if parentname!=None:
                parent = self.worldobjects.get(parentname, None)
                if parent==None:
                    print "WARNING: Parent '%s' not found."%parentname
                    parent = self.root_parent
        else:
            parent = self.root_parent
        args["parent"] = parent

        # Check if the world matrix is connected to a rigid body.
        # In this case, the rigid body stores the initial position and
        # orientation
        rbnode,attr = tnode.getOutAttr("worldMatrix", "wm", "rigidBody")

        # Compute transform
        if rbnode==None:
            t = vec3(tnode.getAttrValue("translate", "t", "double3", 1, vec3(0)))
            r = vec3(tnode.getAttrValue("rotate", "r", "double3", 1, vec3(0)))
        else:
            t = vec3(rbnode.getAttrValue("initialPosition", "ip", "double3", 1, vec3(0)))
            r = vec3(rbnode.getAttrValue("initialOrientation", "ior", "double3", 1, vec3(0)))
        s = vec3(tnode.getAttrValue("scale", "s", "double3", 1, vec3(1)))
        sp = vec3(tnode.getAttrValue("scalePivot", "sp", "double3", 1, vec3(0)))
        spt = vec3(tnode.getAttrValue("scalePivotTranslate", "spt", "double3", 1, vec3(0)))
        sh = vec3(tnode.getAttrValue("shear", "sh", "double3", 1, vec3(0)))
        rp = vec3(tnode.getAttrValue("rotatePivot", "rp", "double3", 1, vec3(0)))
        rpt = vec3(tnode.getAttrValue("rotatePivotTranslate", "rpt", "double3", 1, vec3(0)))
        ra = vec3(tnode.getAttrValue("rotateAxis", "ra", "double3", 1, vec3(0)))
        roi = tnode.getAttrValue("rotationInterpolation", "roi", "int", 1, 1)
        ro = tnode.getAttrValue("rotateOrder", "ro", "int", 1, default=0)

        SP = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, sp.x,sp.y,sp.z,1)
        ST = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, spt.x,spt.y,spt.z,1)
        S = mat4().scaling(s)
        SH = mat4(1,0,0,0, sh.x,1,0,0, sh.y,sh.z,1,0, 0,0,0,1)
        RP = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, rp.x,rp.y,rp.z,1)
        RT = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, rpt.x,rpt.y,rpt.z,1)
        T = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, t.x,t.y,t.z,1)

        sx = sin(radians(ra.x))
        cx = cos(radians(ra.x))
        sy = sin(radians(ra.y))
        cy = cos(radians(ra.y))
        sz = sin(radians(ra.z))
        cz = cos(radians(ra.z))
        AX = mat4(1,0,0,0, 0,cx,sx,0, 0,-sx,cx,0, 0,0,0,1)
        AY = mat4(cy,0,-sy,0, 0,1,0,0, sy,0,cy,0, 0,0,0,1)
        AZ = mat4(cz,sz,0,0, -sz,cz,0,0, 0,0,1,0, 0,0,0,1)
        RA = AX*AY*AZ

        # Euler-angle rotation (todo: quat)
        sx = sin(radians(r.x))
        cx = cos(radians(r.x))
        sy = sin(radians(r.y))
        cy = cos(radians(r.y))
        sz = sin(radians(r.z))
        cz = cos(radians(r.z))
        RX = mat4(1,0,0,0, 0,cx,sx,0, 0,-sx,cx,0, 0,0,0,1)
        RY = mat4(cy,0,-sy,0, 0,1,0,0, sy,0,cy,0, 0,0,0,1)
        RZ = mat4(cz,sz,0,0, -sz,cz,0,0, 0,0,1,0, 0,0,0,1)
        a,b,c = ["XYZ", "YZX", "ZXY", "XZY", "YXZ", "ZYX"][ro]
        exec "R=R%s*R%s*R%s"%(a,b,c)

        WT = SP.inverse()*S*SH*SP*ST*RP.inverse()*RA*R*RP*RT*T
        WT = WT.transpose()
        args["transform"] = WT

        # Visibility
        args["visible"] = tnode.getAttrValue("visibility", "v", "bool", 1, True)
        
        return args


######################################################################

# Register the importer class as a plugin class
pluginmanager.register(MAImporter)
