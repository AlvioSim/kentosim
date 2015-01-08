

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>
#include <string>

#include <utils/simulationconfiguration.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;
using namespace Simulator;
using namespace ArchDataModel;


const char * invocation_name = 0;
const char * const Program_name    = "eNanos Workload Simulator";
const char * const program_name    = "sim_mn2";
const char * const program_version = "1.2";
const char * const program_year    = "2007";



void show_help( const bool verbose ) throw()
  {
  std::printf( "%s - This is the Alvio Workload Simulator.\n", Program_name );
  std::printf( "See the file README to learn how to use %s in\n", Program_name );
  std::printf( "For further information, documentation and so on visit:\n");
  std::printf( "http://alvio.guim.net\n");
  std::printf( "\nUsage: %s [options]\n", invocation_name );
  std::printf( "Options:\n" );
  std::printf( "  -h, --help                   display this help and exit\n" );
  std::printf( "  -V, --version                output version information and exit\n" );
  std::printf( "  -d, --debuglevel=<arg>       The debug level (1,2,4) Default is 1.\n" );
  std::printf( "  -c, --configfile=<arg>       The configuration file for the simulator.\n" );
  std::printf( "  -b, --workload=<arg>         The workload that will be used the input for the simulation.\n" );
  std::printf( "  -B, --jobrequirements=<arg>  The job requirements file that will be used the input for the policy DEEP.\n" );
  std::printf( "  -t, --workloadtype=<arg>     The workload type of the input file (Default is SWF).\n" );
  std::printf( "  -p, --policy=<arg>           The policy that will be used the input for the simulation. (Default FCFS)\n" );
  std::printf( "  -s, --startime=<arg>         The time when the simulation will start. (defaul 0)\n" );
  std::printf( "  -S, --simulationconfig=<arg> The simulation configuration file, (see README) ");
  std::printf( "  -o, --outputdir=<arg>        The output dir where all the simulation results will be stored.\n" );
  std::printf( "  -T, --paraveroutputdir=<arg> The paraver output dir. Where paraver traces will be stored.\n" );
  std::printf( "  -N, --paravertrace=<arg>     The paraver trace name.\n" );
  std::printf( "  -D, --DebugFile=<arg>        The debug file where store all the debug info.\n" );
  std::printf( "  -E, --ErrorFile=<arg>        The error file where store all the error info.\n" );
  std::printf( "  -a, --ArchitectureFile=<arg> The file containing the architecture definition.\n" );
  std::printf( "  -A, --ArchitectureType=<arg> The architecture model (Default Marenostrum I).\n" );
  std::printf( "  -W, --withparaver            Generate the paraver traces.\n" );
  std::printf( "  -l, --policies               List the available simulation policies\n" );
  std::printf( "  -L, --workloads              List the workload types that are recognized\n" );
  std::printf( "  -z, --architectures          List the architecture models that are supported\n" );
  std::printf( "  -v, --verbose                verbose operation\n" );
  std::printf( "  -C, --statisticsfile=<arg>   File containing the statistics to be collected\n" );
  if( verbose )
    {
    std::printf( "  -H, --hidden                 example of hidden option (shown with -v -h)\n" );
    }
  std::printf( "\nReport bugs to fguim@guim.net\n");
  }


void show_version() throw()
  {
  std::printf( "%s version %s\n", Program_name, program_version );
  std::printf( "Copyright (C) %s Francesc Guim Bernat .\n", program_year );
  std::printf( "This program is Barcelona Supercomputing Center and Francesc Guim Software software; you may redistribute it under the terms of\n" );
  std::printf( "the GNU General Public License.  This program has absolutely no warranty.\n" );
  }


void show_error( const char * msg, const int errcode = 0, const bool help = false ) throw()
  {
  if( msg && msg[0] != 0 )
    {
    std::fprintf( stderr, "%s: %s", program_name, msg );
    if( errcode > 0 ) std::fprintf( stderr, ": %s", strerror( errcode ) );
    std::fprintf( stderr, "\n" );
    }
  if( help && invocation_name && invocation_name[0] != 0 )
    std::fprintf( stderr, "Try `%s --help' for more information.\n", invocation_name );
  }


void internal_error( const char * msg ) throw()
  {
  char buf[80];
  std::snprintf( buf, sizeof( buf ), "internal error: %s.\n", msg );
  show_error( buf );
  exit( 3 );
  }


