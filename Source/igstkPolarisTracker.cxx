/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisTracker.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more DEBUGrmation.

=========================================================================*/

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters in the
// debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkPolarisTracker.h"
#include "igstkTrackerPort.h"

#include <iostream>
#include <fstream>

namespace igstk
{

/** Constructor: Initializes all internal variables. */
PolarisTracker::PolarisTracker(void)
{
  m_CommandInterpreter = CommandInterpreterType::New();
  m_NumberOfTools = 0;
  for (unsigned int i = 0; i < NumberOfPorts; i++)
    {
    this->m_PortEnabled[i] = 0;
    this->m_PortHandle[i] = 0;
    }

  this->SetThreadingEnabled( true );

  m_BufferLock = itk::MutexLock::New();
}

/** Destructor */
PolarisTracker::~PolarisTracker(void)
{
}

/** Set the communication object, it will be initialized as necessary
  * for use with the Polaris */
void PolarisTracker::SetCommunication( CommunicationType *communication )
{
  igstkLogMacro( DEBUG, "PolarisTracker:: Entered SetCommunication ...\n");
  m_Communication = communication;
  m_CommandInterpreter->SetCommunication( communication );

  // data records are of variable length and end with a carriage return
  if( communication )
    {
    communication->SetUseReadTerminationCharacter( true );
    communication->SetReadTerminationCharacter( '\r' );
    }

  igstkLogMacro( DEBUG, "PolarisTracker:: Exiting SetCommunication ...\n"); 
}

/** Open communication with the tracking device. */
PolarisTracker::ResultType PolarisTracker::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "PolarisTracker::InternalOpen called ...\n");

  // Initialize the device 
  m_CommandInterpreter->INIT();

  // Reset and try again if error
  if (m_CommandInterpreter->GetError())
    {
    m_CommandInterpreter->RESET();
    m_CommandInterpreter->INIT();
    }

  // If it still failed to initialize, fail
  const int errnum = m_CommandInterpreter->GetError();
  if (errnum)
    {
    igstkLogMacro( DEBUG, "PolarisTracker::InternalOpen: Error ...\n");
    igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
    return FAILURE;
    }

  return SUCCESS;
}

/** Close communication with the tracking device. */
PolarisTracker::ResultType PolarisTracker::InternalClose( void )
{
  // return the device back to its initial comm setttings
  m_CommandInterpreter->COMM(CommandInterpreterType::NDI_9600,
                             CommandInterpreterType::NDI_8N1,
                             CommandInterpreterType::NDI_NOHANDSHAKE);
  const int errnum = m_CommandInterpreter->GetError();
  if (errnum) 
    {
    igstkLogMacro( DEBUG, "PolarisTracker::InternalClose: Error ...\n");
    igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
    return FAILURE;
    }

  return SUCCESS;
}

/** Activate the tools attached to the tracking device. */
PolarisTracker::ResultType PolarisTracker::InternalActivateTools( void )
{
  igstkLogMacro( DEBUG, "PolarisTracker::InternalActivateTools called ...\n");

  // load any SROMS that are needed
  for (unsigned int i = 0; i < NumberOfPorts; i++)
    { 
    if (!m_SROMFileNames[i].empty())
      {
      this->LoadVirtualSROM(i, m_SROMFileNames[i]);
      }
    }

  this->EnableToolPorts();

  this->ClearPorts();

  m_NumberOfTools = 0;

  for(unsigned int i = 0; i < NumberOfPorts; i++)
    { 
    if( this->m_PortEnabled[i] )
      {
      PolarisTrackerToolPointer tool = PolarisTrackerToolType::New();
      TrackerPortPointer port = TrackerPortType::New();
      port->AddTool(tool);
      this->AddPort(port);
      m_NumberOfTools++;
      }
    }

  return SUCCESS;
}

