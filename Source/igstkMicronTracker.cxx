/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMicronTracker.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters in the
// debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "Markers.h"
#include "Marker.h"
#include "Persistence.h"
#include "Cameras.h"
#include "MCamera.h"
#include "Facet.h"
#include "Xform3D.h"
#include "MTC.h"

#include <math.h>

#include "igstkMicronTracker.h"
#include <itksys/SystemTools.hxx>

#include <sstream>

namespace igstk
{

//Initialize static variables
std::map< unsigned int, std::string> MicronTracker::m_ErrorCodeContainer;
bool MicronTracker::m_ErrorCodeListCreated = false;

/** Constructor: Initializes all internal variables. */
MicronTracker::MicronTracker(void):m_StateMachine(this)
{
  m_NumberOfTools = 0;

  this->SetThreadingEnabled( true );

  // Lock for the data buffer that is used to transfer the
  // transformations from thread that is communicating
  // with the tracker to the main thread.
  m_BufferLock = itk::MutexLock::New();

  //instantiate Persistance object
  m_Persistence = new Persistence();

  //instantiate Markers object
  m_Markers = new Markers();

  //instantiate cameras
  m_Cameras = new Cameras();

  // initialize selected camera
  m_SelectedCamera = NULL;

  // Camera light coolness
  m_CameraLightCoolness = 0.1;

  // Create error code list if it hasn't been
  // created.
  //
  if ( ! m_ErrorCodeListCreated )
    {
    this->CreateErrorCodeList();
    m_ErrorCodeListCreated = true;
    }
}

/** Destructor */
MicronTracker::~MicronTracker(void)
{
  if ( m_Cameras != NULL )
    {
    delete m_Cameras;
    }

  if ( m_Markers != NULL )
    {
    delete m_Markers;
    }

  if ( m_Persistence != NULL )
    {
    delete m_Persistence;
    }
}

/** Create a map data structure containing MTC error code and description */
void MicronTracker::CreateErrorCodeList()
{
  m_ErrorCodeContainer[0]  = "OK"; 
  m_ErrorCodeContainer[1]  = "Invalid object handle";
  m_ErrorCodeContainer[2]  = "Reentrant access - library is not thread-safe";
  m_ErrorCodeContainer[3]  = "Internal MicronTracker software error";
  m_ErrorCodeContainer[4]  = "Null pointer parameter";
  m_ErrorCodeContainer[5]  = "Out of memory";
  m_ErrorCodeContainer[6]  = "Parameter out of range";
  m_ErrorCodeContainer[7]  = "String parameter too long";
  m_ErrorCodeContainer[8]  = "Insufficient space allocated by the client to the output buffer";
  m_ErrorCodeContainer[9]  = "Camera not initialized";
  m_ErrorCodeContainer[10] = "Camera already initialized - cannot be initialized twice";
  m_ErrorCodeContainer[11] = "Camera initialization failed";
  m_ErrorCodeContainer[12] = "MTC is incompatible with a software module it calls";
  m_ErrorCodeContainer[13] = "Calibration file error: unrecognized camera model";
  m_ErrorCodeContainer[14] = "Path not set";
  m_ErrorCodeContainer[15] = "Cannot access the directory specified";
  m_ErrorCodeContainer[16] = "Write to file failed";
  m_ErrorCodeContainer[17] = "Invalid Index parameter";
  m_ErrorCodeContainer[18] = "Invalid SideI parameter";
  m_ErrorCodeContainer[19] = "Invalid Divisor parameter";
  m_ErrorCodeContainer[20] = "Attempting to access an item of an empty IntCollection";
  m_ErrorCodeContainer[21] = "Insufficient samples";
  m_ErrorCodeContainer[22] = "Insufficient samples that fit within the acceptance tolerance";
  m_ErrorCodeContainer[23] = "Odd number of vector samples";
  m_ErrorCodeContainer[24] = "Less than 2 vectors";
  m_ErrorCodeContainer[25] = "More than maximum vectors per facet";
  m_ErrorCodeContainer[26] = "Error exceeds tolerance";
  m_ErrorCodeContainer[27] = "Insufficient angle between vectors";
  m_ErrorCodeContainer[28] = "First vector is shorter than the second";
  m_ErrorCodeContainer[29] = "Vector lengths are too similar";
  m_ErrorCodeContainer[30] = "Template vector has 0 length";
  m_ErrorCodeContainer[31] = "The template has not been created or loaded";
  m_ErrorCodeContainer[32] = "Template file is corrupt";
  m_ErrorCodeContainer[33] = "Maximum number of marker templates allowed exceeded";
  m_ErrorCodeContainer[34] = "Geometries of different facets are too similar";
  m_ErrorCodeContainer[35] = "Noncompliant facet definition";
  m_ErrorCodeContainer[36] = "The SampledVectorPairsCollection contains non-Vector handles";
  m_ErrorCodeContainer[37] = "Empty pixels buffer";
  m_ErrorCodeContainer[38] = "Dimensions do not match";
  m_ErrorCodeContainer[39] = "File open failed";
  m_ErrorCodeContainer[40] = "File read failed";
  m_ErrorCodeContainer[41] = "File write failed";
  m_ErrorCodeContainer[42] = "Cannot open calibration file (typically named [driver]_[ser num].calib";
  m_ErrorCodeContainer[43] = "Not a calibration file";
  m_ErrorCodeContainer[44] = "Calibration file contents corrupt";
  m_ErrorCodeContainer[45] = "Calibration file was not generated from this camera";
  m_ErrorCodeContainer[46] = "Calibration file not loaded";
  m_ErrorCodeContainer[47] = "Incorrect file version";
  m_ErrorCodeContainer[48] = "Input image location is out of bounds of the measurement volume";
  m_ErrorCodeContainer[49] = "Input image locations do not triangulate to a valid 3-D point";
  m_ErrorCodeContainer[50] = "Transform between coordinate spaces is unknown";
  m_ErrorCodeContainer[51] = "The given camera object was not found in the cameras array";
  m_ErrorCodeContainer[52] = "Feature Data unavailable for the current frame";
  m_ErrorCodeContainer[53] = "Feature Data is corrupt or incompatible with the current version";
  m_ErrorCodeContainer[54] = "XYZ position is outside of calibrated field of view";
  m_ErrorCodeContainer[55] = "Grab frame error";
}

const std::string  MicronTracker::GetErrorDescription( unsigned int code ) 
{
  if ( code >= 0 && code <= 55 )
    {
    return MicronTracker::m_ErrorCodeContainer[code];
    } 
  else
    {
    return "Unknown error code";
    }
}

/** Specify camera calibration directory */
void MicronTracker::SetCameraCalibrationFilesDirectory( std::string fileName )
{
  igstkLogMacro( DEBUG,
                "igstk::MicronTracker::SetCameraCalibrationFilesDirectory called..\n");

  m_CalibrationFilesDirectory = fileName;
}

/** Specify the initialization filename */
void MicronTracker::SetInitializationFile( std::string fileName )
{
  igstkLogMacro( DEBUG,
                "igstk::MicronTracker::SetInitializationFileDirectoryPath called..\n");

  m_InitializationFile = fileName;
}

/** Load marker template  */
void
MicronTracker::LoadMarkerTemplate( std::string filename )
{
  igstkLogMacro( DEBUG,
                "igstk::MicronTracker::LoadMarkerTemplate called..\n");

  // clear templates already loaded
  this->m_Markers->clearTemplates();

  this->m_MarkerTemplateDirectory = filename;

  if ( m_MarkerTemplateDirectory == "" )
    {
    std::cerr << "Marker template directory name is empty string " << std::endl;
    return ;
    }
 
  char * markerTemplateDirectory = 
            const_cast< char *> ( m_MarkerTemplateDirectory.c_str() );
 
  unsigned int  status = Markers_LoadTemplates( markerTemplateDirectory );

  if ( status != 0 )
    {
    std::cerr << "Error loading the templates: " << MicronTracker::GetErrorDescription( status ) 
                                                << std::endl; 
    }
}

MicronTracker::ResultType MicronTracker::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "igstk::MicronTracker::InternalOpen called ...\n");

  /* Initialization involves two steps
   * 1) Set algorithm and camera attributes
   * 2) Attach the cameras
   */

  if ( ! this->Initialize() )
      {
      std::cerr << "Error initializing" << std::endl;
      return FAILURE;
      }

  if ( ! this->SetUpCameras() )
      {
      std::cerr << "Error setting up cameras " << std::endl;
      return FAILURE;
      }
  return SUCCESS;
}

