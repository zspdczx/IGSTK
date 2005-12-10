/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCommunication.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkCommunication.h"
#include "igstkMacros.h"

namespace igstk
{

Communication::Communication( void ):m_StateMachine(this)
{
  m_TimeoutPeriod = 5000;
  m_ReadTerminationCharacter = 255;
  m_UseReadTerminationCharacter = false;
}

Communication::~Communication( void )
{
}

Communication::ResultType Communication::OpenCommunication( void )
{ 
  igstkLogMacro( FATAL, " Communication::OpenCommunication() "
            << " method is not implemented yet.\n" );

  return SUCCESS;
}

Communication::ResultType Communication::CloseCommunication( void )
{
  igstkLogMacro( FATAL, " Communication::CloseCommunication() "
            << " method is not implemented yet.\n" );
 
 return SUCCESS;
}

void Communication::PrintSelf(std::ostream &os, itk::Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  
  os << indent << "TimeoutPeriod: " << m_TimeoutPeriod 
     << std::endl;
  os << indent << "ReadTerminationCharacter: " << m_ReadTerminationCharacter 
     << std::endl;
  os << indent << "UseReadTerminationCharacter: "
     << m_UseReadTerminationCharacter 
     << std::endl;
}


}


