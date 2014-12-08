# Analysis
csharris and sxchen

## Architecture 
For our architecture, we decided to have one dispatcher thread that would process the packet and decide which type of worker thread to pass it onto - a writer or reader thread. We hold onto information about the threads with a PNG, R, Histogram that is universally shared by all the threads. The histogram holds onto the fingerprint results. PNG (persona-non-grata) tells us whether or not a source address is allowed to send a packet. R tells us the permissions for specific destination addresses and that source addresses they are willing to accept from. R has a coarse grained lock on it so that no more than one thread has access to it at one time, so each thread is processed serially. Each worker thread will have its own work queue that has a mutex lock on it as well.

For more information on how our firewall works, please read our README.

## Experiments
In order to get data for our experiments, we compared the multithreaded version's runtime of how long it took to process all the generated packets over the serial version's. 
We originally created a queue with spinlocks, but quickly that this was inefficient due to scheduling. Threads were being scheduled and they were just spinning. So instead, we changed it to use condition variables and mutexes and this improved the runtime.
Next, we created a fingerprint method to make it more realistic (and to add runtime) to both the serial version and the multithreaded version to see how it compared. By doing this, it yet again improved our runtime, and made our multithreaded version better for the first 2 parameter mixes.
Then, we realized we should use our own Time method instead of the time.h get_time to avoid further bottlenecks. This helped improve our runtime, and made our multithreadeed version better for the first 3 parameter mixes.
Below is our experiments with different number of reader threads since it only ever makes sense to have multiple reader threads and one writer thread (and we can only have one dispatcher). We did up to a total of 6 threads: 4 readers, 1 writer, 1 dispatcher since our computers are only quadcore and increasing the number of worker threads will do little to improve our throughput.
![chart]
[chart]: https://github.com/csharrison/firewall/blob/master/chart.png
## Conclusions
We saw that our multithreaded version did well for the first 3 parameter mixes. This is due to the fact that they're all CPU bound on the fingerprint method where most of the work is being done. The other ones are not faring as well because few to no packets are getting through due to a bug in our packet generator (which may have occured because we are importing the Java code to C code) causing no fingerprints to occur. This means no work is being done so the serial version fares better than the multithreaded version.
