#include <iostream>
#include <string>
#include "crow.h"
#include "cache.hh"
#include <list>


using namespace std;

int main()
{
    // get user input for cache size
    uint32_t cache_length = 500;
    Cache::index_type size = sizeof(uint32_t);
    Cache* cache_ = new Cache(cache_length*size);
    // define set to store keys for deletion later
    list<string> list_;

    crow::SimpleApp app;

    // create route for getting key value pair
    CROW_ROUTE(app, "/key/<string>")
    // define method type as get
    .methods("GET"_method, "HEAD"_method, "DELETE"_method)
    // use auto to cast whatever value the user passes
    // use & to capture pointers since this is a lambda function
    ([&](const crow::request& req, string k) {
        // handle get requests
        if (req.method == "GET"_method)
        {
           // define json object to return key value pair
           crow::json::wvalue resp;
           // get value from cache
           Cache::val_type address = cache_ -> get(k, size);
           // handle the key not being in the cache
           if (address == nullptr) {
              resp["error"] = "The key doesn't exist in cache";
              return resp;
           } 
           else 
           {
              // key is the value the user passed
              resp["key"] = k;
              // deep copy value from address
              int* address_data = new int[1];
              memcpy(address_data, address, size);
              // set the value in our json response
              resp["value"] = *address_data;
              // return json object
              return resp;
           }  
        }
        else if (req.method == "HEAD"_method)
        {
           // return empty body with header
           crow::json::wvalue resp;
           return resp;
        }
        else if (req.method == "DELETE"_method)
        {
           // define json object
           crow::json::wvalue resp;
           // delete from cache
           cache_->del(k);
           // return empty response body
           return resp;
        }
        else {
           // condition to avoid returning a non void return etatement
           crow::json::wvalue resp;
           resp["error"] = "The request method is not allowed.";
           return resp;
        }
    });

    // create route for getting memory used
    CROW_ROUTE(app, "/memsize")
    // define route type as get
    .methods("GET"_method)
    ([&]() {
        // define json object to return key value pair
        crow::json::wvalue resp;
        // set json response
        resp["memused"] = cache_ -> space_used();
        // return json object
        return resp;
    });

    // create route for putting key value pair in cache
    CROW_ROUTE(app, "/key/<string>/<string>")
    // define route type as put
    .methods("PUT"_method)
    ([&](string k, string v) {
       // try setting the value if not return a 404 error
       try {
           // insert value into cache
           cache_ -> set(k, &v, size);
           // save key to be emptied later
           list_.push_back(k);
           // return 200 response
           return crow::response(200);
       } catch (const std::exception& e) {
           // return 404 response if the put request failed
           return crow::response(404);
       }
    }); 

    // create route for safely resetting the cache
    CROW_ROUTE(app, "/shutdown")
    // define route type as delete
    .methods("POST"_method)
    ([&]() {
       // get the beginning of the list
       auto it = list_.begin();
       // iterate through all the keys and delete them from cache
       for (it=list_.begin(); it!=list_.end(); ++it)
           // delete each key
           cache_ -> del(*it);
       // free iterator
       // free cache
       free(cache_);
       // end server
       app.stop();
       // create response saying the server has been closed
       return "Server has been shut down";
    });

    app.port(8080).multithreaded().run();
} 