/** Initialize camera and algorithm attributes such as 
 *  Frame interleave, template matching tolerance, extrapolate frame etc*/
bool MicronTracker::Initialize( void )
{
  igstkLogMacro( DEBUG, "igstk::MicronTracker::Initialize called ...\n");

  bool result = true;
  if ( m_InitializationFile == "" || !itksys::SystemTools::FileExists( m_InitializationFile.c_str() ) ) 
    {
    std::cerr << "Initialization file (.ini ) is not properly set" << std::endl;
    return FAILURE;
    }
  char * initializationFilename = 
            const_cast< char *> ( m_InitializationFile.c_str() );
  this->m_Persistence->setPath( initializationFilename );
  this->m_Persistence->setSection ("General");

  /* The LightCoolness property provides an indication to the camera of how the
  illumination spectrum is distributed, allowing it to correct for the difference
  between the calibration light spectrum and the measurement light spectrum.
  The LightCoolness value indicates the balance between reds (warm) and blue (cool)
  colors in the lighting.*/ 
  double defaultLightCoolness = 0.1;
  // light coolness value will be set on the camera after it is attached. 
  m_CameraLightCoolness = 
      this->m_Persistence->retrieveDouble(
         "LightCoolness", defaultLightCoolness);
 

  /* PredictiveFramesInterleave controls the number of predictive-only matching
  *frames inserted between each subsequent full-matching frames. A valued of 0
  *means that all frames are full-matching (require more computing resource). 
  *A value of 3, for example, means that 3 out of every 4 frames would be 
  *predictive-only */
  int defaultFrameInterleave = 0;
  this->m_Markers->setPredictiveFramesInterleave(
  this->m_Persistence->retrieveInt("PredictiveFramesInterleave",
       defaultFrameInterleave) ); 
  
  //Sets the template match tolerance (in millimeters).
  //This tolerance determines the sensitivity of the MicronTracker to deviations
  //between the measured relative positions of XPoints and their relative positions
  //as described in the facet templates . The lower the tolerance, the more 
  //likely it is tracking would be momentarily lost under some
  //circumstances (poor lighting, large distance), but the more closely the geometry
  //of different markers can be without the possibility of accidentally confusing
  //them.
  double defaultTempMatchToleranceMM = 1.0;
  this->m_Markers->setTemplateMatchToleranceMM( 
      this->m_Persistence->retrieveDouble(
         "TemplateMatchToleranceMM", defaultTempMatchToleranceMM) );
  
  //Selects which image footprint size to use for the XPoint detection algorithm.
  //Smaller XPoint footprint is 9 pixels in diameter, while the regular size is 11.
  //Using the smaller footprint allows to detect smaller markers at larger distances
  //from the camera, at the expense of reduced accuracy near the far end of the FOM.
  bool defaultSmallerXPFootprint = true;
  bool SmallerXPFootprint = 
            (bool)(this->m_Persistence->retrieveInt(
                       "DetectSmallMarkers", defaultSmallerXPFootprint));
  this->m_Markers->setSmallerXPFootprint(SmallerXPFootprint);


  // ExtrapolatedFrames controls the number of future frames for which the
  // markers, which have been identified in the current frame, are remembered.
  // The predicted locations of the remembered markers can then be reported if
  // not found in the future frames. Setting this property to 0 disables
  // extrapolation.
  int  defaultExtrapolatedFrames = 5;
  int ExtrapolatedFrames = this->m_Persistence->retrieveInt(
                        "ExtrapolatedFrames", defaultExtrapolatedFrames);
  this->m_Markers->setExtrapolatedFrames(ExtrapolatedFrames);

  
  return result;
}

