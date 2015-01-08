#ifndef DATAMININGWEKAC45_H
#define DATAMININGWEKAC45_H

#include <datamining/wekaclassifier.h>

namespace Datamining {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** This class implements a C45 classifier, currently is quite simple since it only sets some variables in the constructor. However more sophisticate stuff could be done */
class WekaC45: public WekaClassifier{
public:
    WekaC45(vector<string>* attributes, vector<native_t>* attributesTypes,string ModelFilePath,bool NewFilePerModel, Log* log,discretizer_t discretizer,int numberOfBins);
    ~WekaC45();

};

}

#endif
