/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImagerController.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkImagerController_h
#define __igstkImagerController_h

#include "igstkSandboxConfigure.h"

#include "igstkImagerConfiguration.h"

#include "igstkStateMachine.h"
#include "igstkMacros.h"
#include "igstkEvents.h"
#include "igstkObject.h"

#include "igstkImager.h"
#include "igstkImagerTool.h"

//Terason Ultrasound specific headers
#include "igstkTerasonImagerConfiguration.h"
#include "igstkTerasonImagerTool.h"
#include "igtlServerSocket.h"

//ImagingSource framegrabber specific headers
#include "igstkImagingSourceImagerConfiguration.h"
#include "igstkImagingSourceImagerTool.h"

//CompressedDV framegrabber specific headers
#include "igstkCompressedDVImagerConfiguration.h"
#include "igstkCompressedDVImagerTool.h"




namespace igstk
{

/** \class ImagerController
 *
 *  \brief This class encapsulates the startup and shutdown procedures required
 *         for the imager classes supported by IGSTK.
 *
 *  This class enables the user to intialize a imager with minimal effort.
 *  Initialization is performed by passing the controller a ImagerConfiguration
 *  object. The controller identifies the specific imager internally. The
 *  user can then request to intialize the imager or to tell the imager
 *  controller to shut down a imager that is running.
 *
 *  NOTE: This class is usefull only when all tools are connected to the imager
 *        in advance (no swapping tools in and out after initialization).
 */
class ImagerController : public Object
{

public:
  /** Macro with standard traits declarations (Self, SuperClass, State
   *  Machine etc.). */
  igstkStandardClassTraitsMacro( ImagerController, Object )

  /**
   * Initialize a imager using the given configuration. The imager and
   * communication are established and the imager is in imaging mode.
   * NOTE: If an imager was already initialized using this object it will be
   *       shut down (stop imaging, close communication, and if using serial
   *       communication that will be closed too).
   *
   * @param configuration A specific imager configuration (Terason, ImagingSource, etc)
   *                      which is identified at runtime.
   * This method generates two events, InitializationSuccessEvent if everything
   * went well and, InitializationFailureEvent if the imager could not be
   * initialized (the specific error is described in the string contained in the
   * error object).
   */
  void RequestInitialize(const ImagerConfiguration *configuration);

  void RequestStart();

  void RequestStop();

  void RequestSetProgressCallback(itk::Command *progressCallback);

  /**
   * Stop the imager and close its communication.
   * This method generates ShutdownSuccessEvent if everything
   * went well and, a ShutdownErrorEvent if there were problems during
   * shutdown.
   */
  void RequestShutdown();

  void RequestGetImager();

  void RequestGetToolList();

   /** This event is generated if the initialization succeeds. */
  igstkEventMacro( InitializeSuccessEvent, IGSTKEvent );

   /** This event is generated if the start imaging procedure succeeds. */
  igstkEventMacro( StartSuccessEvent, IGSTKEvent );

   /** This event is generated if the stop imaging procedure succeeds. */
  igstkEventMacro( StopSuccessEvent, IGSTKEvent );

   /** This event is generated if the shutdown succeeds. */
  igstkEventMacro( ShutdownSuccessEvent, IGSTKEvent );

   /** This event is generated if communication open fails*/
  igstkLoadedEventMacro( OpenCommunicationFailureEvent,
                         IGSTKErrorEvent,
                         EventHelperType::StringType );

   /** This event is generated if the initialization fails*/
  igstkLoadedEventMacro( InitializeFailureEvent,
                         IGSTKErrorEvent,
                         EventHelperType::StringType );

   /** This event is generated if the start imaging procedure fails*/
  igstkLoadedEventMacro( StartFailureEvent,
                         IGSTKErrorEvent,
                         EventHelperType::StringType );

   /** This event is generated if the user requests the imager and it is
    *  initialized. */
  igstkLoadedEventMacro( RequestImagerEvent,
                         IGSTKEvent,
                         Imager::Pointer );

   /** This event is generated if the user requests the imager tools and the
    *  imager is initialized. */
  igstkLoadedEventMacro( RequestToolsEvent,
                         IGSTKEvent,
                         std::vector<ImagerTool::Pointer> );

   /** This event is generated if the stop imaging procedure encountered
    *  difficulties */
  igstkLoadedEventMacro( StopFailureEvent,
                         IGSTKErrorEvent,
                         EventHelperType::StringType );

   /** This event is generated if the shutdown encountered difficulties
    *   (e.g. imager communication was already closed from outside). */
  igstkLoadedEventMacro( ShutdownFailureEvent,
                         IGSTKErrorEvent,
                         EventHelperType::StringType );

protected:

  ImagerController( void );
  ~ImagerController( void );

private:

  /** List of state machine states */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( AttemptingToInitialize );
  igstkDeclareStateMacro( AttemptingToStart );
  igstkDeclareStateMacro( AttemptingToStop );
  igstkDeclareStateMacro( AttemptingToShutdown );

  //Terason Ultrasound specific :begin
  igstkDeclareStateMacro( AttemptingToInitializeTerason );
  //Terason Ultrasound specific :end

  //ImagingSource framegrabber specific :begin
  igstkDeclareStateMacro( AttemptingToInitializeImagingSource  );
  //ImagingSource framegrabber specific :end

