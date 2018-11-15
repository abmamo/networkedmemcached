#include <iostream>
#include <string>
#include "crow.h"
#include "cache.hh"
#include "date.h"
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
           try {
             // define response object
             crow::response resp;
             // define json object to hold the key value pair
             crow::json::wvalue body;
             // set header information
             resp.code = 200;
             resp.add_header("Content-Type", "application/json");
             resp.add_header("Accept", "text/html");
             // get current date as string
             string timestamp = date::format("%F %T", std::chrono::system_clock::now());
             // add date to header
             resp.add_header("Date", timestamp);
             resp.add_header("HTTP", "HTTP/1.1");
             // close the connection after the request is complete
             resp.add_header("Connection", "Close");
 
             // get value from cache
             Cache::val_type address = cache_ -> get(k, size);
             // handle the key not being in the cache
             if (address == nullptr) {
                body["error"] = "The key doesn't exist in cache";
                resp.write(crow::json::dump(body));
                return resp;
             } 
             else 
             {
                // key is the value the user passed
                body["key"] = k;
                // deep copy value from address
                int* address_data = new int[1];
                memcpy(address_data, address, size);
                // set the value in our json response
                body["value"] = *address_data;
                // write json to the body of the response
                resp.write(crow::json::dump(body));
                return resp;
             }  
          } catch (const std::exception& e) {
               // return 500 response if server fails to get the mem used
               return crow::response(500);
           }
        }
        else if (req.method == "HEAD"_method)
        {
            try {
                // define response object
                crow::response resp;
                // define json object to hold the key value pair
                crow::json::wvalue body;
                // set header information
                resp.code = 200;
                resp.add_header("Content-Type", "application/json");
                resp.add_header("Accept", "text/html");
                // get current date as string
                string timestamp = date::format("%F %T", std::chrono::system_clock::now());
                // add date to header
                resp.add_header("Date", timestamp);
                resp.add_header("HTTP", "HTTP/1.1");
                // close the connection after the request is complete
                resp.add_header("Connection", "Close");
                return resp;

             } catch (const std::exception& e) {
                // return 500 response if server fails to get the mem used
                return crow::response(500);
             }

        }
        else if (req.method == "DELETE"_method)
        {
             try {
                // define response object
                crow::response resp;
                // delete from cache
                cache_->del(k);
                // set header information
                resp.code = 200;
                resp.add_header("Content-Type", "application/json");
                resp.add_header("Accept", "text/html");
                // get current date as string
                string timestamp = date::format("%F %T", std::chrono::system_clock::now());
                // add date to header
                resp.add_header("Date", timestamp);
                resp.add_header("HTTP", "HTTP/1.1");
                // close the connection after the request is complete
                resp.add_header("Connection", "Close");
                return resp;
                
             } catch (const std::exception& e) {
                // return 500 response if server fails to get the mem used
                return crow::response(500);
             }
       }
       else {
           // other methods are not allowed return a 405 code if user tries to use them
           return crow::response(405);
        }
    });

    // create route for getting memory used
    CROW_ROUTE(app, "/memsize")
    // define route type as get
    .methods("GET"_method)
    ([&]() {
        // try to get the space used so far
        try {
            // define response object
            crow::response resp;
            // define json object to hold the key value pair
            crow::json::wvalue body;
            // set header information
            resp.code = 200;
            resp.add_header("Content-Type", "application/json");
            resp.add_header("Accept", "text/html");
            // get current date as string
            string timestamp = date::format("%F %T", std::chrono::system_clock::now());
            // add date to header
            resp.add_header("Date", timestamp);
            resp.add_header("HTTP", "HTTP/1.1");
            // close the connection after the request is complete
            resp.add_header("Connection", "Close");
            // set json response
            body["memused"] = cache_ -> space_used();
            // write the json object to the body of the string
            resp.write(crow::json::dump(body));
            return resp;
        } catch (const std::exception& e) {
            // return 500 response if server fails to get the mem used
            return crow::response(500);
        }
    });

    // create route for putting key value pair in cache
    CROW_ROUTE(app, "/key/<string>/<string>")
    // define route type as put
    .methods("PUT"_method)
    ([&](string k, string v) {
       // try setting the value if not return a 404 error
       try {
           // define response object
           crow::response resp;
           // insert value into cache
           cache_ -> set(k, &v, size);
           // save key to be emptied later
           list_.push_back(k);
           // set header information
           resp.code = 201;
           resp.add_header("Content-Type", "application/json");
           resp.add_header("Accept", "text/html");
           // get current date as string
           string timestamp = date::format("%F %T", std::chrono::system_clock::now());
           // add date to header
           resp.add_header("Date", timestamp);
           resp.add_header("HTTP", "HTTP/1.1");
           // close the connection after the request is complete
           resp.add_header("Connection", "Close");
           return resp;
 
       } catch (const std::exception& e) {
           // return 500 response if the put request failed
           return crow::response(500);
       }
    }); 

    // create route for safely resetting the cache
    CROW_ROUTE(app, "/shutdown")
    // define route type as delete
    .methods("POST"_method)
    ([&]() {
       try {
         // define response object
         crow::response resp;
         // set header information
         resp.code = 200;
         resp.add_header("Content-Type", "application/json");
         resp.add_header("Accept", "text/html");
         // get current date as string
         string timestamp = date::format("%F %T", std::chrono::system_clock::now());
         // add date to header
         resp.add_header("Date", timestamp);
         resp.add_header("HTTP", "HTTP/1.1");
         // close the connection after the request is complete
         resp.add_header("Connection", "Close");
 

         // get the beginning of the list where we stored all keys
         auto it = list_.begin();
         // iterate through all the keys in that list and delete them from cache
         for (it=list_.begin(); it!=list_.end(); ++it)
           // delete each key from cache
           cache_ -> del(*it);
         // free cache pointer
         free(cache_);
         // return an accepted status code
         return resp;
         // stop server
         app.stop(); 
       } catch (const std::exception& e) {
         // if we fail to shutdown the server return 500 error code
         return crow::response(500);
       }
    });

    app.port(8080).multithreaded().run();
} 
