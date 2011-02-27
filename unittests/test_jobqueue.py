# Test the jobqueue module

import unittest, os.path
import sys, shutil
from cgkit.jobqueue import createJobQueue, JobQueue, JobQueueError
from cgkit.jobqueue.jobhandle import JobHandle

class TestJobQueue(unittest.TestCase):
    
    def testMultiRef(self):
        jq = self.queue()
        jr = jq.createJobRoot("blender", blendFile="multiple_references.blend")
        shad = jr.createJob("blender", blendFile="shadow_map")
        j1 = jr.createJob("blender", blendFile="frame1")
        j2 = jr.createJob("blender", blendFile="frame2")
        j1.addDependency(shad)
        j2.addDependency(shad)
        jr.activate()
        print jr._location
    
    def testDependencySimplification(self):
        """Check that redundant dependencies are removed.
        """
        jq = self.queue()
        jr = jq.createJobRoot("blender", blendFile="check_redundant_deps.blend")
        j1 = jr.createJob("blender", blendFile="foo1")
        j2 = jr.createJob("blender", blendFile="foo2")
        # The direct dependency between jr and j1 should be removed again
        # (because jr depends on j2 and j2 on j1, so jr indirectly depends on j1 anyway)
        j2.addDependency(j1)
        jr.activate()
        
        jh = JobHandle(jr._location, jr._location)
        subJobs = list(jh.listSubJobs())
        self.assertEqual(1, len(subJobs))
        self.assertEqual('Blender "foo2"', subJobs[0].label)
        subSubJobs = list(subJobs[0].listSubJobs())
        self.assertEqual(1, len(subSubJobs))
        self.assertEqual('Blender "foo1"', subSubJobs[0].label)

        # Do the same as above but reverse the order of the dependency
        jr = jq.createJobRoot("blender", blendFile="check_redundant_deps.blend")
        j1 = jr.createJob("blender", blendFile="foo1")
        j2 = jr.createJob("blender", blendFile="foo2")
        # The direct dependency between jr and j2 should be removed again
        # (because jr depends on j1 and j1 on j2, so jr indirectly depends on j2 anyway)
        j1.addDependency(j2)
        jr.activate()
        
        jh = JobHandle(jr._location, jr._location)
        subJobs = list(jh.listSubJobs())
        self.assertEqual(1, len(subJobs))
        self.assertEqual('Blender "foo1"', subJobs[0].label)
        subSubJobs = list(subJobs[0].listSubJobs())
        self.assertEqual(1, len(subSubJobs))
        self.assertEqual('Blender "foo2"', subSubJobs[0].label)

    def testCycleDetection(self):
        """Check if cycles in the dependency graph are detected.
        """
        jq = self.queue()
        jr = jq.createJobRoot("blender", blendFile="check_cycle_detection.blend")
        j1 = jr.createJob("blender", blendFile="foo1")
        j2 = j1.createJob("blender", blendFile="foo2")
        j3 = j2.createJob("blender", blendFile="foo3")
        j3.addDependency(j1)
        self.assertRaises(JobQueueError, lambda: jr.activate())
        
    def queue(self):
        """Return a JobQueue object.
        """
        if not os.path.exists("tstqueue"):
            createJobQueue("tstqueue")
        return JobQueue("tstqueue")

######################################################################

# Remove the queue from a previous run
if os.path.exists("tstqueue"):
    shutil.rmtree("tstqueue")

if __name__=="__main__":
    unittest.main()
