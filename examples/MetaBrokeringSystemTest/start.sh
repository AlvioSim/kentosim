
export XERCESCROOT=/aplic/XMLLibs/xerces-c-src_2_7_0
export XALANCROOT=/aplic/XMLLibs/xml-xalan/c
export LD_LIBRARY_PATH=/aplic/XMLLibs/xerces-c-src_2_7_0/lib:/aplic/XMLLibs/xml-xalan/c/lib:$LD_LIBRARY_PATH

#./sim_mn2 -S simulationConfiguration.xml

./sim_mn2 -c simulatorConfiguration.xml -S simulationConfiguration.xml
# -S /home/fguim/experiments/BRANKPolicyExperiments/experiment1/simulationConfiguration.xml
