#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
 
using namespace std;

size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata) {
      std::ostringstream *stream = (std::ostringstream*)userdata;
      size_t count = size * nmemb;
      stream->write(ptr, count);
      return count;
}
