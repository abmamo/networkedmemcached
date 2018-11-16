// guards
#ifndef WRITE_DATA_HEADER_
#define WRITE_DATA_HEADER_

#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
 
using namespace std;

// function to be used by libcurl to write response data to variable
size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata);

#endif
