#include <datamining/wekaefdiscretizer.h>
#include <utils/utilities.h>
#include <assert.h>

namespace Datamining {

/**
 * @param Log A reference to the logging engine 
 * @param DirOutputFiles Indicates the dir we the temporary files will be output 
 * @param NumbBins Indicates the number of beens to create, if -1 is provided the nubmer of bins for the discretization has to be automatically found 
 */
WekaEFDiscretizer::WekaEFDiscretizer(Log* log,string DirOutputFiles,int NumbBins) : WekaDiscretizer(log,DirOutputFiles)
{
  this->NumbBins = NumbBins;

  this->WekaDiscretizationClass = "weka.filters.unsupervised.attribute.Discretize"; 
  
  if(NumbBins != -1)
    this->WekaDiscretizationParameters = "-F -M -B "+itos(NumbBins)+" -1.0";
  else
    this->WekaDiscretizationParameters = "-O -F -1.0 -M";

}

/**
 * The destructor for the class 
 */
WekaEFDiscretizer::~WekaEFDiscretizer()
{
}


}
