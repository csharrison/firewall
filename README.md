bloomer
=======

A multi-threaded packet filter using bloom filters

Configuration Packets: 
one thread acts as dispatcher so lock free
	data packet that takes in the source
	R[D] is the set of source addresses
only 256 at a time
png = whether it's allowed through. just a set. buckethash. initialized to some value.
R = hashmap. needs padding. every element of R points to a linked list/small set?
		R stores a set of ranges of sources. need a datastructure that can store a set of ranges. needs to be amenable to adding and subtracting a range.
need to make processes done atomically/as though they were processed serially - may need a lock to do this.
	REMEMBER TO SHOW CORRECTNESS IN REPORT

what's in config:
-address (addr)
-whether it is permitted to send packets(PNG)
-Range
-Accept

need to update PNG when you get a config packet. 
Config -> Dispatcher (which hashes the S,D to whatever number thread value it is and send it to the thread. may need to use another service to hash the config stuff which needs a lock?) will CAS PNG, take lock on R to update (have to use a mutex to edit the part of R )

PNG -> just malloc onto the heap with dipatcher and all threads just know about it. (do a CAS. just make it atomic.)

use readers-writers lock for R. (writer needs to wait until all the readers are off. readers won't read anything valid until writer finished)

need histogram of fingerprint results (which is a way of passing along the body, so we can make sure nothing is being sent over twice. kinda like preventing pirating) we have 0-2^16 spots in the array. use getAndIncrement to keep count of how many times that fingerprint has occured. all threads will have to have access to this. make it so you can do get and add. make it size 2^16. 

typedef our_int = size 16

separate file for histogram, separate file for R, separate file for PNG