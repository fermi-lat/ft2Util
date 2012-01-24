// //Boresight.cxx
#include "Configuration.h"
#include <typeinfo>
#include <iostream>
#include <string>
#include <fstream>

#include "ft2Util/Boresight.h"
#include "util.h"
#include "xmlBase/XmlParser.h"
#include "xmlBase/Dom.h"
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include "facilities/Util.h"
#include "facilities/commonUtilities.h"

ft2Util::Boresight::Boresight()
{
  //Default values for boresight angles. This could be avoided by
  //using one of the other two constructors.
  m_Rx = ft2Util::Configuration::Instance()->Rx;
  m_Ry = ft2Util::Configuration::Instance()->Ry;
  m_Rz = ft2Util::Configuration::Instance()->Rz;
  ft2Util::Boresight::update_rotation();
}

ft2Util::Boresight::Boresight(double Rx, double Ry, double Rz) 
                              : m_Rx(Rx), m_Ry(Ry), m_Rz(Rz)
{
  ft2Util::Boresight::update_rotation();
}

ft2Util::Boresight::Boresight(const std::string borefile)
{
  ft2Util::Boresight::xmlParser(borefile, &m_Rx,&m_Ry,&m_Rz);
  ft2Util::Boresight::update_rotation();
}

void ft2Util::Boresight::update_rotation() 
{ 
  //transform angles from arcsec to deg, then from deg to rad,
  //then obtain the boresight rotation matrix
  m_rotation = CLHEP::HepRotationX ( ft2Util::util::Deg2Rad(m_Rx/3600.0) ) 
                    * CLHEP::HepRotationY ( ft2Util::util::Deg2Rad(m_Ry/3600.0) ) 
                    * CLHEP::HepRotationZ ( ft2Util::util::Deg2Rad(m_Rz/3600.0) );
}

const CLHEP::HepRotation& ft2Util::Boresight::getRotation() const
{
  return m_rotation;
}

const astro::Quaternion ft2Util::Boresight::getQuaternion()
{
  return astro::Quaternion(m_rotation);
}

ft2Util::Boresight::~Boresight()
{
}

int ft2Util::Boresight::xmlParser (std::string infile, double* Rx, double* Ry, double* Rz )
{
    XERCES_CPP_NAMESPACE_USE
    facilities::commonUtilities::setupEnvironment();

    facilities::Util::expandEnvVar ( &infile );

    xmlBase::XmlParser* parser = new xmlBase::XmlParser ( true );

    DOMDocument* doc = 0;
    try
        {
            doc = parser->parse ( infile.c_str() );
        }
    catch ( xmlBase::ParseException ex )
        {
            std::cout << "caught exception with message " << std::endl;
            std::cout << ex.getMsg() << std::endl;
            throw ex;
            delete parser;
            return 0;
        }

    if ( doc != 0 )  // successful
        {
            //std::cout << "Document successfully parsed" << std::endl;

            // look up some attributes
            DOMElement* docElt = doc->getDocumentElement();
            DOMElement* attElt =
                xmlBase::Dom::findFirstChildByName ( docElt, "angle" );

            try
                {
                    *Rx = xmlBase::Dom::getDoubleAttribute ( attElt, "Rx" );
                }
            catch ( xmlBase::DomException ex )
                {
                    std::cout << std::endl << "DomException:  " << ex.getMsg()
                              << std::endl << std::endl;
                }
            try
                {
                    *Ry = xmlBase::Dom::getDoubleAttribute ( attElt, "Ry" );
                }
            catch ( xmlBase::DomException ex )
                {
                    std::cout << std::endl << "DomException:  " << ex.getMsg()
                              << std::endl << std::endl;
                }
            try
                {
                    *Rz = xmlBase::Dom::getDoubleAttribute ( attElt, "Rz" );
                }
            catch ( xmlBase::DomException ex )
                {
                    std::cout << std::endl << "DomException:  " << ex.getMsg()
                              << std::endl << std::endl;
                }
        }
        return 0;
}
