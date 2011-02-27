/*
  5DT Data Glove SDK wrapper

  This file can be used to wrap the SDK v1 and v2. For v2 the variable
  GLOVESDK_2_0 has to be defined.
 */

#include <boost/python.hpp>
#include <iostream>
#include <exception>
#include <stdlib.h>

#ifdef WIN32
// windows.h is included so that LPVOID is defined (which is used in fglove.h
// from the SDK v2)
#include <windows.h>
#endif

#include <fglove.h>

using namespace boost::python;

// Exception class
class GloveException : public std::exception
{
  public:
  std::string msg;

  GloveException(std::string amsg="") : msg(amsg) {}
  ~GloveException() throw() {}

  /// Return exception message.
  const char* what() const throw()
  {
    return msg.c_str();
  }
};

/**
  This class contains the driver functions as methods.
 */
class DataGlove
{
  private:
  /// Glove handle
  fdGlove* glovePtr;

  /// Number of sensors
  int numSensors;

  /// Buffer for glove info
  unsigned char gloveInfo[32];
  /// Buffer for driver info
  unsigned char driverInfo[32];

  /// Buffer for sensor values (and other stuff)
  // Size = Max. number of sensor values * sizeof(float) * 2
  // (is also used for the integer values)
  void* sensorBuf;

  public:
  // Callback function (Python callable)
  object callback;

  public:
  DataGlove();
  ~DataGlove();

  bool isConnected();
  void open(std::string port);
  bool close();
  int  getGloveHand();
  int  getGloveType();
  int  getNumSensors();
  int  getNumGestures();
  int  getGesture();
  tuple getThresholdAll();
  tuple getThreshold(int sensor);
  void setThresholdAll(list lower, list upper);
  void setThreshold(int sensor, float lower, float upper);
  boost::python::str getGloveInfo();
  boost::python::str getDriverInfo();
  list getSensorRawAll();
  unsigned short getSensorRaw(int sensor);
  void setSensorRawAll(list data);
  void setSensorRaw(int sensor, unsigned short raw);
  list getSensorScaledAll();
  float getSensorScaled(int sensor);
  tuple getCalibrationAll();
  tuple getCalibration(int sensor);
  void setCalibrationAll(list lower, list upper);
  void setCalibration(int sensor, unsigned short lower, unsigned short upper);
  void resetCalibration();
  list getSensorMaxAll();
  float getSensorMax(int sensor);
  void setSensorMaxAll(list max);
  void setSensorMax(int sensor, float max);

  // Version 2.0 SDK
  #ifdef GLOVESDK_2_0
  void resetCalibration(int sensor);
  list scanUSB();
  int getPacketRate();
  bool newData();
  int getFWVersionMajor();
  int getFWVersionMinor();
  bool getAutoCalibrate();
  bool setAutoCalibrate(bool flag);
  void saveCalibration(std::string filename);
  void loadCalibration(std::string filename);
  void setCallback(object acallback);
  #endif
};

/**---------------------------------------------------------------------
  Constructor.
  ----------------------------------------------------------------------*/
DataGlove::DataGlove()
  : glovePtr(NULL), numSensors(0), sensorBuf(NULL), callback()
{ 
}

/**---------------------------------------------------------------------
  Destructor.
  ----------------------------------------------------------------------*/
DataGlove::~DataGlove()
{
  close();
}

/**---------------------------------------------------------------------
  Returns true if there's a connection to a data glove.

  \return True, if the connection exists
  ----------------------------------------------------------------------*/
bool DataGlove::isConnected()
{
  return glovePtr!=NULL;
}

/**---------------------------------------------------------------------
  Connect to a glove.

  \param port Port name ("COM1", ...)
  \return False, bei einem Fehler.
  ----------------------------------------------------------------------*/
void DataGlove::open(std::string port)
{
  // Initialize glove device
  glovePtr = fdOpen((char*)(port.c_str()));
  if (glovePtr==NULL)
    throw GloveException("Connecting to a glove device at port "+port+" failed.");

  numSensors = fdGetNumSensors(glovePtr);

  // Allocate the buffer for the sensor values...
  sensorBuf = malloc(2*sizeof(float)*numSensors);
  if (sensorBuf==NULL)
  {
    fdClose(glovePtr);
    glovePtr = NULL;
    throw GloveException("Out of memory");
  }
}

/**---------------------------------------------------------------------
  Disconnect from glove.

  \return False if an error occured.
  ----------------------------------------------------------------------*/
