######################################################################
# Visualizing the vertices and normals of a triangle mesh
#
# This script loads a scene and shows the vertices and normals of
# every mesh in the scene.
#
# Invoke the script via the viewer tool:
# $ viewer.py shownormals.py
######################################################################


def showVertices(mesh):
    """Draw markers at the vertex positions of a triangle mesh.
    """

    # Vertices are stored in the local coordinate system of the geometry.
    # But as we need the coordinates relative to the world coordinate
    # system we have to obtain the transformation L that transforms from
    # local to world coordinates.
    L = mesh.worldtransform

    # Iterate over the vertices and draw a marker at the global vertex
    # position L*v.
    for v in mesh.verts:
        drawMarker(L*v, size=0.07, color=(0,0,1))


def showNormals(mesh):
    """Draw the normals as lines.

    A normal is drawn at every vertex position. The normals may either be
    specified as "varying normal N" or as "user normal N"/"uniform int Nfaces[3]".
    The actual drawing code is in showVaryingNormals() and showUserNormals().
    """

    # Check if there are normals available in this mesh
    desc = mesh.findVariable("N")
    if desc==None:
        print 'Mesh "%s" has no normals set.'%mesh.name
        return

    # Unpack the variable descriptor
    name, storage_class, type, mult = desc

    # Are the normals stored as "varying normal N"?
    if storage_class==VARYING and type==NORMAL and mult==1:
        showVaryingNormals(mesh)
        
    # Or are they stored as "user normal N"?
    elif storage_class==USER and type==NORMAL and mult==1:
        # Check if the normal faces are available
        desc = mesh.findVariable("Nfaces")
        if desc==None:
            print 'Mesh "%s" has no normal faces set.'%mesh.name
        else:    
            name, storage_class, type, mult = desc
            if storage_class==UNIFORM and type==INT and mult==3:
                # "N" and "Nfaces" is available and of the right type...
                showUserNormals(mesh)
            else:
                print 'Mesh "%s": Invalid declaration of "Nfaces"'%mesh.name
                
    else:
        print 'Mesh "%s": Invalid declaration of variable "N".'%mesh.name


def showVaryingNormals(mesh):
    """Draw normals given as "varying normal N".
    """
    # Get the world transform to transform the local vertices and normals
    # into the world coordinate system
    L = mesh.worldtransform
    L0 = L*vec3(0)
    # Obtain the slot carrying the normals
    N = mesh.geom.slot("N")
    # Iterate over the faces and draw the vertices and normals
    for i,j,k in mesh.faces:
        a = L*mesh.verts[i]
        b = L*mesh.verts[j]
        c = L*mesh.verts[k]
        na = L*N[i]-L0
        nb = L*N[j]-L0
        nc = L*N[k]-L0
        drawLine(a, a+na)
        drawLine(b, b+nb)
        drawLine(c, c+nc)


def showUserNormals(mesh):
    """Draw normals given as "user normal N"/"uniform int Nfaces[3]".
    """
    # Get the world transform to transform the local vertices and normals
    # into the world coordinate system
    L = mesh.worldtransform
    L0 = L*vec3(0)
    # Obtain the slot carrying the normals and the normal faces
    N = mesh.geom.slot("N")
    Nfaces = mesh.geom.slot("Nfaces")
    # Iterate overt the faces and normal faces...
    for face, Nface in zip(mesh.faces, Nfaces):
        i,j,k = face
        ni,nj,nk = Nface
        a = L*mesh.verts[i]
        b = L*mesh.verts[j]
        c = L*mesh.verts[k]
        na = L*N[ni]-L0
        nb = L*N[nj]-L0
        nc = L*N[nk]-L0
        drawLine(a, a+na)
        drawLine(b, b+nb)
        drawLine(c, c+nc)            

######################################################################

# Load a scene file
load("3ds/bunny.3ds")

# Show its contents
listWorld()

# Draw the vertices & normals of every triangle mesh in the scene
for obj in scene.walkWorld():
    # Check if the object is a triangle mesh
    if isinstance(obj.geom, TriMeshGeom):
        # Uncomment the following line to convert the normals from
        # "user normal" to "varying normal"
#        obj.geom = convMeshAttr(obj.geom, "N")
        print 'Creating vertex markers for "%s"...'%obj.name
        showVertices(obj)
        print 'Creating normal lines for "%s"...'%obj.name
        showNormals(obj)

