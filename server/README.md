This implementation of memcached is a FIFO implementation. It uses an unordered map for hashing and storing key values and a queue to keep track of first in elements. When the cache needs to evict a value it gets the front value from the queue which is the key of the first inserted element and deletes that key from unordered map. We then pop the key from our queue to reflect the eviction.

To compile the code use the command

   > g++ -std=c++11 -o output server.cpp cache.cc -lboost_system -lpthread
  
Running the cache.out file produced from the above command will run all tests automatically. If any tests fail it should throw an assert error.

The output from running the program on my machine produces the following table:

| Test Name          | Description            | Status  |
| ------------- |:-------------:| -----:|
| empty_cache_spaceused       | Return zero when spaceused is called on an empty cache | PASS |
| nonempty_cache_spaceused         | Return correct size of memused for a non empty cache | PASS |
| empty_cache_set       | Correctly insert a key value pair into an empty cache  | PASS |
| empty_cache_get          | Return a null pointer if asked to get on an empty cache | PASS |
| nonexistent_key_get       | Return a null pointer if asked to get a non existent key | PASS |
| nonempty_cache_del         | Remove a given key value pair from our cache | PASS |
| empty_cache_del       | Do nothing if asked to delete a value from an empty cache | PASS |
| nonexistent_cache_del         | Do nothing if asked to delete a non existent key  | PASS |
| evict_fifo      | Test if the first in element is evicted when space is needed | PASS |
