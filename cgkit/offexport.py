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
# $Id: offexport.py,v 1.2 2005/04/14 17:22:24 mbaas Exp $

import os.path, sys
from cgtypes import *
from scene import getScene
from geomobject import *
from trimeshgeom import TriMeshGeom
from polyhedrongeom import PolyhedronGeom
import pluginmanager
import cmds

# OffExporter
class OffExporter:

    _protocols = ["Export"]

    # extension
    def extension():
        """Return the file extensions for this format."""
        return ["off"]
    extension = staticmethod(extension)

    # description
    def description(self):
        """Return a short description for the file dialog."""
        return "Geomview OFF file"
    description = staticmethod(description)

    # exportFile
    def exportFile(self, filename, root=None):
        """Export an OFF file.

        root is the root of the subtree that should be exported.
        """
        self.N_flag = False
        self.C_flag = False
        self.ST_flag = False

        # Create a list of objects that should be exported
        scene = getScene()
        self.objects = []
        root = cmds.worldObject(root)
        if root!=None:
            self.objects.append(root)
        self.objects += list(scene.walkWorld(root))

        # Initialize variable flags and return number of verts and faces
        numverts, numfaces = self.getNumVertsNFaces()

        self.fhandle = file(filename, "w")
        # Write header line
        kw = ""
        if self.ST_flag:
            kw += "ST"
        if self.C_flag:
            kw += "C"
        if self.N_flag:
            kw += "N"
        kw += "OFF"
        print >>self.fhandle, kw

        # Write number of vertices and faces
        print >>self.fhandle, "%d %d 0"%(numverts, numfaces)

        # Write vertices...
        self.writeVertices()

        # Write faces
        self.writeFaces()

        self.fhandle.close()

    # writeFaces
    def writeFaces(self):
        """Write the faces.
        """
        for obj in self.objects:
            geom = self.convertObject(obj)
            if geom==None:
                continue

            # Check for primitive variables...
            Cs = None
            info = geom.findVariable("Cs")
            if info!=None and info[1]==UNIFORM and info[2]==COLOR and info[3]==1:
                Cs = geom.slot("Cs")
            
            vo = self.voffsets[obj]
            if isinstance(geom, TriMeshGeom):
                for i in range(geom.faces.size()):
                    f = geom.faces[i]
                    s = "3 %d %d %d"%(f[0]+vo, f[1]+vo, f[2]+vo)
                    if Cs!=None:
                        s += "  %f %f %f"%tuple(Cs[i])
                    print >>self.fhandle, s
            else:
                for i in range(geom.getNumPolys()):
                    poly = geom.getPoly(i)[0]
                    nv = len(poly)
                    loop = map(lambda n: n+vo, poly)
                    s = ("%d "%nv)+(nv*"% d")%tuple(loop)
                    if Cs!=None:
                        s += "  %f %f %f"%tuple(Cs[i])
                    print >>self.fhandle, s
                    

    # writeVertices
    def writeVertices(self):
        """Write the vertices and corresponding varying primitive variables.

        The method initializes the voffsets dictionary that contains
        the vertex offsets for each object.
        """
        voffset = 0
        voffsets = {}
        for obj in self.objects:
            geom = self.convertObject(obj)
            if geom==None:
                continue

            voffsets[obj] = voffset

            # Check for primitive variables...
            N = None
            info = geom.findVariable("N")
            if info!=None and info[1]==VARYING and info[2]==NORMAL and info[3]==1:
                N = geom.slot("N")
                
            Cs = None
            info = geom.findVariable("Cs")
            if info!=None and info[1]==VARYING and info[2]==COLOR and info[3]==1:
                Cs = geom.slot("Cs")

            st = None
            info = geom.findVariable("st")
            if info!=None and info[1]==VARYING and info[2]==FLOAT and info[3]==2:
                st = geom.slot("st")

            # Get the world transform to adjust the vertices...
            WT = obj.worldtransform
            WT3 = WT.getMat3()

            # Iterate over all vertices and write the stuff...
            for i in range(geom.verts.size()):
                v = WT*geom.verts[i]
                s = "%f %f %f"%tuple(v)
                if self.N_flag:
                    if N!=None:
                        norm = WT3*N[i]
                        try:
                            norm = norm.normalize()
                        except:
                            pass
                        s += "  %f %f %f"%tuple(norm)
                    else:
                        s += "  0 0 0"
                if self.C_flag:
                    if Cs!=None:
                        s += "  %f %f %f"%tuple(Cs[i])
                    else:
                        s += "  0.666 0.666 0.666"
                if self.ST_flag:
                    if st!=None:
                        s += "  %f %f"%st[i]
                    else:
                        s += "  0.0 0.0"
                
                print >>self.fhandle, s

            voffset += geom.verts.size()
            
        self.voffsets = voffsets

    # getNumVertsNFaces
    def getNumVertsNFaces(self):
        """Return the total number of vertices and faces.

        The method also sets the variable flags to True if there is
        an object that has those variables set.
        """
        numverts = 0
        numfaces = 0
        for obj in self.objects:
            geom = self.convertObject(obj)
            if geom!=None:
                numverts += geom.verts.size()
                if isinstance(geom, TriMeshGeom):
                    numfaces += geom.faces.size()
                else:
                    numfaces += geom.getNumPolys()

                info = geom.findVariable("N")
                if info!=None and info[1]==VARYING and info[2]==NORMAL and info[3]==1:
                    self.N_flag = True
                info = geom.findVariable("Cs")
                if info!=None and info[1]==VARYING and info[2]==COLOR and info[3]==1:
                    self.C_flag = True
                info = geom.findVariable("st")
                if info!=None and info[1]==VARYING and info[2]==FLOAT and info[3]==2:
                    self.ST_flag = True
                    
        return numverts, numfaces


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
pluginmanager.register(OffExporter)
