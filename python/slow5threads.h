/* @file slow5threads.h
**
******************************************************************************/

#ifndef SLOW5THREADS_H
#define SLOW5THREADS_H

int slow5_get_batch(slow5_rec_t ***read, slow5_file_t *s5p, char **rid, int num_rid, int num_threads);
int slow5_get_next_batch(slow5_rec_t ***read, slow5_file_t *s5p, int batch_size, int num_threads);
int slow5_write_batch(slow5_rec_t **read, slow5_file_t *s5p, int batch_size, int num_threads);
void slow5_free_batch(slow5_rec_t ***read, int num_rec);

#endif