bool DataGlove::close()
{
  int res;
  if (glovePtr==NULL)
    return true;

  res = fdClose(glovePtr);
  free(sensorBuf);
  glovePtr = NULL;
  sensorBuf = NULL;
  numSensors = 0;
  return res!=0;
}

/**---------------------------------------------------------------------
  Obtain the handedness of the glove.

  \return 0=Left, 1=Right
  ----------------------------------------------------------------------*/
int DataGlove::getGloveHand()
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  return fdGetGloveHand(glovePtr);
}

/**---------------------------------------------------------------------
  Return the type of the data glove.

  \return Glove type as integer (FD_GLOVENONE, ...)
  ----------------------------------------------------------------------*/
int DataGlove::getGloveType()
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  return fdGetGloveType(glovePtr);
}

/**---------------------------------------------------------------------
  Return the number of sensors.

  \return Number of sensors.
  ----------------------------------------------------------------------*/
int DataGlove::getNumSensors()
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  return numSensors;
}

/**---------------------------------------------------------------------
  Return the raw sensor values.

  \return List with all sensor values (each in the range from 0-4095)
  ----------------------------------------------------------------------*/
list DataGlove::getSensorRawAll()
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  unsigned short* buf = (unsigned short*)sensorBuf;
  list res;
  fdGetSensorRawAll(glovePtr, buf);
  for(int i=0; i<numSensors; i++)
  {
    res.append(buf[i]);
  }
  return res;
}

/**---------------------------------------------------------------------
  Return a raw sensor value.

  \param sensor Sensor index (0,1,...)
  ----------------------------------------------------------------------*/
unsigned short DataGlove::getSensorRaw(int sensor)
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  if ((sensor<0) || (sensor>=numSensors))
    throw GloveException("Sensor index out of range");

  return fdGetSensorRaw(glovePtr, sensor);
}

/**---------------------------------------------------------------------
  Set the raw sensor values.
  ----------------------------------------------------------------------*/
void DataGlove::setSensorRawAll(list data)
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  unsigned short* pData = (unsigned short*)sensorBuf;

  if (data.attr("__len__")()!=numSensors)
    throw GloveException("Invalid number of sensor values.");

  for(int i=0; i<numSensors; i++)
  {
    pData[i] = extract<unsigned short>(data[i]);
  }

  fdSetSensorRawAll(glovePtr, pData);
}

/**---------------------------------------------------------------------
  Set a raw sensor value.

  \param sensor Sensor index (0,1,...)
  \param raw Sensor value
  ----------------------------------------------------------------------*/
void DataGlove::setSensorRaw(int sensor, unsigned short raw)
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  if ((sensor<0) || (sensor>=numSensors))
    throw GloveException("Sensor index out of range");

  fdSetSensorRaw(glovePtr, sensor, raw);
}

/**---------------------------------------------------------------------
  Return the scaled sensor values.

  \return List with all sensor values (as floats)
  ----------------------------------------------------------------------*/
list DataGlove::getSensorScaledAll()
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  float* buf = (float*)sensorBuf;
  list res;
  fdGetSensorScaledAll(glovePtr, buf);
  for(int i=0; i<numSensors; i++)
  {
    res.append(buf[i]);
  }
  return res;
}

/**---------------------------------------------------------------------
  Return a scaled sensor value.

  \param sensor Sensor index (0,1,...)
  ----------------------------------------------------------------------*/
float DataGlove::getSensorScaled(int sensor)
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  if ((sensor<0) || (sensor>=numSensors))
    throw GloveException("Sensor index out of range");

  return fdGetSensorScaled(glovePtr, sensor);
}

/**---------------------------------------------------------------------
  Return the current auto-calibration settings.

  \return Tuple (LowerValues, UpperValues)
  ----------------------------------------------------------------------*/
tuple DataGlove::getCalibrationAll()
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  unsigned short* upper = (unsigned short*)sensorBuf;
  unsigned short* lower = ((unsigned short*)(sensorBuf))+numSensors;
  list resU, resL;
  fdGetCalibrationAll(glovePtr, upper, lower);
  for(int i=0; i<numSensors; i++)
  {
    resU.append(upper[i]);
    resL.append(lower[i]);
  }
  return make_tuple(resL, resU);
}

/**---------------------------------------------------------------------
  Return the auto-calibration settings for one particular sensor.

  \param sensor Sensor index (0,1,...)
  \return Tuple (lower, upper)
  ----------------------------------------------------------------------*/
