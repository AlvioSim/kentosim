#plotting the slowdonw during the time
barplot(trace$JobSimBSLD,
     main="Bounded Slowdown for the jobs",
     sub="During the time" ,
     xlab="Job Identifier",
     ylab="Slowdown");
