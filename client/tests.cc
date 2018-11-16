#include <cassert>
#include <iostream>
#include <string>
#include <cstring>

#include "cache.hh"

using namespace std;

// function to test space used for empty cache returns 0
void empty_cache_spaceused() {
    // Arragen
    uint32_t cache_length = 500;
    Cache::index_type size = sizeof(uint32_t);
    // Act
    Cache* c = new Cache(cache_length*size);
    // Assert
    assert(c->space_used() == 0 && "Memused for an empty cache is different from 0");
}

// function to test space used for a non empty cache
void nonempty_cache_spaceused() {
     // Arrange
    uint32_t cache_length = 500;
    Cache::index_type size = sizeof(uint32_t);
    Cache* c = new Cache(cache_length*size);
    // Act
    Cache::key_type key = "val";
    int val = 18;
    c->set(key, &val, sizeof(val));
    cout << sizeof(val);
    // Assert
    assert(c->space_used() != 0 && "Memused for non empty cache is different from the sum of size of values" );     
}


// function to test inserting element into an empty cache
void empty_cache_set() {
     // Arrange
    uint32_t cache_length = 500;
    Cache::index_type size = sizeof(uint32_t);
    Cache* c = new Cache(cache_length*size);
    // Act
    Cache::key_type key = "val";
    int val = 18;
    c->set(key, &val, sizeof(val));
    // Assert
    Cache::val_type address = c -> get(key, size);
    // copy the value from address
    int* address_data = new int[1];
    memcpy(address_data, address, size);
    assert(val == *address_data && "set maps incorrect value to key in an empty cache");      
}

// function to test getting from a non empty cache
void nonempty_cache_get() {
    // Arrange
    uint32_t cache_length = 500;
    Cache::index_type size = sizeof(uint32_t);
    Cache* c = new Cache(cache_length*size);
    // Act
    Cache::key_type key = "val";
    int val = 18;
    c->set(key, &val, size);

    Cache::val_type address = c -> get(key, size);
    // copy the value from the address returned
    // use this assignment to prevent segmentation fault
    int* address_data = new int[1];
    memcpy(address_data, address, size);
    // Assert
    assert(val == *address_data && "set maps incorrect value to key in a non empty cache");
}

// function to test get return null for an empty cache
void empty_cache_get() {
    // Arrange
    uint32_t cache_length = 500;
    Cache::index_type size = sizeof(uint32_t);
    Cache* c = new Cache(cache_length*size);
    // Act
    Cache::key_type key = "val";

    Cache::val_type address = c -> get(key, size);
    // Assert
    assert(address == nullptr);
}

// function to test we return a null pointer for a non existent key
void nonexistent_key_get() {
   // Arrange
   uint32_t cache_length = 500;
   Cache::index_type size = sizeof(uint32_t);
   Cache * c = new Cache(cache_length * size);
   // Act
   Cache::key_type key = "val";
   Cache::key_type nonexistent_key = "valll";
   int val = 18;
   c->set(key, &val, size);

   Cache::val_type address = c -> get(nonexistent_key, size);

   // Assert 
   assert(address == nullptr);

}

// function to test delete function of a non empty cache
void nonempty_cache_del() {
    // Arrange
    uint32_t cache_length = 500;
    Cache::index_type size = sizeof(uint32_t);
    Cache* c = new Cache(cache_length*size);
    // Act
    Cache::key_type key = "val";
    int val = 18;
    c->set(key, &val, size);
    c->del(key);
    
    Cache::val_type address_two = c -> get(key, size);
    // Assert
    assert(address_two == nullptr && "del filed to remove key value pair from cache");
}

// function to test delete function in an empty cache
void empty_cache_del() {
   // Arrange
   uint32_t cache_length = 500;
   Cache::index_type size = sizeof(uint32_t);
   Cache* c = new Cache(cache_length * size);
   // Act
   Cache::key_type key = "val";
   c -> del(key);
}

// function to test the eviction of the first eleemnt
void evict_fifo() {
    // Arrange
    uint32_t cache_length = 2;
    Cache::index_type size = sizeof(uint32_t);
    Cache* c = new Cache(cache_length*size);

    Cache::key_type key_one = "val_one";
    Cache::key_type key_two = "val_two";
    Cache::key_type key_three = "val_three";
    Cache::key_type key_four = "val_four";

    int val_one = 18;
    int val_two = 180;
    int val_three = 18;
    int val_four = 22;
    // Act
    c->set(key_one, &val_one, size);
    c->set(key_two, &val_two, size);
    c->set(key_three, &val_three, size);
    c->set(key_four, &val_four, size);
    // try to get evicted value
    Cache::val_type address = c -> get(key_one, size);
    // Assert
    assert(address == nullptr && "first in element not evicted when there is not enough memory");
    
}

int main() {
    empty_cache_spaceused();
    nonempty_cache_spaceused();
    //empty_cache_set();
    //nonempty_cache_get();
    //empty_cache_get();
    //nonempty_cache_del();
    //evict_fifo();
}

