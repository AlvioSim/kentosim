#plotting the wait tim during the time 
barplot(trace$JobSimWaitTime,
     main="Wait time for the jobs",
     sub="During the time" ,
     xlab="Job Identifier",
     ylab="Wait time in seconds");
