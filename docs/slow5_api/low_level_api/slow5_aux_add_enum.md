# slow5_aux_add_enum

## NAME

slow5_aux_add_enum -  adds an auxiliary field of type enum to a SLOW5 header

## SYNOPSYS

`int slow5_aux_add_enum(const char *field, const char **enum_labels, uint8_t num_labels, slow5_hdr_t *header)`

## DESCRIPTION
`slow5_aux_add_enum()` adds an auxiliary field named *field* of the datatype *enum* whose enum labels are pointed by *enum_labels* to a SLOW5 file header pointed by *header*.

The number of enum labels in should be in *num_labels*.

The argument *header* points to a SLOW5 header of type *slow5_hdr_t* and typically this is the *s5p->header* member inside the *slow5_file_t \*s5p* returned by  `slow5_open()`.

## RETURN VALUE
Upon successful completion, `slow5_aux_add_enum()` returns a non negative integer (>=0). Otherwise, a negative value is returned.

## ERRORS

A negative value is returned when an error occurs and can be due to following occasions (not an exhaustive list):

- input parameter is NULL
- enum value is invalid
- other error

## NOTES

In the future `slow5_errno` will be set to indicate the error.

## EXAMPLES

```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <slow5/slow5.h>

#define FILE_PATH "test.blow5"

int main(){

    //open the SLOW5 file for writing
    slow5_file_t *sp = slow5_open(FILE_PATH, "w");
    if(sp==NULL){
        fprintf(stderr,"Error opening file!\n");
        exit(EXIT_FAILURE);
    }

    /*********************** Header ******************/
    /*
    @run_id	run_id_0
    */
    slow5_hdr_t *header=sp->header; //pointer to the SLOW5 header

    //add a header group attribute called run_id
    if (slow5_hdr_add("run_id", header) < 0){
        fprintf(stderr,"Error adding run_id attribute\n");
        exit(EXIT_FAILURE);
    }

    //set the run_id attribute to "run_0" for read group 0
    if (slow5_hdr_set("run_id", "run_0", 0, header) < 0){
        fprintf(stderr,"Error setting run_id attribute in read group 0\n");
        exit(EXIT_FAILURE);
    }

    /*
    enum{unknown,partial,mux_change,unblock_mux_change,signal_positive,signal_negative}
    end_reason
    */
    const char *enum_labels[] = {"unknown", "partial", "mux_change", "unblock_mux_change", "signal_positive", "signal_negative"};
    uint8_t num_labels = 6;
    if (slow5_aux_add_enum("end_reason", enum_labels, num_labels, sp->header) < 0){
        fprintf(stderr,"Error adding end_reason auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    if(slow5_hdr_write(sp) < 0){
        fprintf(stderr,"Error writing header!\n");
        exit(EXIT_FAILURE);
    }


    /******************* A SLOW5 record ************************/
    slow5_rec_t *slow5_record = slow5_rec_init();
    if(slow5_record == NULL){
        fprintf(stderr,"Could not allocate space for a slow5 record.");
        exit(EXIT_FAILURE);
    }

    /* primary fields
    #read_id	read_group	digitisation	offset	range	sampling_rate	len_raw_signal	raw_signal
    read_0	    0	        4096	        3	    10	    4000	        10	            0,1,2,3,4,5,6,7,8,9
    */
    slow5_record -> read_id = strdup("read_0");
    if(slow5_record->read_id == NULL){
        fprintf(stderr,"Could not do strdup.");
        exit(EXIT_FAILURE);
    }
    slow5_record-> read_id_len = strlen(slow5_record -> read_id);
    slow5_record -> read_group = 0;
    slow5_record -> digitisation = 4096.0;
    slow5_record -> offset = 3.0;
    slow5_record -> range = 10.0;
    slow5_record -> sampling_rate = 4000.0;
    slow5_record -> len_raw_signal = 10;
    slow5_record -> raw_signal = (int16_t *)malloc(sizeof(int16_t) * slow5_record->len_raw_signal);
    if(slow5_record->raw_signal == NULL){
        fprintf(stderr,"Could not allocate space for raw signal.");
        exit(EXIT_FAILURE);
    }
    for(int i=0; i<slow5_record->len_raw_signal; i++){
        slow5_record->raw_signal[i] = i;
    }

    /* auxiliary fileds
    end_reason
    1
    */
    uint8_t end_reason = 1;
    if(slow5_aux_set(slow5_record, "end_reason", &end_reason, sp->header) < 0){
        fprintf(stderr,"Error setting end_reason auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    //write to file
    if(slow5_write(slow5_record, sp) < 0){
        fprintf(stderr,"Error writing record!\n");
        exit(EXIT_FAILURE);
    }

    //free the slow5 record
    slow5_rec_free(slow5_record);

    //close the SLOW5 file
    slow5_close(sp);

    return 0;

}

```

## SEE ALSO
[`slow5_aux_set()`](../slow5_aux_set.md).
