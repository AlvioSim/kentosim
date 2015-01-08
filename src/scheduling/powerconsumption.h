#ifndef SIMULATIONPOWERCONSUMPTION_H
#define SIMULATIONPOWERCONSUMPTION_H

#include <map>
#include <vector>


#include <scheduling/job.h>

using std::map; 
using namespace std;


typedef pair<double, int> pairConfs;
typedef vector<pairConfs> rconf;

namespace Simulator {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
class PowerConsumption{
public:
    PowerConsumption();
    PowerConsumption(double act, double por);

    ~PowerConsumption();

     void updateEnergyConsumedSimple(int active, int total, double globaltime, Job* job);
     void updateEnergyConsumed(int active, int total, double globaltime);
     double computeSaving();  
     double gettopFrequency();
     double getpreviousTime();
     void setPreviousTime(double time);
     double getenergyConsumed();
     void setEnergyConsumed(double energy);
     double getScalingRatio(int jobNumber, double freq);
     double getBeta(int jobNumber);
     void increaseNumberOfReducedModeCPUS(int number, double freq);
     void decreaseNumberOfReducedModeCPUS(int number, double freq);
     void setGears(vector<double> freq, vector<double> volt); 

     vector<rconf> confs; 


private:
     double Activity; /* it presents the ratio of the running and idle activities */
     double Portion; /* the portion of static in the total CPU power      */
     double alpha; /* a parameter from the the power model, Pstatic=alpha*V  */
     double topFrequency;  /* we need to know which is the top frequency */
     double previousTime;   
     double energyConsumed;
      map<int, double> betas;   /* beta for one job is randomly determined, so it has to be stored in order to have always same beta for one job     */
      map<double,double> gears; 
      map<double,int> nCPUS; /* the first member presents a frequency and the second is the number of processors that run at that frequency  */


};

}

#endif
