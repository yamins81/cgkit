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
# $Id: cmds.py,v 1.12 2006/02/16 17:49:38 mbaas Exp $

## \file cmds.py
## Contains builtin and user defined commands.

# (this line separates the \file comment from the module docs)

"""Commands.
"""

import sys, os, os.path, types, string
import pluginmanager
import eventmanager
import events
from scene import getScene
import worldobject
import draw, drawtextgeom
import trimeshgeom
import polyhedrongeom
import group as group_module
import _core
from cgtypes import *

class NoImporter(Exception): pass
class NoExporter(Exception): pass



# TriMeshPolyAdapter
class TriMeshPolyAdapter:
    """Provides the PolyhedronGeom interface for a TriMeshGeom.

    This is a preliminary class that's used in the convFaceVarToVar()
    function so that TriMeshGeoms can be handled the same way than
    PolyhedronGeoms.

    Todo: This class should be stored somewhere else and probably be made
    available as a true adapter that can be used with the protocols framework.

    Todo: Most Component methods are still missing.
    """
    def __init__(self, tm):
        self.tm = tm
        
        self.verts = tm.verts
        self.verts_slot = tm.verts_slot

    def slot(self, name):
        return self.tm.slot(name)

    def boundingBox(self):
        return self.tm.boundingBox()

    def drawGL(self):
        self.tm.drawGL()

    def uniformCount(self):
        return self.tm.uniformCount()

    def vertexCount(self):
        return self.tm.vertexCount()

    def varyingCount(self):
        return self.tm.varyingCount()

    def faceVaryingCount(self):
        return self.tm.faceVaryingCount()

    def faceVertexCount(self):
        return self.tm.faceVertexCount()

    def newVariable(self, name, storage, type, multiplicity=1, user_n=0):
        self.tm.newVariable(name, storage, type, multiplicity, user_n)

    def deleteVariable(self, name):
        self.tm.deleteVariable(name)

    def deleteAllVariables(self):
        self.tm.deleteAllVariables()

    def findVariable(self, name):
        return self.tm.findVariable(name)

    def iterVariables(self):
        return self.tm.iterVariables()

    def convert(self, targetgeom):
        self.tm.convert(targetgeom)
        
    def hasPolysWithHoles(self):
        return False

    def getNumPolys(self):
        return self.tm.faces.size()

    def getNumLoops(self, poly):
        return 1

    def getNumVerts(self, poly, loop):
        return 3

    def setNumPolys(self, num):
        self.tm.faces.resize(num)

    def setNumLoops(self, poly, num):
        if num>1:
            raise RuntimeError, "Triangles cannot have holes"

    def getLoop(self, poly, loop):
        if loop==0:
            return self.tm.faces[poly]
        else:
            raise RuntimeError, "Triangles have no holes"

    def setLoop(self, poly, loop, vloop):
        if loop==0:
            self.tm.faces[poly] = vloop
        else:
            raise RuntimeError, "Triangles cannot have holes"

    def getPoly(self, poly):
        return [self.tm.faces[poly]]

    def setPoly(self, poly, polydef):
        self.tm.faces[poly] = polydef[0]
    

