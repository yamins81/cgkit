# Render using Maya

import os, os.path, subprocess, sys, traceback
import cgkit.jobqueue

class maya(cgkit.jobqueue.JobProc):
    """Render a Maya file.
    """
    
    def __init__(self, sceneFile=None, project=None,
                 startFrame=None, endFrame=None,
                 renderCmd = "Render",
                 label=None):
        """Constructor.
        
        label: The GUI label string
        """
        # Check the parameter types...
        if not isinstance(sceneFile, basestring):
            raise TypeError("sceneFile argument must be a string, got a %s instead"%getattr(type(sceneFile), "__name__", "?"))
        if not (project is None or isinstance(project, basestring)):
            raise TypeError("project argument must be a string or None, got a %s instead"%getattr(type(project), "__name__", "?"))
        if not (startFrame is None or type(startFrame) in [int, float]):
            raise TypeError("startFrame must be a number type or None, got a %s instead"%getattr(type(startFrame), "__name__", "?"))
        if not (endFrame is None or type(endFrame) in [int, float]):
            raise TypeError("endFrame must be a number type or None, got a %s instead"%getattr(type(endFrame), "__name__", "?"))
        if not isinstance(renderCmd, basestring):
            raise TypeError("renderCmd argument must be a string, got a %s instead"%getattr(type(renderCmd), "__name__", "?"))

        # Set the default label if no label was given
        if label is None:
            label = 'Maya "%s"'%os.path.basename(sceneFile)
            
        cgkit.jobqueue.JobProc.__init__(self, label=label)
        
        # Store the parameters
        toks = [renderCmd] 
        if startFrame is not None:
            toks.append("-s %s"%startFrame)
        if endFrame is not None:
            toks.append("-e %s"%endFrame)
        if project is not None and project!="":
            toks.append('-proj "%s"'%project)
        toks.append('"%s"'%sceneFile)
    
        self._cmdLine = " ".join(toks)
        
    def run(self):
        """Run the Render tool.
        """
        print >>self.stdout, self._cmdLine
        ret = self.execCmd(self._cmdLine)
        if ret!=0:
            print >>self.stderr, "\nReturn code:",ret
            self.setError()
        else:
            self.setStatusLine("Done")

    def stdoutCallback(self, lineNr, line):
        """Scan stdout for status/errors.
        """
        pass

    def stderrCallback(self, lineNr, line):
        """Scan stderr for status/errors.
        """
        # Does a frame begin?
        if line.startswith("Starting Rendering"):
            # Get the image path (remove the prefix and the trailing dot)
            fileName = os.path.normpath(line.strip()[19:-1])
            p,n = os.path.split(fileName)
            self.setStatusLine("Rendering %s (%s)"%(n,p))
        # Is there an error line?
        elif line.startswith("Error:"):
            self.setStatusLine(line.strip())
