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

import os.path, threading, subprocess


class _StreamPipe(threading.Thread):
    """Read from a pipe (stdout/stderr) and write the data into a file.
    
    This is used to read the stdout/stderr streams from a process launched
    via the subprocess module.
    """
    
    def __init__(self, pipe, outputFile, lineCallback=None, errStream=None, startLineNr=1, name=None):
        """Constructor.
        
        pipe is the stdout or stdin pipe of the launched process.
        outputFile is an open file object that will receive the data that
        is read from pipe.
        lineCallback is a callable (or None) that gets called for every
        line read from pipe. The callback is called with two arguments,
        the line number of the current line and the line itself.
        errStream is a file-like object that receives error messages. If None
        is passed then sys.stderr is used.
        name is the name of the thread.
        """
        threading.Thread.__init__(self, name=name)
        self.setDaemon(True)
        self._pipe = pipe
        self._outputFile = outputFile
        self._lineCallback = lineCallback
        if errStream is None:
            errStream = sys.stderr
        self._errStream = errStream
        self._startLineNr = startLineNr-1
        self.numLines = None
        
    def run(self):
        lines = self._startLineNr
        while 1:
            s = self._pipe.readline()
            if s=="":
                self.numLines = lines
                return
            if self._outputFile is not None:
                try:
                    self._outputFile.write(s)
                except:
                    traceback.print_exc(file=self._errStream)
                    self._outputFile = None
            lines += 1
            # Call the line callback function
            if self._lineCallback is not None:
                try:
                    self._lineCallback(lines, s)
                except:
                    traceback.print_exc(file=self._errStream)
                    self._lineCallback = None


class JobProc(object):
    """Job procedure base class.
    """

    def __init__(self, label=None):
        """Constructor.
        
        Note that for one particular job, the job procedure may get created
        several times (e.g. just to validate parameters).
        """
        object.__init__(self)
        # As a convenience for the user this is set outside the class
        # (in Job._initJobDir()) instead of postCreate())
        self._jobDir = None
        self._jobHandle = None
        if label is not None:
            label = str(label)
        self.label = label
        self.stdout = None
        self.stderr = None
        
    def run(self):
        """Do whatever this job has to do.
        
        When this method is run, the current directory is set to the 
        corresponding :file:`.running` directory.
        This method has to be implemented in a derived class. The base
        method does nothing.
        
        The method can use the :meth:`execCmd()<JobProc.execCmd>` method to run
        command line applications. It can call :meth:`setStatusLine()<JobProc.setStatusLine>`
        and :meth:`setProgress()<JobProc.setProgress>` to report status
        information to the user and it can call :meth:`setError()<JobProc.setError>`
        when the job has failed to run successfully.
        """
        pass
    
    def postCreate(self, jobDir):
        """This method is called once when the job is created.
        
        *jobDir* is the directory in which the job will be run (the attribute
        :attr:`runningDir` will not yet be initialised).
        
        This method can be implemented in a derived class. The base method
        does nothing.
        
        Note: The method is not executed when the job is actually run, so you
        can use it to do initializations that must only be run exactly once.
        """
        pass
    
    def execCmd(self, cmd):
        """Execute a command line string.
        
        *cmds* is a string containing a command that will be executed.
        stdout/stderr is captured into the job's stdout/stderr stream.
        The return value is the return code that was returned by the command.
        
        Derived classes can implement :meth:`stdoutCallback()<JobProc.stdoutCallback>`
        and :meth:`stderrCallback()<JobProc.stderrCallback>` which get called
        for every line.
        """
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        stdoutReader = _StreamPipe(proc.stdout, self.stdout, lineCallback=self.stdoutCallback, errStream=self.stderr, name="StdOutReader")
        stderrReader = _StreamPipe(proc.stderr, self.stderr, lineCallback=self.stderrCallback, errStream=self.stderr, name="StdErrReader")
        stdoutReader.start()
        stderrReader.start()
        ret = proc.wait()
        return ret
        
    def stdoutCallback(self, lineNr, line):
        """This method gets called for every line written to stdout.
        
        When :meth:`execCmd()<JobProc.execCmd>` is used to run a command
        line application, then this callback is called for every line written
        by the application to stdout.
        
        *lineNr* is the line number and *line* the actual contents of the line.
        
        Derived classes may implement this method to scan for errors, status
        messages or progress reports. The default implementation does nothing.
        """
        pass

    def stderrCallback(self, lineNr, line):
        """This method gets called for every line written to stderr.
        
        When :meth:`execCmd()<JobProc.execCmd>` is used to run a command
        line application, then this callback is called for every line written
        by the application to stderr.
        
        *lineNr* is the line number and *line* the actual contents of the line.
        
        Derived classes may implement this method to scan for errors, status
        messages or progress reports. The default implementation does nothing.
        """
        pass
    
    def setProgress(self, value):
        """Set the progress percentage value.
        
        *value* is a number between 0 and 100 (it is clamped) which indicates
        the progress the job has made.
        """
        value = int(value)
        # Clamp between 0 and 100
        value = max(0, min(value, 100))
        progressFile = self._jobHandle.progressFile
        if os.path.exists(progressFile):
            size = os.path.getsize(progressFile)
            if size<value:
                n = value-size
            else:
                n = 0
        else:
            n = value
        
        if n>0:
            try:
                f = open(progressFile, "ab")
            except:
                print >>sys.stderr, "Failed to update progress file "%progressFile
                return
            try:
                f.write(n*"*")
            finally:
                f.close()
                
    def setStatusLine(self, s):
        """Set a status line string.
        
        *s* is a string with a short message that reflects the current status
        of the job.
        """
        s = str(s)
        statusLineFile = self._jobHandle.statusLineFile
        try:
            f = open(statusLineFile, "wt")
        except:
            print >>sys.stderr, "Failed to update progress file "%progressFile
            return
        try:
            f.write(s)
        finally:
            f.close()
    
    def setError(self):
        """Set the job's error flag.
        """
        self._jobHandle.setError()
        
    @property
    def runningDir(self):
        """The directory in which the job is run.
        """
        return self._jobHandle.runningDir

    def _begin(self, jobHandle):
        """This is called internally before run() is called.
        
        jobHandle is the JobHandle object that is associated with this job. 
        """
        self._jobHandle = jobHandle
        self._jobDir = jobHandle.location

        # stdout (line-buffered)
        self.stdout = open(self._jobHandle.stdoutFile, "wt", 1)
        # stderr (unbuffered)
        self.stderr = open(self._jobHandle.stderrFile, "wt", 0)
        
    def _end(self):
        """This is called after run().
        
        This is also called when either  _begin() or run() raised an exception.
        """
        if self.stdout is not None:
            self.stdout.close()
            self.stdout = None
        if self.stderr is not None:
            self.stderr.close()
            self.stderr = None
