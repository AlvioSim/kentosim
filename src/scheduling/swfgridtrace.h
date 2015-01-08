#ifndef SIMULATORSWFGRIDTRACE_H
#define SIMULATORSWFGRIDTRACE_H

#include <scheduling/swftracefile.h>
#include <scheduling/jobrequirement.h>

namespace Simulator {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
/** 
 This class loads an extension of the SWF trace for load a trace that conatins description of grid jobs 
*/

class SWFGridTrace :  public SWFTraceFile{
public:
  SWFGridTrace();
  ~SWFGridTrace();
  SWFGridTrace(string FilePath,string JobRequirementsFile,Log* log);

  //the following function loads a trace resource workload requirement files
  bool LoadRequirements();
  

private:
  string JobRequirementsFile; /**< The file path of the file that contains the requirements for the jobs of the workload*/
  operator_t getOperatorType(string operatorString);
};

}

#endif