def convFaceVarToVar(geom):
    """Convert all facevarying into varying.

    All facevarying variables are converted into varying variables and
    all facevertex variables into vertex variables.
    This is done by splitting the vertices in the mesh.
    The number of values (and the values themselves) of the facevarying
    variables do not change, only the verts and varying/vertex variables
    are modified.

    geom can be either a TriMeshGeom or a PolyhedronGeom.

    The return value is the new geom object (the original object is not
    modified).
    """

    if isinstance(geom, trimeshgeom.TriMeshGeom):
        newgeom = trimeshgeom.TriMeshGeom()
        newgeom.faces.resize(geom.faces.size())
        geom.faces.copyValues(0, geom.faces.size(), newgeom.faces, 0)
        workinggeom = TriMeshPolyAdapter(newgeom)
    elif isinstance(geom, polyhedrongeom.PolyhedronGeom):
        newgeom = polyhedrongeom.PolyhedronGeom()
        newgeom.setNumPolys(geom.getNumPolys())
        workinggeom = newgeom
        # Copy the polygons (even though the indices have to be modified
        # afterwards), so that the variables can be created
        for i in range(geom.getNumPolys()):
            poly = geom.getPoly(i)
            newgeom.setPoly(i, poly)
    else:
        raise TypeError, "geom must be a TriMeshGeom or PolyhedronGeom"

    # Allocate storage for the vertices
    workinggeom.verts.resize(geom.faceVaryingCount())

    # This is a list with (oldslot,newslot) tuples. This is later used to
    # initialize the new varying/vertex slots.
    varyingvars = [(geom.verts, newgeom.verts)]

    # Create a copy of all variables...
    for name,storage,type,mult in geom.iterVariables():

        # facevarying/facevertex will be converted to varying/vertex
        if storage==_core.VarStorage.FACEVARYING:
            newstorage = _core.VarStorage.VARYING
        elif storage==_core.VarStorage.FACEVERTEX:
            newstorage = _core.VarStorage.VERTEX
        else:
            newstorage = storage            
            
        slot = geom.slot(name)
        workinggeom.newVariable(name, newstorage, type, mult, slot.size())
        newslot = workinggeom.slot(name)
        # Original varying/vertex variables will be initialized later,
        # everything else is just a copy.
        if storage==_core.VarStorage.VARYING or storage==_core.VarStorage.VERTEX:
            varyingvars.append((slot, newslot))
        else:
            slot.copyValues(0, slot.size(), newslot, 0)


    # Initialize the varying/vertex variables and update the vertex indices
    # in the faces...
    vidx = 0
    for i in range(workinggeom.getNumPolys()):
        poly = workinggeom.getPoly(i)
        # ids: Flattened version of poly (i.e. all vertex ids)
        ids = []
        # newpoly: New polygon with new indices (sequentially numbered)
        newpoly = []
        vidx2 = vidx
        for loop in poly:
            ids += loop
            newpoly.append(range(vidx2, vidx2+len(loop)))
            vidx2 += len(loop)

        workinggeom.setPoly(i, newpoly)

        for id in ids:
            for slot,newslot in varyingvars:
                newslot[vidx] = slot[id]
            vidx += 1

    return newgeom
        

def removeIsolatedVertices(tm):

    print >>sys.stderr, 'Removing isolated vertices...'

    newgeom = trimeshgeom.TriMeshGeom()
    newgeom.faces.resize(len(tm.faces))

    # Key: Old vertex index   Value: New vertex index
    vert_used = {}
    # Index: New id   Value: Old id
    newverts = []
    for i in range(len(tm.faces)):
        ai,bi,ci = tm.faces[i]
        if ai not in vert_used:
            vert_used[ai] = len(newverts)
            newverts.append(ai)
        if bi not in vert_used:
            vert_used[bi] = len(newverts)
            newverts.append(bi)
        if ci not in vert_used:
            vert_used[ci] = len(newverts)
            newverts.append(ci)

        newgeom.faces[i] = (vert_used[ai], vert_used[bi], vert_used[ci])
            
    newgeom.verts.resize(len(newverts))

    # Set new vertices
    for i in range(len(newverts)):
        newgeom.verts[i] = tm.verts[newverts[i]]

    # Copy remaining variables...
    for name,storage,type,mult in tm.iterVariables():
        if storage==_core.VarStorage.VARYING:
            newgeom.newVariable(name, _core.VarStorage.VARYING, type, mult)
            src = tm.slot(name)
            dst = newgeom.slot(name)
            for i in range(len(newverts)):
                oldid = newverts[i]
                dst[i] = src[oldid]
        else:
            src = tm.slot(name)
            # Create slot
            if storage==_core.VarStorage.USER:
                newgeom.newVariable(name, _core.VarStorage.USER, type, mult, len(src))
            else:
                newgeom.newVariable(name, storage, type, mult)
            dst = newgeom.slot(name)
            # Copy slot
            src.copyValues(0, len(src), dst, 0)
#            for i in range(len(src)):
#                dst[i] = src[i]

    return newgeom
    

