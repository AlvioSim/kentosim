#include <scheduling/resourceusage.h>

namespace Simulator {

ResourceUsage::ResourceUsage()
{
  NumberCpusUsed = 0;
  nodeSizeUsed = 0;
  NumberNodesUsed = 0;
  disKSizeUsed = 0;
  MemoryUsed = 0;
}


ResourceUsage::~ResourceUsage()
{
}

/**
 * Returns the architecture
 * @return A architecture the architecture used in the simulation
 */
ArchitectureConfiguration* ResourceUsage::getarchitecture() const
{
	return architecture;
}

/**
 * Sets the EthernetBWLoad to the ResourceUsage
 * @param theValue The EthernetBWLoad
 */
void ResourceUsage::setArchitecture ( ArchitectureConfiguration* theValue )
{
	architecture = theValue;
}

/**
 * Returns the disKSizeUsed
 * @return A double containing the disKSizeUsed used 
 */
double ResourceUsage::getdisKSizeUsed() const
{
	return disKSizeUsed;
}

/**
 * Sets the disKSizeUsed to the ResourceUsage
 * @param theValue The disKSizeUsed
 */
void ResourceUsage::setDisKSizeUsed ( double theValue )
{
	disKSizeUsed = theValue;
}

/**
 * Returns the MemoryUsed
 * @return A double containing the MemoryUsed used 
 */
double ResourceUsage::getMemoryUsed() const
{
	return MemoryUsed;
}

/**
 * Sets the MemoryUsed to the ResourceUsage
 * @param theValue The MemoryUsed
 */
void ResourceUsage::setMemoryUsed ( double theValue )
{
	MemoryUsed = theValue;
}

/**
 * Returns the nodeSizeUsed
 * @return A double containing the nodeSizeUsed used 
 */
int ResourceUsage::getnodeSizeUsed() const
{
	return nodeSizeUsed;
}

/**
 * Sets the nodeSizeUsed to the ResourceUsage
 * @param theValue The nodeSizeUsed
 */
void ResourceUsage::setNodeSizeUsed ( int theValue )
{
	nodeSizeUsed = theValue;
}

/**
 * Returns the NumberCpusUsed
 * @return A double containing the NumberCpusUsed used 
 */
int  ResourceUsage::getNumberCpusUsed() const
{
	return NumberCpusUsed;
}

/**
 * Sets the NumberCpusUsed to the ResourceUsage
 * @param theValue The NumberCpusUsed
 */
void  ResourceUsage::setNumberCpusUsed ( int theValue )
{
	NumberCpusUsed = theValue;
}


int  ResourceUsage::getNumberNodesUsed() const
{
	return NumberNodesUsed;
}

/**
 * Sets the NumberNodesUsed to the ResourceUsage
 * @param theValue The NumberNodesUsed
 */
void  ResourceUsage::setNumberNodesUsed ( int theValue )
{
	NumberNodesUsed = theValue;
}

}
