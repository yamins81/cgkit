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
# Portions created by the Initial Developer are Copyright (C) 2009
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

import sys, os, os.path, glob, inspect, socket
import random, traceback, shutil
import xml.dom.minidom
import ConfigParser as configparser
from jobproc import JobProc
from jobhandle import JobHandle

class JobQueueError(Exception):
    pass


def _convertPyValueToDOM(val, doc):
    """Convert a Python value into a DOM element.
    
    val can be a bool, int, float, str, tuple, list, dict or None.
    This is used for writing values into a job definition file.
    """
    t = type(val)
    typeName = t.__name__
    el = doc.createElement(typeName)
    # None?
    if val is None:
        pass
    # Basic type?
    elif t in [bool, int, float, str]:
        el.setAttribute("value", str(val))
    # Sequence?
    elif t in [tuple, list]:
        for v in val:
            cel = _convertPyValueToDOM(v, doc)
            el.appendChild(cel)
    # Dict
    elif t==dict:
        for key,v in val.items():
            keyEl = _convertPyValueToDOM(key, doc)
            valEl = _convertPyValueToDOM(v, doc)
            el.appendChild(keyEl)
            el.appendChild(valEl)
    else:
        raise ValueError("Unsupport parameter type: %s"%typeName)
    
    return el

def _convertDOMToPyValue(element):
    """Convert a DOM element into a Python object.
    
    This is used to convert the parameters from a job definition file
    into Python values.
    """
    typeName = element.nodeName
    
    if typeName=="dict":
        res = {}
        keyValid = False
        key = None
        for childEl in element.childNodes:
            if childEl.nodeType==childEl.ELEMENT_NODE:
                v = _convertDOMToPyValue(childEl)
                if keyValid:
                    res[key] = v
                    keyValid = False
                else:
                    key = v
                    keyValid = True
        if keyValid:
            raise ValueError("Dict key without value")
    elif typeName=="str":
        res = str(element.getAttribute("value"))
    elif typeName=="bool":
        val = str(element.getAttribute("value"))
        res = {"True":True, "False":False}.get(val)
        if res is None:
            raise ValueError("Invalid boolean value: %s"%val)
    elif typeName=="int":
        res = int(element.getAttribute("value"))
    elif typeName=="float":
        res = float(element.getAttribute("value"))
    elif typeName in ["list", "tuple"]:
        res = []
        for childEl in element.childNodes:
            if childEl.nodeType==childEl.ELEMENT_NODE:
                res.append(_convertDOMToPyValue(childEl))
        if typeName=="tuple":
            res = tuple(res)
    elif typeName=="NoneType":
        res = None
    else:
        raise ValueError("Unknown parameter type: %s"%typeName)
    return res


class _DummyLogger:
    """Dummy logger object that does nothing.
    """
    def debug(self, *params, **kwargs):
        pass
    
    info = debug
    warning = debug
    error = debug
    critical = debug
    log = debug
    exception = debug


