#include <datamining/wekaeldiscretizer.h>
#include <utils/utilities.h>
#include <assert.h>

namespace Datamining {

/**
 * @param Log A reference to the logging engine 
 * @param DirOutputFiles Indicates the dir we the temporary files will be output 
 * @param NumbBins Indicates the number of beens to create, if -1 is provided the nubmer of bins for the discretization has to be automatically found 
 */
WekaELDiscretizer::WekaELDiscretizer(Log* log,string DirOutputFiles,int NumbBins) : WekaDiscretizer(log,DirOutputFiles)
{
  this->NumbBins = NumbBins;

  this->WekaDiscretizationClass = "weka.filters.unsupervised.attribute.Discretize"; 
  
  if(NumbBins != -1)
    this->WekaDiscretizationParameters = "-F -B "+itos(NumbBins)+" -M -1.0";
  else
    this->WekaDiscretizationParameters = "-O -M -1.0";
}

/**
 * The destructor for the class 
 */
WekaELDiscretizer::~WekaELDiscretizer()
{
}


}
