# slow5lib easy multi-thread API

**NOTE: This API documentation is sparse. If something is not clear do not hesitate to open an [issue](https://github.com/hasindu2008/slow5lib/issues).**

The easy multi-thread (called *mt* API) API allows the user to access SLOW5 files using multiple threads, simply by specifying the number of threads, rather than needing to write their own multi-threading code. This API is for programmer's convenience, rather than for efficiency. This API is not meant to be used by a programmer who has the expertise to write multi-threaded code and use the slow5 low-level API directly. While fetching using this API would be faster than using a single thread, it would not be as efficient as using low-level API functions directly from the programmer's own multi-threaded code.

To use this API, include <slow5/slow5_mt.h> in your programme. Make sure to explicitly enable easy multi-thread API as `make slow5_mt=1`, during compile time of slow5lib.

## Data structures

The *slow5_mt_t* structure stores internal multi-threading related data. This structure has the following form:

```
typedef struct {
    // private members that are not to be directly accessed*/
} slow5_mt_t;
```

The *slow5_batch_t* structure stores a batch of SLOW5 records. This structure has the following form:

```
typedef struct {
    // private members that are not to be directly accessed*/
    slow5_rec_t **slow5_rec;
} slow5_batch_t;
```

## API functions

### Common

* [slow5_init_mt](mt_api/slow5_init_mt.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;initialises a slow5 multi-thread structure
* [slow5_free_mt](mt_api/slow5_free_mt.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;frees up a slow5 multi-thread structure
* [slow5_init_batch](mt_api/slow5_init_batch.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;initialises a slow5 record batch
* [slow5_free_batch](mt_api/slow5_free_batch.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;frees up a slow5 record batch

### Reading and access

* [slow5_get_next_batch](mt_api/slow5_get_next_batch.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;fetches the next batch of records at the current file pointer of a SLOW5 file
* [slow5_get_batch](mt_api/slow5_get_batch.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;fetches a batch of SLOW5 records corresponding to a given batch of read IDs


### Writing and editing

* [slow5_write_batch](mt_api/slow5_write_batch.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;writes a batch of SLOW5 records to a SLOW5 file


You may also refer to the examples [here](https://github.com/hasindu2008/slow5lib/tree/master/examples/mt) on how these functions can be used.
