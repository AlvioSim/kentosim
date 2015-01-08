#include <scheduling/powerconsumption.h>
#include <cstdlib>
#include <iostream>


using namespace std;

namespace Simulator {

PowerConsumption::PowerConsumption()
{
  this->previousTime=0;
  this->energyConsumed=0;
  
}

PowerConsumption::PowerConsumption(double act, double por)
{
  this->previousTime=0;
  this->energyConsumed=0;
  this->Activity=act;  
  this->Portion=por;
}


PowerConsumption::~PowerConsumption()
{
}

void PowerConsumption::updateEnergyConsumedSimple(int active, int total, double globaltime, Job* job)
{

// RUTGERS

  double kk = gears[topFrequency];
  double kk2 = globaltime-previousTime;

  // Here we only consider the difference between the configuration in idle state and running (for example, if the configuration has DISK, it counts the energy -considering that this is for the whole NODE- that the job consumes when the disk is running = energy_running - energy_at_idle_state)

  switch(job->getQueueNumber()){ //getUserID()){
	case 1: // ALL
		energyConsumed += 20 * job->getRunTime() * job->getNumberProcessors(); //CPU
		energyConsumed += 5 * job->getRunTime() * job->getNumberProcessors(); //DISK
		energyConsumed += 2 * job->getRunTime() * job->getNumberProcessors(); //NIC
		//energyConsumed += 20 * job->getRunTime() * job->getNumberProcessors() * job->getMemoryUsed(); //MEM - hay que discretizar
		// NOTE: Not allways this is multiplied for CPUs (for example DISK and NIC are related to the node, so they must be considered once per each NODE used => ATENTION: different jobs may be computed in the same time intervals!! -> WE CAN TRY TO PROVIDE A "PROVISIONAL" APROXIMATE SOLUTION (this solution will potentially consider #cpus/node times NIC and DISK active energy consumption
		break;
	case 2: // CPU LOW
		energyConsumed += 50 * job->getRunTime() * job->getNumberProcessors();
		break;
	case 3: // CPU LOW, NO DISK
		energyConsumed += 70 * job->getRunTime() * job->getNumberProcessors();
		break;
	case 4: // CPU LOW, NO DISK, NO NIC
		energyConsumed += 75 * job->getRunTime() * job->getNumberProcessors();
		break;
	case -1: // regular case, without types...
		energyConsumed += 20 * job->getRunTime() * job->getNumberProcessors(); //CPU
		energyConsumed += 5 * job->getRunTime() * job->getNumberProcessors(); //DISK
		energyConsumed += 2 * job->getRunTime() * job->getNumberProcessors(); //NIC
		//energyConsumed += 20 * job->getRunTime() * job->getNumberProcessors() * job->getMemoryUsed(); //MEM - hay que discretizar
		break;
	default:break;
  }

  //energyConsumed+= (globaltime-previousTime)*((active*Activity+total-active )*topFrequency*gears[topFrequency]*gears[topFrequency]); //dynamic part

  //energyConsumed+=(globaltime-previousTime)*total*alpha*gears[topFrequency]; // static part

  //energyConsumed is product of elapsed time and power during the interval 
  // the power is sum of dynamic power (idle power, running power) and static 

  previousTime=globaltime;
}


/*
void PowerConsumption::updateEnergyConsumedSimple(int active, int total, double globaltime)
{
 
  energyConsumed+= (globaltime-previousTime)*((active*Activity+total-active )*topFrequency*gears[topFrequency]*gears[topFrequency]); //dynamic part

  energyConsumed+=(globaltime-previousTime)*total*alpha*gears[topFrequency]; // static part

  //energyConsumed is product of elapsed time and power during the interval 
  // the power is sum of dynamic power (idle power, running power) and static 

  previousTime=globaltime;
}
*/

void PowerConsumption::updateEnergyConsumed(int active, int total, double globaltime)
{
  double totalPower=0; 
  int reduced=0;  // the number of processors that run at reduced speed
  totalPower+=(total-active)*(topFrequency*gears[topFrequency]*gears[topFrequency]); // power of idle CPUs

  map<double,int>::iterator it;
  for(it=nCPUS.begin();it!=nCPUS.end();it++)
 { 
   if (it->first < topFrequency) 
    {
    totalPower+=Activity*(it->second)*(it->first)*(gears[it->first]*gears[it->first]); //dynamic part
    totalPower+=it->second*alpha*gears[it->first]; //static part
    reduced+=it->second; 
     }
      
  }
  totalPower+=Activity*(active-reduced)*(topFrequency*gears[topFrequency]*gears[topFrequency]);
  totalPower+=(total-reduced)*alpha*gears[topFrequency];

  energyConsumed+=(globaltime-previousTime)*totalPower;

  previousTime=globaltime;
}




double PowerConsumption::gettopFrequency()
{
  return topFrequency;
}


double PowerConsumption::getpreviousTime()
{
  return previousTime;
}



void PowerConsumption::setPreviousTime(double time)
{
  
  this->previousTime=time;
}



void PowerConsumption::setEnergyConsumed(double energy)
{
  energyConsumed=energy;
}

double PowerConsumption::getenergyConsumed()
{
  return energyConsumed;
}





double PowerConsumption::getScalingRatio(int jobNumber, double freq)
{
 
 double beta=this->getBeta(jobNumber);
 double ratio=beta*(topFrequency/freq-1)+1;  
 return ratio; 
           
}

double PowerConsumption::getBeta(int jobNumber)
{
   map<int, double>::iterator it = betas.find(jobNumber);

  if (it == betas.end())
                     { 
                        double beta=0.5 + 0.5*((double)rand())/RAND_MAX;  
                        betas.insert(pair<int, double>(jobNumber,beta));
                        return beta; }
              else return it-> second;
   

}



void PowerConsumption::increaseNumberOfReducedModeCPUS(int number, double freq)
{
   nCPUS[freq]+=number;
}

void PowerConsumption::decreaseNumberOfReducedModeCPUS(int number, double freq)
{
   nCPUS[freq]-=number;
}

void PowerConsumption::setGears(vector<double> freq, vector<double> volt)
{
  vector<double>::iterator iteratorf=freq.begin();
  vector<double>::iterator iteratorv=volt.begin();
  while (iteratorf!=freq.end()) {
  
  gears.insert(pair<double,double> (*iteratorf,*iteratorv));

  nCPUS.insert(pair<double,int>(*iteratorf,0));
  
  iteratorf++;
  iteratorv++;

  }
  this->topFrequency=freq.back();
  this->alpha=(Portion/(1-Portion))*Activity*topFrequency*gears[topFrequency];
  
  
  
}

}
