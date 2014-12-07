# Analysis
csharris and sxchen

## Log Of What We Did
Created PNG and Histogram as set size arrays.
Created R with skip lists. Used coarse grain loacking.
Created queue with spinlocks. 
Created dispatcher, reader, writer threads. 
Currently 1 dispatcher, 2 readers, 1 writer. 
Tested out how well it was working with the 8 parameter mixes and how many packets were going through using time and timing it after the packets were initialized.
Created serial version. 
Tested out how well multithreaded versions worked compared to serial versions. 
Did poorly compared to serial.
Found bottleneck with spinlocks. Changed queue to use condition variables and mutexes. 
Created fingerprint method to make it more realistic to add runtime to serial version as well.
Improved with first 2 parameter mixes.
Created own timing method to avoid bottleneck from that and created throttler that had a mutex.
Improved with first 3 parameter mixes. 

## Experiments


## Conclusions
