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
# $Id: objimport.py,v 1.11 2006/03/20 19:33:24 mbaas Exp $

import os.path, sys
from cgtypes import *
from worldobject import WorldObject
from trimesh import TriMesh
from trimeshgeom import TriMeshGeom
from polyhedron import Polyhedron
from polyhedrongeom import PolyhedronGeom
import pluginmanager
from cgkit.all import UNIFORM, VARYING, FACEVARYING, NORMAL, FLOAT, INT, OBJMaterial, OBJTextureMap
import cmds
import objmtl


class _MTLReader(objmtl.MTLReader):
    """Read MTL files and create a list of materials.

    Reads an MTL file and stores the materials as OBJMaterial objects
    in the list self.material.
    """
    
    def __init__(self):
        objmtl.MTLReader.__init__(self)
        self.materials = []
        self.currentmat = None

    def begin(self):
        self.materials = []
        self.currentmat = None

    def end(self):
        pass

    def newmtl(self, name):
        mat = OBJMaterial(name=name)
        self.materials.append(mat)
        self.currentmat = mat

    def illum(self, model):
        if self.currentmat!=None:
            self.currentmat.illum = model

    def Ka(self, c):
        if self.currentmat!=None:
            self.currentmat.Ka = c

    def Kd(self, c):
        if self.currentmat!=None:
            self.currentmat.Kd = c

    def Ks(self, c):
        if self.currentmat!=None:
            self.currentmat.Ks = c

    def Ks(self, c):
        if self.currentmat!=None:
            self.currentmat.Ks = c

    def Ke(self, c):
        if self.currentmat!=None:
            self.currentmat.Ke = c

    def Ns(self, shininess):
        if self.currentmat!=None:
            self.currentmat.Ns = shininess

    def Ni(self, refl):
        if self.currentmat!=None:
            self.currentmat.Ni = refl

    def d(self, alpha):
        if self.currentmat!=None:
            self.currentmat.d = alpha

    def Tr(self, alpha):
        if self.currentmat!=None:
            self.currentmat.Tr = 1-alpha

    def Tf(self, c):
        if self.currentmat!=None:
            self.currentmat.Tf = c

    def sharpness(self, v):
        if self.currentmat!=None:
            self.currentmat.sharpness = v

    def map_Ka(self, name, options):
        if self.currentmat!=None:
            self.currentmat.map_Ka = self.createMap(name, options)

    def map_Kd(self, name, options):
        if self.currentmat!=None:
            self.currentmat.map_Kd = self.createMap(name, options)

    def map_Ks(self, name, options):
        if self.currentmat!=None:
            self.currentmat.map_Ks = self.createMap(name, options)

    def map_Ke(self, name, options):
        if self.currentmat!=None:
            self.currentmat.map_Ke = self.createMap(name, options)

    def map_Ns(self, name, options):
        if self.currentmat!=None:
            self.currentmat.map_Ns = self.createMap(name, options)

    def map_d(self, name, options):
        if self.currentmat!=None:
            self.currentmat.map_d = self.createMap(name, options)

    def map_Bump(self, name, options):
        if self.currentmat!=None:
            map = self.createMap(name, options)
            map.bumpsize = options.get("-bm", 1.0)  
            self.currentmat.map_Bump = map

    def refl(self, name, options):
        if self.currentmat!=None:
            map = self.createMap(name, options)
            map.refltype = options.get("-type", "sphere")
            self.currentmat.refl.append(map)


    def createMap(self, name, options):
        map = OBJTextureMap(filename = name,
                            offset = options.get("-o", (0,0,0)),
                            scale = options.get("-s", (1,1,1)),
                            turb = options.get("-t", (0,0,0)),
                            mm = options.get("-mm", (0,1.0)),
                            clamp = options.get("-clamp", False),
                            blendu = options.get("-blendu", True),
                            blendv = options.get("-blendv", True))
        return map
        

