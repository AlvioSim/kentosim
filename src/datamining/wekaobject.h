#ifndef DATAMININGWEKAOBJECT_H
#define DATAMININGWEKAOBJECT_H

#include <string>
#include <scheduling/metric.h>
#include <map>
#include <vector>

#include <utils/log.h>

using namespace std;
using std::map; 
using std::vector;
using namespace Simulator;
using namespace Utils;

namespace Datamining {

/**
 @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** This typedef defines the sets of instances that will be used for constructring the model. This is a vector of attributes, and each vector contains the values for each instance */
typedef vector<vector<string>*> InstancesSet;

/** This class implements that main functionalities for accessing to the weka functionalities using the command line interface */
class WekaObject{
public:
  WekaObject(Log* log);
  ~WekaObject();

  void setWekaEnv(string JavaBinary,string classpath,string wekaPath,string TempDir);
  vector<string>* InvoqueWekaFunctionality(string WekaClass, string Parameters,bool neededOutput);
  void readAttributesDeffinition(string file,vector<string>* output);
  string getIntervalFromReal(string attributeDefinition, string real);

protected:
  string JavaBinary; /**< Indicates the file path for the java binary */
  string classpath; /**< Indicates the class path for the java run time files */
  string wekaPath; /**< Indicates the path where weka jar file is located*/
  string TemporaryDir; /**< Indicates the path for the temporary files dir */
  
  /* Auxiliar functions */
  bool generateArffFile(string FilePath, InstancesSet* instances,vector<string>* attributes,vector<native_t>* attributesTypes,int numberOfInstances);
  bool generateArffFileWithHeader(string FilePath, InstancesSet* instances,vector<string>* headerEntries,int numberOfInstances);
  
  bool saveArffFile(string FilePath,vector<string>* AttLines,vector<string>* InstanceLines);
  InstancesSet* loadInstancesArffFile(string FilePath,int NumberAttributes);
  
  void generateHeaderLinesForEvaluate(vector<string>* toClassify,vector<string>* attributes, vector<native_t>* attributesTypes,vector<string> * InstanceLine,vector<string>* AtributeLine );
  void generateHeaderLines(InstancesSet* instances,vector<string>* attributes, vector<native_t>* attributesTypes,int numberOfInstances,vector<string> * InstanceLine,vector<string>* AtributeLine );
  
  
private:
  string GetAttributeWekaType(native_t type);
  Log* logWeka;
  
 

};

}

#endif
