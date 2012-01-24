/** @file LatCondition.cxx
*/
#include <iostream>
#include "ft2Util/LatCondition.h"

  ft2Util::LatCondition::LatCondition (int inmode, int latconfig, int dataquality) : 
                                                                 m_mode(inmode), m_config(latconfig), m_dataQuality(dataquality) 
  {
  }

