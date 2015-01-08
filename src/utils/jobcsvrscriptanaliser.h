#ifndef UTILSJOBCSVRSCRIPTANALISER_H
#define UTILSJOBCSVRSCRIPTANALISER_H

#include <utils/rscript.h>
#include <utils/architectureconfiguration.h>

namespace Utils {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
class JobCSVRScriptAnaliser : public RScript{
public:
  JobCSVRScriptAnaliser(string CSVToAnalyse,string ScriptPath,string OutPutDir,ArchitectureConfiguration* arch, Log* log,string ScriptsDir,string RBinaryPath,string ScriptsBaseDir);
  ~JobCSVRScriptAnaliser();

  virtual void analyze();

private: 
  string CSVToAnalyse; /**< File path of the CSV file to be analysed */
  string ScriptsDir; /**< File path for the scripts dir where to generate the temporary R scripts and where the original auxiliar R scripts are located*/
};

}

#endif