# extractUniform
def extractUniform(tm, varname, value):
    """Extract a uniform variable from a TriMesh.

    The resulting mesh will only contain faces that have the
    given value.
    """

    print >>sys.stderr, 'Extracting variable "%s"...'%(varname)

    var_slot = tm.slot(varname)
    # A list representing the new faces.
    # Each entry newfaces[i] represents face i of the new mesh
    # The value newfaces[i] is the index of the old face
    newfaces = []

    # Determine the new faces...
    for i in range(len(tm.faces)):
        if var_slot[i]==value:
            newfaces.append(i)

    newgeom = trimeshgeom.TriMeshGeom()
    newgeom.faces.resize(len(newfaces))
    newgeom.verts.resize(len(tm.verts))

    # Initialize new faces...
    for i in range(len(newfaces)):
        oldid = newfaces[i]
        newgeom.faces[i] = tm.faces[oldid]

    # Copy vertices...
    tm.verts.copyValues(0, len(tm.verts), newgeom.verts, 0)
#    for i in range(len(tm.verts)):
#        newgeom.verts[i] = tm.verts[i]

    # Copy remaining variables...
    for name,storage,type,mult in tm.iterVariables():
        if storage==_core.VarStorage.UNIFORM:
            newgeom.newVariable(name, _core.VarStorage.UNIFORM, type, mult)
            src = tm.slot(name)
            dst = newgeom.slot(name)
            for i in range(len(newfaces)):
                oldid = newfaces[i]
                dst[i] = src[oldid]
        elif (storage==_core.VarStorage.FACEVARYING or
              storage==_core.VarStorage.FACEVERTEX):
            newgeom.newVariable(name, storage, type, mult)
            src = tm.slot(name)
            dst = newgeom.slot(name)
            for i in range(len(newfaces)):
                oldid = newfaces[i]
                dst[3*i] = src[3*oldid]
                dst[3*i+1] = src[3*oldid+1]
                dst[3*i+2] = src[3*oldid+2]
        else:
            src = tm.slot(name)
            # Create slot
            if storage==_core.VarStorage.USER:
                newgeom.newVariable(name, _core.VarStorage.USER, type, mult, len(src))
            else:
                newgeom.newVariable(name, storage, type, mult)
            dst = newgeom.slot(name)
            # Copy slot
            src.copyValues(0, len(src), dst, 0)
#            for i in range(len(src)):
#                dst[i] = src[i]

    return newgeom


def _mapID(id, attrid, attr, attr_lut, newverts):
    """Determine new vertex ID.

    attr_lut is a dictionary that servers as attribute lookup table.
    Key is the vertex ID and the value is another dictionary that
    has the attribute ID as key and the new ID as value.

    \param id  Old vertex ID
    \param attrid  Attribute ID
    \param attr  Attribute value
    \param attr_lut  Attribute lookup table
    \partam newverts
    \return New vertex ID
    """
    if id not in attr_lut:
        attr_lut[id] = {}
        
    newid_lut = attr_lut[id]
    if attrid in newid_lut:
        return newid_lut[attrid]
    else:
        newid = len(newverts)
        newid_lut[attrid] = newid
        newverts.append((id, attr))
        return newid


def convMeshAttr(tm, attrname, attrfacesname=None):
    """Convert attribute type.

    attrname is the name of the attribute that should be turned into
    a "varying" attribute.
    attrfacesnames is the name of the corresponding attribute
    faces. This must be a uniform variable.

    The result is a copy of tm where the attribute attrname is a
    "varying" variable. This means each vertex has a unique attribute.
    The result has the same number of faces than tm, but the number
    of vertices is usally higher.

    \param tm TriMeshGeom
    \param attrname Attribute 
    \param attrsfacesnames Faces attribute
    \return New TriMeshGeom
    """
    if attrfacesname==None:
        attrfacesname = attrname+"faces"
        
    print >>sys.stderr, 'Converting "%s"/"%s" into "varying %s"...'%(attrname,attrfacesname,attrname)
    newgeom = trimeshgeom.TriMeshGeom()

    attrdesc = tm.findVariable(attrname)

    attr_slot = tm.slot(attrname)
    attrfaces_slot = tm.slot(attrfacesname)

    attr_lut = {}
    # A list of tuples (Old vertex ID, Attribute ID)
    # An entry newverts[i] represents vertex i of the new mesh
    newverts = []
    
    newgeom.faces.resize(len(tm.faces))
    
    # Check each face...
    for i in range(len(tm.faces)):
        ai,bi,ci = tm.faces[i]
        attrai,attrbi,attrci = attrfaces_slot[i]

        # Get the 3 attributes associated to the current face
        attra = attr_slot[attrai]
        attrb = attr_slot[attrbi]
        attrc = attr_slot[attrci]

        newai = _mapID(ai, attrai, attra, attr_lut, newverts)
