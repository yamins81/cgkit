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
# $Id: riutil.py,v 1.1.1.1 2004/12/12 14:31:21 mbaas Exp $

"""This module provides helper functions for the pointcloud module.
"""

ctypedef void* PtcPointCloud 
ctypedef int (*PtcReadDataPointPtr)(PtcPointCloud, float* pnt, float* normal, float* radius, float* data)
ctypedef int (*PtcWriteDataPointPtr)(PtcPointCloud, float* pnt, float* normal, float radius, float* data)

def readDataPoints(long readFuncLoc, long ptcHandle, long numPoints,
                   long pointBuf, int pointStride,
                   long normalBuf, int normalStride,
                   long radiusBuf, int radiusStride,
                   long dataBuf, int dataStride):
    """Read a sequence of data points.
    
    readFuncLoc is the location of the PtcReadDataPoint function pointer.
    ptcHandle is the open point cloud handle. numPoints is the number of
    points that should be read.
    All the *Buf variables are the respective buffers where the data is written
    to. *Stride is the number of floats to advance the pointers to get to the
    next data location. The buffers must be large enough to hold numPoints
    items.
    numPoints must at least be the number of points that are still left
    in the file (otherwise the function may generate an exception or return
    bogus values, whatever the underlying RenderMan implementation does).
    """
    cdef PtcReadDataPointPtr PtcReadDataPoint
    cdef PtcPointCloud pointCloud
    PtcReadDataPoint = (<PtcReadDataPointPtr*>readFuncLoc)[0]
    pointCloud = <PtcPointCloud>(ptcHandle)
    
    cdef float* pntPtr
    cdef float* normalPtr
    cdef float* radiusPtr
    cdef float* dataPtr
    cdef int n
    cdef int res
    
    pntPtr = <float*>pointBuf
    normalPtr = <float*>normalBuf
    radiusPtr = <float*>radiusBuf
    dataPtr = <float*>dataBuf
    
    # Read the point data into the given buffers...
    n = 0
    while n<numPoints:
        res = PtcReadDataPoint(pointCloud, pntPtr, normalPtr, radiusPtr, dataPtr)
        if res==0:
            raise IOError("Failed to read data point from point cloud file")
        pntPtr += pointStride
        normalPtr += normalStride
        radiusPtr += radiusStride
        dataPtr += dataStride
        n += 1

def writeDataPoints(long writeFuncLoc, long ptcHandle, long numPoints,
                    long pointBuf, int pointStride,
                    long normalBuf, int normalStride,
                    long radiusBuf, int radiusStride,
                    long dataBuf, int dataStride):
    """Write a sequence of data points.
    
    writeFuncLoc is the location of the PtcWriteDataPoint function pointer.
    ptcHandle is the open point cloud handle. numPoints is the number of
    points that should be written.
    All the *Buf variables are the respective buffers where the data is read
    from. *Stride is the number of floats to advance the pointers to get to the
    next data location. The buffers must contain at least numPoints values.
    """
    cdef PtcWriteDataPointPtr PtcWriteDataPoint
    cdef PtcPointCloud pointCloud
    PtcWriteDataPoint = (<PtcWriteDataPointPtr*>writeFuncLoc)[0]
    pointCloud = <PtcPointCloud>(ptcHandle)
    
    cdef float* pntPtr
    cdef float* normalPtr
    cdef float* radiusPtr
    cdef float* dataPtr
    cdef int n
    cdef int res
    
    pntPtr = <float*>pointBuf
    normalPtr = <float*>normalBuf
    radiusPtr = <float*>radiusBuf
    dataPtr = <float*>dataBuf
    
    # Write the point data into the file...
    n = 0
    while n<numPoints:
        res = PtcWriteDataPoint(pointCloud, pntPtr, normalPtr, radiusPtr[0], dataPtr)
        if res==0:
            raise IOError("Failed to write data point into point cloud file")
        pntPtr += pointStride
        normalPtr += normalStride
        radiusPtr += radiusStride
        dataPtr += dataStride
        n += 1