tuple DataGlove::getCalibration(int sensor)
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  if ((sensor<0) || (sensor>=numSensors))
    throw GloveException("Sensor index out of range");

  unsigned short lower, upper;
  fdGetCalibration(glovePtr, sensor, &upper, &lower);
  return make_tuple(lower, upper);
}

/**---------------------------------------------------------------------
  Set the auto-calibration settings.

  ----------------------------------------------------------------------*/
void DataGlove::setCalibrationAll(list lower, list upper)
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  unsigned short* pupper = (unsigned short*)sensorBuf;
  unsigned short* plower = ((unsigned short*)sensorBuf)+numSensors;

  if ((lower.attr("__len__")()!=numSensors) || 
      (upper.attr("__len__")()!=numSensors))
  {
    throw GloveException("Invalid number of calibration values.");
  }

  for(int i=0; i<numSensors; i++)
  {
    plower[i] = extract<unsigned short>(lower[i]);
    pupper[i] = extract<unsigned short>(upper[i]);
  }

  fdSetCalibrationAll(glovePtr, pupper, plower);
}

/**---------------------------------------------------------------------
  Set the auto-calibration settings for one particular sensor.

  \param sensor Sensor index (0,1,...)
  \param lower Minimum value
  \param upper Maximum value
  ----------------------------------------------------------------------*/
void DataGlove::setCalibration(int sensor, unsigned short lower, unsigned short upper)
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  if ((sensor<0) || (sensor>=numSensors))
    throw GloveException("Sensor index out of range");

  fdSetCalibration(glovePtr, sensor, upper, lower);
}

/**---------------------------------------------------------------------
  Reset auto-calibration values.
  ----------------------------------------------------------------------*/
void DataGlove::resetCalibration()
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  fdResetCalibration(glovePtr);
}

/**---------------------------------------------------------------------
  Return the maximum scaled values of the sensors.

  \return List with all maximum sensor values (as floats)
  ----------------------------------------------------------------------*/
list DataGlove::getSensorMaxAll()
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  float* buf = (float*)sensorBuf;
  list res;
  fdGetSensorMaxAll(glovePtr, buf);
  for(int i=0; i<numSensors; i++)
  {
    res.append(buf[i]);
  }
  return res;
}

/**---------------------------------------------------------------------
  Return a maximum scaled sensor value for one particular sensor.

  \param sensor Sensor index (0,1,...)
  ----------------------------------------------------------------------*/
float DataGlove::getSensorMax(int sensor)
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  if ((sensor<0) || (sensor>=numSensors))
    throw GloveException("Sensor index out of range");

  return fdGetSensorMax(glovePtr, sensor);
}

/**---------------------------------------------------------------------
  Set the maximum scaled sensor values.
  ----------------------------------------------------------------------*/
void DataGlove::setSensorMaxAll(list max)
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  float* pMax = (float*)sensorBuf;

  if (max.attr("__len__")()!=numSensors)
    throw GloveException("Invalid number of maximum sensor values.");

  for(int i=0; i<numSensors; i++)
  {
    pMax[i] = extract<float>(max[i]);
  }

  fdSetSensorMaxAll(glovePtr, pMax);
}

/**---------------------------------------------------------------------
  Set the maximum scaled sensor value.

  \param sensor Sensor index (0,1,...)
  \param max Maximum sensor value
  ----------------------------------------------------------------------*/
void DataGlove::setSensorMax(int sensor, float max)
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  if ((sensor<0) || (sensor>=numSensors))
    throw GloveException("Sensor index out of range");

  fdSetSensorMax(glovePtr, sensor, max);
}


/**---------------------------------------------------------------------
  Return the number of available gestures.

  \return Number of gestures
  ----------------------------------------------------------------------*/
int DataGlove::getNumGestures()
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  return fdGetNumGestures(glovePtr);
}

/**---------------------------------------------------------------------
  Returns the current gesture recognition threshold values.

  \return (Lower, Upper)
  ----------------------------------------------------------------------*/
tuple DataGlove::getThresholdAll()
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  float* upper = (float*)sensorBuf;
  float* lower = ((float*)sensorBuf)+numSensors;
  list resL, resU;
  fdGetThresholdAll(glovePtr, upper, lower);
  for(int i=0; i<numSensors; i++)
  {
    resU.append(upper[i]);
    resL.append(lower[i]);
  }
  return make_tuple(resL, resU);
}

/**---------------------------------------------------------------------
  Return the gesture recognition threshold values.

  \param sensor Sensor index (0,1,...)
  \return Tuple (lower, upper)
  ----------------------------------------------------------------------*/