#        print "A: %d -> %d"%(ai,newai)
        newbi = _mapID(bi, attrbi, attrb, attr_lut, newverts)
#        print "B: %d -> %d"%(bi,newbi)
        newci = _mapID(ci, attrci, attrc, attr_lut, newverts)
#        print "C: %d -> %d"%(ci,newci)
        
        newgeom.faces[i] = (newai,newbi,newci)

    # Set the new vertices...
    numnewverts = len(newverts)
    newgeom.verts.resize(numnewverts)
    newgeom.newVariable(attrname, _core.VarStorage.VARYING, attrdesc[2], attrdesc[3])
    newattr_slot = newgeom.slot(attrname)
    for i in range(numnewverts):
        oldid,attr = newverts[i]
        newgeom.verts[i] = tm.verts[oldid]
        newattr_slot[i] = attr

    # Copy remaining variables...
    for name,storage,type,mult in tm.iterVariables():
        if name==attrname or name==attrfacesname:
            continue
        if storage==_core.VarStorage.VARYING:
            newgeom.newVariable(name, _core.VarStorage.VARYING, type, mult)
            src = tm.slot(name)
            dst = newgeom.slot(name)
            for i in range(numnewverts):
                oldid,attr = newverts[i]
                dst[i] = src[oldid]
        else:
            src = tm.slot(name)
            # Create slot
            if storage==_core.VarStorage.USER:
                newgeom.newVariable(name, _core.VarStorage.USER, type, mult, len(src))
            else:
                newgeom.newVariable(name, storage, type, mult)
            dst = newgeom.slot(name)
            # Copy slot
            src.copyValues(0, len(src), dst, 0)
#            for i in range(len(src)):
#                dst[i] = src[i]

    return newgeom

# fitCone
def fitCone(pos, obj):
    """Compute a cone that has its apex at pos and that includes obj.

    The generated cone is the minimal cone that entirely contains the
    bounding box of obj (which must be a WorldObject).
    pos is the apex of the cone given in world coordinates.
    The return value is a tuple (n,w) where n is the axis direction of the
    cone and w is the (full) angle in radians.
    """
    pos = vec3(pos)
    W = obj.worldtransform
    Winv = W.inverse()
    p = Winv*pos
    
    bb = obj.boundingBox()
    a,b,c,d,e,f,g,h = _bbCorners(bb)
    a = (a-p).normalize()
    b = (b-p).normalize()
    c = (c-p).normalize()
    d = (d-p).normalize()
    e = (e-p).normalize()
    f = (f-p).normalize()
    g = (g-p).normalize()
    h = (h-p).normalize()
    n1,w1 = _cone(a,h)
    n2,w2 = _cone(b,g)
    n3,w3 = _cone(c,f)
    n4,w4 = _cone(d,e)
    w = max(w1,w2,w3,w4)
    if w1==w:
        n = n1
    elif w2==w:
        n = n2
    elif w3==w:
        n = n3
    else:
        n = n4

    n = W.getMat3()*n
    return n,w

# _cone
def _cone(a,b):
    """Create a cone from two directions.

    a and b are two normalized directions that point along the generated
    cone on two opposite directions, i.e. the returned cone is the minmal
    cone that has the 'rays' a and b on its surface.
    The return value is a tuple (n,w) where n is the cone axis and w
    the (full) angle in radians.

    This is a helper function for the fitCone() function.
    """
    n = (a+b).normalize()
    w = a.angle(b)
    return n,w
    

# _bbCorners
def _bbCorners(bb):
    """Return the eight corners of a bounding box.

    bb is a BoundingBox object. The index of the returned points is
    such that bit 0 determines if the point is minmal or maximal in x
    direction. Bit 1 is the y direction and bit 2 the z
    direction. This means point n is opposite to point (~n)&0x7.

    This is a helper function for the fitCone() function.
    """
    a,h = bb.getBounds()
    b = vec3(h.x, a.y, a.z)
    c = vec3(a.x, h.y, a.z)
    d = vec3(h.x, h.y, a.z)
    e = vec3(a.x, a.y, h.z)
    f = vec3(h.x, a.y, h.z)
    g = vec3(a.x, h.y, h.z)
    return a,b,c,d,e,f,g,h


