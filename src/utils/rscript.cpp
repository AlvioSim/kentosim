#include <utils/rscript.h>

namespace Utils {

/**
 * 
 * @param command The command line to be executed (recommended to use the complet path)
 * @param paramas The params that will be provided to the executable
 * @param outputRequired A bool indicating if the output is required
 * @param Log The loggin engine 
 * @param headerFile A set of vector fields indicating the lines that have to be put inside the header R file, this in case required.. usually this vector is used for setup some R variables that are used for the simulator. This mainly depends on the R script used
 * @param tempDir The directory where the temporary files will be dump
 */
RScript::RScript(string command,string params,bool outputRequired,Log* log, vector<string>* headerFile,string tempDir) : ExecuteCommand(command,params,outputRequired,log,tempDir)
{
  this->headerFile = headerFile;
}

/**
 * The default constructor
 * @return 
 */
RScript::RScript()
{
}

/**
 * The default constructor for the class.
 */
RScript::~RScript()
{
}

/**
 * Returns the r script analyzer type used in the current implementation
 * @return A r_scripts_type with the analyzer type used in the simulator
 */
r_scripts_type RScript::getscriptType() const
{
  return scriptType;
}


/**
 * Sets the analyzer type used in the implementation
 * @param theValue The analyzer type used
 */
void RScript::setScriptType(const r_scripts_type& theValue)
{
  scriptType = theValue;
}

}