class Job(object):
    """Job object (during creation).
    
    Objects of this class represent jobs that are just being created.
    
    """
    def __init__(self, jobRoot, jobType, **params):
        """Constructor.
        
        jobRoot is the JobRoot object that this job belongs to.
        jobType is a string containing the name of the job class that should
        be instantiated.
        params are the job parameters which must be passed as keyword
        arguments.
        """
        if not isinstance(jobRoot, JobRoot):
            raise TypeError("jobRoot must be a JobRoot object")
        if not isinstance(jobType, basestring):
            raise TypeError("Job type must be a string")

        # The root of the job
        self._jobRoot = jobRoot
        # The job proc class name
        self._jobType = jobType
        # The job proc parameters
        self._params = params
        # A list of Job object which this job depends on
        self._dependencies = []
        
        # This is used later on to detect cycles
        self._marked = False
        # The maximum distance to a leaf (job nodes without a dependency 
        # always have a value of 0). This is used to sort the dependencies
        # with respect to the longest path in that sub-tree which is necessary
        # to remove redundant dependencies.
        self._maxLeafDist = 0
        # This is used later on to remove redundant dependencies
        self._depNr = 0
        
        # The job location on disk.
        self._location = self._initJobDir(jobType, **params)
        # Indicates whether the job directory is inside the repository or not
        # (all jobs begin in the repository except for the root job)
        self._isInsideRepository = True
    
    def createJob(self, jobType, **params):
        """Create a new sub-job.
        
        *jobType* is the name of the job procedure that should be created. Any
        additional keyword arguments are passed to the constructor of the job
        procedure.
        
        Returns a :class:`Job<jobqueue.Job>` object that represents the newly created job.
        
        This method is equivalent to creating a job object manually and
        calling ``addDependency(job)``.
        """
        job = Job(self._jobRoot, jobType, **params)
        self.addDependency(job)
        return job

    def addDependency(self, job):
        """Establish a dependency between another job.
        
        This job will only be run when all dependencies have been successfully
        finished.
        """
        if job is self:
            raise ValueError("A job cannot depend on itself")
        if job._jobRoot!=self._jobRoot:
            raise ValueError("A job cannot depend on a job from another job hierarchy")
        if isinstance(job, JobRoot):
            raise ValueError("A job cannot depend on the job root")
        
        # Set the dependency
        self._dependencies.append(job)
        
        # TODO: If self has a job dir, then create job dirs for all jobs
        # in the job subtree that don't have a job dir yet
        
    def _initJobDir(self, jobType, **params):
        """Create and initialize the job directory.
        
        jobType is the name of the job procedure that should be created. Any
        additional keyword arguments are passed to the constructor of the job
        procedure.
        Returns the job directory.
        """
        
        if type(jobType) is not str:
            raise TypeError("Job type must be a string")

        # Create an instance of the job procedure to validate the parameters and get the job label
        jobProc = self._instantiateJobProc(jobType, **params)

        # Create the root job directory...
        jobDir = self._createJobDir()
        # This handle is only used for retrieving file locations (so the root is set to None)
        jobHandle = JobHandle(jobDir, None)

        # Call the job procedure's postCreate method
        jobProc.postCreate(jobDir)
        jobProc._jobDir = jobDir
        
        # Write the label
        label = jobProc.label
        if label is not None:
            f = open(jobHandle.labelFile, "wt")
            f.write(label)
            f.close()

        # Write the job procedure description. 
        # If this job node is the job root then write the file under a temporary
        # name (so that the job won't get picked up yet)
        procDefFile = jobHandle.procDefFile
        if self is self._jobRoot:
            procDefFile += "_tmp"
        self._writeJobDef(procDefFile, jobType, params)
        
        return jobDir
    
    def _writeJobDef(self, fileName, jobType, params):
        """Write the job definition XML file.
        """
        impl = xml.dom.minidom.getDOMImplementation()
        doc = impl.createDocument(None, "JobProcedure", None)
        jobProc = doc.documentElement
        # Set the job type
        jobProc.setAttribute("type", jobType)
        
        # Set the job parameters
        paramEl = doc.createElement("parameters")
        jobProc.appendChild(paramEl)
        paramDictEl = _convertPyValueToDOM(params, doc)
        paramEl.appendChild(paramDictEl)
        
        # Write the XML file
        f = open(fileName, "wb")
        jobProc.writexml(f, addindent="  ", newl="\n")
        f.close()
        
        jobProc.unlink()
    
    def _instantiateJobProc(self, jobType, **params):
        """Create an instance of a job procedure.
        """
        return self._jobRoot._instantiateJobProc(jobType, **params)
    
    def _createJobDir(self):
        """Determine the name of the job directory and create it.
        """
        return self._jobRoot._createSubJobDir()

    def _preprocessGraph(self):
        """Preprocess the dependency graph.
        
        Sets the maxLeafDist value on the sub-jobs and checks for cycles.
        
        Raises a JobQueueError if the graph contains a cycle.
        """
        # Have we visited this job already before in this particular path? -> Cycle
        if self._marked:
            raise JobQueueError("Job hierarchy is impossible to process because it contains a cycle")

        # Mark this job as visited (if we see it again while processing children,
        # we have detected a cycle)
        self._marked = True

        for subJob in self._dependencies:
            # Descend the tree
            subJob._preprocessGraph()
            self._maxLeafDist = max(self._maxLeafDist, subJob._maxLeafDist+1)
        
        # Reset the cycle detection mark (as this sub-tree is done and does not contain a cycle)
        self._marked = False

    def _writeDependencies(self):
        """Write the dependency hierarchy.
        
        Creates the final job directories (or links) that make up the
        dependency hierarchy.
        """
        jobDir = self._location
        
        myDepNr = self._jobRoot._getNextDepNr()
        self._depNr = myDepNr
        
        # Create a dependency list that has the current order associated with each dependency.
        # This number will be used as local job number (so that the jobs are
        # picked up in the same order as they were added).
        deps = [x for x in enumerate(self._dependencies)]
        # Sort according to descending maxLeafDist.
        # This is done so that the longest paths are processed first and
        # redundant dependencies (which always have a shorter path) can be removed.
        deps.sort(key=lambda x: -x[1]._maxLeafDist)
        
        # Create the dependency graph job directories for the children
        for jobNr,subJob in deps:
            # Do we already have a (possibly indirect) dependency set up?
            # Then there's nothing to do anymore
            if subJob._depNr>=myDepNr:
                continue
            
            dirName = os.path.join(jobDir, "job%d"%(jobNr))
            if not self._jobRoot._keepJobsInRepository and subJob._isInsideRepository:
                # Move the directory from the repository into the dependency hierarchy 
                shutil.move(subJob._location, dirName)
                subJob._location = dirName
                subJob._isInsideRepository = False
            else:
                # Create a link to the actual job dir
                self._jobRoot._linkJobDir(subJob._location, dirName)
                subJob._setDepNr(myDepNr)
                
            subJob._writeDependencies()
    
    def _setDepNr(self, nr):
        self._depNr = nr
        for j in self._dependencies:
            j._setDepNr(nr)


