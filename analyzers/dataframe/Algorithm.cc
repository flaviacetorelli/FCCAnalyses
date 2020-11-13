#include "Algorithm.h"
#include "Math/Minimizer.h"
#include "Math/IFunction.h"
#include "Math/Factory.h"
#include "Math/Functor.h"

std::vector<double> m_px;
std::vector<double> m_py;
std::vector<double> m_pz;

double thrust(const double *pars){

  //Magnitude of the thrust axis vector to make a unit vector
  double mag = sqrt(pars[0]*pars[0] + pars[1]*pars[1] + pars[2]*pars[2]);
  
  //Numerator of the thrust expression
  double T_num = 0.;
  //Denominator of the thrust expression
  double T_den = 0.;
   
  for (unsigned int i =0; i<m_px.size(); i++){
    T_num += abs(m_px.at(i)*(pars[0]/mag) + m_py.at(i)*(pars[1]/mag) + m_pz.at(i)*(pars[2]/mag));                 
    T_den += sqrt(m_px.at(i)*m_px.at(i) + m_py.at(i)*m_py.at(i) + m_pz.at(i)*m_pz.at(i));
  }           
  //Thrust value
  if (T_den>0.){
    double val = T_num / T_den;
    //Return minus so that scipy minimises -T i.e. maximises T
    return -val;
  }
  return 0.;
}


double minimize_thrust(std::vector<double> px, std::vector<double> py, std::vector<double> pz) {

  m_px.clear();
  m_py.clear();
  m_pz.clear();

  for (unsigned int i =0; i<px.size(); i++){
    m_px.push_back(px.at(i));
    m_py.push_back(py.at(i));
    m_pz.push_back(pz.at(i));
  }
  
  
  ROOT::Math::Minimizer *min = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
  //ROOT::Math::Minimizer* min = new ROOT::Minuit2::Minuit2Minimizer("Minuit2") ;
  min->SetMaxFunctionCalls(100000); // for Minuit/Minuit2 
  min->SetMaxIterations(1000);  // for GSL 
  min->SetTolerance(0.001);
  min->SetPrintLevel(0);

  // create functon wrapper for minmizer
  // a IMultiGenFunction type 
  ROOT::Math::Functor f(&thrust,3); 
  double step[3] = {0.001,0.001,0.001};
  
  // starting point
  
  double variable[3] = { 0.1,-0.1,0.1};
  min->SetFunction(f);
  
  min->SetVariable(0,"x",variable[0], step[0]);
  min->SetVariable(1,"y",variable[1], step[1]);
  min->SetVariable(2,"z",variable[2], step[2]);
  
  min->Minimize();
  
  
  const double *xs = min->X();
  std::cout << "Minimum: f(" << xs[0] << "," << xs[1] << "," << xs[2] << "): " 
	    << min->MinValue()  << std::endl;

  double result = -1.*min->MinValue();
  delete min;
  return result;
}

  /* TFitter minuit(3);  // 1 parameter
     minuit.SetFCN(fcn);

	 minuit.SetParameter(0, "x", 0, 0.5, 0, 0);
	 minuit.SetParameter(0, "y", 0, 0.5, 0, 0);
	 minuit.SetParameter(0, "z", 0, 0.5, 0, 0);
	 
	 minuit.ExecuteCommand("MIGRAD", 0, 0);
	 
	 float best_fit_x = minuit.GetParameter(0);
	 float best_fit_y = minuit.GetParameter(1);
	 float best_fit_z = minuit.GetParameter(2);

	 std::cout << "Best fit x : " << best_fit_x << std::endl;
	 std::cout << "Best fit y : " << best_fit_y << std::endl;
	 std::cout << "Best fit z : " << best_fit_z << std::endl;
      */
