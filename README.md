# Firewall
csharris and sxchen

## Architecture                                             
For our architecture, we decided to have one dispatcher thread that would process the packet and decide which type of worker thread to pass it onto - a writer or reader worker thread. We hold onto information about the threads with a PNG, R, Histogram that is universally shared by all the threads. The histogram holds onto the fingerprint results. PNG (persona-non-grata) tells us whether or not a source address is allowed to send a packet. R tells us the permissions for specific destination addresses and that source addresses they are willing to accept from. R is implemented as an array of skip lists of address ranges. These skip lists are protected by a coarse grained lock each. Each worker thread will have its own work queue that is implemented with a condition variable/mutex pair.

### Packets
Because we wrote this in C, we had to port the java packet generator code. After getting the packet, we have 2 types of packets: cpacket and dpacket. The cpackets are handled by reader threads and the dpackets are handled by writer threads.

### Dispatcher
One thread acts as a dispatcher. It processes the packet and decides whether or not to send it to a reader or writer thread. A dispatcher holds onto the PNG array, R, the histogram of the packets, an array of the reader threads and an array of the writer threads (and their associated info struct).
It has four methods:
* dispatcher_setup - Initializes the dispatcher and sets the number of readers and writers threads it will be sending to. Also sets up the other data structures that the dispatcher has knowledge of.  
* dispatcher_tear_down - Frees all the memory allocated to the data structures dispatcher has. 
* dispatcher_send_packet - Lets the throttler know we're sending a packet so that it will lock the mutex lock. Creates information about what type of thread we are sending the packet to and send its over. It choose which respective thread to send it to with a hash.
* dispatch - After getting the packet and deciding whether or not it is a cpacket (for reader) or dpacket (for writer), we send the packet. This method takes an optional parameter to "act serial" and process everything on the dispatcher thread (useful for serializing the architecture).

### Throttler
The throttler a light abstraction over an "in_flight" parameter shared by the worker threads and the dispatcher. This is used to ensure only MAXPACKETS can be in flight at the same time. Implementation wise this is just a mutex and an int. 
It has 5 methods:
* throttler_setup - Initializes the mutex lock and the number of inflights it can have.
* throttler_tear_down - Frees the mutex and throttler.
* throttler_send - Locks the mutex and increases the number of threads in flight. Then unlocks.
* throttler_receive - Locks the mutex and decreases the number of threads in flight. Then unlocks.
* throttler_can_send - Locks the mutex. Checks to see if we can have anymore threads inflight. Unlocks.

### Reader
Reader threads process dpackets. Each reacher thread has its information, its work queue, and a pointer to png, r, histogram, and throttler. It has 6 methods:
* reader_setup - Initializes all of its data structures.
* reader_tear_down - Frees the queue and the reader itself.
* reader_start - Given the reader info, it keeps dequeuing work until it's finished. It dequeues the work and processes the packet and alerts the throttler to let it know it's received a dpacket. 
* reader_send_packet - This is the method the dispatcher uses to send the packet to the reader. It enqueues it onto the reader's queue.
* process_dpacket - It checks to make sure the readerthread is allowed to process this packet (based on its source destination) by checking PNG. If so, it checks to see if R will accept it and updates the histogram. 
* fingerprint - It creates a fingerprint to put into the histogram. Our fingerprint method ORs the packet's seed number and a prime number for a huge number of iterations.

### Writer
Writer threads process cpackets. Each writer thread has its information, its work queue, and a pointer to png, r, and throttler. It has 5 methods:
* writer_setup - Initializes all of its data structures.  
* writer_tear_down - Frees the queue and the writer itself.
* writer_start - Given the writer info, it keeps dequeueing work until it's finished. It dequeues the work and processes the packet and alerts the throttler to let it know it's received a cpacket.
* writer_send_packet - This is the method the dispatcher uses to send the packet to the reader. It enqueues it onto the writer's queue.
* process_cpacket - It updates PNG with the correct permissions and updates r's new range.  

