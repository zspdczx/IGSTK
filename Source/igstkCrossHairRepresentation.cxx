/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCrossHairRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkCrossHairRepresentation.h"
#include "igstkEvents.h"

#include <vtkActor.h>
#include <vtkAxisActor2D.h>
#include <vtkProperty.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkTubeFilter.h>



namespace igstk
{ 

/** Constructor */
CrossHairRepresentation::CrossHairRepresentation():m_StateMachine(this)
{

  m_LineSourceZ = NULL;
  m_LineSourceY = NULL;
  m_LineSourceX = NULL;

  m_ImageBounds[0] = 0;
  m_ImageBounds[1] = 0;
  m_ImageBounds[2] = 0;
  m_ImageBounds[3] = 0;
  m_ImageBounds[4] = 0;
  m_ImageBounds[5] = 0;

  m_CrossHairPositionObserver = CrossHairPositionObserver::New();

  m_CrossHairSpatialObject = NULL;
  this->RequestSetSpatialObject( m_CrossHairSpatialObject );

  //List of states
  igstkAddStateMacro( NullCrossHairObject  );
  igstkAddStateMacro( ValidCrossHairObject );

  // List of state machine inputs
  igstkAddInputMacro( ValidCrossHairObject );
  igstkAddInputMacro( NullCrossHairObject  );

  //from NullCrossHairObject

  igstkAddTransitionMacro( NullCrossHairObject, ValidCrossHairObject, 
                           ValidCrossHairObject,  SetCrossHairObject );

  igstkAddTransitionMacro( NullCrossHairObject, NullCrossHairObject, 
                           NullCrossHairObject,  No );

  //from ValidCrossHairObject

  igstkAddTransitionMacro( ValidCrossHairObject, NullCrossHairObject, 
                           NullCrossHairObject,  No ); 

  igstkAddTransitionMacro( ValidCrossHairObject, ValidCrossHairObject, 
                           ValidCrossHairObject,  No );

  m_StateMachine.SelectInitialState( m_NullCrossHairObjectState );

  m_StateMachine.SetReadyToRun();
} 

/** Destructor */
CrossHairRepresentation
::~CrossHairRepresentation()  
{
  this->DeleteActors();

  if (m_LineSourceZ != NULL)
  {
    m_LineSourceZ->Delete();
    m_LineSourceZ=NULL;
  }

  if (m_LineSourceY != NULL)
  {
    m_LineSourceY->Delete();
    m_LineSourceY=NULL;
  }

  if (m_LineSourceX != NULL)
  {
    m_LineSourceX->Delete();
    m_LineSourceX=NULL;
  }

}

/** Set the actor�s visibility */
void 
CrossHairRepresentation
::SetVisibility(bool visibility)
{
  // Update all the actors
  ActorsListType::iterator it = this->m_Actors.begin();
  while(it != this->m_Actors.end())
    {
    vtkActor * va = dynamic_cast< vtkActor * >( *it );
    if( va )
      {
      va->SetVisibility(visibility);
      }
    it++;
    }
}

/** Verify time stamp of the attached tool*/
bool
CrossHairRepresentation
::VerifyTimeStamp( ) const
{
  igstkLogMacro( DEBUG, 
    "igstk::CrossHairRepresentation::VerifyTimeStamp called...\n");

  if( m_CrossHairSpatialObject.IsNull() )
    {
    return false;
    }

  // if a tool spatial object is driving the reslicing, compare the 
  //   tool spatial object transform with the view render time
  if( m_CrossHairSpatialObject->IsToolSpatialObjectSet() )
    {
      if( this->GetRenderTimeStamp().GetExpirationTime() <
        this->m_CrossHairSpatialObject->GetToolTransform().GetStartTime() ||
        this->GetRenderTimeStamp().GetStartTime() >
        this->m_CrossHairSpatialObject->GetToolTransform().GetExpirationTime() )
        {
          // fixme
          double diff = 
            this->GetRenderTimeStamp().GetStartTime() - this->m_CrossHairSpatialObject->GetToolTransform().GetExpirationTime();

          if (diff > 250 )
          {
            //std::cout << diff << std::endl;
            return false;
          }
          else
            return true;
        }
      else
        {
        return true;
        }
    }
  else
    {
    return true;
    }
}

/** Request to Set the CrossHairSpatial Object */
void CrossHairRepresentation
::RequestSetCrossHairObject( const CrossHairSpatialObjectType * CrossHair )
{
  m_CrossHairSpatialObjectToAdd = const_cast< CrossHairSpatialObjectType*> ( CrossHair );

  if( !m_CrossHairSpatialObjectToAdd )
    {
    m_StateMachine.PushInput( m_NullCrossHairObjectInput );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    m_StateMachine.PushInput( m_ValidCrossHairObjectInput );
    m_StateMachine.ProcessInputs();
    }
}

/** Set the CrossHair Spatial Object */
void CrossHairRepresentation
::NoProcessing()
{
}

/** Set the CrossHair Spatial Object */
void CrossHairRepresentation
::SetCrossHairObjectProcessing()
{
  m_CrossHairSpatialObject = m_CrossHairSpatialObjectToAdd;

  m_CrossHairSpatialObject->AddObserver( PointEvent(), m_CrossHairPositionObserver );

  this->RequestSetSpatialObject( m_CrossHairSpatialObject );

  //get image bounds (use a payloaded event instead)
  
  m_ImageBounds[0] = m_CrossHairSpatialObject->GetBoundingBoxDimensionByIndex(0);
  m_ImageBounds[1] = m_CrossHairSpatialObject->GetBoundingBoxDimensionByIndex(1);
  m_ImageBounds[2] = m_CrossHairSpatialObject->GetBoundingBoxDimensionByIndex(2);
  m_ImageBounds[3] = m_CrossHairSpatialObject->GetBoundingBoxDimensionByIndex(3);
  m_ImageBounds[4] = m_CrossHairSpatialObject->GetBoundingBoxDimensionByIndex(4);
  m_ImageBounds[5] = m_CrossHairSpatialObject->GetBoundingBoxDimensionByIndex(5);

} 

void CrossHairRepresentation
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

/** Update the visual representation in response to changes in the geometric
 * object */
void CrossHairRepresentation
::UpdateRepresentationProcessing()
{
  igstkLogMacro( DEBUG, "CrossHairRepresentation::UpdateRepresentationProcessing called ....\n");

  m_CrossHairPositionObserver->Reset();

  m_CrossHairSpatialObject->RequestGetCrossHairPosition();
  
  if( m_CrossHairPositionObserver->GotCrossHairPosition() )
    {
      const PointType& position = m_CrossHairPositionObserver->GetCrossHairPosition();

      m_LineSourceY->SetPoint1( position[0]+0.1, m_ImageBounds[2], position[2]+0.1 );
      m_LineSourceY->SetPoint2( position[0]+0.1, m_ImageBounds[3], position[2]+0.1 );

      m_LineSourceX->SetPoint1( m_ImageBounds[0], position[1]+0.1, position[2]+0.1 );
      m_LineSourceX->SetPoint2( m_ImageBounds[1], position[1]+0.1, position[2]+0.1 );

      m_LineSourceZ->SetPoint1( position[0]+0.1, position[1]+0.1, m_ImageBounds[4] );
      m_LineSourceZ->SetPoint2( position[0]+0.1, position[1]+0.1, m_ImageBounds[5] );
    }
}

/** Create the vtk Actors */
void CrossHairRepresentation
::CreateActors()
{
  // to avoid duplicates we clean the previous actors
  this->DeleteActors();

  // build x cross hair

  //give any initial value
  m_LineSourceX = vtkLineSource::New();
  m_LineSourceX->SetPoint1( 0, 0, 0 );
  m_LineSourceX->SetPoint2( 0, 1, 0 );

  vtkPolyDataMapper* lineMapperX = vtkPolyDataMapper::New();
  lineMapperX = vtkPolyDataMapper::New();
  lineMapperX->SetInput( m_LineSourceX->GetOutput() );
  lineMapperX->SetResolveCoincidentTopologyToPolygonOffset();

  vtkActor* lineActorX = vtkActor::New();
  lineActorX->SetMapper (lineMapperX);
  lineActorX->GetProperty()->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());

