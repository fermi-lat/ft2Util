/** @file Extrapolator.h
@brief declare class Extrapolator
*/

//Avoid to include this header more than one time
#ifndef EXTRAPOLATOR_H
#define EXTRAPOLATOR_H

#include "astro/PointingInfo.h"
//ROOT
class TGraph;
class TF1;
//#include "TGraph.h"
#include "Ttypes.h"
//#include "TF1.h"

namespace ft2Util
    {
      /**
      *
      * @brief This implements the method to extrapolate a generic 1-d function y = f(x)
      *
      * Given a vector of x (x1...xN) and y (y1...yN), the best fitting polynomial of the requested degree is used to
      * extrapolate the value y = f(x), where x > xN or x < x1.
      * @author Giacomo Vianello <giacomov@slac.stanford.edu>
      *
      * $Header$
      */
        class Extrapolator
            {
                public:
                  /**
                  Constructor.
                  @param x a std::vector containing a sample of the independent variable
                  @param y a std::vector containing the corresponding sample of dependent variable
                  @param polGrade the grade of the polynomial to be used for the fit
                  */ 
                  Extrapolator(std::vector<double> x, std::vector<double> y, int polGrade);
                  ~Extrapolator();
                  /**
                  Estimate the value of y at x
                  @param x the value where you want an estimate of y=f(x)
                  */ 
                  const double evaluateIn(double x);
                  
                private:
                  std::vector<double> m_x;
                  std::vector<double> m_y;
                  double m_timeOffset;
                  std::string m_functionName;
                  TGraph* m_graph;
                  TF1 *m_function;
                  Double_t* m_par;
            };
    }
#endif
