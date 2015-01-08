Characteristics of the simulation:

- The statistical metrics collected are the once set by default.
- This configuration simulates a FCFS algorithm with no optimitzations.
- The configuration used is the marenostrum default configuration with 64 CPUS  
- The trace files , contains the first 100 jobs from the trace http://www.cs.huji.ac.il/labs/parallel/workload/l_das2/DAS2-fs1-2003-1.swf.gz
  + cleaned log -- RECOMMENDED
  + The DAS2 5-Cluster Grid Logs
  + Research grid composed of five Pentium/Linux clusters (one cluster with 144 CPUs and the rest with 64) - WE USE THE fs1 TRACE FILE that has 64 cpus
- As can be seen in the paraver trace the original workload contains a fully sequential workload. Since all the jobs were submitted when its precursor finished.
