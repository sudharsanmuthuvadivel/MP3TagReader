#ifndef VIEW_H
#define VIEW_H

#include <stdio.h>
#include <stdlib.h>

/*Contain user define data types from type.h file*/
#include "types.h"

/*Structure to store datas for view .mp3 file tags*/
typedef struct Mp3TagInfo
{
    /*MP3 Source file*/
    FILE *fptr_mp3;
    char *audio_file_name;
    char frame_id[MAXI_FRAME_ID_SIZE + 1];

    /*Title Info*/
    uint title_tag_size;
    char *title_tag_id;

    /*Artist Info*/
    uint artist_tag_size;
    char *artist_tag_id;

    /*Album Info*/
    char *album_tag_id;
    uint album_tag_size;

    /*year of release info*/
    char *year_tag_id;
    uint year_tag_size;

    /*content type Info*/
    char *content_tag_id;
    uint content_type_size;

    /*comment info*/
    char *comment_tag_id;
    uint comment_size;

} TagInfo;

/*main function to view mp3 tags*/
Status mp3_file_tag_view(char **agrv, TagInfo *tag_info);

/*Function to read and validate mp3 file for tag view */
Status read_and_validate_mp3_file(char **argv, TagInfo *tag_info);

/*Reusable Function to read data from .mp3 file and display the data*/
Status read_and_display_data(const char *str_frame, const char *str_id, char *frame_id, uint *tag_size, char *tag, FILE *fptr_mp3);

#endif