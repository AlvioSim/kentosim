#ifndef SIMULATORSWFTRACEEXTENDED_H
#define SIMULATORSWFTRACEEXTENDED_H

#include <scheduling/swftracefile.h>

namespace Simulator {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/**
* This class loads an SWF trace file .. but also loads another trace file that contains the  requirements information or estimation of each of the jobs that are contained in the main trace file
*/
class SWFTraceExtended : public SWFTraceFile{
public:
  SWFTraceExtended();
  ~SWFTraceExtended();
    
  SWFTraceExtended(string FilePath,string JobRequirementsFile,Log* log);
      
  //the following function loads a trace resource workload requirement files
  bool LoadRequirements();

  //the following function loads a monetary cost workload requirement files
  bool LoadCostRequirements();

private:
  string JobRequirementsFile; /**< The file path of the file that contains the requirements for the jobs of the workload*/

};

}

#endif
