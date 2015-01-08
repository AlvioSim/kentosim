#ifndef UTILSCSVCONVERTER_H
#define UTILSCSVCONVERTER_H

#include <string>
#include <vector>

#include <assert.h>
#include <utils/utilities.h>
#include <utils/configurationfile.h>
#include <time.h>
#include <fstream>
#include <utils/log.h>

using namespace std;
using std::vector;

namespace Utils {


/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** Implements a general cvs generator */
class CSVConverter{
public:
  CSVConverter(string FilePath,Log* log);
  ~CSVConverter();
  
  /* sets and gets*/
  void setCSVHeaderFields(vector< string >* theValue);
  vector< string >* getCSVHeaderFields() const;
  void setFieldTypes(vector< file_field_t >* theValue);
  vector< file_field_t >* getfieldTypes() const;	
	 
  /* functions regarding the cvs file management */
  void open();
  virtual void addEntry(vector<string>* fields);
  void close(); 

  void setType(const csv_converter_t& theValue);
  csv_converter_t gettype() const;

  void setFilePath(const string& theValue);
  string getFilePath() const;
	
	

protected:
  string FilePath; /**< The output file for the cvs  */
  vector<string>* CSVHeaderFields;/**< The vector containing the different columns names  for the cvs output file  */
  vector<file_field_t>* fieldTypes;/**<  The vector containing the different columns types for the cvs output file */
 
  ofstream* cvsFile;/**< The stream for the output file */
  Log* log;/**< The logging engine */
  bool opened;/**< Indicates if the output file is opened*/
  string separator;/**< The separator between the fields of the cvs */

  csv_converter_t type; /**< Indicates the converter cvs converter type implemented by the current instance  **/
};

}

#endif