# delete
def delete(objs):
    """Delete objects from the scene.

    Currently, this only works on WorldObjects...
    """
    objs = worldObjects(objs)
    for obj in objs:
        obj.parent.removeChild(obj)
        

# convertToTriMesh
def convertToTriMesh(objs):
    """Convert one or more objects into triangle meshes.
    """
    objs = worldObjects(objs)
    for obj in objs:
        tm = trimeshgeom.TriMeshGeom()
        obj.geom.convert(tm)
        obj.geom = tm

# convertToPolyhedron
def convertToPolyhedron(objs):
    """Convert one or more objects into polyhedron objects.
    """
    objs = worldObjects(objs)
    for obj in objs:
        pg = polyhedrongeom.PolyhedronGeom()
        obj.geom.convert(pg)
        obj.geom = pg

# setupObjectNames
def setupObjectNames():
    """Create a string that can be executed to 'import' all scene names.
    """
    res = ""
    valid_chars = string.ascii_letters + "_" + string.digits
    for obj in getScene().item("WorldRoot").iterChilds():
        varname = ""
        for c in obj.name:
            if c not in valid_chars:
                c = "_"
            varname += c
        if varname=="":
            continue
        if varname[0] in string.digits:
            varname = "_"+varname[1:]
        res += '%s = worldObject("%s")\n'%(varname, obj.name)
    return res

# importDefaultPlugins
def importDefaultPlugins():
    """Import the default plugins.

    The plugin files/directories specified by the CGKIT_PLUGIN_PATH
    environment variable (if it exists) are imported.
    The function already outputs error messages and returns a list
    of plugin descriptors.
    """
    s = os.environ.get("CGKIT_PLUGIN_PATH", "")
    plugins = splitPaths(s)
        
    descs = pluginmanager.importPlugins(plugins)
    
    for desc in descs:
        if desc.status!=pluginmanager.STATUS_OK:
            sys.stderr.write(70*"-"+"\n")
            sys.stderr.write('ERROR: Loading plugin "%s" failed:\n'%os.path.basename(desc.filename))
            sys.stderr.write("\n"+desc.traceback)
            sys.stderr.write(70*"-"+"\n")

    return descs
            
# splitPaths
def splitPaths(paths):
    """Split a string containing paths into the individual paths.
    
    The paths can either be separated by ':' or ';'. Windows drive
    letters are maintained, even when ':' is used as separator.

    Example:

    \code
    >>> splitPaths("&:c:\\shaders:c:\\more_shaders;")
    ['&', 'c:\\shaders', 'c:\\more_shaders']    
    \endcode
    
    \param paths (\c str) Paths string
    \return A list with individual path strings.
    """
        
    a = paths.split(";")
    chunks = []
    for s in a:
        if s!="":
            chunks += s.split(":")

    pathlist = []
    while len(chunks)>0:
        # Is this item only a path drive?
        if len(chunks[0])==1 and chunks[0] in string.letters:
            pathlist.append(":".join(chunks[:2]))
            chunks = chunks[2:]
        else:
            pathlist.append(chunks[0])
            chunks = chunks[1:]

    return map(lambda x: x.strip(), pathlist)

# group
def group(*children, **keyargs):
    """Group several world objects together.

    All non keyword arguments somehow refer to world objects that will
    all be grouped together. An argument may either be a WorldObject,
    the name of a world object or as a sequence of world objects or names.
    The name of the new group may be given via the \a name keyword
    argument.
    The return value is the new Group object.
    
    \param name (\c str) Name of the group
    \param children The world objects or their names.
    """

    # Check the key arguments
    for k in keyargs:
        if k not in ["name"]:
            raise TypeError, "Unknown keyword argument: '%s'"%k

    # "Flatten" the children list...
    childs = []
    for c in children:
        if isinstance(c, types.StringTypes):
            childs += [c]
        else:
            # Is c a sequence?
            try:
                childs += list(c)
            except TypeError:
                # obviously not...
                childs += [c]
                
    childs = map(worldObject, childs)
    name = keyargs.get("name", "group")
    grp = group_module.Group(name=name, childs=childs)
    return grp
        

