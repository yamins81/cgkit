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
# $Id: objexport.py,v 1.4 2005/06/07 12:01:05 mbaas Exp $

import os.path, sys, re
from cgtypes import *
from scene import getScene
from geomobject import *
from trimeshgeom import TriMeshGeom
from polyhedrongeom import PolyhedronGeom
import pluginmanager
import cmds

# OBJExporter
class OBJExporter:

    _protocols = ["Export"]

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

    # exportFile
    def exportFile(self, filename, root=None, mtlname=None, exportmtl=True):
        """Export an OBJ file.

        root is the root of the subtree that should be exported.
        If exportmtl is False, no MTL file is generated.
        mtlname determines the name of the MTL file (default: the same base
        name than filename).
        If exportmtl is False and no mtlname is given, then no material
        information will be written.
        """

        self.fhandle = file(filename, "w")

        self.use_materials = (exportmtl or mtlname!=None)
        self.root = cmds.worldObject(root)
        self.v_offset = 0
        self.vt_offset = 0
        self.vn_offset = 0

        self.group_offset = 0

        # This dictionary is used to find name clashes and store the materials
        # that have to be exported.
        # Key is the name of the material, value is the material object
        self.materials = {}

        # Determine the name of the MTL file (by changing the suffix)
        if mtlname==None:
            name, ext = os.path.splitext(filename)
            mtlname = name+".mtl"

        if self.use_materials:
            print >>self.fhandle, "mtllib %s"%os.path.basename(mtlname)

        # Export objects...
        if root!=None:
            self.group_offset = len(self.getGroups(self.root))-1
            self.exportObject(self.root)
        for obj in getScene().walkWorld(self.root):
            self.exportObject(obj)

        self.fhandle.close()

        # Export the MTL file...
        if exportmtl:
            self.exportMTL(mtlname)


    # exportMTL
    def exportMTL(self, filename):
        f = file(filename, "w")
        for matname in self.materials:
            mat = self.materials[matname]
            print >>f, "newmtl %s"%matname
            if hasattr(mat, "mtlDefinition"):
                f.write(mat.mtlDefinition())
        f.close()
        

    # exportObject
    def exportObject(self, obj):
        
        geom = self.convertObject(obj)
        if geom==None:
            return

        self.vt_mode = 0
        self.vn_mode = 0

        # Get the world transform to transform the vertices...
        WT = obj.worldtransform
        WT3 = WT.getMat3()

        # Export vertices...
        for v in geom.verts:
            print >>self.fhandle, "v %f %f %f"%tuple(WT*v)

        # Export normals...
        N = None
        info = geom.findVariable("N")
        if info!=None and info[2]==NORMAL and info[3]==1:
            N = geom.slot("N")
            for norm in N:
                norm = WT3*norm
                try:
                    norm = norm.normalize()
                except:
                    pass
                print >>self.fhandle, "vn %f %f %f"%tuple(norm)

            if info[1]==VARYING:
                self.vn_mode = 1
            elif info[1]==FACEVARYING:
                self.vn_mode = 2
            elif info[1]==USER:
                info = geom.findVariable("Nfaces")
                if info!=None and info[1]==UNIFORM and info[2]==INT and info[3]==3:
                    self.vn_mode = 3
            
        # Export texture coordinates...
        st = None
        info = geom.findVariable("st")
        if info!=None and info[2]==FLOAT and info[3]==2:
            st = geom.slot("st")
            for vt in st:
                print >>self.fhandle, "vt %f %f"%vt

            if info[1]==VARYING:
                self.vt_mode = 1
            elif info[1]==FACEVARYING:
                self.vt_mode = 2
            elif info[1]==USER:
                info = geom.findVariable("stfaces")
                if info!=None and info[1]==UNIFORM and info[2]==INT and info[3]==3:
                    self.vt_mode = 3

        # Export groups...
        print >>self.fhandle, "g %s"%" ".join(self.getGroups(obj))

        # Export material name...
        mat = obj.getMaterial()
        if mat!=None and self.use_materials:
            mname = self.preProcessMaterial(mat)
            print >>self.fhandle, "usemtl %s"%mname
        
        # Export faces...
        if isinstance(geom, TriMeshGeom):
            self.exportTriFaces(geom)
        else:
            self.exportPolyFaces(geom)

        self.v_offset += geom.verts.size()
        if st!=None:
            self.vt_offset += st.size()
        if N!=None:
            self.vn_offset += N.size()

    # exportTriFaces
    def exportTriFaces(self, geom):
        """Export the faces of a TriMesh geom.
        """
        vt_mode = self.vt_mode
        vn_mode = self.vn_mode
        stfaces = None
        if vt_mode==3:
            # It has been previously checked that the variable exists...
            stfaces = geom.slot("stfaces")
        Nfaces = None
        if vn_mode==3:
            # It has been previously checked that the variable exists...
            Nfaces = geom.slot("Nfaces")
            
        vt = ""
        vn = ""
        # The current index of facevarying variables (starting at 1 because
        # OBJ is 1-based)
        facevaridx = 1
        for i in range(geom.faces.size()):
            f = geom.faces[i]
            if vt_mode==3:
                tf = stfaces[i]
            if vn_mode==3:
                nf = Nfaces[i]
                
            a = []
            for j in range(3):
                v = f[j]
                # OBJ indices are 1-based
                v += 1
                
                # varying texture coordinates?
                if vt_mode==1:
                    vt = str(v+self.vt_offset)
                # facevarying texture coordinates?
                elif vt_mode==2:
                    vt = str(self.vt_offset+facevaridx)
                # user?
                elif vt_mode==3:
                    vt = str(tf[j]+1+self.vt_offset)
                    
                # varying normals?
                if vn_mode==1:
                    vn = str(v+self.vn_offset)
                # facevarying normals?
                elif vn_mode==2:
                    vn = str(self.vn_offset+facevaridx)
                # user?
                elif vn_mode==3:
                    vn = str(nf[j]+1+self.vn_offset)

                if vn=="":
                    if vt=="":
                        a.append("%d"%(v+self.v_offset))
                    else:
                        a.append("%d/%s"%(v+self.v_offset, vt))
                else:
                    a.append("%d/%s/%s"%(v+self.v_offset, vt, vn))
                facevaridx += 1
            print >>self.fhandle, "f %s"%" ".join(a)
        

    # exportPolyFaces
    def exportPolyFaces(self, geom):
        """Export the faces of a polyhedron geom.
        """
        vt_mode = self.vt_mode
        vn_mode = self.vn_mode
        vt = ""
        vn = ""
        # The current index of facevarying variables (starting at 1 because
        # OBJ is 1-based)
        facevaridx = 1
        for i in range(geom.getNumPolys()):
            poly = geom.getPoly(i)
            a = []
            for v in poly[0]:
                # OBJ indices are 1-based
                v += 1
                
                # varying texture coordinates?
                if vt_mode==1:
                    vt = str(v+self.vt_offset)
                # facevarying texture coordinates?
                elif vt_mode==2:
                    vt = str(self.vt_offset+facevaridx)
                    
                # varying normals?
                if vn_mode==1:
                    vn = str(v+self.vn_offset)
                # facevarying normals?
                elif vn_mode==2:
                    vn = str(self.vn_offset+facevaridx)
                    
                if vn=="":
                    if vt=="":
                        a.append("%d"%(v+self.v_offset))
                    else:
                        a.append("%d/%s"%(v+self.v_offset, vt))
                else:
                    a.append("%d/%s/%s"%(v+self.v_offset, vt, vn))
                facevaridx += 1
            print >>self.fhandle, "f %s"%" ".join(a)

    # preProcessMaterial
    def preProcessMaterial(self, mat):
        """Check and store the material.

        The return value is the material name (which might have been
        modified to make it unique).
        """
        # Check if the material was already exported
        if mat==self.materials.get(mat.name, None):
            return mat.name

        # Determine a unique name for the material
        name = mat.name
        m = re.search("[0-9]+$", name)
        if m==None:
            basename = name
            num = 0
        else:
            basename = name[:m.start()]
            num = int(name[m.start():m.end()])
        while name in self.materials:
            num += 1
            name = basename + str(num)

        self.materials[name] = mat
            
        return name

    # getGroups
    def getGroups(self, obj):
        """Get a list of "groups".

        Return a list that contains the names from the root to the object
        (excluding the world root).
        """
        res = []
        wr = getScene().worldRoot()
        while obj!=None:
            res = [obj.name]+res
            obj = obj.parent
        return res[1+self.group_offset:]

    # convertObject
    def convertObject(self, obj):
        """Converts an object into a polyhedron or trimesh if necessary.

        The return value is a GeomObject (TriMeshGeom or PolyhedronGeom)
        or None.
        """
        geom = obj.geom
        if isinstance(geom, TriMeshGeom):
            return geom
        
        if not isinstance(geom, PolyhedronGeom):
            # Try to convert into a polyhedron...
            pg = PolyhedronGeom()
            try:
                geom.convert(pg)
                geom = pg
            except:
                pass

        # Is it a PolyhedronGeom that has no polys with holes? then return
        # the geom...
        if isinstance(geom, PolyhedronGeom) and not geom.hasPolysWithHoles():
            return geom

        # Try to convert into a triangle mesh...
        tm = TriMeshGeom()
        try:
            geom.convert(tm)
            return tm
        except:
            pass

        return None
        

######################################################################

# Register the exporter class as a plugin class
pluginmanager.register(OBJExporter)