  //CompressedDV framegrabber specific :begin
    igstkDeclareStateMacro( AttemptingToInitializeCompressedDV  );
  //CompressedDV framegrabber specific :end

  igstkDeclareStateMacro( Initialized );
  igstkDeclareStateMacro( Started );

  /** List of state machine inputs */
  igstkDeclareInputMacro( ImagerInitialize );
  igstkDeclareInputMacro( ImagerStart );
  igstkDeclareInputMacro( ImagerStop );
  igstkDeclareInputMacro( ImagerShutdown );

  //Terason Ultrasound specific :begin
  igstkDeclareInputMacro( TerasonInitialize );
  //Terason Ultrasound specific :end

  //ImagingSource framegrabber specific :begin
  igstkDeclareInputMacro( ImagingSourceInitialize );
  //ImagingSource framegrabber specific :end

  //CompressedDV framegrabber specific :begin
    igstkDeclareInputMacro( CompressedDVInitialize );
  //CompressedDV framegrabber specific :end

  igstkDeclareInputMacro( Failed  );
  igstkDeclareInputMacro( Succeeded  );
  igstkDeclareInputMacro( GetImager  );
  igstkDeclareInputMacro( GetTools  );

  /**List of state machine actions*/
  void ImagerInitializeProcessing();
  void ImagerStartProcessing();
  void ImagerStopProcessing();
  void ImagerShutdownProcessing();

  //Terason Ultrasound specific :begin
  void TerasonInitializeProcessing();
  //Terason Ultrasound specific :end

  //ImagingSource framegrabber specific :begin
  void ImagingSourceInitializeProcessing();
  //ImagingSource framegrabber specific :end

  //CompressedDV framegrabber specific :begin
    void CompressedDVInitializeProcessing();
  //CompressedDV framegrabber specific :end

  void GetImagerProcessing();
  void GetToolsProcessing();
  void ReportInvalidRequestProcessing();

  void ReportInitializationFailureProcessing();
  void ReportInitializationSuccessProcessing();

  void ReportStartFailureProcessing();
  void ReportStartSuccessProcessing();

  void ReportStopFailureProcessing();
  void ReportStopSuccessProcessing();

  void ReportShutdownFailureProcessing();
  void ReportShutdownSuccessProcessing();

  bool InitializeSerialCommunication();

  //Terason Ultrasound specific :begin
  bool InitializeSocketCommunication();

  TerasonImagerTool::Pointer InitializeTerasonTool(
    const TerasonToolConfiguration *toolConfiguration );
  //Terason Ultrasound specific :end

  //ImagingSource framegrabber specific :begin
  ImagingSourceImagerTool::Pointer InitializeImagingSourceTool(
    const ImagingSourceToolConfiguration *toolConfiguration );
  //ImagingSource framegrabber specific :end

  //CompressedDV framegrabber specific :begin
  CompressedDVImagerTool::Pointer InitializeCompressedDVTool(
    const CompressedDVToolConfiguration *toolConfiguration );
    //CompressedDV framegrabber specific :end


  class ErrorObserver : public itk::Command
  {
  public:
    typedef ErrorObserver                    Self;
    typedef ::itk::Command                   Superclass;
    typedef ::itk::SmartPointer<Self>        Pointer;
    typedef ::itk::SmartPointer<const Self>  ConstPointer;

    igstkNewMacro(Self)
    igstkTypeMacro(ErrorObserver, itk::Command)

    /** When an error occurs in an IGSTK component it will invoke this method
     *  with the appropriate error event object as a parameter.*/
    virtual void Execute(itk::Object *caller,
                         const itk::EventObject & event) throw (std::exception);

    /** When an error occurs in an IGSTK component it will invoke this method
     *  with the appropriate error event object as a parameter.*/
    virtual void Execute(const itk::Object *caller,
                         const itk::EventObject & event) throw (std::exception);

    /**Clear the current error.*/
    void ClearError() { this->m_ErrorOccured = false;
                        this->m_ErrorMessage.clear(); }
    /**If an error occurs in one of the observed IGSTK components this method
     * will return true.*/
    bool ErrorOccured() { return this->m_ErrorOccured; }
    /**Get the error message associated with the last IGSTK error.*/
    void GetErrorMessage(std::string &errorMessage)
    {
      errorMessage = this->m_ErrorMessage;
    }

  protected:

    /**Construct an error observer for all the possible errors that occur in
     * the observed IGSTK components.*/
    ErrorObserver();
    virtual ~ErrorObserver(){}
  private:
    bool m_ErrorOccured;
    std::string m_ErrorMessage;
    std::map<std::string,std::string> m_ErrorEvent2ErrorMessage;

             //purposely not implemented
    ErrorObserver(const Self&);
    void operator=(const Self&);
  };

  ErrorObserver::Pointer m_ErrorObserver;
  std::string m_ErrorMessage;

  ImagerConfiguration  *m_TmpImagerConfiguration;
  ImagerConfiguration  *m_ImagerConfiguration;
  Imager::Pointer       m_Imager;
  std::vector<ImagerTool::Pointer> m_Tools;

  //Terason Ultrasound specific :begin
  igtl::ServerSocket::Pointer m_SocketCommunication;
  SerialCommunication::Pointer m_SerialCommunication;
  //igtl::ClientSocket::Pointer m_SocketCommunication;
  //Terason Ultrasound specific :end

  itk::Command *m_ProgressCallback;
};

} // end of namespace
#endif