class JobRoot(Job):
    """The root of a job hierarchy.
    
    In principle, this is a job like every other job as well, but because
    it has a directory inside the main job directory it must remain the
    root of the job hierarchy.
    The root is also the only job directory that has a special job repository
    directory that may store all the sub-jobs.
    
    This class is derived from the :class:`Job<jobqueue.Job>` class.
    """
    
    def __init__(self, jobQueue, jobType, **params):
        """Constructor.
        
        jobQueue is the JobQueue object that this job is associated with.
        jobType is a string containing the name of the job class that should
        be instantiated.
        params are the job parameters which must be passed as keyword
        arguments.
        """
        if not isinstance(jobQueue, JobQueue):
            raise TypeError("jobQueue must be a JobQueue object")
        self._jobQueue = jobQueue
        # This counter is used to create the job directory names in the job repository dir
        self._subJobCounter = 0
        # Initial value for the redundant dependency removal part
        self._nextDepNr = 0
        # This flag determines whether the jobs will be kept in the job repository
        # or moved out into the dependency hierarchy when _writeDependencies()
        # is called on any job. 
        self._keepJobsInRepository = self._jobQueue.keepJobsInRepository
        # Can we use sym links or do we have to emulate them?
        self._useSymLinks = self._jobQueue.useSymLinks
        
        Job.__init__(self, self, jobType, **params)
        
        # Overwrite the isInsideRepository flag (as the root is never inside the repo)
        self._isInsideRepository = False

    def activate(self):
        """Activate the job so that it can be processed.
        
        This is the last step of a job submission. After calling this method,
        the job hierarchy must not be changed anymore.
        
        Raises a :exc:`JobQueueError` if the graph contains a cycle.
        """
        # Check for cycles, set the minimum job depth in the graph and sort
        # the dependencies of each job according to the minimum depth
        self._preprocessGraph()
        
        # Create the actual job* directories that make up the job hierarchy
        self._writeDependencies()
        
        # Clean up the job repo dir if the jobs have been moved out.
        if not self._keepJobsInRepository:
            subJobRepo = self._getSubJobRepo()
            if os.path.exists(subJobRepo):
                # At this point, the directory must be empty, so a single rmdir is enough.
                # (it's empty because every job (except for the root job) has at least
                # one parent which means the job has been moved out of the repo and
                # put under a parent)
                os.rmdir(subJobRepo)
        
        jobHandle = JobHandle(self._location, self._location)
        procDefFile = jobHandle.procDefFile
        tmpProcDefFile = procDefFile+"_tmp"
        if not os.path.exists(tmpProcDefFile):
            raise JobQueueError("Failed to activate")
        os.rename(tmpProcDefFile, procDefFile)

    def _linkJobDir(self, src, dst):
        """Create a link at dst pointing to src.
        """
        if not src.startswith(self._location):
            raise ValueError("Cannot link directories outside the job root")
        
        if self._useSymLinks:
            # Create a sym link
            os.symlink(src, dst)
        else:
            # Create a file that contains the target path
            f = open(dst, "wt")
            f.write("[link]\ntarget=$JOBROOT%s\n"%src[len(self._location):])
            f.close()

    def _createSubJobDir(self):
        """Create a new directory for a sub-job.
        
        Note: The sub-job may be anywhere in the final dependency graph.
        """
        self._subJobCounter += 1
        # The directory where all sub-jobs are stored
        subJobRepo = self._getSubJobRepo()
        # Create the repo dir if it doesn't already exist
        if not os.path.exists(subJobRepo):
            os.mkdir(subJobRepo)
        # Create the actual sub-job directory
        subJobDir = os.path.join(self._location, ".jobs", "j%d"%self._subJobCounter)
        os.mkdir(subJobDir)
        return subJobDir
    
    def _getNextDepNr(self):
        self._depNr += 1
        return self._depNr
    
    def _getSubJobRepo(self):
        """Return the location of the job repository directory.
        """
        return os.path.join(self._location, ".jobs")
        
    def _instantiateJobProc(self, jobType, **params):
        """Create an instance of a job procedure.
        """
        return self._jobQueue._instantiateJobProc(jobType, **params)
    
    def _createJobDir(self):
        """Determine the name of the job directory and create it.
        """
        # Create the root job directory...
        for i in range(10):
            jobDir = self._jobQueue._newJobDir()
            try:
                os.mkdir(jobDir)
                break
            except OSError:
                pass
        else:
            raise JobQueueError("Failed to create a new job directory")
        
        return jobDir


