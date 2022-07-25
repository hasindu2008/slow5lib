# slow5_get_aux_enum_labels

## NAME

slow5_get_aux_enum_labels - gets the enum labels for a specific auxiliary field.

## SYNOPSYS

`char **slow5_get_aux_enum_labels(const slow5_hdr_t *header, const char *field, uint8_t *n)`

## DESCRIPTION

`slow5_xx()` fetches a record from a SLOW5 file *s5p* for a specified *read_id* into a *slow5_rec_t* and stores the address of the *slow5_rec_t* in **read*.

`slow5_get_aux_enum_labels` returns a list of labels specified by the auxiliary enumeration field *field* in SLOW5 header *header*. The number of labels is set in *n*.


## RETURN VALUE

Return `NULL` on error and `slow5_errno` is set to following errors.

## ERRORS

* `SLOW5_ERR_ARG` - if header, field NULL, n can be NULL
* `SLOW5_ERR_NOAUX` - if auxiliary header is NULL
* `SLOW5_ERR_TYPE` - if the enum labels or num_labels array is NULL, or the field type is not an enum type
* `SLOW5_ERR_NOFLD` - if the auxiliary field was not found
* `SLOW5_ERR_MEM` - memory allocation error

## NOTES


## EXAMPLES

```
#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

#define FILE_PATH "examples/example.slow5"

int main(){

    slow5_file_t *sp = slow5_open(FILE_PATH,"r");
    if(sp==NULL){
       fprintf(stderr,"Error in opening file\n");
       exit(EXIT_FAILURE);
    }
    
    slow5_aux_meta_t* aux_ptr = sp->header->aux_meta;
    uint32_t num_aux_attrs = aux_ptr->num;
    int aux_add_fail = 0;
    for(uint32_t r=0; r<num_aux_attrs; r++){
        if(aux_ptr->types[r]==SLOW5_ENUM || aux_ptr->types[r]==SLOW5_ENUM_ARRAY){
            uint8_t n;
            const char **enum_labels = (const char** )slow5_get_aux_enum_labels(slow5File_i->header, aux_ptr->attrs[r], &n);
            if(enum_labels){
                fprintf(stdout,"Auxiliary field %s has following enum labels:\n", aux_ptr->attrs[r]);
                for(uint8_t i; i<n; i++){
                    fprintf(stdout,"%s\n", enum_labesl[i]);
                }
            }
        }
    }

    slow5_close(sp);

}
```

## SEE ALSO
[slow5_aux_meta_add_enum()](slow5_aux_meta_add_enum.md)