/** Deactivate the tools attached to the tracking device. */
PolarisTracker::ResultType PolarisTracker::InternalDeactivateTools( void )
{
  for (unsigned int i = 0; i < NumberOfPorts; i++)
    { 
    if (!m_SROMFileNames[i].empty())
      {
      this->ClearVirtualSROM(i);
      }
    }

  this->DisableToolPorts();

  return SUCCESS;
}

/** Put the tracking device into tracking mode. */
PolarisTracker::ResultType PolarisTracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG, "PolarisTracker::InternalStartTracking called ...\n");  

  m_CommandInterpreter->TSTART();

  const int errnum = m_CommandInterpreter->GetError();
  if (errnum) 
    {
    igstkLogMacro( DEBUG, "PolarisTracker::LoadVirtualSROM: Error ...\n");
    igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
    return FAILURE;
    }

  return SUCCESS;
}

/** Take the tracking device out of tracking mode. */
PolarisTracker::ResultType PolarisTracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG, "PolarisTracker::InternalStopTracking called ...\n");

  m_CommandInterpreter->TSTOP();
  const int errnum = m_CommandInterpreter->GetError();
  if (errnum) 
    {
    igstkLogMacro( DEBUG, "PolarisTracker::LoadVirtualSROM: Error ...\n");
    igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
    return FAILURE;
    }

  return SUCCESS;
}

/** Reset the tracking device to put it back to its original state. */
PolarisTracker::ResultType PolarisTracker::InternalReset( void )
{
  m_CommandInterpreter->RESET();
  m_CommandInterpreter->INIT();

  // If it still failed to initialize, fail
  if ( m_CommandInterpreter->GetError() )
    {
    return FAILURE;
    }

  return SUCCESS;
}


/** Update the status and the transforms for all TrackerTools. */
PolarisTracker::ResultType PolarisTracker::InternalUpdateStatus()
{
  igstkLogMacro( DEBUG, "PolarisTracker::InternalUpdateStatus called ...\n");

  // these flags are set for tools that can be used for tracking
  const unsigned long mflags = (CommandInterpreterType::NDI_TOOL_IN_PORT |
                                CommandInterpreterType::NDI_INITIALIZED |
                                CommandInterpreterType::NDI_ENABLED);

  m_BufferLock->Lock();

  for (unsigned int port = 0; port < NumberOfPorts; port++) 
    {
    // convert m_StatusBuffer flags from NDI to vtkTracker format
    const int portStatus = m_StatusBuffer[port];

    // only report tools that are enabled
    if ((portStatus & mflags) != mflags) 
      {
      continue;
      }

    // only report tools that are in view
    if (m_AbsentBuffer[port] ||
        portStatus & CommandInterpreterType::NDI_OUT_OF_VOLUME)
      {
      // there should be a method to set that the tool is not in view
      continue;
      }

    // create the transform
    TransformType transform;

    typedef TransformType::VectorType TranslationType;
    TranslationType translation;

    translation[0] = m_TransformBuffer[port][4];
    translation[1] = m_TransformBuffer[port][5];
    translation[2] = m_TransformBuffer[port][6];

    typedef TransformType::VersorType RotationType;
    RotationType rotation;
    double normsquared = 
      m_TransformBuffer[port][0]*m_TransformBuffer[port][0] +
      m_TransformBuffer[port][1]*m_TransformBuffer[port][1] +
      m_TransformBuffer[port][2]*m_TransformBuffer[port][2] +
      m_TransformBuffer[port][3]*m_TransformBuffer[port][3];

    // don't allow null quaternions
    if (normsquared < 1e-6)
      {
      rotation.Set(1.0, 0.0, 0.0, 0.0);
      igstkLogMacro( WARNING, "PolarisTracker::InternUpdateStatus: bad "
                     "quaternion, norm=" << sqrt(normsquared) << "\n");
      }
    else
      {
      rotation.Set(m_TransformBuffer[port][0],
                   m_TransformBuffer[port][1],
                   m_TransformBuffer[port][2],
                   m_TransformBuffer[port][3]);
      }

    // report NDI error value
    typedef TransformType::ErrorType  ErrorType;
    ErrorType errorValue = m_TransformBuffer[port][7];

    typedef TransformType::TimePeriodType TimePeriodType;
    const TimePeriodType validityTime = 100.0;

    transform.SetToIdentity(validityTime);
    transform.SetTranslationAndRotation(translation, rotation, errorValue,
                                        validityTime);

    this->SetToolTransform(port, 0, transform);
    }
  m_BufferLock->Unlock();

  return SUCCESS;
}

