/*=========================================================================
  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoImagerConfiguration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/

#include "igstkVideoImagerConfiguration.h"

namespace igstk
{


VideoImagerConfiguration::VideoImagerConfiguration()//: m_ReferenceTool( NULL )
{
}


VideoImagerConfiguration::~VideoImagerConfiguration()
{
  std::vector< VideoImagerToolConfiguration * >::iterator it;
  std::vector< VideoImagerToolConfiguration * >::iterator endIt =
    this->m_VideoImagerToolList.end();

  for( it=this->m_VideoImagerToolList.begin(); it!=endIt; it++ )
  {
    delete (*it);
  }
  this->m_VideoImagerToolList.clear();
}


void
VideoImagerConfiguration::RequestSetFrequency( double frequency )
{
  if( frequency<=0 || frequency>GetMaximalRefreshRate() )
  {
    std::ostringstream msg;
    msg.str("");
    msg<<"Invalid VideoImager frequency specified.";
    msg<<"Valid values are in [0,"<<GetMaximalRefreshRate()<<"].";

    FrequencySetFailureEvent evt;
    evt.Set( msg.str() );
    this->InvokeEvent( evt );
  }
  else
  {
    this->m_Frequency = frequency;
    this->InvokeEvent( FrequencySetSuccessEvent() );
  }
}

void
VideoImagerConfiguration::RequestAddTool( const VideoImagerToolConfiguration *tool )
{
  InternalAddTool( tool );
}

VideoImagerToolConfiguration::VideoImagerToolConfiguration()
{

}

VideoImagerToolConfiguration::VideoImagerToolConfiguration(const
  VideoImagerToolConfiguration &other)
{
  this->m_ToolUniqueIdentifier = other.m_ToolUniqueIdentifier;
  this->m_FrameDimensions[0] = other.m_FrameDimensions[0];
  this->m_FrameDimensions[1] = other.m_FrameDimensions[1];
  this->m_FrameDimensions[2] = other.m_FrameDimensions[2];
  this->m_PixelDepth = other.m_PixelDepth;
}

VideoImagerToolConfiguration::~VideoImagerToolConfiguration()
{
}

void
VideoImagerToolConfiguration::GetFrameDimensions(unsigned int* dims) const
{
  dims[0] = m_FrameDimensions[0];
  dims[1] = m_FrameDimensions[1];
  dims[2] = m_FrameDimensions[2];
}

void
VideoImagerToolConfiguration::SetFrameDimensions(unsigned int* dims)
{
  this->m_FrameDimensions[0] = dims[0];
  this->m_FrameDimensions[1] = dims[1];
  this->m_FrameDimensions[2] = dims[2];
}

void
VideoImagerToolConfiguration::SetPixelDepth (unsigned int value)
{
  m_PixelDepth = value;
}

unsigned int
VideoImagerToolConfiguration::GetPixelDepth ( void ) const
{
  return this->m_PixelDepth;
}

SocketCommunicatingVideoImagerConfiguration::SocketCommunicatingVideoImagerConfiguration()
{
  m_SocketPort = 0;
}

SocketCommunicatingVideoImagerConfiguration::
                                       ~SocketCommunicatingVideoImagerConfiguration()
{
}

SerialCommunicatingVideoImagerConfiguration::SerialCommunicatingVideoImagerConfiguration()
{
  igstk::SerialCommunication::PortNumberType DEFAULT_COM_PORT =
    igstk::SerialCommunication::PortNumber0;
  igstk::SerialCommunication::BaudRateType DEFAULT_BAUD_RATE =
    igstk::SerialCommunication::BaudRate115200;
  igstk::SerialCommunication::DataBitsType DEFAULT_DATA_BITS =
    igstk::SerialCommunication::DataBits8;
  igstk::SerialCommunication::ParityType DEFAULT_PARITY =
    igstk::SerialCommunication::NoParity;
  igstk::SerialCommunication::StopBitsType DEFAULT_STOP_BITS =
    igstk::SerialCommunication::StopBits1;
  igstk::SerialCommunication::HandshakeType DEFAULT_HANDSHAKE =
    igstk::SerialCommunication::HandshakeOff;

  m_COMPort   = DEFAULT_COM_PORT;
  m_BaudRate = DEFAULT_BAUD_RATE;
  m_DataBits = DEFAULT_DATA_BITS;
  m_Parity = DEFAULT_PARITY;
  m_StopBits = DEFAULT_STOP_BITS;
  m_Handshake = DEFAULT_HANDSHAKE;
}


SerialCommunicatingVideoImagerConfiguration::
                                       ~SerialCommunicatingVideoImagerConfiguration()
{
}

void
SerialCommunicatingVideoImagerConfiguration::RequestSetCOMPort(
  igstk::SerialCommunication::PortNumberType portNumber)
{
  switch( portNumber )
  {
    case igstk::SerialCommunication::PortNumber0:
    case igstk::SerialCommunication::PortNumber1:
    case igstk::SerialCommunication::PortNumber2:
    case igstk::SerialCommunication::PortNumber3:
    case igstk::SerialCommunication::PortNumber4:
    case igstk::SerialCommunication::PortNumber5:
    case igstk::SerialCommunication::PortNumber6:
    case igstk::SerialCommunication::PortNumber7:
      this->m_COMPort = portNumber;
      this->InvokeEvent( ComPortSetEvent() );
      break;
    default:
      ComPortSetErrorEvent errorEvt;
      std::ostringstream msg;
      msg<<"Invalid COM port ("<<portNumber<<"). Supported ports are ";
      msg<<"in the range ["<<igstk::SerialCommunication::PortNumber0<<",";
      msg<<igstk::SerialCommunication::PortNumber7<<"].";
      errorEvt.Set( msg.str() );
      this->InvokeEvent( errorEvt );
  }
}


void
SerialCommunicatingVideoImagerConfiguration::RequestSetBaudRate(
  igstk::SerialCommunication::BaudRateType baudRate)
{
  switch( baudRate )
  {
    case igstk::SerialCommunication::BaudRate9600:
    case igstk::SerialCommunication::BaudRate19200:
    case igstk::SerialCommunication::BaudRate38400:
    case igstk::SerialCommunication::BaudRate57600:
    case igstk::SerialCommunication::BaudRate115200:
      this->m_BaudRate = baudRate;
      this->InvokeEvent( BaudRateSetEvent() );
      break;
    default:
      BaudRateSetErrorEvent errorEvt;
      std::ostringstream msg;
      msg<<"Invalid baud rate ("<<baudRate<<")";
      errorEvt.Set( msg.str() );
      this->InvokeEvent( errorEvt );
  }
}


void
SerialCommunicatingVideoImagerConfiguration::RequestSetDataBits(
  igstk::SerialCommunication::DataBitsType dataBits)
{
  switch( dataBits )
    {
    case igstk::SerialCommunication::DataBits7:
    case igstk::SerialCommunication::DataBits8:
      this->m_DataBits = dataBits;
      this->InvokeEvent( DataBitsSetEvent() );
      break;
    default:
      DataBitsSetErrorEvent errorEvt;
      std::ostringstream msg;
      msg<<"Invalid data bits value ("<<dataBits<<")";
      errorEvt.Set( msg.str() );
      this->InvokeEvent( errorEvt );
    }
}

void
SerialCommunicatingVideoImagerConfiguration::RequestSetParity(
  igstk::SerialCommunication::ParityType parity)
{
  switch( parity )
    {
    case igstk::SerialCommunication::NoParity:
    case igstk::SerialCommunication::OddParity:
    case igstk::SerialCommunication::EvenParity:
      this->m_Parity = parity;
      this->InvokeEvent( ParitySetEvent() );
    default:
      ParitySetErrorEvent errorEvt;
      std::ostringstream msg;
      msg<<"Invalid parity value ("<<parity<<")";
      errorEvt.Set( msg.str() );
      this->InvokeEvent( errorEvt );
    }
}

void
SerialCommunicatingVideoImagerConfiguration::RequestSetStopBits(
  igstk::SerialCommunication::StopBitsType stopBits)
{
  switch( stopBits )
    {
    case igstk::SerialCommunication::StopBits1:
    case igstk::SerialCommunication::StopBits2:
      this->m_StopBits = stopBits;
      this->InvokeEvent( StopBitsSetEvent() );
    default:
      StopBitsSetErrorEvent errorEvt;
      std::ostringstream msg;
      msg<<"Invalid stop bits value ("<<stopBits<<")";
      errorEvt.Set( msg.str() );
      this->InvokeEvent( errorEvt );
    }
}


void
SerialCommunicatingVideoImagerConfiguration::RequestSetHandshake(
  igstk::SerialCommunication::HandshakeType handShake)
{
  switch( handShake )
    {
    case igstk::SerialCommunication::HandshakeOff:
    case igstk::SerialCommunication::HandshakeOn:
      this->m_Handshake = handShake;
      this->InvokeEvent( HandshakeSetEvent() );
    default:
      HandshakeSetErrorEvent errorEvt;
      std::ostringstream msg;
      msg<<"Invalid handshake value ("<<handShake<<")";
      errorEvt.Set( msg.str() );
      this->InvokeEvent( errorEvt );
    }
}

} // end of name space
