#ifndef SWFTRACEFILE_H
#define SWFTRACEFILE_H

#include <scheduling/tracefile.h>
#include <utils/log.h>

using namespace Utils;

namespace Simulator {

//Forward declaration 

/**
@author Francesc Guim,C6-E201,93 401 16 50,
*/

/** Class for loading SWF traces files */
class SWFTraceFile: public TraceFile {
public:
  SWFTraceFile();
  ~SWFTraceFile();
  SWFTraceFile(string FilePath,Log* log);
    
  virtual bool loadTrace();
  virtual void processHeaderEntry(string headerLine);

protected:
  bool isGrid; /**< This variable inidicates if the workload is a grid workload, this will be considered in the job object instantiation to decide with kind of job has to be created */
  bool isExtended;  /**< Similar to the previous value indicates when the current workload is an extended workload, this may be used by the the loader to decide which job has to be instantiated */

};

}
#endif