tuple DataGlove::getThreshold(int sensor)
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  if ((sensor<0) || (sensor>=numSensors))
    throw GloveException("Sensor index out of range");

  float lower, upper;
  fdGetThreshold(glovePtr, sensor, &upper, &lower);
  return make_tuple(lower, upper);
}

/**---------------------------------------------------------------------
  Set the gesture recognition threshold values.

  ----------------------------------------------------------------------*/
void DataGlove::setThresholdAll(list lower, list upper)
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  float* pupper = (float*)sensorBuf;
  float* plower = ((float*)sensorBuf)+numSensors;

  if ((lower.attr("__len__")()!=numSensors) || 
      (upper.attr("__len__")()!=numSensors))
  {
    throw GloveException("Invalid number of threshold values.");
  }

  for(int i=0; i<numSensors; i++)
  {
    plower[i] = extract<float>(lower[i]);
    pupper[i] = extract<float>(upper[i]);
  }

  fdSetThresholdAll(glovePtr, pupper, plower);
}

/**---------------------------------------------------------------------
  Set the gesture recognition threshold values.

  \param sensor Sensor index (0,1,...)
  ----------------------------------------------------------------------*/
void DataGlove::setThreshold(int sensor, float lower, float upper)
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  if ((sensor<0) || (sensor>=numSensors))
    throw GloveException("Sensor index out of range");

  fdSetThreshold(glovePtr, sensor, upper, lower);
}


/**---------------------------------------------------------------------
  Return the current gesture.

  \return Gesture
  ----------------------------------------------------------------------*/
int DataGlove::getGesture()
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  return fdGetGesture(glovePtr);
}


/**---------------------------------------------------------------------
  Return glove information.

  \return Info string.
  ----------------------------------------------------------------------*/
boost::python::str DataGlove::getGloveInfo()
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  fdGetGloveInfo(glovePtr, gloveInfo);
  return boost::python::str((const char*)gloveInfo);
}

/**---------------------------------------------------------------------
  Return driver information.

  \return Driver info (string).
  ----------------------------------------------------------------------*/
boost::python::str DataGlove::getDriverInfo()
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  fdGetDriverInfo(glovePtr, driverInfo);
  return boost::python::str((const char*)driverInfo);
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// SDK 2.0 methods
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#ifdef GLOVESDK_2_0

/**---------------------------------------------------------------------
  Reset auto-calibration values for one sensor.
  ----------------------------------------------------------------------*/
void DataGlove::resetCalibration(int sensor)
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  if ((sensor<0) || (sensor>=numSensors))
    throw GloveException("Sensor index out of range");

  fdResetCalibration(glovePtr, sensor);
}

/**---------------------------------------------------------------------
  Scans the USB for available gloves.

  Returns a list of product IDs of the gloves found.
  ----------------------------------------------------------------------*/
list DataGlove::scanUSB()
{
  list res;
  unsigned short pid[10];
  int numMax = 10;
  int n;

  n = fdScanUSB(pid, numMax);
  for(int i=0; i<n; i++)
  {
    res.append(pid[i]);
  }
  return res;
}

/**---------------------------------------------------------------------
  Return the packet rate.
  ----------------------------------------------------------------------*/
int DataGlove::getPacketRate()
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  return fdGetPacketRate(glovePtr);
}

/**---------------------------------------------------------------------
  Check if new data is available.
  ----------------------------------------------------------------------*/
bool DataGlove::newData()
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  return fdNewData(glovePtr);
}

/**---------------------------------------------------------------------
  Get the major version of the glove's firmware.
  ----------------------------------------------------------------------*/
int DataGlove::getFWVersionMajor()
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  return fdGetFWVersionMajor(glovePtr);
}

/**---------------------------------------------------------------------
  Get the minor version of the glove's firmware.
  ----------------------------------------------------------------------*/
int DataGlove::getFWVersionMinor()
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  return fdGetFWVersionMinor(glovePtr);
}

/**---------------------------------------------------------------------
  Check if auto calibrate is on.
  ----------------------------------------------------------------------*/
bool DataGlove::getAutoCalibrate()
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  return fdGetAutoCalibrate(glovePtr);
}

/**---------------------------------------------------------------------
  Enable/disable auto calibration.
  ----------------------------------------------------------------------*/
bool DataGlove::setAutoCalibrate(bool flag)
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  return fdSetAutoCalibrate(glovePtr, flag);
}

/**---------------------------------------------------------------------
  Save calibration data.
  ----------------------------------------------------------------------*/
