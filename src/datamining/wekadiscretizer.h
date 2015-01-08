#ifndef DATAMININGWEKADISCRETIZER_H
#define DATAMININGWEKADISCRETIZER_H

#include <datamining/discretizer.h>
#include <datamining/wekaobject.h>

namespace Datamining {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** This class implements a datamining weka discretizer based on the interfaces defined in the discretizer type */
class WekaDiscretizer : public Discretizer, WekaObject {
public:
  WekaDiscretizer(Log* log,string DirOutputFiles);
  ~WekaDiscretizer();

  virtual vector<string>* discretize(vector<string>* input);
  void discretize(string IntPutArffFile,string OutPutArffFile,vector<string>* attributes);
  
protected:
  string WekaDiscretizationClass ; /** Indicates the java weka class that implements the discretization method - to be set by the subclasses */
  string WekaDiscretizationParameters; /** Indicates the parametrs that have to be provided to the discretizer - to be set by the subclasses */

private: 
  string DirOutputFiles ; /** Indicates the directory where the temporary weka arff files will be stored  */
  
};

}

#endif
