/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCTImageSpatialObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkCTImageSpatialObjectRepresentation_h
#define __igstkCTImageSpatialObjectRepresentation_h

#include "igstkImageSpatialObjectRepresentation.h"
#include "igstkCTImageSpatialObject.h"
#include "igstkStateMachine.h"

namespace igstk
{

/** \class CTImageSpatialObjectRepresentation
 *  \brief This class implements a spatial object representation for CT images.
 *
 * \ingroup Spatial object representation
 */

class CTImageSpatialObjectRepresentation : 
         public ImageSpatialObjectRepresentation< CTImageSpatialObject >
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( CTImageSpatialObjectRepresentation, \
                    ImageSpatialObjectRepresentation< CTImageSpatialObject > );

protected:

  CTImageSpatialObjectRepresentation( void );
  virtual ~CTImageSpatialObjectRepresentation( void ) {};

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

};

} // end namespace igstk

#endif // __igstkCTImageSpatialObjectRepresentation_h