# ungroup
def ungroup(group):
    """Break up a group in its individual components.

    \param group (\c WorldObject or str) Group object
    """
    group = worldObject(group)
    if group.geom!=None:
        raise ValueError, 'Object "%s" is not a mere group. Remove the geometry before ungrouping.'%group.name

    # Move the children up one level
    newparent = group.parent
    for child in list(group.iterChilds()):
        link(child, newparent)

    # Remove the group
    newparent.removeChild(group)
        

# replaceMaterial
def replaceMaterial(name, newmat):
    """Iterate over all world objects and replace a material with a new one.

    \param name (\c str) The name of an already assigned material
    \param newmat (\c Material) The new material
    """
    for obj in getScene().walkWorld():
        # Check all materials...
        for i in range(obj.getNumMaterials()):
            mat = obj.getMaterial(i)
            if mat==None:
                continue
            if mat.name==name:
                obj.setMaterial(newmat, i)
    

# link
def link(childs, parent=None, relative=False):
    """Link all childs to parent.

    The function modifies the name of a child object if there would
    be a clash with an existing object under the new parent.

    \param childs (\c list or WorldObject) A single WorldObject or a sequence of WorldObjects
    \param parent (\c WorldObject) Parent object or None (=unlink)
    \param relative (\c bool) If True the local child transform is not modified (i.e. it is interpreted as a relative position)
    """

    # Check if childs is a sequence (other than a string)...
    try:
        len(childs)
        if isinstance(childs, types.StringTypes):
            is_sequence = False
        else:
            is_sequence = True
    except:
        is_sequence = False

    if not is_sequence:
        childs = [childs]

    # No parent given? Then use the world root (=unlink)
    if parent==None:
        parent = getScene().worldRoot()
    else:
        parent = worldObject(parent)

    # Relink all childs...
    for child in childs:
        child = worldObject(child)
        oldparent = child.parent
        if oldparent!=None:
            oldparent.removeChild(child)
        if not relative:
            Lp1 = oldparent.worldtransform
            Lp2 = parent.worldtransform
            child.transform = Lp2.inverse()*Lp1*child.transform
        child.name = parent.makeChildNameUnique(child.name) 
        parent.addChild(child)
    

# drawClear
def drawClear():
    """Clear all drawing objects."""
    # Clear all markers and lines
    try:
        drw = getScene().worldObject("Global_Draw")
        drw.clear()
    except KeyError:
        pass

    # Clear all texts
    try:
        drw = getScene().worldObject("Global_DrawText")
        drw.geom.clear()
    except KeyError:
        pass
    
    
# drawMarker
def drawMarker(pos, color=(1,1,1), size=1):
    """Draw a marker.

    \param pos Marker position 
    \param color Marker color
    \param size Marker size (radius)
    """
    try:
        drw = getScene().worldObject("Global_Draw")
    except KeyError:
        drw = draw.Draw(name="Global_Draw")
    drw.marker(pos, color, size)

# drawLine
def drawLine(pos1, pos2, color=(1,1,1), size=1):
    """Draw a line.

    \param pos1 First line point
    \param pos2 Second line point
    \param color Line color
    \param size Line width
    """
    try:
        drw = getScene().worldObject("Global_Draw")
    except KeyError:
        drw = draw.Draw(name="Global_Draw")
    drw.line(pos1, pos2, color, size)

# drawText
def drawText(pos, txt, font=None, color=(1,1,1)):
    """Draw a text string.

    font can be one of the constants defined in GLUT:

    - GLUT_BITMAP_8_BY_13
    - GLUT_BITMAP_9_BY_15 (default)
    - GLUT_BITMAP_TIMES_ROMAN_10
    - GLUT_BITMAP_TIMES_ROMAN_24
    - GLUT_BITMAP_HELVETICA_10
    - GLUT_BITMAP_HELVETICA_12
    - GLUT_BITMAP_HELVETICA_18

    \param pos Text position (3D)
    \param txt The actual text
    \param font A GLUT font constant
    \param color Text color    
    """
    try:
        drw = getScene().worldObject("Global_DrawText")
    except KeyError:
        drw = worldobject.WorldObject(name="Global_DrawText")
        drw.geom = drawtextgeom.DrawTextGeom()
    drw.geom.addText(pos, txt, font, color)

# listWorld
def listWorld():
    """List the contents of the world as a tree.
    """
    scn = getScene()
    print "Root"
    _listWorld(scn.worldRoot(), "")

