#ifndef SIMULATORMNCONFIGURATION_H
#define SIMULATORMNCONFIGURATION_H

#include <utils/architectureconfiguration.h>
#include <archdatamodel/marenostrum.h>
#include <string>
#include <iostream>



using namespace std;
using namespace ArchDataModel;


namespace Utils {

//forward declaration


/**
* @author Francesc Guim,C6-E201,93 401 16 50,
*/

/** This class provides the methods  for accessing a definition of an architectureconfiguration file that models a marenostrum type architecture. */
class MNConfiguration : public ArchitectureConfiguration{
public:
  MNConfiguration();
  MNConfiguration(string ConfigurationFile,Log* log);
  ~MNConfiguration();
  virtual bool loadConfiguration();
  virtual int getNodeIdCpu(int cpuId);
  void setMarenostrum(MareNostrum* theValue);
  MareNostrum* getmarenostrum() const;  	
  bool extractCommonFields(); 
  
private:
  MareNostrum* marenostrum; /**< Contains a reference to the marenostrum object with the information concerning the architecture definintion @see ArchDataModel namespace */

};

}

#endif