const char * optname( const int code, const Arg_parser::Option options[] ) throw()
  {
  static char buf[2] = "?";

  if( code != 0 )
    for( int i = 0; options[i].code; ++i )
      if( code == options[i].code )
        { if( options[i].name ) return options[i].name; else break; }
  if( code > 0 && code < 256 ) buf[0] = code; else buf[0] = '?';
  return buf;
  }
  
  void show_simulation_policies() throw()
  {
    std::printf( "The available scheduling policies that are being simulated are:\n" );
    std::printf( "First Come First Serve - ID : FCFS\n" );
    std::printf( "EASY Backfilling - ID: EASY");
    std::printf( "DeepSearch Policy - ID DEEP");
  }
  
    void show_simulation_workloads() throw()
  {
    std::printf( "The workloads formats that are currently recognized by the simulator are:\n" );
    std::printf( "Standard Workload Format - ID : SWF\n" );    
    std::printf( "Standard Workload Format EXtended (SWF+job requirements) ID: SWFExt ");
  }
  
      void show_simulation_architectures() throw()
  {
    std::printf( "The architecture models that are currently recognized by the simulator are:\n" );
    std::printf( "Marenostrum I (Blades/Nodes/Cpus)- ID : MN1\n" );    
  }
  




/* WE KEEP THIS AS A GLOBAL FOR THE SIGNAL TREATMENT */
Simulation * simulator = NULL;

void terminate (int param)
{
  std::printf ("Terminating simulation a sigkill has been received...\n");
  std::printf("All the statistic files and traces are being generated..\n");
  simulator->setStopSimulation(true);
}

void showStats (int param)
{
  simulator->showCurrentStatistics();
  signal(SIGINT,showStats);
}