bool MicronTracker::SetUpCameras( void )
{
  igstkLogMacro( DEBUG, "igstk::MicronTracker::SetUpCameras called ...\n");

  bool result = true;

  if ( m_CalibrationFilesDirectory == "" || 
       !itksys::SystemTools::FileExists( m_CalibrationFilesDirectory.c_str() ) ) 
    {
    std::cerr << "Camera calibration directory is not properly set" << std::endl;
    return FAILURE;
    }
 
  this->m_Cameras->SetCameraCalibrationFilesDirectory( this->m_CalibrationFilesDirectory );
  int success = this->m_Cameras->AttachAvailableCameras();

  if ( success )
    {
    std::cerr << " No camera available or missing calibration file in:\t " 
              << this->m_CalibrationFilesDirectory << endl;
    std::cerr << "Error returned: " << MTLastErrorString() << std::endl; 
    result = false;
    }
  else
    {
    //This tracker implementation only handles a single camera. However, MTC
    //library provides 
    if ( this->m_Cameras->getCount() >= 1 ) 
      {
      this->m_SelectedCamera = this->m_Cameras->m_vCameras[0];
      // set the camera Light coolness parameter
      this->m_SelectedCamera->setLightCoolness( m_CameraLightCoolness );
      } 
    }

  return result;
}