void DataGlove::saveCalibration(std::string filename)
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  if (!fdSaveCalibration(glovePtr, filename.c_str()))
    throw GloveException("Could not save calibration file \""+filename+"\".");
}

/**---------------------------------------------------------------------
  Load calibration data.
  ----------------------------------------------------------------------*/
void DataGlove::loadCalibration(std::string filename)
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  if (!fdLoadCalibration(glovePtr, filename.c_str()))
    throw GloveException("Could not load calibration file \""+filename+"\".");
}

// Dispatcher function.
// This function is used as actual callback function. arg is a pointer
// to the DataGlove object that activated the callback. This function will
// then call the callback function stored in the DataGlove object.
void dispatcher(void* arg)
{
  object& f = ((DataGlove*)arg)->callback;
  f();
}

/**---------------------------------------------------------------------
  Set a callback function that gets called when a new packet was received.
  ----------------------------------------------------------------------*/
void DataGlove::setCallback(object acallback)
{
  if (glovePtr==NULL)
    throw GloveException("No connection to a data glove established.");

  // Is callback None?
  if (acallback==object())
  {
    fdSetCallback(glovePtr, NULL, NULL);
    return;
  }

  callback = acallback;
  fdSetCallback(glovePtr, (void*)dispatcher, this);
}

#endif  // GLOVESDK_2_0

//////////////////////////////////////////////////////////////////////

BOOST_PYTHON_MODULE(_glove)
{
  void (DataGlove::*resetCalibrationAll)() = &DataGlove::resetCalibration;
  #ifdef GLOVESDK_2_0
  void (DataGlove::*resetCalibration)(int) = &DataGlove::resetCalibration;
  #endif

  class_<DataGlove>("DataGlove")
    .def("isConnected", &DataGlove::isConnected)
    .def("open", &DataGlove::open)
    .def("close", &DataGlove::close)
    .def("getGloveHand", &DataGlove::getGloveHand)
    .def("getGloveType", &DataGlove::getGloveType)
    .def("getNumSensors", &DataGlove::getNumSensors)
    .def("getNumGestures", &DataGlove::getNumGestures)
    .def("getGloveInfo", &DataGlove::getGloveInfo)
    .def("getDriverInfo", &DataGlove::getDriverInfo)
    .def("getSensorRawAll", &DataGlove::getSensorRawAll)
    .def("getSensorRaw", &DataGlove::getSensorRaw)
    .def("setSensorRawAll", &DataGlove::setSensorRawAll)
    .def("setSensorRaw", &DataGlove::setSensorRaw)
    .def("getSensorScaledAll", &DataGlove::getSensorScaledAll)
    .def("getSensorScaled", &DataGlove::getSensorScaled)
    .def("getCalibrationAll", &DataGlove::getCalibrationAll)
    .def("getCalibration", &DataGlove::getCalibration)
    .def("setCalibrationAll", &DataGlove::setCalibrationAll)
    .def("setCalibration", &DataGlove::setCalibration)
    .def("getSensorMaxAll", &DataGlove::getSensorMaxAll)
    .def("getSensorMax", &DataGlove::getSensorMax)
    .def("setSensorMaxAll", &DataGlove::setSensorMaxAll)
    .def("setSensorMax", &DataGlove::setSensorMax)
    .def("resetCalibration", resetCalibrationAll)
    .def("getGesture", &DataGlove::getGesture)
    .def("getThresholdAll", &DataGlove::getThresholdAll)
    .def("getThreshold", &DataGlove::getThreshold)
    .def("setThresholdAll", &DataGlove::setThresholdAll)
    .def("setThreshold", &DataGlove::setThreshold)

    // version 2.0 SDK
    #ifdef GLOVESDK_2_0
    .def("resetCalibration", resetCalibration)
    .def("scanUSB", &DataGlove::scanUSB)
    .def("setCallback", &DataGlove::setCallback)
    .def("getPacketRate", &DataGlove::getPacketRate)
    .def("newData", &DataGlove::newData)
    .def("getFWVersionMajor", &DataGlove::getFWVersionMajor)
    .def("getFWVersionMinor", &DataGlove::getFWVersionMinor)
    .def("getAutoCalibrate", &DataGlove::getAutoCalibrate)
    .def("setAutoCalibrate", &DataGlove::setAutoCalibrate)
    .def("saveCalibration", &DataGlove::saveCalibration)
    .def("loadCalibration", &DataGlove::loadCalibration)  
    #endif
  ;

}
