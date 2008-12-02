/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    UltrasoundCalibrationWizardQuadrantViews.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// generated by Fast Light User Interface Designer (fluid) version 1.0107

#include "UltrasoundCalibrationWizardQuadrantViews.h"

namespace igstk
{

UltrasoundCalibrationWizardQuadrantViews::UltrasoundCalibrationWizardQuadrantViews(int X, int Y, int W, int H, const char *L)
  : Fl_Group(X, Y, W, H, L)
{

  m_Reporter = ::itk::Object::New();
 
  const int N = 15;    // Height of the slider bar
  const int C = 5;    // Width of the vertical separator

  Fl_Group * parentGroup = this->parent();

  m_X = 150; // width of the control panel
  m_Y = 0;

  m_Width = parentGroup->w() - m_X;
  m_Height = parentGroup->h(); 

  m_WW = (int) (m_Width-2*C)/2;
  m_HH = (int) m_Height;

  // Create widgets
  typedef igstk::FLTKWidget   WidgetType;

  m_VideoWidget    = new WidgetType(X, Y, m_WW, m_HH, "Display 0");
  m_3DWidget       = new WidgetType(X+m_WW+C, Y, m_WW, m_HH, "Display 1");       
 
  // Create views
  m_VideoView    = ViewType2D::New();
  m_VideoView->RequestSetOrientation( View2D::Axial );

  m_3DView = ViewType3D::New();

  m_VideoWidget->RequestSetView( m_VideoView );
  m_3DWidget->RequestSetView( m_3DView );

  end();
}

UltrasoundCalibrationWizardQuadrantViews::~UltrasoundCalibrationWizardQuadrantViews()
{
  delete m_VideoWidget;
  delete m_3DWidget;
}

unsigned long UltrasoundCalibrationWizardQuadrantViews::AddObserver(
  const ::itk::EventObject & event, ::itk::Command * observer )
{
  return m_Reporter->AddObserver( event, observer );
}

void UltrasoundCalibrationWizardQuadrantViews::RemoveObserver( unsigned long tag )
{
  m_Reporter->RemoveObserver( tag );
}

void UltrasoundCalibrationWizardQuadrantViews::RemoveAllObservers()
{
  m_Reporter->RemoveAllObservers();
}

int 
UltrasoundCalibrationWizardQuadrantViews
::handle_mouse(int event, MouseCommandType &mouseCommand) 
{
    mouseCommand.quadrant = 0;
    MousePressedEvent mousePressedEvent;
    mousePressedEvent.Set( mouseCommand );
    m_Reporter->InvokeEvent( mousePressedEvent );

    damage(1); 
    return 1;
}

int 
UltrasoundCalibrationWizardQuadrantViews
::handle_key(int event, KeyboardCommandType &keyCommand) 
{
    KeyPressedEvent kpEvent;
    kpEvent.Set( keyCommand );
    m_Reporter->InvokeEvent( kpEvent );
    damage(1); 
    return 1;
}

int 
UltrasoundCalibrationWizardQuadrantViews
::handle(int event)
{
  switch (event) 
  {      
    case FL_KEYUP:
      KeyboardCommandType keyCommand;
      keyCommand.key = Fl::event_key();
      keyCommand.state = Fl::event_state();
      return handle_key(event,keyCommand);

    case FL_PUSH:  
      if ( (Fl::event_button() == FL_RIGHT_MOUSE) && (Fl::event_state() == FL_SHIFT + FL_BUTTON3) ) 
      {
        m_wl.current_x = Fl::event_x();
        m_wl.current_y = Fl::event_y();
        return 1;
      }
      return Fl_Group::handle(event);

    case FL_DRAG:
      if (Fl::event_state() == FL_SHIFT + FL_BUTTON3) 
      {            
        MouseCommandType mouseCommand;
        mouseCommand.button = Fl::event_button();
        mouseCommand.state = Fl::event_state();            
        m_wl.prev_x = m_wl.current_x;
        m_wl.prev_y = m_wl.current_y;
        m_wl.current_x = Fl::event_x();
        m_wl.current_y = Fl::event_y();
        mouseCommand.x = Fl::event_x();
        mouseCommand.y = Fl::event_y();

        mouseCommand.dx = m_wl.current_x - m_wl.prev_x;
        mouseCommand.dy = m_wl.current_y - m_wl.prev_y;

        return handle_mouse(event, mouseCommand);
      }
      else
        return Fl_Group::handle(event);

    default: 
      return Fl_Group::handle(event);
  }
}


} // end namespace igstk
