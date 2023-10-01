# Replicated Block Store.

## GPRC install path  
```
export MY_INSTALL_DIR=$HOME/.local  
export PATH="$MY_INSTALL_DIR/bin:$PATH"  
```
## To Run

```
cd replicated_block_store  
mkdir build   
cd build  
cmake -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../  
make -j  
cd bin  
./server  
./client  
```
1.1 Replication Strategy

* Replication strategy = Primary-Backup (1 Backup server only)
* Clients can access either of the 2 servers. However, the Backup
will redirect clients to the Primary and not service any requests
itself.
* When does replication happen? - On each WRITE request the Primary
receives (block_server.cpp#L140)
* Crash invisibility = hidden by our client library which tries to
contact the other server on failure (client_library.cpp#L63)

1.2 Durability
* pwrite() to one large 256GB file, fsync() after each write
to maintain durability (local_read_write.cpp#L19)

1.3 Crash Recovery Protocol
* All requests are served only by the Primary. To ensure strong consistency,
all WRITES are serialized at the Primary (via a mutex - block_server.cpp#L121).
Once the lock is acquired, the WRITE is performed locally on the Primary, then
on the Backup and only on success of both does the client get an ACK.
* When one of the nodes crashes, we have 2 cases - 
	a. Primary Crash : In this case, Backup takes over and becomes the Primary
	                   when it receives a request (block_server.cpp#L85)
	b. Backup Crash : In this case, the Primary keeps track of all modified 
                      block numbers during WRITES(block_server.cpp#L142) and 
					  retransmits these blocks to the Backup during RESYNC

2.1 Correctness
* Availability - Crash hidden from user - https://docs.google.com/file/d/1UmBQEqQj2KiWeosc7DhD2qFE1iZgOBXX/preview
* Strong Consistency - Read back latest value - https://docs.google.com/file/d/14NXFGE0vecb-l6I4U7HZlxUZ4Vib3WIh/preview
Other video links - RESYNC process (https://docs.google.com/file/d/1fqPV0cyLNjER_dgmSKBd9NvxN-tLg5Hg/preview)
				  - Fail-over process (https://docs.google.com/file/d/1uUf8F3JcSq4kuZT8S4bBL0BvT1p-eo6-/preview)
* Testing Strategy - Wrote random data to multiple overlapping/non-overlapping blocks and emulated the writes on a local in-memory buffer.
                     Then read the data back and compared it with our local buffer (client.cpp#L18).
					 For multiple clients, used carefully chosen delays() to get a deterministic order.
					 Injected crash by using a special address for crashing. Verified correct output by
					 comparing result with local buffer.

2.2 Performance
* Latency, 2 servers - Read = 885 us, Write = 38602 us
* Latency, 1 server - Read = 894 us, Write = 22929 us
* Fail-over time - 16ms, RESYNC time - 256ms
* No performance difference b/w aligned and unaligned requests
* Please refer to the report for a detailed explanation
