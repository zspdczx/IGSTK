/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkPolarisTrackerConfiguration.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkPolarisTrackerConfiguration_h
#define __igstkPolarisTrackerConfiguration_h

#include "igstkTrackerConfiguration.h"

namespace igstk
{

/*
* In this header file, define classes for each supported tracker. These
* are container classes with minimal functionality
* Each tracker configuration contains information for setting up the tracker.
* It also keeps an internal list of TrackerToolConfiguration, which stores 
* information for all the tracker tools.
*
* The tracker classes only perform "static" error checking. That is, they check
* the data for adherence to known values (e.g. tracker frequency is within the
* manufacturer specified bounds). "Dynamic" error checking is relegated to the 
* TrackerController (e.g. com port can be opened for serial communication).
*/



/**
 * A wireless polaris tool.
 */
class PolarisWirelessToolConfiguration :
  public TrackerToolConfiguration
{
public:
  PolarisWirelessToolConfiguration();
  PolarisWirelessToolConfiguration(const 
    PolarisWirelessToolConfiguration &other);
  virtual ~PolarisWirelessToolConfiguration(); 

  igstkSetMacro( SROMFile, std::string );
  igstkGetMacro( SROMFile, std::string );

  virtual std::string GetToolTypeAsString();

protected:
        //SROM file to use with the tool, required for wireless, optional for
        //wired
  std::string        m_SROMFile;
};

/**
 * A wired polaris tool.
 */
class PolarisWiredToolConfiguration :
  public TrackerToolConfiguration
{
public:
  PolarisWiredToolConfiguration();
  PolarisWiredToolConfiguration(const 
    PolarisWiredToolConfiguration &other);
  virtual ~PolarisWiredToolConfiguration(); 

  igstkSetMacro( PortNumber, unsigned );
  igstkGetMacro( PortNumber, unsigned );

  igstkSetMacro( SROMFile, std::string );
  igstkGetMacro( SROMFile, std::string );

  virtual std::string GetToolTypeAsString();

protected:
        //physical port to which the wired tool is attached
  unsigned m_PortNumber;

        //SROM file to use with the tool, required for wireless, optional for
        //wired
  std::string m_SROMFile;
};


/**
 * Configuration for the Polaris Vicra tracker.
 */
class PolarisWirelessTrackerConfiguration : 
  public SerialCommunicatingTrackerConfiguration
{
public:
          //standard typedefs
  igstkStandardClassBasicTraitsMacro( PolarisWirelessTrackerConfiguration, 
                                      SerialCommunicatingTrackerConfiguration )
protected:
  PolarisWirelessTrackerConfiguration() {}
  virtual ~PolarisWirelessTrackerConfiguration() {}

  virtual void InternalAddTool( const TrackerToolConfiguration *tool, 
                                bool isReference );
};


/**
 * Configuration for the Polaris Vicra tracker.
 */
class PolarisVicraTrackerConfiguration : 
  public PolarisWirelessTrackerConfiguration
{
public:
          //standard typedefs
  igstkStandardClassBasicTraitsMacro( PolarisVicraTrackerConfiguration, 
                                      PolarisWirelessTrackerConfiguration )

  igstkNewMacro( Self );

  /**
   * Get the manufacturer specified maximal refresh rate.
   */
  virtual double GetMaximalRefreshRate();

protected:
    PolarisVicraTrackerConfiguration();
    virtual ~PolarisVicraTrackerConfiguration();

private:                  
                //manufacturer specified maximal refresh rate [Hz]
  static const double MAXIMAL_REFERESH_RATE;
};


/**
 * Configuration for the Polaris Spectra tracker.
 */
class PolarisSpectraTrackerConfiguration : 
  public PolarisWirelessTrackerConfiguration
{
public:
          //standard typedefs
  igstkStandardClassBasicTraitsMacro( PolarisSpectraTrackerConfiguration, 
                                      PolarisWirelessTrackerConfiguration )

  igstkNewMacro( Self );

  /**
   * Get the manufacturer specified maximal refresh rate.
   */
  virtual double GetMaximalRefreshRate();

protected:
  PolarisSpectraTrackerConfiguration();
  virtual ~PolarisSpectraTrackerConfiguration();

private:                  
                //manufacturer specified maximal refresh rate [Hz]
  static const double MAXIMAL_REFERESH_RATE;
};


/**
 * Configuration for the Hybrid Polaris trackers.
 */
class PolarisHybridTrackerConfiguration : 
  public SerialCommunicatingTrackerConfiguration
{
public:

          //standard typedefs
  igstkStandardClassBasicTraitsMacro( PolarisHybridTrackerConfiguration, 
                                      SerialCommunicatingTrackerConfiguration )

  igstkNewMacro( Self );

  /**
   * Get the manufacturer specified maximal refresh rate.
   */
  virtual double GetMaximalRefreshRate();

protected:
  PolarisHybridTrackerConfiguration();
  virtual ~PolarisHybridTrackerConfiguration();

  virtual void InternalAddTool( const TrackerToolConfiguration *tool, 
                                bool isReference );

private:
                  //manufacturer specified maximal refresh rate [Hz]
  static const double MAXIMAL_REFERESH_RATE;
                 //maximal number of physical ports for attaching wired tools
  static const unsigned MAXIMAL_PORT_NUMBER;
};


} // end of name space
#endif