:mod:`jobqueue` --- Job queue
=============================

.. module:: cgkit.jobqueue
   :synopsis: Queue jobs and process them

This module implements a simple job queue where you queue jobs (such as
rendering an animation) which can then be processed by one or more machines.
A job is made up of a hierarchy of individual sub-jobs that are processed
in a depth-first order (a job is only executed when all of its sub-jobs
have executed successfully). Running an individual job means running
a particular job procedure which is implemented as a Python object.

The module defines the following function to create a new job queue:

.. autofunction:: cgkit.jobqueue.createJobQueue


:class:`JobQueue` class
-----------------------

.. class:: cgkit.jobqueue.JobQueue(location, logger=None)
   
    A job queue contains a list of jobs which can be run by one or more
    processes. Each job may be composed of sub-jobs which all have to
    be successfully completed before the job is allowed to be processed.
    Jobs that are on the same level in the hierarchy are independent and
    can be run concurrently.
    
    An individual job is represented by a job procedure (:class:`JobProc<cgkit.jobqueue.jobqueue.JobProc>`)
    object that implements the actual functionality of a particular type of job.
    A job is considered done when the :meth:`run()<cgkit.jobqueue.jobqueue.JobProc.run>` method of a job procedure has
    been executed successfully and the job procedure did not indicate an
    error.
    
    On disk, the entire job queue is stored as a directory which contains
    a job directory for each job in the queue. Sub-jobs are sub-directories
    of the job directories.

    *location* is a string containing the directory where the job queue
    is stored (the directory should already exist, it is not created).
    It is also valid to pass ``None`` (which is equivalent to an empty
    queue that cannot receive jobs).
    
    *logger* can be set to a logger object from the :mod:`logging` module
    (or an object with the same interface) which will receive log message.
 
    Note: A job queue object does not store any data about the queue state
    in memory. This means there is no open-close cycle and you can discard
    the job queue object at any time without that the job queue directory
    would get corrupted. Any number of job queue objects (even from different
    processes or machines) can be associated with the same job queue directory
    and submit or run jobs at the same time. Synchronization is entirely
    disk-based, there is no other communication between job queue objects.
 
    .. autoattribute:: location

    .. automethod:: createJobRoot
    
    .. method:: JobQueue.createJob(jobType=None, **params)
    
       This is an alias for :meth:`createJobRoot()<JobQueue.createJobRoot>`.
    
    .. automethod:: listJobs
    
    .. automethod:: deleteJobs
    
    .. automethod:: runNextAvailableJob

:class:`Job` class
------------------
   
.. autoclass:: cgkit.jobqueue.jobqueue.Job
   :members:

:class:`JobRoot` class
----------------------
   
.. autoclass:: cgkit.jobqueue.jobqueue.JobRoot
   :members:
   :inherited-members:

:class:`JobHandle` class
------------------------
   
.. autoclass:: cgkit.jobqueue.jobqueue.JobHandle
   :members:

:class:`JobProc` class
----------------------
   
.. class:: cgkit.jobqueue.JobProc(label=None)

    This is the base class for all job procedures. Every node in the job
    graph is associated with a job procedure object that gets run when
    that particular node is being processed.
    
    *label* is a string with a short informational message about what the
    procedure is going to do (this can be displayed in a GUI).
    
    .. note::
    
       A job procedure object may get instantiated several times per
       job (e.g. once at submission time to validate parameters and once
       when the job is actually run), so the constructor shouldn't do any
       initialization that must only be done exactly once. This can be
       done in the :meth:`postCreate()<JobProc.postCreate>` method instead.

   .. autoattribute:: runningDir

   .. automethod:: run
   
   .. automethod:: postCreate
   
   .. automethod:: stdoutCallback
   
   .. automethod:: stderrCallback
   
   .. automethod:: setProgress
   
   .. automethod:: setStatusLine
   
   .. automethod:: setError
   
   .. automethod:: execCmd
   