int main(int argc, char *argv[])
{
  
  
  /********************************************************************************************************************
  ***************************************** PARSING THE COMMAND LINE **************************************************
  *********************************************************************************************************************/   
  //Variables for the command line parsing

  bool verbose = false;
  invocation_name = argv[0];
  bool generateParaver = false;
  double startime = 0;



  static const Arg_parser::Option options[] =
    {
    { 'V', "version",  Arg_parser::no    },
    { 'd', "debuglevel",Arg_parser::yes    },
    { 'c', "configfile",Arg_parser::yes   },
    { 'b', "workload",  Arg_parser::yes },
    { 'B', "jobrequirements",  Arg_parser::yes },
    { 't', "workloadtype",     Arg_parser::yes    },
    { 'p', "policy",    Arg_parser::yes    },
    { 'o', "outputdir", Arg_parser::yes    },
    { 'T', "paraveroutputdir", Arg_parser::yes    },
    { 'N', "paravertrace", Arg_parser::yes    },
    { 'D', "DebugFile", Arg_parser::yes    },
    { 'E', "ErrorFile", Arg_parser::yes    },
    { 'a', "ArchitectureFile", Arg_parser::yes    },
    { 'A', "ArchitectureType", Arg_parser::yes    },
    { 'v', "verbose",  Arg_parser::no    },
    { 'q', "quiet",  Arg_parser::no    },
    { 'l', "policies",  Arg_parser::no    },
    { 'L', "workloads",  Arg_parser::no    },
    { 'z', "architectures",  Arg_parser::no    },
    { 'W', "withparaver",  Arg_parser::no    },    
    { 'h', "help",     Arg_parser::no    },
    { 's', "starttime",     Arg_parser::yes    },
    { 'S', "simulationconfig",     Arg_parser::yes    },
    { 'C', "statisticsfile",     Arg_parser::yes    },
    {   0, 0,          Arg_parser::no    } };

  Arg_parser parser( argc, argv, options );
  if( parser.error().size() )				// bad option
    { show_error( parser.error().c_str(), 0, true ); return 1; }

  //we load the configuration default file -- Be aware that this is only for debveloping ..
//string defaultConfigurationFile = "/cac/u01/irodero/kento/SimulatorConfigurations/simulatorConfiguration.xml";






































  string defaultConfigurationFile = "/home/irodero/kento-sim_64/kento_sim/SimulatorConfigurations/simulatorConfiguration.xml";






































  ConfigurationFile* configuration = new ConfigurationFile(defaultConfigurationFile);  

  if(parser.arguments() == 0)
  {
    show_help(verbose);
    return EXIT_SUCCESS;
  }

  for( int argind = 0; argind < parser.arguments(); ++argind )
    {
    const int code = parser.code( argind );
    const char * arg = parser.argument( argind ).c_str();
   


    if( !code ) break;					// no more options
    switch( code )
      {      
      case 'V': show_version(); return EXIT_SUCCESS;
      case 'l': show_simulation_policies(); return EXIT_SUCCESS;
      case 'L': show_simulation_workloads(); return EXIT_SUCCESS;
      case 's': startime = atof(arg); break;
      case 'd':
      { 
        configuration->DebugLevel = atoi(arg);
        break;	
      }  
      case 'p': break;
      {
        const char* pol = arg;
        
        if(strcmp(pol,"FCFS"))
        {
          configuration->policy = FCFS;
        }
        else if(strcmp(pol,"EASY"))
        {
          configuration->policy = EASY;
        }
        else if(strcmp(pol,"DEEP"))
        {
          configuration->policy = DEEP;
        }
        else
        {
          std::printf("The simulation type is incorrect");
          show_simulation_policies();
          return EXIT_SUCCESS;
        }
        break;
      }
      case 'c': 
      {
         const char* name = arg;
         ConfigurationFile* newconfiguration = new ConfigurationFile(name);
         
         //we import the current values of the configuration file
         newconfiguration->import(configuration);
         delete configuration;
         configuration = newconfiguration;
        break;	
      }      
      case 'b':
      { 
        configuration->workloadPath = arg;
        break;	
      }    
      case 'B':
      { 
        configuration->jobRequirementsFile = arg;
        break;	
      }    
      case 't': 
      {
        const char* wor = arg;
        
        if(strcmp(wor,"SWF"))
        {
          configuration->workload = SWF;
        }
        else
        {
          std::printf("The workload type is incorrect");
          show_simulation_workloads();
          return EXIT_SUCCESS;
        }
        break;
      }      
      case 'A': 
      {
        const char* wor = arg;
        
        if(strcmp(wor,"MN1"))
        {
          configuration->architecture = MARENOSTRUM;
        }
        else
        {
          std::printf("The architecture model is incorrect");
          show_simulation_architectures();
          return EXIT_SUCCESS;
        }
        break;
      }    
      case 'a': 
      { 
        configuration->architectureFile = arg;
        break;	
      }
      case 'S': 
      { 
        configuration->simulationconfigFile = arg;
        break;	
      }      
      case 'C':
      {
        configuration->statisticsConfigFile = arg;
      }
      case 'T': 
      { 
        configuration->paraverOutPutDir = arg;
        break;	
      }      
      case 'N': 
      { 
        configuration->paraverTrace = arg;
        break;	
      }          
      case 'D':  
      { 
        configuration->DebugFilePath = arg;
        break;	
      }  
      case 'E': 
      { 
        configuration->ErrorFilePath = arg;
        break;	
      }  
      case 'W': generateParaver = true; break;	
      case 'h': show_help( verbose ); return 0;      
      case 'q': verbose = false; break;
      case 'v': verbose = true; break;
      default : show_help(verbose ); return EXIT_SUCCESS;
      }
    } 
  
  //if the simulation configuration file is provided , it will be loaded and all the fields that refereing to the simulation 
  //stuff will be overloded by this one 
  SimulationConfiguration* simConfig  = NULL;
  
  Log * log = new Log(configuration->ErrorFilePath,configuration->DebugFilePath,configuration->DebugLevel);
  
  if(configuration->simulationconfigFile != "")  
    simConfig = new SimulationConfiguration(configuration->simulationconfigFile,log,configuration);     
  else
    simConfig = new SimulationConfiguration();
    
  simConfig->import(configuration);
  
  if(simConfig->DebugFilePath != "" && simConfig->ErrorFilePath != "")
  {
     delete log;
     log = new Log(simConfig->ErrorFilePath,simConfig->DebugFilePath,simConfig->DebugLevel);   
  }
  
  simConfig->log = log;
  
  //Now that the we have parsed all the parameters and the configuration file has been loaded we 
  //carry out all the simulation stuff related to the simulation
    
  log->setVerboseMode(verbose);
  
  std::cout << "Using the debuglevel " << itos(simConfig->DebugLevel) << " and using the debug file " << simConfig->DebugFilePath << endl;
    

  
  if(generateParaver)
    //in case has pushed it from command line .. we enforce it independent of the simulationconfiguration file
    simConfig->generateParaver = true;
  
  /*we create the simulation stuff*/ 
  log->debug("Loading the simConfig file of the architecture..");

  /* now the architecture */
  ArchitectureConfiguration* architectureConfig = simConfig->CreateArchitecture(log);

  log->debug("The architecture file has been loaded correctly !!! The simConfig file is "+simConfig->architectureFile);
     
  //First the workload 
  std::cout << "Using the workload " << simConfig->workloadPath << endl;
  
  //If the user has specified the number of jobs to load in the main simulator configuration file we overload the jobs to load value of the simulation configuration by the once specified in the configuration file 
  if(configuration->JobsToSimulate != -1)
   simConfig->JobsToLoad = configuration->JobsToSimulate;

  TraceFile* workload  = simConfig->CreateWorkload(log) ;  
  
  log->debug("Loading the workload file "+simConfig->workloadPath);
     
  log->debug("Creating the simulation simConfig container..");
  
  log->debug("Loading the statistic configuration file "+simConfig->StatisticsConfigFile);
  
  StatisticsConfigurationFile* statisticsConfig = new StatisticsConfigurationFile(simConfig->StatisticsConfigFile,log);
  
  log->debug("Creating the statistical analisis instance for the simulation");
  
  SimStatistics* statistics = new SimStatistics(log);
  statistics->setJobMetricsDefinition(statisticsConfig->getjobMetrics());
  statistics->setPolicyMetricsDefinition(statisticsConfig->getPolicyMetrics());
  
  CSVJobsInfoConverter* jobsSimulationCSV = simConfig->createJobCSVDumper();
  
  if(jobsSimulationCSV != NULL)
  {
    log->debug("Creating the cvs file "+simConfig->JobsSimPerformanceCSVFile+" for dump the jobs simulation performance variables",2);    
  }
  
  CSVPolicyInfoConverter* policySimulationCSV = simConfig->createPolicyCSVDumper();
  
  if(policySimulationCSV != NULL)
  {
    log->debug("Creating the cvs file "+simConfig->JobsSimPerformanceCSVFile+" for dump the policy simulation performance variables",2);    
  }
  
  statistics->setGlobalStatisticsOutputFile(simConfig->GlobalStatisticsOutputFile);
  
  log->debug("Creating the container for the simulator characteristics");
  
  SimulatorCharacteristics * chars = new SimulatorCharacteristics();

  chars->setWorkload(workload);
  chars->setArchConfiguration(architectureConfig);
  chars->setSimStatistics(statistics); 
    
  log->debug("Creating the simulator policy...");
   
  SchedulingPolicy* policy  = simConfig->CreatePolicy(architectureConfig,log,startime);
 
 
  simulator = new Simulation(chars,log,policy);
  
  simulator->setCollectStatisticsInterval(simConfig->collectStatisticsInterval);
  simulator->setArrivalFactor(simConfig->ArrivalFactor);
  simulator->setJobsSimulationPerformance(jobsSimulationCSV);
  simulator->setPolicySimulationPerformance(policySimulationCSV);
  simulator->setShowSimulationProgress(simConfig->showSimulationProgress);
  
  policy->setSim(simulator);
       
  if(jobsSimulationCSV!=NULL)
    jobsSimulationCSV->setPolicy(policy);
       
  ParaverTrace* paravertrace = 0;
  
  if(simConfig->generateParaver) 
  {
    log->debug("Creating the paraver containers...");
    log->debug("The trace file name is "+simConfig->paraverTrace + " {*.row, *.prv and *.cfg}");
    log->debug("The output trace directory is "+simConfig->paraverOutPutDir);
    
    paravertrace = new ParaverTrace(simConfig->paraverTrace,
                                                 simConfig->paraverOutPutDir,
                                                 workload->getNumberOfJobs(),
                                                 architectureConfig,
                                                 simConfig->architecture);
    paravertrace->setLog(log);
    paravertrace->setStartJobTrace(simConfig->startJobTrace);
    paravertrace->setEndJobTrace(simConfig->endJobTrace);
    simulator->setParaverTrace(paravertrace);
  }
  else
  {
    log->debug("Paraver trace generation is disbled..");
  }
  
  log->debug("Setting the SIGNAL function");
  signal(SIGUSR1,terminate);
  signal(SIGINT,showStats);
  pid_t pidsimulation = getpid(); 
  std::cout << "The pid for the current simulation is "+itos(pidsimulation) << endl;
  
  std::cout << "Initializing the simulation" << endl;
  simulator->initSimulation();
  std::cout <<  "doing the simulation"  << endl;
  simulator->doSimulation();
  std::cout <<  "Simulation finished." << endl;
  if(simConfig->generateParaver)
  {
    log->debug("Closing paraver traces and genarting it content...");
    paravertrace->setLast_arrival(simulator->getglobaltime());
    paravertrace->close();
  }
    
  log->debug("Generating the R Analyzer scripts and launching the analysis");
  log->debug("The Job CSV Analysers ..");
  
  if(jobsSimulationCSV != NULL)
  {
    vector<RScript*>* analyzers = simConfig->CreateJobCSVRAnalyzers(simulator->getJobsSimulationPerformance(),architectureConfig);
    
    for(vector<RScript*>::iterator it = analyzers->begin();it != analyzers->end(); ++it)
    {
      RScript* analyzer = *it;
      analyzer->analyze();
    }
  }
  
  return EXIT_SUCCESS;
}