/** Verify tracker tool information*/
MicronTracker::ResultType
MicronTracker
::VerifyTrackerToolInformation( TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG, "igstk::MicronTracker::VerifyTrackerToolInformation called ...\n");  

  // Verify that the template file for the marker is found
  unsigned int totalNumberOfTemplates = Markers_TemplatesCount();


  MicronTrackerToolType * micronTrackerTool  = 
        dynamic_cast< MicronTrackerToolType *> ( trackerTool );

  if ( micronTrackerTool == NULL )
    {
    return FAILURE;
    }

  for (unsigned int idx=1 ; idx < totalNumberOfTemplates ; idx++ )
    {
    std::string templateName;
    this->m_Markers->getTemplateItemName(idx, templateName );
    if( micronTrackerTool->GetMarkerName() == templateName )
      {
      std::vector< double > transform;
      transform.push_back( 0.0 );
      transform.push_back( 0.0 );
      transform.push_back( 0.0 );
      transform.push_back( 0.0 );
      transform.push_back( 0.0 );
      transform.push_back( 0.0 );
      transform.push_back( 1.0 );

      m_ToolTransformBuffer[ micronTrackerTool->GetMarkerName() ] = transform;
      m_ToolStatusContainer[micronTrackerTool->GetMarkerName()] = 0;
      return SUCCESS;
      }
    }

  std::cerr << "Tracker tool template NOT FOUND" << std::endl;
  return FAILURE;
}
 
/** Detach camera . */
MicronTracker::ResultType MicronTracker::InternalClose( void )
{
  igstkLogMacro( DEBUG, "igstk::MicronTracker::InternalClose called ...\n");  
  m_Cameras->Detach();
  
  return SUCCESS;
}

/** Put the tracking device into tracking mode. */
MicronTracker::ResultType MicronTracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::MicronTracker::InternalStartTracking called ...\n");  

  // Report errors, if any, and return SUCCESS or FAILURE
  // (the return value will be used by the superclass to
  //  set the appropriate input to the state machine) 
  return SUCCESS; 
}

