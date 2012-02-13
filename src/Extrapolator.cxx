#include <sstream>
#include <algorithm>
#include <vector>
#include "ft2Util/Extrapolator.h"
#include "util.h"

//ROOT
#include "TGraph.h"
#include "TF1.h"

//This class implements the extrapolation of points, using ROOT

using namespace ft2Util;

void applyOffset(std::vector<double> &v, double offset)
{
  std::transform(v.begin(), v.end(), v.begin(),
                 std::bind2nd(std::minus<double>(), offset));
}
void undoOffset(std::vector<double> &v, double offset)
{
  std::transform(v.begin(), v.end(), v.begin(),
                 std::bind2nd(std::plus<double>(), offset));
}

Extrapolator::Extrapolator(std::vector<double> x, std::vector<double> y, int polGrade) :
                     m_x(x),m_y(y)
{
  //Apply a offset to improve numerical precision
  m_timeOffset=m_x.front()-1e2;
  applyOffset(m_x,m_timeOffset);

  //Generate the TGraph with the current points
  m_graph=new TGraph(m_x.size(),&m_x[0],&m_y[0]);  

  //Generate the name of a polynomial with the requested grade
  std::stringstream str;
  str << "pol" << polGrade;
  m_functionName=str.str();

  //Fit the points (not a real fit, since we have no errors...)
  TFitResultPtr r;
  r = m_graph->Fit(m_functionName.c_str(),"Q");
  int fitStatus = (int) r;
  if (fitStatus != 0) {
    m_graph->SaveAs("extrapolationGraph.root");
    throw std::runtime_error("FATAL: Fit for extrapolation FAILED. A graph containing the last known point has been dumped (extrapolationGraph.root).");
  }
  
  m_function = m_graph->GetFunction(m_functionName.c_str());
  //Get best fit parameters
  m_par=m_function->GetParameters();
}

Extrapolator::~Extrapolator()
{
  delete(m_function);
  delete(m_graph);
}

const double Extrapolator::evaluateIn(double x)
{
  //Evaluate the function in the requested point
  double extrapolation = m_function->Eval(x-m_timeOffset);
  
//   std::vector<double>::iterator itor, itor2;
//   itor = std::lower_bound(m_x.begin(),m_x.end(),x-m_timeOffset);
//   if(itor == m_x.end()) 
//   {
//     //Forward extrapolation
//     std::cout.precision(14);
//     std::cout << "Extrapolation summary:" << std::endl;
//     std::cout << "t = " << x << ", v = " << extrapolation << "   (<- forward extrapolation)"<< std::endl;
//     for(int i=m_x.size()-1;i>(int) m_x.size()-3;--i)
//     {
//       std::cout << "t = " << m_x[i]+m_timeOffset << ", v = " << m_y[i] << std::endl;
//     }
//     std::cout << std::endl;
//   } else 
//   {
//     //Backward extrapolation
//     std::cout << "Extrapolation summary:" << std::endl;
//     std::cout << "t = " << x << ", v = " << extrapolation << "   (<- backward extrapolation)" << std::endl;
//     for(int i=0;i<2;++i)
//     {
//       std::cout << "t = " << m_x[i]+m_timeOffset << ", v = " << m_y[i] << std::endl;
//     }
//     std::cout << std::endl;
//   }
  
  return extrapolation;
}
