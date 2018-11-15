/*
 * Implementation for a generic cache object.
 * Data is given as blobs (void *) of a given size,
 * and indexed by a string key.
 */

#include "cache.hh"
#include "write_data.hh"
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <cstring>
#include <string>
#include <stdio.h>
#include <curl/curl.h>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

class Cache::Impl {
  index_type maxmem_;
  hash_func hasher_;

public:
  Impl(index_type maxmem, hash_func hasher)
    : maxmem_(maxmem), hasher_(hasher)
  {

  }

  ~Impl() = default;

  int set(key_type key, val_type val, index_type size) {
      // declare a curl object pointer
      CURL *curl;
      // declare a response object
      CURLcode res;

      // build the url
      // define stream variable
      ostringstream url_stream;
      // add the base url
      url_stream << "0.0.0.0:8080";
      // append the key
      url_stream << key;
      // append the value
      url_stream << val;
      // conver to C++ string
      string str = url_stream.str();
      // convert to char * since that's what libcurl accepts
      const char* url = str.c_str();

      // initialise curl object
      curl = curl_easy_init();
      // check if initialisation worked
      if (curl) {
          // set the request type to be PUT
          curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
          // pass the url
          curl_easy_setopt(curl, CURLOPT_URL, url);
          // send teh request
          res = curl_easy_perform(curl);
          // check the request was successful
          if (res != CURLE_OK)
              // if the response returned an error message output the message
              fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
          // clean up the curl object
          curl_easy_cleanup(curl);
      }
      return 0;
  }

  // function to get the value of a given key from the server
  val_type get(key_type key, index_type& size) const {
      // declare curl objec tpointer
      CURL *curl;
      // declare a response object
      CURLcode res;
      // define stream object to store response
      std::ostringstream stream;
      // define url base
      ostringstream temp_url;
      // concatenate base
      temp_url << "0.0.0.0:8080";
      // concatenate key
      temp_url << key;
      // convert to c string
      string str = temp_url.str();
      // conver to char *
      const char* url = str.c_str();
      
      // initialise curl object
      curl = curl_easy_init();
      // check if initialisation worked
      if (curl) {
          // pass the function that writes the response to a stream object
          curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
          // pass the pointer to the stream object to store the data
          // write_data writes
          curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream);
          // pass the url to the curl object
          curl_easy_setopt(curl, CURLOPT_URL, url);
          // send the request
          res = curl_easy_perform(curl);
          // check for errors
          if (res != CURLE_OK)
              // display the error
              fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
          // clean up curl
          curl_easy_cleanup(curl);
          // save the response to a varible
          auto response = json::parse(stream.str());
          // process the response data
          // have to cast it to void * because of val_type
          static const val_type e = (void *)response.value(key, 0);
          return e;

      }
      val_type res_value = "";
      return res_value;
  }

  // function to remove a given key from our cache
  int del(key_type key) {
      // initialise curl object
      CURL *curl;
      // initialise response object
      CURLcode res;
      // construct url
      ostringstream temp_url;
      temp_url << "0.0.0.0:8080";
      temp_url << key;
      string str = temp_url.str();
      const char* url = str.c_str();

      // initialise curl
      curl = curl_easy_init();
      // check if initialisation worked
      if (curl) {
          curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
          curl_easy_setopt(curl, CURLOPT_URL, url);
          /* Perform the request, res will get the return code */
          res = curl_easy_perform(curl);
          /* Check for errors */
          if (res != CURLE_OK)
             fprintf(stderr, "curl_easy_perform() failed: %s\n",
                   curl_easy_strerror(res));
 
         /* always cleanup */
         curl_easy_cleanup(curl);
      }
      return 0;
  }


  // function to return the memory we have used so far 
  index_type space_used() const
  {
      // define CURL object
      CURL *curl;
      // define response object
      CURLcode res;
      // define stream object to store response
      std::ostringstream stream;
      // define url
      const char * url = "0.0.0.0:8080/memsize";

      // initialise curl
      curl = curl_easy_init();
      // check if initialisation worked
      if (curl) {
         // pass the function that writes the response to a stream object
             curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
             // pass the pointer to the stream object to store the data
             // write_data writes
             curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream);
             // pass the url to the curl object
             curl_easy_setopt(curl, CURLOPT_URL, url);
             // send the request
             res = curl_easy_perform(curl);
             // check for errors
             if (res != CURLE_OK)
                 // display the error
                 fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
             // clean up curl
             curl_easy_cleanup(curl);
             // save the response to a varible
             auto response = json::parse(stream.str());
             // process the response data
             int e = response.value("memused", 0);
             return e;
      }
      index_type res_value;
      return res_value;
  }
};

// Create a new cache object with a given maximum memory capacity.
Cache::Cache(index_type maxmem,
hash_func hasher)
  : pImpl_(new Impl(maxmem, hasher))
{

}

Cache::~Cache()
{
}


// Add a <key, value> pair to the cache.
// If key already exists, it will overwrite the old value.
// Both the key and the value are to be deep-copied (not just pointer copied).
// If maxmem capacity is exceeded, sufficient values will be removed
// from the cache to accomodate the new value.
int Cache::set(key_type key, val_type val, index_type size)
{
  pImpl_->set(key,val,size);
}

// Retrieve a pointer to the value associated with key in the cache,
// or NULL if not found.
// Sets the actual size of the returned value (in bytes) in val_size.
Cache::val_type 
Cache::get(key_type key, index_type& val_size) const
{
  return pImpl_->get(key,val_size);
}

// Delete an object from the cache, if it's still there
int Cache::del(key_type key)
{
  pImpl_->del(key);
}

// Compute the total amount of memory used up by all cache values (not keys)
Cache::index_type 
Cache::space_used() const
{
  return pImpl_->space_used();
}