class JobQueue:
    """Job queue class.
    
    A job queue contains a list of jobs which can be run by one or more
    processes. Each job may be composed of sub-jobs which all have to
    be successfully completed before the job is allowed to be processed.
    Jobs that are on the same level in the hierarchy are independent and
    can be run concurrently.
    
    An individual job is represented by a job procedure (JobProc) object
    that implements the actual functionality of a particular type of job.
    A job is considered done when the run() method of a job procedure has
    been executed successfully and the job procedure did not indicate an
    error.
    
    On disk, the entire job queue is stored as a directory which contains
    a job directory for each job in the queue. Sub-jobs are sub-directories
    of the job directories.
    """
    
    def __init__(self, location, logger=None):
        """Constructor.
        
        location is a string containing the directory where the job queue
        is stored (the directory should already exist, it is not created).
        It is also valid to pass None (which is equivalent to an empty
        queue that cannot receive jobs).
        
        logger can be set to a logger object from the logging module (or
        an object with the same interface) which will receive log message.
        """
        # Verify the input types
        if location is not None and not isinstance(location, basestring):
            raise TypeError("Job location must be a string or None.")
        
        # Make the directory path absolute
        if location is not None:
            location = os.path.abspath(location)
            if not os.path.exists(location):
                raise ValueError("Job queue %s does not exist"%location)
        
        if logger is None:
            logger = _DummyLogger()
        
        if location is not None:
            logger.info("Attach to job queue at %s"%location)
        
        self._logger = logger
        self._location = location
        self.keepJobsInRepository = False
        self.useSymLinks = False
        
        # The default parameter values for job procedures.
        # Key:Proc name - Value:Keyword params dict
        self._defaultProcParams = {}
        
        self._nextDepNr = 0
        
        # Read the config file
        if self._location is not None:
            configName = os.path.join(self._location, "queue.cfg")
            self._readConfigFile(configName)
        
        # Import the proc classes...
        self._procClasses = {}
        if self._location is not None:
            origPaths = sys.path[:]
            procDir = os.path.join(self._location, "procs")
            pyNames = glob.glob(os.path.join(procDir, "*.py"))
            sys.path.insert(0, procDir)
            for pyName in pyNames:
                modName = os.path.splitext(os.path.basename(pyName))[0]
                ns = {}
                exec "import %s"%modName in ns
                if hasattr(ns[modName], modName):
                    cls = getattr(ns[modName], modName)
                    if inspect.isclass(cls) and cls!=JobProc and issubclass(cls, JobProc):
                        self._procClasses[modName] = cls
            sys.path = origPaths
    
    @property
    def location(self):
        """Return the absolute location of the job queue directory.
        
        Returns ``None`` if no directory is set.
        """
        return self._location
    
    def listJobs(self):
        """Return a list of all top-level jobs.
        
        Returns a list of :class:`JobHandle<jobqueue.JobHandle>` objects that represent the
        roots of all jobs. 
        The order of the jobs is in the order they would be processed.
        If the job queue is not associated with a job directory, the return
        value is an empty list.
        """
        if self._location is None:
            return []
        
        # List all job* sub-directories
        jobDirPattern = os.path.join(self._location, "job*")
        jobDirs = glob.glob(jobDirPattern)
        #jobDirs = filter(lambda p: os.path.isdir(p), jobDirs)
        jobs = map(lambda jobDir: (jobDir, int(os.path.basename(jobDir)[3:])), jobDirs)
        jobs.sort(key=lambda a: a[1])
        return [JobHandle(jobDir, jobDir) for jobDir,nr in jobs]
    
    def createJobRoot(self, jobType=None, **params):
        """Create a new top-level job.
        
        *jobType* is the name of the job procedure that should be created. Any
        additional keyword arguments are passed to the constructor of the job
        procedure.
        
        Returns a :class:`JobRoot<cgkit.jobqueue.jobqueue.JobRoot>` object that
        represents the newly created job.
        Sub-jobs can be created by calling :meth:`createJob()<cgkit.jobqueue.jobqueue.JobRoot.createJob>`
        on the returned job root.
        
        After the entire job hierarchy has been created, the
        :meth:`activate()<cgkit.jobqueue.jobqueue.JobRoot.activate>`
        method must be called on the job root, otherwise the job will not
        be processed.
        """
        jobRoot = JobRoot(self, jobType, **params) 
        return jobRoot
    
    # An alias for createJobRoot (for single command jobs)
    createJob = createJobRoot
    
    def deleteJobs(self, jobs):
        """Delete one or more jobs.
        
        *jobs* is either a single :class:`JobHandle<jobqueue.JobHandle>` object
        or a sequence of  :class:`JobHandle<jobqueue.JobHandle>` objects.
        *jobs* may also be ``None`` in which case the method returns immediately.
        
        The method raises an error (:exc:`OSError`) if it couldn't delete a job.
        This will abort the entire operation and some of the input jobs
        may not even have been processed.
        Jobs that don't exist at all anymore are not considered an error
        and won't trigger an exception.
        
        Note: The method is meant to be used on top-level jobs. Currently,
        it can also be used for sub-jobs, but it doesn't check if deleting
        a sub-job breaks another sub-job that references a job within the
        deleted job.
        """
        if jobs is None:
            return
        if isinstance(jobs, JobHandle):
            jobs = [jobs]

        for job in jobs:
            if os.path.exists(job.location):
                self._logger.info("Deleting job directory %s"%job.location)
                # Delete the proc def file first, so that nobody can pick up this job anymore
                try:
                    os.remove(job.procDefFile)
                except:
                    pass
                # Now delete everything
                shutil.rmtree(job.location)
    
    def runNextAvailableJob(self, retries=10):
        """Run the next available job in the queue that is in a waiting state.
        
        The return value is ``True`` if a job could be run or ``False`` if there
        are no more waiting jobs at the moment.
        When there are waiting jobs but the method fails to allocate one of them,
        then the integer *retries* determines how many attempts should be made
        before giving up. Once the last attempt has failed, a :exc:`JobQueueError`
        exception is thrown. Some causes for this could be:
        
        - The process doesn't have file access permissions for the job directory
        - There were too many other processes that were slightly quicker in
          getting permission
        - There is a broken job directory somewhere (if it's in waiting state
          but can't be run for some reason (maybe the proc is missing))
        """
        for i in range(retries):
            # Get a list of all available jobs
            jobs = self.listJobs()
            # Pick the next available job
            job = self._findNextWaitingJob(jobs)
            if job is None:
                return False
            # Try to run it
            if self._runJob(job):
                return True
        
        raise JobQueueError("Failed to get permission to run a job")
    
    def _findNextWaitingJob(self, jobs):
        """Find the next job that should be processed.
        
        jobs is a sequence of JobHandle objects. The method returns a JobHandle
        object that is the "deepest" job that is in waiting state.
        """
        # Ignore any job that is not currently waiting
        jobs = filter(lambda job: job.isWaiting(), jobs)
        
        # Search for the first waiting sub-job...
        for job in jobs:
            subJobs = job.listSubJobs()
            if self._isReady(subJobs):
                return job
                    
            # The job is not ready, so try to pick one of the sub-jobs...
            j = self._findNextWaitingJob(subJobs)
            if j is not None:
                return j
        
        return None
        
    def _isReady(self, subJobs):
        """Check if a job with the given sub-jobs is ready to run.
        
        subJobs is a sequence of JobHandle objects (obtained from job.listSubJobs()).
        Returns True when all jobs in subJobs have been finished without
        an error.
        """
        for job in subJobs:
            if not job.isFinished() or job.hasError():
                return False
        return True
    
    def _runJob(self, job):
        """Try to run the job.
        
        job is a JobHandle object representing the job to run.
        The return value is True if the job could be run, otherwise the
        value is False. A value of False means, this process couldn't bring
        the job into running state (e.g. because it was not in waiting state
        or another process was slightly quicker to run it or there were
        permission issues). It is absolutely possible that the method returns
        False even when a previous call to isWaiting() returned True.
        
        Note: A return value of True only means this process got permission
        to run the job. It doesn't mean that the job also completed successfully.
        
        It is the callers responsibility to make sure that all sub-jobs
        have completed successfully.
        """
        self._logger.info("Trying to run job %s"%job.location)
        # Read the job procedure first
        try:
            jobType,params = self._readJobDef(job.procDefFile)
        except:
            self._logger.info("Failed to read job procedure definition: %s"%sys.exc_info()[1])
            return False
        
        # Apply the default parameter values from the config file
        defaultParams = self._defaultProcParams.get(jobType, {})
        for name,val in defaultParams.items():
            if name not in params:
                params[name] = val
        
        # Create a new instance of the job procedure
        proc = self._instantiateJobProc(jobType, **params)

        # Try to bring the job into running state
        if not self._setJobToRunningState(job):
            return False

        # Run the job procedure
        self._logger.info("Running job")
        os.chdir(job.runningDir)
        try:
            proc._begin(job)
            proc.run()
            self._logger.info("Job succeeded")
        except:
            self._logger.info("Job failed to run successfully")
            job.setError()
            f = open(job.procTracebackFile, "wt")
            traceback.print_exc(file=f)
            f.close()
        try:
            proc._end()
        except:
            pass
            
        # Mark the job as being finished
        self._logger.debug("Creating finished directory %s"%job.finishedDir)
        try:
            os.mkdir(job.finishedDir, 0777)
        except:
            self._logger.warn("Failed to create directory %s: "%(job.finishedDir, sys.exc_info()[1]))
            
        return True
    
    def _setJobToRunningState(self, job):
        """Mark a job as being run by this process.
        
        Returns True if the job could be allocated (i.e. brought into running
        state) or False if we are not allowed to run the job.
        """
        runningDir = job.runningDir
        pidFile = job.pidFile
        hostFile = job.hostFile
        
        # Try to create the ".running" directory. If it fails, we assume
        # it exists already and return. Otherwise we have permission to run
        # the job procedure
        self._logger.debug("Creating %s"%runningDir)
        try:
            os.mkdir(runningDir, 0777)
        except OSError, exc:
            self._logger.debug("Failed to create directory: %s"%exc)
            return False
        
        # Write the info files into the .running dir
        try:
            # Store the process id of this process inside the directory
            pid = os.getpid()
            f = open(pidFile, "wt")
            try:
                print >>f, pid
            finally:
                f.close()
    
            # Store the host name inside the directory
            host = socket.gethostname()
            f = open(hostFile, "wt")
            try:
                print >>f, host
            finally:
                f.close()
        except:
            err = sys.exc_info()[1]
            self._logger.warning("Failed to write pid/host file: %s"%err)
            # Something went wrong while trying to write the pid and host file.
            # So try to remove the .running directory again and return False.
            try:
                os.remove(pidFile)
            except:
                pass
            try:
                os.remove(hostFile)
            except:
                pass
            try:
                os.rmdir(runningDir)
            except:
                pass
            return False
            
        return True

    
    def _readJobDef(self, fileName):
        """Read the job definition.
        
        Returns the job type string and the parameter dict.
        Raises an error when there was an error reading the file.
        """
        doc = xml.dom.minidom.parse(fileName)
        
        jobProc = doc.documentElement
        if jobProc.nodeName!="JobProcedure":
            raise ValueError("Error in job definition file, expected a job procedure element")
        
        jobType = jobProc.getAttribute("type")
        if jobType=="":
            raise ValueError("Error in job definition file. No job type given.")
        
        params = jobProc.getElementsByTagName("parameters")
        if len(params)==0:
            raise ValueError("Error in job definition file. No job parameters found.")
        if len(params)>1:
            raise ValueError("Error in job definition file. Multiple parameter elements found.")
        params = params[0]
    
        paramDict = jobProc.getElementsByTagName("dict")
        if len(paramDict)!=1:
            raise ValueError("Error in job definition file. Invalid parameter element.")
        paramDict = paramDict[0]
        
        params = _convertDOMToPyValue(paramDict)
        
        return jobType, params
    
    def _readConfigFile(self, fileName):
        """Read the job queue config file.
        
        Updates the local attributes that are initialized from the config file.
        """
        self._logger.info("Read config file %s"%fileName)
        cp = configparser.ConfigParser()
        # Override the optionxform method so that it returns the input string
        # unmodified. We need case-senstivity as some options are actually
        # argument names for the job procedures.
        cp.optionxform = str
        cp.read(fileName)
        
        # This dictionary defines the valid options that may appear in the
        # [main] section. The values are the default values which also 
        # define the valid type of the variable.
        cfgDict = {"keepJobsInRepository":self.keepJobsInRepository,
                   "useSymLinks":self.useSymLinks}
        
        # Check if there are unknown options on the main section and issue
        # warnings if there are...
        if cp.has_section("main"):
            knownOptNames = {}
            for key in cfgDict.keys():
                knownOptNames[key.lower()] = 1
            opts = cp.options("main")
            for opt in opts:
                if opt.lower() not in knownOptNames:
                    self._logger.warning('Unknown job queue config variable "%s" in file %s\n'%(opt, fileName))
        
        # Get the values from the main section. The config variable values
        # are set as local attributes.
        for key in cfgDict.keys():
            defaultVal = cfgDict[key]
            val = defaultVal
            if cp.has_option("main", key):
                if type(defaultVal) is bool:
                    val = cp.getboolean("main", key)
                else:
                    raise TypeError("Internal error: Unknown config var type")
            setattr(self, key, val)
        
        # Read the proc defaults
        for section in cp.sections():
            if section.endswith(" proc"):
                defaultParams = {}
                procName = section[:-5]
                for option in cp.options(section):
                    val = cp.get(section, option)
                    defaultParams[option] = val
                self._defaultProcParams[procName] = defaultParams
    
    def _instantiateJobProc(self, jobType, **params):
        """Create an instance of a job procedure.
        """
        if jobType not in self._procClasses:
            raise ValueError("Unknown job type '%s'"%jobType)
            
        cls = self._procClasses[jobType]
        return cls(**params)

    def _newJobDir(self):
        """Return a full new potential job directory name.
        
        The returned directory name may already be taken if another process
        has picked exactly the same name at almost the same time. The returned
        names contain some randomness to reduce the likeliness of such a
        name clash.
        """
        # Determine the largest existing job number
        jobs = self.listJobs()
        if len(jobs)==0:
            last = 0
        else:
            lastJobLocation = jobs[-1].location
            # Jobs are named "jobX" where X is the job number, so strip
            # off the "job" part of the name to get the job number back.
            last = int(os.path.basename(lastJobLocation)[3:])
            
        # The next job number will be at least this much higher (must be >0)
        # The higher this value, the more jobs fit in-between this job and
        # the previous job in case, priorities are modified later on.
        minDelta = 100
        # The next job number will at most be this much higher (must be >=minDelta)
        # The difference between the min and max delta should at least be about
        # the number of submissions that are expected to be done simultaneously.
        maxDelta = 1000
        nr = random.randint(last+minDelta, last+maxDelta)
        jobDir = os.path.join(self._location, "job%d"%nr)
        return jobDir