/** Take the tracking device out of tracking mode. */
MicronTracker::ResultType MicronTracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::MicronTracker::InternalStopTracking called ...\n");

  // Send the command to stop tracking.

  // Report errors, if any, and return SUCCESS or FAILURE
  // (the return value will be used by the superclass to
  //  set the appropriate input to the state machine) 
  return SUCCESS; 
}

/** Reset the tracking device to put it back to its original state. */
MicronTracker::ResultType MicronTracker::InternalReset( void )
{
  igstkLogMacro( DEBUG, "igstk::MicronTracker::InternalReset called ...\n");
  return SUCCESS;
}

/** Update the status and the transforms for all TrackerTools. */
MicronTracker::ResultType MicronTracker::InternalUpdateStatus()
{
  igstkLogMacro( DEBUG, "igstk::MicronTracker::InternalUpdateStatus called ...\n");

  // This method and the InternalThreadedUpdateStatus are both called
  // continuously in the Tracking state.  This method is called from
  // the main thread, while InternalThreadedUpdateStatus is called
  // from the thread that actually communicates with the device.
  // A shared memory buffer is used to transfer information between
  // the threads, and it must be locked when either thread is
  // accessing it.
  m_BufferLock->Lock();

  typedef TrackerToolTransformContainerType::const_iterator  InputConstIterator;

  InputConstIterator inputItr = m_ToolTransformBuffer.begin();
  InputConstIterator inputEnd = m_ToolTransformBuffer.end();

  TrackerToolsContainerType trackerToolContainer = this->GetTrackerToolContainer();

  unsigned int toolId = 0;

  while( inputItr != inputEnd )
    {
    // only report tools that are in view
    if (! m_ToolStatusContainer[inputItr->first])
      {
      igstkLogMacro( DEBUG, "igstk::MicronTracker::InternalUpdateStatus: " <<
                     "tool " << inputItr->first << " is not in view\n");
      // report to the tracker tool that the tracker is not available 
      this->ReportTrackingToolNotAvailable(trackerToolContainer[inputItr->first]);
      ++inputItr;
      continue;
      }
    // report to the tracker tool that the tracker is Visible 
    this->ReportTrackingToolVisible(trackerToolContainer[inputItr->first]);

    // create the transform
    TransformType transform;

    typedef TransformType::VectorType TranslationType;
    TranslationType translation;

    translation[0] = (inputItr->second)[0];
    translation[1] = (inputItr->second)[1];
    translation[2] = (inputItr->second)[2];

    typedef TransformType::VersorType RotationType;
    RotationType rotation;

    rotation.Set( (inputItr->second)[3],
                   (inputItr->second)[4],
                   (inputItr->second)[5],
                   (inputItr->second)[6]);

    // report error value
    // Get error value from the tracker. TODO
    typedef TransformType::ErrorType  ErrorType;
    ErrorType errorValue = 0.0; 

    transform.SetToIdentity(this->GetValidityTime());
    transform.SetTranslationAndRotation(translation, rotation, errorValue,
                                        this->GetValidityTime());

    // set the raw transform
    this->SetTrackerToolRawTransform( trackerToolContainer[inputItr->first], transform );
    this->SetTrackerToolTransformUpdate( trackerToolContainer[inputItr->first], true );

    ++inputItr;
    ++toolId;
    }

  m_BufferLock->Unlock();

  return SUCCESS;
}

/** Update the shared memory buffer and the transforms.  This function
 *  is called by the thread that communicates with the tracker while
 *  the tracker is in the Tracking state. */