class _OBJReader(objmtl.OBJReader):
    """Read an OBJ file.
    """
    
    def __init__(self, root=None):
        objmtl.OBJReader.__init__(self)

        # Dummy vertex at position 0 because the indices in an OBJ file
        # start with 1
        self.verts = [vec3(0)]
        self.normals = [vec3(0)]
        self.tverts = [vec3(0)]

        # Local object hierarchy. Each "node" is a 2-tuple (WorldObject,
        # Childs) where Childs is a dictionary that contains the children
        # "nodes". The value is the object name.
        self.hierarchy = (root, {})

        # Store the group names ('g')
        self.groupnames = []

        # A dictionary with OBJMaterial objects read from the MTL files
        # Key is the material name
        self.materials = {}
        # Material stack. Contains 2-tuples (offset, OBJMaterial) where
        # offset is the face index of the first face (since the last 'g'
        # command that uses this material)
        # The stack is cleared (the last element remains) whenever the 'g'
        # command occurs.
        self.materialstack = [(0, OBJMaterial())]

        # The collected faces
        self.faces = []
        # Flag that indicates if self.faces only contains triangles
        self.trimesh_flag = True

        # Was it already reported that points aren't supported?
        self.point_msg_flag = False
        # Was it already reported that lines aren't supported?
        self.line_msg_flag = False


    def end(self):
        # Trigger the creation of the last object
        self.g("default")

    # mtllib
    def mtllib(self, *files):
        mtlreader = _MTLReader()
        # Iterate over all MTL file names
        for fname in files:
            # Read the MTL file...
            try:
                f = file(fname)
            except IOError, e:
                print e
                continue
            mtlreader.read(f)
            f.close()
            # ...and keep the materials that don't exist yet
            for mat in mtlreader.materials:
                if mat.name not in self.materials:
                    self.materials[mat.name] = mat

    # usemtl
    def usemtl(self, name):
        if name not in self.materials:
            print >>sys.stderr, 'WARNING: No definition found for material "%s"'%name
            # Store None so that the warning message will only get printed
            # once for each missing material name
            self.materials[name] = None

        # Put the material on the stack (replace the top material if it has
        # the same offset (i.e. it is unused))
        offset = len(self.faces)
        mat = self.materials[name]
        if self.materialstack[-1][0]==offset:
            self.materialstack.pop()
        self.materialstack.append((len(self.faces), self.materials[name]))

    # v
    def v(self, vert):
        """Vertex."""
        v = vec3(vert.x, vert.y, vert.z)
        try:
            v /= vert.w
        except:
            pass
        self.verts.append(v)

    # vn
    def vn(self, n):
        """Normal."""
        self.normals.append(n)

    # vt
    def vt(self, tv):
        """Texture vertex."""
        self.tverts.append(tv)

    # p
    def p(self, *verts):
        """Points."""
        if self.point_msg_flag:
            return
        print "OBJ import: Points are not supported"
        self.point_msg_flag = True

    # l
    def l(self, *verts):
        """Line."""
        if self.line_msg_flag:
            return
        print "OBJ import: Lines are not supported"
        self.line_msg_flag = True

    # f
    def f(self, *verts):
        """Face."""
#        ve = []
#        for v,tv,n in verts:
#            ve.append((v,None,None))

        self.faces.append(verts)
