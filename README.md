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

Currently the client doesn't work as expected because I could not find a good JSON library to parse the server response.

The server works as expected and can be tested once it is up and running (using the command above) using the following curl requests in terminal.

##### H5 PUT & GET Test
##### H6 Arrange
`curl -X PUT localhost:8080/key/val/10`
##### H6 Assert
`curl -X GET localhost:8080/key/val`

##### H5 PUT & DELETE Test
##### H6 Arrange
`curl -X DELETE localhost:8080/key/val`
##### H6 Assert
`curl -X GET localhost:8080/key/val`
The above line should return a JSON object saying
`{"error":"The key doesn't exist in cache"}`

##### H5 MEMUSED Test
`curl GET localhost:8080/memsize`

##### SHUTDOWN Test
`curl -X POST localhost:8080/shutdown`

The above command line command will shutdown the server.








