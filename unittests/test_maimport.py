# Test the MA import

import unittest
from cgkit.all import *

class TestMAImport(unittest.TestCase):

    def testMAImport(self):

        scene = getScene()
        scene.clear()

        # Load the file...
        load("data/objects.ma")
#        listWorld()

        self.assertEqual(len(list(scene.walkWorld())), 2)

        # Check the box...
        obj = worldObject("pCube1")
        geom = obj.geom
        self.assertEqual(type(obj), Box)
        self.assertEqual(type(geom), BoxGeom)
        self.assertEqual(geom.lx, 2.0)
        self.assertEqual(geom.ly, 1.0)
        self.assertEqual(geom.lz, 3.0)

        # Check the mesh...
        obj = worldObject("mesh")
        geom = obj.geom
        self.assertEqual(type(obj), Polyhedron)
        self.assertEqual(type(geom), PolyhedronGeom)
        eps = setEpsilon(1E-6)
        self.assertEqual(list(geom.verts), [vec3(-0.25, 0.5, 0.25), vec3(-0.5, -0.5, 0.5), vec3(0.25, 0.5, 0.25), vec3(0.5, -0.5, 0.5), vec3(0.25, 0.5, -0.25), vec3(0.5, -0.5, -0.5), vec3(-0.25, 0.5, -0.25), vec3(-0.5, -0.5, -0.5)])

        self.assertEqual(geom.getNumPolys(), 6)
        polys = []
        for i in range(geom.getNumPolys()):
            polys.append(geom.getPoly(i))
        self.assertEqual(polys, [[[0, 1, 3, 2]], [[2, 3, 5, 4]], [[4, 5, 7, 6]], [[6, 7, 1, 0]], [[1, 7, 5, 3]], [[6, 0, 2, 4]]])
        
        setEpsilon(eps)


######################################################################

if __name__=="__main__":
    unittest.main()
