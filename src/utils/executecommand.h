#ifndef UTILSEXECUTECOMMAND_H
#define UTILSEXECUTECOMMAND_H

#include <vector>

#include <utils/log.h>

using namespace std;
using std::vector;

namespace Utils {

/**
  @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** This class has been defined for executed shell commands, with a given set of arguments. For instance scripts, binaris and others */
class ExecuteCommand{
public:
  ExecuteCommand(string command,string params,bool outputRequired,Log* log,string tmpdir);
  ExecuteCommand();
  ~ExecuteCommand();
  virtual vector<string>* execute();
  int MySystem(const char *cmd);
   
protected: 
  string command; /**< The command line that will be executed*/
  string params; /**< The parameters to be provided to the command line*/
  bool outputRequired; /**< A boolean indicating if the output is required or not*/
  string TemporaryDir; /**< The directory where the temporary files will be stored */
  Log* log; /**< The logging engine*/

};

}

#endif