MicronTracker::ResultType MicronTracker::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "igstk::MicronTracker::InternalThreadedUpdateStatus "
                 "called ...\n");

  // Send the commands to the device that will get the transforms
  // for all of the tools.
  // Lock the buffer that this method shares with InternalUpdateStatus
  m_BufferLock->Lock();

  // Grab frame
  if ( ! m_Cameras->grabFrame( m_SelectedCamera ) )
    {
    std::cerr << "Error grabbing a frame" << std::endl;
    m_BufferLock->Unlock();
    return FAILURE;
    }

  // process frame
  unsigned int completionCode = m_Markers->processFrame( m_SelectedCamera ); 

  if ( completionCode != 0 ) 
    {
    std::cerr << "Error in processing frame: " << MicronTracker::GetErrorDescription( completionCode ) << std::endl;
    m_BufferLock->Unlock();
    return FAILURE;
    }

  // process identified markers
  Collection* markersCollection = new Collection(this->m_Markers->identifiedMarkers(this->m_SelectedCamera));
 
  if (markersCollection->count() == 0) 
    {
    delete markersCollection; 
    m_BufferLock->Unlock();
    return FAILURE;
    }

  for (unsigned int markerNum = 1; markerNum <= markersCollection->count(); markerNum++)
    {
    Marker * marker = new Marker(markersCollection->itemI(markerNum));
    if (marker->wasIdentified(this->m_SelectedCamera) )
      {
      //Get postion and pose information 
      Xform3D* Marker2CurrCameraXf = NULL;
      Marker2CurrCameraXf = marker->marker2CameraXf(this->m_SelectedCamera->Handle());

      if(Marker2CurrCameraXf != NULL)
        {
        // Tooltip calibration information which could be available in the 
        // marker template file will not be used here. If needed, the calibration
        // transform should be set to the tracker tool using the 
        // SetCalibrationTransform method in the trackertool and the Tracker
        // base class will computed the composition.

        // Add the translation and rotation to the transform buffer
        std::vector < double > transform;

        double translation[3];
        //the first three are translation
        translation[0] = Marker2CurrCameraXf->getShift(0);
        translation[1] = Marker2CurrCameraXf->getShift(1);
        translation[2] = Marker2CurrCameraXf->getShift(2);

        transform.push_back( translation[0] ); 
        transform.push_back( translation[1] ); 
        transform.push_back( translation[2] ); 

        //the next four are quaternion
        double quaternion[4];
    
        quaternion[0] = -1.0 * Marker2CurrCameraXf->getQuaternion(0);
        quaternion[1] = -1.0 * Marker2CurrCameraXf->getQuaternion(1);
        quaternion[2] = -1.0 * Marker2CurrCameraXf->getQuaternion(2);
        quaternion[3] = Marker2CurrCameraXf->getQuaternion(3);

        transform.push_back( quaternion[0] ); 
        transform.push_back( quaternion[1] ); 
        transform.push_back( quaternion[2] ); 
        transform.push_back( quaternion[3] ); 

        //Check if a Tracker tool is added with this marker type 
        //
        typedef TrackerToolTransformContainerType::iterator InputIterator;
        InputIterator markerItr = m_ToolTransformBuffer.find( marker->getName() );
  
        if( markerItr != m_ToolTransformBuffer.end() )
          {
          m_ToolTransformBuffer[ marker->getName() ] = transform;
          m_ToolStatusContainer[ marker->getName() ] = 1;
          }
        }
      }
    // DO NOT delete marker. This is a possible bug in Marker class.
    // Invoking the marker class destructor causes misidentification of the
    // markers in the subsequent frames. 
    //delete marker;
    }

  delete markersCollection; 

  // unlock the buffer
  m_BufferLock->Unlock();

  return SUCCESS;
}

MicronTracker::ResultType 
MicronTracker::
RemoveTrackerToolFromInternalDataContainers( TrackerToolType * trackerTool ) 
{
 igstkLogMacro( DEBUG, "igstk::MicronTracker::RemoveTrackerToolFromInternalDataContainers "
                 "called ...\n");

  std::string trackerToolIdentifier = trackerTool->GetTrackerToolIdentifier();

  // remove the tool from the Transform buffer container
  this->m_ToolTransformBuffer.erase( trackerToolIdentifier );

  return SUCCESS;
}


/** Print Self function */
void MicronTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Number of tools: " << m_NumberOfTools << std::endl;
}

} // end of namespace igstk
