#include "igstkTrackerConfiguration.h"
#include "igstkTrackerController.h"
#include "igstkTrackerConfigurationFileReader.h"
#include "igstkPolarisVicraConfigurationXMLFileReader.h"
#include "igstkPolarisSpectraConfigurationXMLFileReader.h"
#include "igstkPolarisHybridConfigurationXMLFileReader.h"
#include "igstkAuroraConfigurationXMLFileReader.h"
#include "igstkMicronConfigurationXMLFileReader.h"

/**
 * Class that observers both read success, 
 * igstk::TrackerConfigurationFileReader::ReadSuccessEventand 
 * and read failure,
 * igstk::TrackerConfigurationFileReader::ReadFailureEvent
 * events generated by the TrackerConfigurationFileReader class.
 */
class ReadFailSuccessObserver : public ::itk::Command
{
public:
  typedef  ReadFailSuccessObserver    Self;
  typedef  ::itk::Command             Superclass;
  typedef  ::itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  ReadFailSuccessObserver() : m_GotFailure( false ), m_GotSuccess( false ) {}
  ~ReadFailSuccessObserver() {}
public:
  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    const itk::Object * constCaller = caller;
    this->Execute( constCaller, event );
  }
  void Execute(const itk::Object *caller, const itk::EventObject & event)
  {
  if( const igstk::TrackerConfigurationFileReader::ReadSuccessEvent *rse = 
      dynamic_cast<const igstk::TrackerConfigurationFileReader::ReadSuccessEvent *>( &event) )
      m_GotSuccess = true;
  else if( const igstk::TrackerConfigurationFileReader::ReadFailureEvent *rfe = 
           dynamic_cast<const igstk::TrackerConfigurationFileReader::ReadFailureEvent *>( &event) )
    {
      m_GotFailure = true;
      m_FailureMessage = rfe->Get();
    }
  }
  bool GotSuccess() {return this->m_GotSuccess;}
  bool GotFailure() {return this->m_GotFailure;}
  void Reset() 
  { 
    this->m_GotSuccess = false; 
    this->m_GotFailure = false;
    this->m_FailureMessage.clear();
  }
  std::string GetFailureMessage() const
  {
    return m_FailureMessage;
  }
private:
  std::string m_FailureMessage;
  bool m_GotSuccess;
  bool m_GotFailure;
};

/**
 * Observer for the event generated by 
 * TrackerConfigurationFileReader->RequestGetData() method.
 */
igstkObserverMacro( TrackerConfiguration, 
                   igstk::TrackerConfigurationFileReader::TrackerConfigurationDataEvent, 
                   igstk::TrackerConfigurationFileReader::TrackerConfigurationDataType )

/**
 * Observer for the TrackerController->RequestInitialize() failure.
 */
igstkObserverMacro( InitializeError, 
                   igstk::TrackerController::InitializeErrorEvent, 
                   std::string )

                   