def createJobQueue(location, keepJobsInRepository=False, useSymLinks=False):
    """Create and initialize a new job queue directory.
    
    *location* is a string containing the directory path where the job
    queue should be put. This must either refer to an empty directory or
    to a path that does not exist (the parent must exist though).
    
    When jobs are created, they are first stored in a dedicated job repository
    directory (within the job queue directory). The *keepJobsInRepository*
    flag determines whether the jobs will be moved into their respective
    parent job directory or whether they remain in the repository directory.
    
    *useSymLinks* determines whether sub-jobs are linked by creating
    symbolic links on disk or by creating files that contain the target
    directory. This can only be set to ``True`` if the file system actually
    supports symbolic links (so on Windows this should always be ``False``).
    The only advantage of using symbolic links is that it will be easier
    to navigate the job directory manually. However, the disadvantage is that
    the entire job queue directory will not be location independent anymore.
    
    Once the job queue has been created successfully, you can queue jobs
    using the :class:`JobQueue` class.
    """
    # Check the type of location
    if not isinstance(location, basestring):
        raise TypeError("Job location must be a string.")
    
    # If the directory exists, check that it is empty, otherwise create it.
    if os.path.exists(location):
        dp,dirNames,fileNames = os.walk(location).next()
        if dirNames!=[] or fileNames!=[]:
            raise ValueError("Cannot create a new job queue in '%s'. The directory is not empty."%location)
    else:
        # Create the root job queue directory
        os.mkdir(location)
    
    # Create the procs directory (only writable for the current user)
    procPath = os.path.join(location, "procs")
    os.mkdir(procPath, 0755)
    
    # Copy the default procedure scripts into the procs directory.
    # The permissions of the scripts are set to -r--r--r-- (to prevent
    # accidental deletion or modification)
    procTemplatePath = os.path.join(os.path.dirname(__file__), "defaultprocs")
    procs = glob.glob(os.path.join(procTemplatePath, "*.py"))
    for proc in procs:
        # Skip __init__.py
        if os.path.basename(proc).startswith("__"):
            continue
        dst = os.path.join(procPath, os.path.basename(proc))
        shutil.copy(proc, dst)
        os.chmod(dst, 0444)
        
    # Create the config file
    configPath = os.path.join(location, "queue.cfg")
    f = open(configPath, "wt")
    f.write("# Job Queue config file\n\n")
    f.write("[main]\n\n")
    f.write("KeepJobsInRepository = %s\n"%bool(keepJobsInRepository))
    f.write("UseSymLinks = %s\n"%bool(useSymLinks))
    f.close()
