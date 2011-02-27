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
# $Id: offimport.py,v 1.3 2005/04/14 08:01:50 mbaas Exp $

import os.path, sys
from cgtypes import *
from worldobject import WorldObject
from geomobject import *
from trimesh import TriMesh
from trimeshgeom import TriMeshGeom
from polyhedron import Polyhedron
from polyhedrongeom import PolyhedronGeom
import pluginmanager

# OffImporter
class OffImporter:

    _protocols = ["Import"]

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

    # importFile
    def importFile(self, filename, invertfaces=False):
        """Import an OFF file.

        If invertfaces is True the orientation of each face is reversed.
        """

        self.invertfaces = invertfaces

        self.texcoord_flag = False
        self.color_flag = False
        self.normal_flag = False
        self.four_flag = False
        self.ndim_flag = False
        self.ndim = 3
        self.is_trimesh = True

        self.fhandle = file(filename)

        # Read the header
        z = self.readLine()
        self.parseHeaderKeyWord(z)

        # nOFF?
        if self.ndim_flag:
            # Read the dimension of vertices
            z = self.readLine()
            self.ndim = int(z)
            if self.ndim>3:
                raise ValueError, "A vertex space dimension of %d is not supported"%(self.ndim)

        # Read the number of vertices and faces...
        z = self.readLine().split(" ")
        self.numverts = int(z[0])
        self.numfaces = int(z[1])

        # Start with a TriMeshGeom
        # (this will be later converted into a PolyhedronGeom if faces
        # with more than 3 vertices are encountered)
        self.geom = TriMeshGeom()
        self.geom.verts.resize(self.numverts)
        self.geom.faces.resize(self.numfaces)

        # Read the vertices...
        self.readVertices()

        # Read the faces...
        self.readFaces()

        self.fhandle.close()

        # Create the actual object...
        nodename = os.path.basename(filename)
        nodename = os.path.splitext(nodename)[0]
        nodename = nodename.replace(" ","_")
        if self.is_trimesh:
            n = TriMesh(name=nodename)
        else:
            n = Polyhedron(name=nodename)
        n.geom = self.geom

    # readFaces
    def readFaces(self):
        """Read the faces.
        """
        geom = self.geom

        Cs = None
        
        for i in range(self.numfaces):
            z = self.readLine()
            a = z.split()
            # Get the number of vertices in this face
            Nv = int(a[0])
            if Nv<3:
                print >>sys.stderr, "Warning: Faces must have at least three vertices"
                continue

            # Set the face...
            face = map(lambda x: int(x), a[1:Nv+1])
            if self.invertfaces:
                face.reverse()
            if self.is_trimesh:
                if Nv==3:
                    geom.faces[i] = face
#                    geom.faces[i] = (int(a[1]), int(a[3]), int(a[2]))
                else:
                    # Convert the TriMeshGeom into a PolyhedronGeom...
                    tm = self.geom
                    geom = self.triMesh2Polyhedron(tm)
                    self.geom = geom
                    self.is_trimesh = False
                    # Get the Cs slot (if there is one)
                    try:
                        Cs = geom.slot("Cs")
                    except:
                        Cs = None
                    # Initialize the faces processed so far...
                    for j in range(i):
                        self.geom.setPoly(j, [tm.faces[j]])
                    # ...and set the current face
                    self.geom.setPoly(i, [face])
            else:
                geom.setPoly(i, [face])

            # Process color...
            ca = a[Nv+1:]
            if len(ca)>2:
                # Check and see if the rgb values are given as ints, in this
                # case they have to be scaled down.
                try:
                    r = int(ca[0])
                    g = int(ca[1])
                    b = int(ca[2])
                    col = vec3(r,g,b)/255
                except:
                    r = float(ca[0])
                    g = float(ca[1])
                    b = float(ca[2])
                    col = vec3(r,g,b)
                    
                if Cs==None:
                    geom.newVariable("Cs", UNIFORM, COLOR)
                    Cs = geom.slot("Cs")

                Cs[i] = col
                    

    # triMesh2Polyhedron
    def triMesh2Polyhedron(self, tm):
        """Convert a TriMeshGeom into a PolyhedronGeom.
        """
        pg = PolyhedronGeom()
        # Copy the vertices...
        pg.verts.resize(tm.verts.size())
        tm.verts.copyValues(0, tm.verts.size(), pg.verts, 0)
        # Allocate polygons...
        pg.setNumPolys(tm.faces.size())

        # Copy primitive variables...
        # (tm must not have any facevarying or facevertex variables)
        for name, storage, type, mult in tm.iterVariables():
            slot = tm.slot(name)
            pg.newVariable(name, storage, type, mult, slot.size())
            newslot = pg.slot(name)
            slot.copyValues(0, slot.size(), newslot, 0)

        return pg
        
    # readVertices
    def readVertices(self):
        """Read the vertices (and varying variables).

        The number of vertices must have been stored in self.numverts
        and the ST, C, N flags must have been initialized. self.geom must
        be either a TriMeshGeom or a PolyhedronGeom.
        """
        geom = self.geom
        
        # Create primitive variables...
        if self.normal_flag:
            geom.newVariable("N", VARYING, NORMAL)
            N = geom.slot("N")
        if self.color_flag:
            geom.newVariable("Cs", VARYING, COLOR)
            Cs = geom.slot("Cs")
        if self.texcoord_flag:
            geom.newVariable("st", VARYING, FLOAT, 2)
            st = geom.slot("st")
        
        # Read the vertices...
        for i in range(self.numverts):
            z = self.readLine()
            f = map(lambda x: float(x), z.split())
            geom.verts[i] = vec3(f[:self.ndim])
            f = f[self.ndim:]
            if self.four_flag:
                w = f[0]
                geom.verts[i] /= w
                f = f[1:]
            if self.normal_flag:
                N[i] = vec3(f[:3])
                f = f[3:]
            if self.color_flag:
                Cs[i] = vec3(f[:3])
                f = f[3:]
            if self.texcoord_flag:
                st[i] = f[:2]
        

    # parseHeaderKeyWord
    def parseHeaderKeyWord(self, header):
        """Parses the first line of an OFF file.

        Returns True if the line actually was the header keyword (as it
        is optional).
        The method sets the ST, C, N, 4 and ndim flag to True if the
        according prefix is present.
        """
        header = header.strip()
        if header[-3:]!="OFF":
            return False

        if header[:2]=="ST":
            self.texcoord_flag = True
            header = header[2:]
        if header[:1]=="C":
            self.color_flag = True
            header = header[1:]
        if header[:1]=="N":
            self.normal_flag = True
            header = header[1:]
        if header[:1]=="4":
            self.four_flag = True
            header = header[1:]
        if header[:1]=="n":
            self.ndim_flag = True
            header = header[1:]

        if header!="OFF":
            print >>sys.stderr, "Warning: Unknown prefixes in the header keyword"
        return True

    # readLine
    def readLine(self):
        """Read the next line.

        Returns the next line (without the trailing newline) that is not
        empty or a comment. If the end of the file was reached, an exception
        is thrown.
        """
        while 1:
            z = self.fhandle.readline()
            if z=="":
                raise SyntaxError, "premature end of file"
            z = z.strip()
            if z=="" or z[0]=='#':
                continue
            return z
        

######################################################################

# Register the OffImporter class as a plugin class
pluginmanager.register(OffImporter)