  lineMapperX->Delete();
  this->AddActor( lineActorX );

  // build y cross hair

  //give any initial value
  m_LineSourceY = vtkLineSource::New();
  m_LineSourceY->SetPoint1( 0, 0, 0 );
  m_LineSourceY->SetPoint2( 0, 1, 0 );

  vtkPolyDataMapper* lineMapperY = vtkPolyDataMapper::New();
  lineMapperY = vtkPolyDataMapper::New();
  lineMapperY->SetInput( m_LineSourceY->GetOutput() );
  lineMapperY->SetResolveCoincidentTopologyToPolygonOffset();

  vtkActor* lineActorY = vtkActor::New();
  lineActorY->SetMapper (lineMapperY);
  lineActorY->GetProperty()->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());

  lineMapperY->Delete();
  this->AddActor( lineActorY );

    // build z cross hair

  //give any initial value
  m_LineSourceZ = vtkLineSource::New();
  m_LineSourceZ->SetPoint1( 0, 0, 0 );
  m_LineSourceZ->SetPoint2( 0, 0, 1 );

  vtkPolyDataMapper* lineMapperZ = vtkPolyDataMapper::New();
  lineMapperZ = vtkPolyDataMapper::New();
  lineMapperZ->SetInput( m_LineSourceZ->GetOutput() );
  lineMapperZ->SetResolveCoincidentTopologyToPolygonOffset();

  vtkActor* lineActorZ = vtkActor::New();
  lineActorZ->SetMapper (lineMapperZ);
  lineActorZ->GetProperty()->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());

  lineMapperZ->Delete();
  this->AddActor( lineActorZ );
}

/** Create a copy of the current object representation */
CrossHairRepresentation::Pointer
CrossHairRepresentation::Copy() const
{
  Pointer newOR = CrossHairRepresentation::New();
  newOR->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());
  newOR->SetOpacity(this->GetOpacity());
  newOR->RequestSetCrossHairObject(m_CrossHairSpatialObject);

  return newOR;
}


} // end namespace igstk