#        self.faces.append(ve)
        if len(verts)!=3:
            self.trimesh_flag = False

    # g
    def g(self, *groups):
        """Grouping info.
        """
        if len(self.faces)!=0:
            parent, names, node = self.findParent(self.groupnames)
            name = "_".join(names)
            if name=="":
                name = "Mesh"
            if self.trimesh_flag:
                obj = self.createTriMesh(parent=parent, name=name)
            else:
                obj = self.createPolyhedron(parent=parent, name=name)
            self.updateHierarchy(node, obj)
        self.faces = []
        self.trimesh_flag = True
        self.groupnames = groups
        # Clear the stack (the last material remains)
        self.materialstack = [(0, self.materialstack[-1][1])]

    # createTriMesh
    def createTriMesh(self, parent=None, name=None):
        """Create a triangle mesh from the current set of faces.

        This method may only be called if the faces really have only
        3 vertices.

        Returns the TriMesh object.
        """

        # Build lookup tables (so that only the verts that are really
        # required are stored in the TriMesh)
        #
        # Key: Original vertex index - Value: New vertex index
        vert_lut = {}
        has_normals = True
        has_tverts = True
        iv = 0
        for f in self.faces:
            for v,tv,n in f:
                if v not in vert_lut:
                    vert_lut[v] = iv
                    iv+=1
                if tv==None:
                    has_tverts = False
                if n==None:
                    has_normals = False

        numfaces = len(self.faces)
        numverts = len(vert_lut)

        tm = TriMeshGeom()
        tm.verts.resize(numverts)
        tm.faces.resize(numfaces)

        # Set vertices
        for v in vert_lut:
            newidx = vert_lut[v]
            tm.verts[newidx] = self.verts[v]

        # Set faces
        idx = 0
        for i,f in enumerate(self.faces):
            fi = []
            for v,tv,n in f:
                fi.append(vert_lut[v])
            tm.faces[i] = fi

        # Create variable N for storing the normals
        if has_normals:
            tm.newVariable("N", FACEVARYING, NORMAL)
            N = tm.slot("N")
            idx = 0
            for f in self.faces:
                for v,tv,n in f:
                    N[idx] = self.normals[n]
                    idx += 1

        # Set texture vertices
        if has_tverts:
            tm.newVariable("st", FACEVARYING, FLOAT, 2)
            st = tm.slot("st")
            idx = 0
            for f in self.faces:
                for v,tv,n in f:
                    st[idx] = self.tverts[tv]
                    idx += 1

        obj = TriMesh(name=name, parent=parent)
        obj.geom = tm
        # Set the materials
        self.initMaterial(obj)
        return obj

    # createPolyhedron
    def createPolyhedron(self, parent=None, name=None):
        """Create a polyhedron from the current set of faces.

        Returns the Polyhedron object.
        """

        # Build lookup tables (so that only the verts that are really
        # required are stored in the Polyhedron)
        #
        # Key: Original vertex index - Value: New vertex index
        vert_lut = {}
        has_normals = True
        has_tverts = True
        iv = 0
        for f in self.faces:
            for v,tv,n in f:
                if v not in vert_lut:
                    vert_lut[v] = iv
                    iv+=1
                if tv==None:
                    has_tverts = False
                if n==None:
                    has_normals = False

        numpolys = len(self.faces)
        numverts = len(vert_lut)

        pg = PolyhedronGeom()
        pg.verts.resize(numverts)
        pg.setNumPolys(numpolys)

        # Set vertices
        for v in vert_lut:
            newidx = vert_lut[v]
            pg.verts[newidx] = self.verts[v]

        # Set polys (this has to be done *before* any FACEVARYING variable
        # is created, otherwise the size of the variable wouldn't be known)
        idx = 0
        for i,f in enumerate(self.faces):
            loop = []
            for v,tv,n in f:
                loop.append(vert_lut[v])
            pg.setPoly(i,[loop])

        # Create variable N for storing the normals
        if has_normals:
            pg.newVariable("N", FACEVARYING, NORMAL)
            N = pg.slot("N")
            idx = 0
            for f in self.faces:
                for v,tv,n in f:
                    N[idx] = self.normals[n]
                    idx += 1

        # Set texture vertices
        if has_tverts:
            pg.newVariable("st", FACEVARYING, FLOAT, 2)
            st = pg.slot("st")
            idx = 0
            for f in self.faces:
                for v,tv,n in f:
                    st[idx] = self.tverts[tv]
                    idx += 1

        obj = Polyhedron(name=name, parent=parent)
        obj.geom = pg
        # Set the materials
        self.initMaterial(obj)
        return obj


    # initMaterial
    def initMaterial(self, obj):
        """Set the material(s) and the matid primitive variable.
        """
        # Set the materials
        nummats = len(self.materialstack)
        obj.setNumMaterials(nummats)
        for i, (offset, mat) in enumerate(self.materialstack):
            obj.setMaterial(mat, i)

        # Create a variable "matid" if there's more than 1 material...
        if nummats>1:
            obj.geom.newVariable("matid", UNIFORM, INT)
            matid = obj.geom.slot("matid")
            for id in range(len(self.materialstack)):
                begin = self.materialstack[id][0]
                if id+1<len(self.materialstack):
                    end = self.materialstack[id+1][0]
                else:
                    end = matid.size()
                for j in range(begin, end):
                    matid[j] = id


    # findParent
    def findParent(self, groupnames):
        """Find out who is the parent and what is the name of an object.

        groupnames is the value of the 'g' tag. The method assumes that
        one of the elements is the name of the object and the other are
        parents. It's just that it is not known what is what.
        The method tests each name if it matches a previously created
        object at the current level and tries to traverse to the point
        where the object has to be inserted. If everything went fine,
        there should be one name left in the end and that's the name
        of the object.
        The methos returns a 3-tuple (parent, names, Hierarchy_node)
        where parent is the parent WorldObject (or None), names is a
        list of unprocessed names (ideally this should contain 1 element
        which is the object's name) and Hierarchy_node is an internal
        data item that is later used in the updateHierarchy() method).
        """
        names = list(groupnames)
        node = self.hierarchy

        while 1:
            for name in names:
                # Is 'name' the name of a previously created object at the
                # current hierarchy level?
                if name in node[1]:
                    node = node[1][name]
                    names.remove(name)
                    # Restart search at the next hierarchy level
                    break
            else:
                # None of the names was found at the current hierarchy level
                return node[0], names, node

    def updateHierarchy(self, node, obj):
        """Update the local hierarchy after a WorldObject was created.

        node is the 3rd element from the tuple returned by findParent().
        obj is the created WorldObject.
        """
        node[1][obj.name] = (obj, {})

# OBJImporter
class OBJImporter:

    _protocols = ["Import"]

    # extension
    def extension():
        """Return the file extensions for this format."""
        return ["obj"]
    extension = staticmethod(extension)

    # description
    def description(self):
        """Return a short description for the file dialog."""
        return "Wavefront object file"
    description = staticmethod(description)

    # importFile
    def importFile(self, filename, parent=None):
        """Import an OBJ file."""

        f = file(filename)

        reader = _OBJReader(root=parent)
        reader.read(f)

######################################################################

# Register the ObjImporter class as a plugin class
pluginmanager.register(OBJImporter)