/** Update the m_StatusBuffer and the transforms. 
    This function is called by a separate thread. */
PolarisTracker::ResultType PolarisTracker::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "PolarisTracker::InternalThreadedUpdateStatus "
                 "called ...\n");

  unsigned long frame[ NumberOfPorts ];

  // Initialize transformations to identity.
  // The NDI transform is 8 values:
  // the first 4 values are a quaternion
  // the next 3 values are an x,y,z position
  // the final value is an error estimate in the range [0,1]
  m_BufferLock->Lock();
  for (unsigned int port = 0; port < NumberOfPorts; port++)
    {
    m_TransformBuffer[port][0] = 1.0;
    m_TransformBuffer[port][1] = 0.0;
    m_TransformBuffer[port][2] = 0.0;
    m_TransformBuffer[port][3] = 0.0;
    m_TransformBuffer[port][4] = 0.0;
    m_TransformBuffer[port][5] = 0.0;
    m_TransformBuffer[port][6] = 0.0;
    m_TransformBuffer[port][7] = 0.0;
    }
  m_BufferLock->Unlock();

  // get the transforms for all tools from the NDI
  m_CommandInterpreter->TX(CommandInterpreterType::NDI_XFORMS_AND_STATUS);
  const int errnum = m_CommandInterpreter->GetError();

  if ( errnum )
    {
    if (errnum == CommandInterpreterType::NDI_BAD_CRC ||
        errnum == CommandInterpreterType::NDI_TIMEOUT) // common errors
      {
      igstkLogMacro( WARNING, m_CommandInterpreter->ErrorString(errnum)<<"\n");
      }
    else
      {
      igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum)<<"\n");
      }
    return FAILURE;
    }

  m_BufferLock->Lock();
  for (unsigned int port = 0; port < NumberOfPorts; port++)
    { 
    // port handle reported by device
    const int ph = this->m_PortHandle[port];
    m_AbsentBuffer[port] = 0;
    m_StatusBuffer[port] = 0;

    frame[port] = 0;
    if (ph == 0)
      {
      continue;
      }

    double transform[8];
    const int result = m_CommandInterpreter->GetTXTransform(ph, transform);
    const int absent = (result != CommandInterpreterType::NDI_VALID);
    const int status = m_CommandInterpreter->GetTXPortStatus(ph);
    frame[port] = m_CommandInterpreter->GetTXFrame(ph);

    for( unsigned int i = 0; i < 8; ++i )
      {
      m_TransformBuffer[port][i] = transform[i];
      }

    m_AbsentBuffer[port] = absent;
    m_StatusBuffer[port] = status;
  }
  m_BufferLock->Unlock();

  // In the original vtkNDITracker code, there was a check at this
  // point in the code to see if any new tools had been plugged in

  return SUCCESS;
}

/** Specify an SROM file to be used with a passive or custom tool. */
void PolarisTracker::AttachSROMFileNameToPort( const unsigned int portNum,
                                               std::string fileName )
{
  // the first 3 ports are active, don't allow SROMS for them
  if ( (portNum >= 3) && (portNum <= NumberOfPorts) )
    {
    m_SROMFileNames[portNum] = fileName;
    }
}