## Data Structures
### Histogram
The histogram holds onto the fingerprint results (which is a way of passing along the body, so we could hypothetically detect file sharing.) We have 2^16 spots in the array. All threads have access to this, and updates are atomic. It has 3 methods:
* hist_setup - It initializes the array and sets all initial fingerprints as 0.
* hist_tear_down - Frees the histogram.
* hist_update - It atomically gets and increments the histogram by 1 for each time it encounters a fingerprint. 

### PNG
PNG (persona-non-grata) tells us whether or not a source address is allowed to send a packet. It gets updated everytime we receive a config packet. Our PNG is just an array that is all initialized to all not allowed until told otherwise. All threads have access to this. It has 4 methods:
* png_setup - Initializes the array and sets all permissions to 0.
* png_tear_down - Frees all memory associated with PNG.
* png_allow - Atomically loads that the source address is now allowed to send packets.
* png_update - Atomically compare and swaps whether or not a packet is still allowed to send a packet.

### Queue
Each thread has its own single-enqueuer single-dequeuer work queue. For our queue structure, it has a buffer array, head pointer, tail pointer, size of the queue, a condition variable, and a mutex. It has 5 methods:
* squeue_setup - Initializes the queue. Sets the array to all 0 and the head and tail pointer to point to 0. Also creates the condition variable and the mutex. 
* squeue_tear_down - Frees the condition variable, mutex, the buffer aray, and the queue.
* squeue_enq - It makes sure the tail and head pointers are not pointing to an index greater than the size. It inserts the packet to the end of the q and then locks the mutex. It increments the tail and if the tail is equal to the head+1 (finally an item in the queue), the condition variable signals all the threads to let them know they can deq things. Then we unlock the mutex.
* squeue_deq - It makes sure the tail and head pointers are not pointing to an index greater than the size. It grabs the first item in the queue and then increments the queue's head. 
* squeue_deq_wait - It locks the mutex and while the queue is empty, the condition variable waits. Then it unlocks the mutex and since it's no longer waiting, we know we can dequeue. 

### R
R holds onto a set of allowed src addresses for every destination address. Each destination address points to a skip list with the address ranges they accept. It has 5 methods:
* r_setup - Initializes the skip list for each destination address and sets it to NULL.
* r_tear_down - Frees the skip list and R
* get_skip_list - Returns the skip list associated with the destination address in R.
* r_accept - It gets the skip list associated with a specific destination and then checks to see if the source address is has permissions to send to the destination address.
* r_update - It allows us to either remove or add addresses that the destination address is willing to accept packets from. It gets the skip list associated with a specific destination. If it's NULL (untouched), then we create a new skip list and atomically compare and swap the new range of source addresses into the allowed list. If it already has previous permissions, we then atomically add or remove this range of addresses depending on how we wanted to update R. 

#### Skip List
For R, the skiplist we implemented is a skip list of ranges following the textbook: _The Art of Multiprocessor Programming by Maurice Herlihy_, but modified (ours has the added complexity of dealing with adding and removing overlapping regions). It uses coarse grain locking. It has 5 methods:
* skip_list_setup - Initializes the skip list with a linked list, tail, head, and a read-write lock. The head pointer sets its next to point to the tail. It also seeds the random number generator. 
* skip_list_tear_down - It frees the linked list and the readwrite lock as well as the skip list.
* skip_list_contains - It checks to see if the skip list contains the source address (if it has the permissions). It locks the readwritelock while calling find and then unlocks it.
* skip_list_add_range - It adds a new range of source addresses that the destination address is willing to accept packagefs from. It locks the read write lock before and after completing it.
* skip_list_remove_range - It remove a range of source addresses that the destination address is willing to accept packagefs from. It locks the read write lock before and after completing it. 

Our Skip List also contains our linked list structure. 

## How To Run
```
make
./build/firewall <num_readers> <num_writers>
Optional Flags: 
* -s for only serial
* -m for only multithreaded
```
