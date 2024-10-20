#ifndef EDIT_H
#define EDIT_H
#include<stdio.h>
#include<stdlib.h>

#include "types.h"  // including types header files

typedef struct Mp3TagData
{
    FILE *fptr_mp3;  // file pointer to handle orginal .mp3 file
    
    char tag_id[MAXI_FRAME_ID_SIZE + 1]; // store tag_id
    uint tag_id_size;   // store Tag_id data size
    char tag_id_data[100];  // store updated data

    FILE *fptr_temp;  // file pointer to handle temporary audio file
}TagData;

/*Main function to contains all function for edit option*/
Status edit_tag(char** argv, TagData *tag_data);

/*Function to read and validate user input in comment line */
Status read_and_validate_file(char **argv, TagData *tag_data);

/*Reusable Function to copy tag_id details from orginal.mp3 file to temp audion file*/
Status copy_mp3_data(TagData* tag_data, int flag);

/*Function to copy remaing data(apart from tag_id, size, data) form orginal.mp3 to temp file*/
Status copy_reamaining_mp3_data(TagData *tag_data);

/*Fucntion to copy all the datas from temp file to orginal .mp3 file*/
Status copy_data_back_to_source(char **argv, TagData* tag_data);

#endif 
