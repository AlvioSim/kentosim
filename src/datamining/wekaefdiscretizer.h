#ifndef DATAMININGWEKAEFDISCRETIZER_H
#define DATAMININGWEKAEFDISCRETIZER_H

#include <datamining/wekadiscretizer.h>

namespace Datamining {

/**	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** 
EqualFrequency

- Will Discretize the attribute with the same number of instances per bin
- Number of bins : Finds automatically the number of intervals in which the attributes will be discretized if -1 is provided. Otherwise the number of bins provided is used.

Parameters: -F -B NUMBERBEANS -M -1.0 -c X1,X2 -i INPUTFILE -o OUTPUTFILE 
Parameters num bins automatic: -O -F -1.0 -M -c X1,X2 -i INPUTFILE -o OUTPUTFILE -c last  
*/
class WekaEFDiscretizer : public WekaDiscretizer {
public:
  WekaEFDiscretizer(Log* log, string OutputDir,int NumbBins);;
  ~WekaEFDiscretizer();
  
  
private: 
 int NumbBins;  

};

}

#endif
