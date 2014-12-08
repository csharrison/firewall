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
Found bug in skip_list. Fixed it. Can now send hundreds of thousands of packets without segfaulting!!!

## Experiments
We originally created a queue with spinlocks, but quickly that this was inefficient due to scheduling. Threads were being scheduled and they were just spinning. So instead, we changed it to use condition variables and mutexes and this improved the runtime.
Next, 
![chart]
[chart]: https://github.com/csharrison/firewall/blob/master/chart.png
## Conclusions
