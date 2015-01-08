#ifndef DATAMININGWEKAID3_H
#define DATAMININGWEKAID3_H

#include <datamining/wekaclassifier.h>

namespace Datamining {

/**
 @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** This class implements a ID3 classifier, currently is quite simple since it only sets some variables in the constructor. However more sophisticate stuff could be done */
class WekaID3 : public WekaClassifier {
public:
    WekaID3(vector<string>* attributes, vector<native_t>* attributesTypes,string ModelFilePath,bool NewFilePerModel, Log* log,discretizer_t discretizer,int numberOfBins);
    ~WekaID3();

};

}

#endif
