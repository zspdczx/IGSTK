/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkObjectRepresentation.h" 
#include <vtkMatrix4x4.h>

namespace igstk 
{ 

/** Constructor */
ObjectRepresentation::ObjectRepresentation()
{
  m_Color[0] = 1.0;
  m_Color[1] = 1.0;
  m_Color[2] = 1.0;
  m_Opacity = 1.0;
  m_SpatialObject = NULL;
} 

/** Destructor */
ObjectRepresentation::~ObjectRepresentation()  
{ 
}

/** Add an actor to the actors list */
void
ObjectRepresentation::AddActor( vtkProp3D * actor )
{
  m_Actors.push_back( actor );
}

/** Set the color */
void ObjectRepresentation::SetColor(float r, float g, float b)
{
  if(m_Color[0] == r && m_Color[1] == g && m_Color[2] == b)
    {
    return;
    }
  m_Color[0] = r;
  m_Color[1] = g;
  m_Color[2] = b;
  this->Modified();
}

/** Set the offset of the object */
void ObjectRepresentation::SetOffset(double x, double y, double z)
{
  SpatialObjectType::VectorType offset;
  offset[0] = x;
  offset[1] = y;
  offset[2] = z;
  m_SpatialObject->GetObjectToParentTransform()->SetOffset(offset);
  m_SpatialObject->ComputeObjectToWorldTransform();
}

/** Check if the SpatialObject has been modified, if yes call an UpdateActors */
void ObjectRepresentation::Update()
{
  itk::Matrix<double,3,3> itkMatrix = m_SpatialObject->GetObjectToWorldTransform()->GetMatrix();
  itk::Vector<double,3>   offset = m_SpatialObject->GetObjectToWorldTransform()->GetOffset();
 
  vtkMatrix4x4* vtkMatrix = vtkMatrix4x4::New();
  for(unsigned int i=0;i<3;i++)
  {
    for(unsigned int j=0;j<3;j++)
    {
      vtkMatrix->SetElement(i,j,itkMatrix.GetVnlMatrix().get(i,j));   
    }

    vtkMatrix->SetElement(i,3,offset[i]/100);
  }

  // Update all the actors
  ActorsListType::iterator it = m_Actors.begin();
  while(it != m_Actors.end())
  {  
    (*it)->SetUserMatrix(vtkMatrix);
    it++;
  }
}


/** Print Self function */
void ObjectRepresentation::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Color: " << m_Color[0] << " : " << m_Color[1] << " : " << m_Color[2] << std::endl;
  os << indent << "Opacity: " << m_Opacity << std::endl;
}

} // end namespace igstk

