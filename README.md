# networkedmemcached
An implementation of a FIFO cache with a RESTful server and clients.

The server doesn't have any dependencies and should compile as it is without installing additional libraries. However dependign on the machine the code is run on the port and host have to be configured manually in the server.cc file.

You can start the server by running 

`g++ -std=c++11 -o output server.cpp cache.cc -lboost_system -lpthread`

and then 

`./output`

The client uses libcurl to make requests. If on OS X libcurl can be installed using

`brew install libcurl`

To compile the client side you can use the command

`g++ -std=c++11 cache.hh cache.cc write_data.hh write_data.cc tests.cc -lcurl`

Similar to the server the client can be run using

`./a.out`

which is the default output for compilation.
