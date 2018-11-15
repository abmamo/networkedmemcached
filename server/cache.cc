/*
 * Implementation for a generic cache object.
 * Data is given as blobs (void *) of a given size,
 * and indexed by a string key.
 */
#include <iostream>
#include "cache.hh"
#include <unordered_map>
#include <queue>
#include <cstring>
using namespace std;

class Cache::Impl {
  index_type maxmem_;
  evictor_type evictor_;
  hash_func hasher_;
  index_type memused_;
  // define queue to store keys in order
  queue<string> queue_;
  // define a data structure to store items and keys in
  unordered_map<string, void*> map_;

public:
  Impl(index_type maxmem, evictor_type evictor, hash_func hasher)
    : maxmem_(maxmem), evictor_(evictor), hasher_(hasher), memused_(0), queue_(), map_()
  {

  }

  ~Impl() = default;

  void set(key_type key, val_type val, index_type size)
  {
    if (size <= maxmem_) {
       // check if there is enough memory for the incoming value
       while (memused_ + size >= maxmem_) {
          // if not enough memory evict the first in element
          string eviction_key = queue_.front();
          // iterate through and find value of key
          auto search = map_.find(eviction_key);
          if (search != map_.end()) {
             auto val = search -> second;
             // decrememt memused
             memused_ = memused_ - sizeof(val);
             // update the mapping
             map_.erase(eviction_key);
             // update queue
             queue_.pop();
          }
       }
       // assign value in unordered map
       map_[key] = new val_type[size];
       // deep copy values into cache
       memcpy(map_[key],val,size);
       // increment memory used
       memused_ += size;
       // add key to queue
       queue_.push(key);
    }
  }

  // function to get value of a given key 
  val_type get(key_type key, index_type& size) const
  {
    // iterate through the unordered map to find the key
    auto search = map_.find(key);
    // if you have reached the end it means the key was not found in cache
    // if the key is found
    if (search != map_.end()){
      // get the value of the key
      return search->second;
    } else {
      // return a null pointer to be used by system (to bring value into cache or to get it from main memory)
      return nullptr;
    }
  }

  // function to remove a given key from our cache
  void del(key_type key)
  {
    // iterate through map to find the key
    auto search = map_.find(key);
    if (search != map_.end()) {
       // get the value of the key
       auto val = search->second;
       // decrement mem used
       memused_ = memused_ - sizeof(val);
       // remove key from mapping and queue
       map_.erase(key);
    }
  }

  // function to return the memory we have used so far 
  index_type space_used() const
  {
    return memused_;
  }
};

// Create a new cache object with a given maximum memory capacity.
Cache::Cache(index_type maxmem,
evictor_type evictor,
hash_func hasher)
  : pImpl_(new Impl(maxmem, evictor, hasher))
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
void 
Cache::set(key_type key, val_type val, index_type size)
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
void 
Cache::del(key_type key)
{
  pImpl_->del(key);
}

// Compute the total amount of memory used up by all cache values (not keys)
Cache::index_type 
Cache::space_used() const
{
  return pImpl_->space_used();
}
