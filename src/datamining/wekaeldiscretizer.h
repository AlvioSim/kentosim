#ifndef DATAMININGWEKAELDISCRETIZER_H
#define DATAMININGWEKAELDISCRETIZER_H

#include <datamining/wekadiscretizer.h>

namespace Datamining {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/**
EqualIntervalLengh

- Will Discretize the attribute with the same lenght per interval
- Number of bins : Finds automatically the number of intervals in which the attributes will be discretized if -1 is provided. Otherwise the number of bins provided is used.

Parameters: -F -B NUMBERBEANS -1.0 -c X1,X2 -i INPUTFILE -o OUTPUTFILE 
Parameters num bins automatic: -O -F -1.0 -c X1,X2 -i INPUTFILE -o OUTPUTFILE -c last  

*/
class WekaELDiscretizer : public WekaDiscretizer{
public:
  WekaELDiscretizer(Log* log, string OutputDir,int NumbBins);
  ~WekaELDiscretizer();
  
private: 
 int NumbBins;

};

}

#endif
