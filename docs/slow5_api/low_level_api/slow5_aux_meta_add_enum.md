# slow5_aux_meta_add_enum

## NAME

slow5_aux_meta_add_enum - write a `SLOW5_ENUM` field as an auxiliary field in the SLOW5 header.

## SYNOPSYS

`int slow5_aux_meta_add_enum(struct slow5_aux_meta *aux_meta, const char *attr, enum slow5_aux_type type, const char **enum_labels, uint8_t enum_num_labels)`

## DESCRIPTION

`slow5_aux_meta_add_enum()` writes a *slow5_aux_type* `SLOW5_ENUM` to *aux_meta* with the attribute name *attr*. The label names and the number of labels are passed using *enum_labels* and *enum_num_labels* arguments respectively.

## RETURN VALUE

Return values are listed below. They might change in the future.
* `0` - success
* `-1` - null input
* `-2` - other failure
* `-3` - use slow5_aux_meta_add instead if type is not SLOW5_ENUM or SLOW5_ENUM_ARRAY
* `-4` - bad enum_labels not good c labels

## NOTES

The return values might change in the future.

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

    slow5_file_t *sp_w = slow5_init_empty("write.slow5", user_opts.arg_fname_out, SLOW5_FORMAT_ASCII);
    int ret0 = slow5_hdr_initialize(sp_w->header, 0);
    if(ret<0){
        fprintf(stderr,"could not initialize header");
        exit(EXIT_FAILURE);
    }

    
    slow5_aux_meta_t* aux_ptr = sp->header->aux_meta;
    uint32_t num_aux_attrs = aux_ptr->num;
    int aux_add_fail = 0;
    for(uint32_t r=0; r<num_aux_attrs; r++){
        if(aux_ptr->types[r]==SLOW5_ENUM || aux_ptr->types[r]==SLOW5_ENUM_ARRAY){
            uint8_t n;
            const char **enum_labels = (const char** )slow5_get_aux_enum_labels(slow5File_i->header, aux_ptr->attrs[r], &n);
            if(!enum_labels){
                aux_add_fail = 1;
            }else{
                if(slow5_aux_meta_add_enum(sp_w->header->aux_meta, aux_ptr->attrs[r], aux_ptr->types[r], enum_labels, n)){
                    aux_add_fail = 1;
                }
            }
        }
        if(aux_add_fail){
            fprintf(stderr,"Could not initialize the record attribute '%s'", aux_ptr->attrs[r]);
            exit(EXIT_FAILURE);
        }
    }
    slow5_close(sp_w);

    slow5_close(sp);

}
```

## SEE ALSO
[slow5_get_aux_enum_labels()](slow5_get_aux_enum_labels.md)