def _listWorld(obj, lines):
    """Helper function for the listWorld() command."""
    childs = list(obj.iterChilds())
    idx = 0
    for child in childs:
        if child.geom!=None:
            g = child.geom.__class__.__name__
        else:
            g = "-"
        print "%s+---%s (%s/%s)"%(lines,child.name,child.__class__.__name__, g)
        if idx<len(childs)-1:
            childlines = lines+"|   "
        else:
            childlines = lines+"    "
        _listWorld(child, childlines)
        idx += 1
    

# load
def load(filename, **options):
    """Load a file.

    This function loads the given file without deleting the scene,
    so the contents of the file is appended to the current scene.

    To be able to load the file there must be an appropriate import class
    (protocol: "Import") available in the plugin manager. The class is
    determined by examining the file extension. If no importer is
    found a NoImporter exception is thrown.

    Any exception generated in the importer is passed to the caller.

    \param filename (\c str) File name
    \param options Options that are passed to the import plugin
    """
    # Extract the extension (without '.') and make it lower case
    ext = os.path.splitext(filename)[1][1:].lower()

    # Find the appropriate import plugin class by comparing the extension
    for objdesc in pluginmanager.iterProtoObjects("Import"):
        if ext in objdesc.object.extension():
            break
    else:
        raise NoImporter('No import plugin found for extension "%s".'%ext)

    if not os.path.exists(filename):
        raise IOError('File "%s" does not exist.'%filename)

    # Change into the directory of the given file
    oldpath = os.getcwd()
    dir = os.path.dirname(os.path.abspath(filename))
    os.chdir(dir)

    # Import the file
    imp = objdesc.object()
    try:
        imp.importFile(os.path.basename(filename), **options)
    except:
        os.chdir(oldpath)
        raise
        
    # Change back to the previous directory
    os.chdir(oldpath)


# save
def save(filename, **options):
    """Save the scene to a file.

    This function saves the current scene.

    To be able to save the scene there must be an appropriate export class
    (protocol: "Export") available in the plugin manager. The class is
    determined by examining the file extension. If no exporter is
    found a NoExporter exception is thrown.

    Any exception generated in the exporter is passed to the caller.

    \param filename (\c str) File name
    \param options Options that are passed to the export plugin
    """
    # Extract the extension (without '.') and make it lower case
    ext = os.path.splitext(filename)[1][1:].lower()

    # Find the appropriate export plugin class by comparing the extension
    for objdesc in pluginmanager.iterProtoObjects("Export"):
        if ext in objdesc.object.extension():
            break
    else:
        raise NoExporter('No export plugin found for extension "%s".'%ext)

    # Change into the directory of the given file
    oldpath = os.getcwd()
    dir = os.path.dirname(os.path.abspath(filename))
    os.chdir(dir)

    # Export the file
    exp = objdesc.object()
    exp.exportFile(os.path.basename(filename), **options)

    # Change back to the previous directory
    os.chdir(oldpath)

# reset
def reset():
    """Reset an animation/simulation.

    Reset effectively sets the time back to 0.
    """
    eventmanager.eventManager().event(events.RESET)
    
# worldObjects
def worldObjects(objs):
    """Return a list of world objects.

    \a objs can be a string, a world object or a sequence of strings or
    world objects. A string always referes to the world object with that
    name. The return value is always a list, even when only an individual
    object was specified as input.

    \param objs A sequence of strings/world objects or an individual string
           or world object.
    \return A list of WorldObject objects.
    """

    if isinstance(objs, types.StringTypes):
        objs = [objs]

    try:
        it = iter(objs)
    except TypeError:
        it = iter([objs])
        
    res = []
    for obj in it:
        res.append(worldObject(obj))
        
    return res

# worldObject
def worldObject(obj):
    """Return a world object.

    If \a obj is a string, the function returns the world object with
    that name. Otherwise \a obj is returned.

    \param obj An object or the name of an object.
    """
    
    if isinstance(obj, types.StringTypes):
        return getScene().worldObject(obj)
    else:
        return obj

# register
def register(*cmds):
    """Register functions in the cmds module.

    The given functions will be added to the cmds module.

    \code
    import cmds
    
    def func1():
        ...

    def func2(v):
        ...

    cmds.register(func1, func2)
            
    \endcode
    
    """
    # Add the commands to the 
    namespace = globals()
    for c in cmds:
        namespace[c.__name__] = c