/** Load a virtual SROM, given the file name of the ROM file */
bool PolarisTracker::LoadVirtualSROM( const unsigned int tool,
                                      const std::string SROMFileName) 
{
  std::ifstream sromFile;
  sromFile.open(SROMFileName.c_str(), std::ios::binary );

  if (!sromFile.is_open())
    {
    igstkLogMacro( DEBUG, "PolarisTracker::LoadVirtualSROM: couldn't "
                   "find SROM file " << SROMFileName << " ...\n");
    return false;
    }

  // most SROM files don't contain the whole 1024 bytes, they only
  // contain whatever is necessary, so the rest should be filled with zero
  char data[1024]; 
  memset( data, 0, 1024 );
  sromFile.read( data, 1024 );
  sromFile.close();

  // passive tools are identified by the letters "A", "B", "C", etc,
  // and correspond to IGSTK port numbers 3 to 11
  char portIdentifier[3] = "00";
  portIdentifier[1] = tool - 3 + 'A';

  m_CommandInterpreter->PHRQ("********", // device number
                             "*",        // TIU or SCU
                             "*",        // wired or wireless
                             portIdentifier,      // port
                             "**");      // channel
 
  const int ph = m_CommandInterpreter->GetPHRQHandle();

  const int errnum = m_CommandInterpreter->GetError();
  if (errnum)
    {
    igstkLogMacro( DEBUG, "PolarisTracker::LoadVirtualSROM: Error ...\n");
    igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");

    return false;
    }

  for ( unsigned int i = 0; i < 1024; i += 64)
    {
    // holds hexidecimal data to be sent to device
    char hexbuffer[129]; 
    
    // convert data to hexidecimal and write to virtual SROM in
    // 64-byte chunks
    m_CommandInterpreter->HexEncode(hexbuffer, &data[i], 64);
    m_CommandInterpreter->PVWR(ph, i, hexbuffer);
    }

  return true;
}

/** Clear a previously loaded SROM. */
void PolarisTracker::ClearVirtualSROM(const unsigned int tool)
{
  const int ph = this->m_PortHandle[tool];
  m_CommandInterpreter->PHF(ph);
  this->m_PortEnabled[tool] = 0;
  this->m_PortHandle[tool] = 0;
}