int main( int argc, char *argv[] )
{
  const unsigned int NUM_TRACKER_TYPES = 5;
  char * trackerTypes[NUM_TRACKER_TYPES]; 
  enum { PolarisVicra, PolarisSpectra, PolarisHybrid, Aurora, Micron };
  trackerTypes[PolarisVicra] = "Polaris Vicra";
  trackerTypes[PolarisSpectra] = "Polaris Spectra";
  trackerTypes[PolarisHybrid] = "Polaris Hybrid";
  trackerTypes[Aurora] = "Aurora";
  trackerTypes[Micron] = "Micron";

  if( argc!=3 )
  {
    std::cerr<<"Usage: "<<argv[0]<<" tracker_type ";
    std::cerr<<"tracker_xml_configuration_file.\n";
    std::cerr<<"Tracker types:\n";
    for( int i=0; i<NUM_TRACKER_TYPES; i++ )
      std::cerr<<"\t"<<i<<" == "<<trackerTypes[i]<<"\n";
    return EXIT_FAILURE;
  }

  igstk::TrackerConfigurationFileReader::Pointer trackerConfigReader = 
    igstk::TrackerConfigurationFileReader::New();

  igstk::TrackerConfigurationXMLFileReaderBase::Pointer 
    trackerCofigurationXMLReader;

             //I'm assuming I get an integer here, skipping the validation
  switch( atoi( argv[1] ) )
  {
    case PolarisVicra:
      trackerCofigurationXMLReader = 
        igstk::PolarisVicraConfigurationXMLFileReader::New();
      break;
    case PolarisSpectra:
      trackerCofigurationXMLReader = 
        igstk::PolarisSpectraConfigurationXMLFileReader::New();
      break;
    case PolarisHybrid:
      trackerCofigurationXMLReader = 
        igstk::PolarisHybridConfigurationXMLFileReader::New();
      break;
    case Aurora:
      trackerCofigurationXMLReader = 
        igstk::AuroraConfigurationXMLFileReader::New();
      break;
    case Micron:
      trackerCofigurationXMLReader = 
        igstk::MicronConfigurationXMLFileReader::New();
      break;
    default:
      std::cerr<<"Invalid tracker type. Valid types are:\n";
      for( int i=0; i<NUM_TRACKER_TYPES; i++ )
        std::cerr<<"\t"<<i<<" == "<<trackerTypes[i]<<"\n";
      return EXIT_FAILURE;
  }

             //setting the file name and reader always succeeds so I don't
             //observe the trackerConfigReader for their success events
  trackerConfigReader->RequestSetFileName( argv[2] );
  trackerConfigReader->RequestSetReader( trackerCofigurationXMLReader );  
  
                //need to observe if the request read succeeds or fails
                //there is a third option that the read is invalid, if the
                //file name or xml reader weren't set
  ReadFailSuccessObserver::Pointer rfso = ReadFailSuccessObserver::New();
  trackerConfigReader->AddObserver( igstk::TrackerConfigurationFileReader::ReadSuccessEvent(),
                                    rfso );
  trackerConfigReader->AddObserver( igstk::TrackerConfigurationFileReader::ReadFailureEvent(),
                                    rfso );
  trackerConfigReader->RequestRead();

  if( rfso->GotFailure() )
  {
    std::cerr<<rfso->GetFailureMessage()<<"\n";
    return EXIT_FAILURE;
  }
  if( !rfso->GotSuccess() )
  {
    std::cerr<<"Reading xml file did not succeed or fail, ";
    std::cerr<<"invoked in an inappropriate manner (prior to setting file name or xml reader).\n";
    return EXIT_FAILURE;
  }
                 //get the configuration data from the reader
  TrackerConfigurationObserver::Pointer tco = TrackerConfigurationObserver::New();
  trackerConfigReader->AddObserver( 
    igstk::TrackerConfigurationFileReader::TrackerConfigurationDataEvent(),
    tco );
  trackerConfigReader->RequestGetData();
  
  if( !tco->GotTrackerConfiguration() )
  {
    std::cerr<< "Did not get tracker configuration...\n";
    return EXIT_FAILURE;
  }

  igstk::TrackerConfiguration *trackerConfiguration = 
    tco->GetTrackerConfiguration();

         //initialize the tracker controller and start tracking
  igstk::TrackerController::Pointer trackerController = 
    igstk::TrackerController::New();
  
  InitializeErrorObserver::Pointer ieo = 
    InitializeErrorObserver::New();
  trackerController->AddObserver( igstk::TrackerController::InitializeErrorEvent(),
                                  ieo );
  trackerController->RequestInitialize( trackerConfiguration );
  if( ieo->GotInitializeError() )
  {
    std::cout<<ieo->GetInitializeError()<<"\n";
    return EXIT_FAILURE;
  }

  trackerController->RequestStartTracking();

  return EXIT_SUCCESS;
}