/** Enable all tool ports that are occupied. */
void PolarisTracker::EnableToolPorts()
{
  // reset our information about the tool ports
  for (unsigned int itool = 0; itool < NumberOfPorts; itool++)
    {
    this->m_PortHandle[itool] = 0;
    this->m_PortEnabled[itool] = 0;
    }

  // free ports that are waiting to be freed
  m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_STALE_HANDLES);
  const unsigned int ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();
  for (unsigned int tool = 0; tool < ntools; tool++)
    {
    const int ph = m_CommandInterpreter->GetPHSRHandle(tool);
    m_CommandInterpreter->PHF(ph);
    const int errnum = m_CommandInterpreter->GetError();
    if (errnum)
      {
      igstkLogMacro( DEBUG, "PolarisTracker::LoadVirtualSROM: Error ...\n");
      igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
      }
    }

  // initialize ports waiting to be initialized
  do // repeat as necessary (in case multi-channel tools are used) 
    {
    m_CommandInterpreter->PHSR(
      CommandInterpreterType::NDI_UNINITIALIZED_HANDLES);
    
    const unsigned int ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();
    for (unsigned int tool = 0; tool < ntools; tool++)
      {
      const int ph = m_CommandInterpreter->GetPHSRHandle(tool);
      m_CommandInterpreter->PINIT(ph);
      const int errnum = m_CommandInterpreter->GetError();
      if (errnum)
        { 
        igstkLogMacro( DEBUG, "PolarisTracker::LoadVirtualSROM: Error ...\n");
        igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum)<<"\n");
        break;
        }
      }
    }
  while( ntools > 0 );

  // enable initialized tools
  m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_UNENABLED_HANDLES);

  const unsigned int nunenabledtools = 
    m_CommandInterpreter->GetPHSRNumberOfHandles();
  
  for (unsigned int tool = 0; tool < nunenabledtools; tool++)
    {
    const int ph = m_CommandInterpreter->GetPHSRHandle(tool);
    m_CommandInterpreter->PHINF(ph,
                                CommandInterpreterType::NDI_BASIC);

    // tool identity and type information
    char identity[512];
    m_CommandInterpreter->GetPHINFToolInfo(identity);

    // use tool type information to figure out mode for enabling
    int mode = CommandInterpreterType::NDI_DYNAMIC;

    if (identity[1] == CommandInterpreterType::NDI_TYPE_BUTTON)
      { // button-box or foot pedal
      mode = CommandInterpreterType::NDI_BUTTON_BOX;
      }
    else if (identity[1] == CommandInterpreterType::NDI_TYPE_REFERENCE)
      { // reference
      mode = CommandInterpreterType::NDI_STATIC;
      }

    // enable the tool
    m_CommandInterpreter->PENA(ph, mode);
    const int errnum = m_CommandInterpreter->GetError();
    if (errnum)
      {
      igstkLogMacro( DEBUG, "PolarisTracker::LoadVirtualSROM: Error ...\n");
      igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
      }
    }

  // get information for all tools
  m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_ALL_HANDLES);

  const unsigned int nalltools = 
    m_CommandInterpreter->GetPHSRNumberOfHandles();

  for (unsigned int tool = 0; tool < nalltools; tool++)
    {
    const int ph = m_CommandInterpreter->GetPHSRHandle(tool);
    m_CommandInterpreter->PHINF(ph,
                                CommandInterpreterType::NDI_PORT_LOCATION |
                                CommandInterpreterType::NDI_PART_NUMBER |
                                CommandInterpreterType::NDI_BASIC );
    const int errnum = m_CommandInterpreter->GetError();
    if (errnum)
      { 
      igstkLogMacro( DEBUG, "PolarisTracker::LoadVirtualSROM: Error ...\n");
      igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
      continue;
      }    

    // get the physical port identifier
    char location[512];
    m_CommandInterpreter->GetPHINFPortLocation(location);

    unsigned int port = 0;
    if (location[11] >= 'A')
      {
      port = location[11] - 'A' + 3;
      }
    else
      {
      port = (location[10]-'0')*10 + (location[11]-'0') - 1;
      }

    if (port < NumberOfPorts)
      {
      this->m_PortHandle[port] = ph;
      }

    const int status = m_CommandInterpreter->GetPHINFPortStatus();

    this->m_PortEnabled[port] = 
      ((status & CommandInterpreterType::NDI_ENABLED) != 0);
    }
}

/** Disable all tool ports. */
void PolarisTracker::DisableToolPorts( void )
{
  // disable all enabled tools
  m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_ENABLED_HANDLES);
  unsigned int ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();

  for (unsigned int tool = 0; tool < ntools; tool++)
    {
    const int ph = m_CommandInterpreter->GetPHSRHandle(tool);
    m_CommandInterpreter->PDIS( ph );

    const int errnum = m_CommandInterpreter->GetError();
    if (errnum)
      { 
      igstkLogMacro( DEBUG, "PolarisTracker::LoadVirtualSROM: Error ...\n");
      igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
      }    
    }

  // disable the enabled ports
  for (unsigned int itool = 0; itool < NumberOfPorts; itool++)
    {
    this->m_PortEnabled[itool] = 0;
    }
}


/** Print Self function */
void PolarisTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  unsigned int i;

  for( i = 0; i < NumberOfPorts; ++i )
    {
    os << indent << "Port " << i << " Enabled: " << m_PortEnabled[i] 
       << std::endl;
    }
  for( i = 0; i < NumberOfPorts; ++i )
    {
    os << indent << "Port " << i << " Handle: " << m_PortHandle[i] 
       << std::endl;
    }
  os << indent << "Number of tools: " << m_NumberOfTools << std::endl;
  for( i = 0; i < NumberOfPorts; ++i )
    {
    os << indent << "SROM filename " << i << " : " << m_SROMFileNames[i]
       << std::endl;
    }
}


} // end of namespace igstk

