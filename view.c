#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "view.h"

static void display();
Status read_and_validate_mp3_file(char **argv, TagInfo *tag_info)
{
    if (argv[2] != NULL)
    {
        tag_info->audio_file_name = argv[2];
        tag_info->fptr_mp3 = fopen(tag_info->audio_file_name, "r");
        if (tag_info->fptr_mp3 == NULL)
        {
            printf("Error: Unable to open the file %s\n", tag_info->audio_file_name);
            return mp3_failure;
        }
        else
        {
            char str[3];
            fread(str, sizeof(char), 3, tag_info->fptr_mp3);
            str[3] = '\0';
            if (strcmp(str, "ID3") == 0)
            {
                printf("INFO: ## File signature match with .mp3 file ##\n");
                printf("INFO: ## Version Id: v2.3\n");
            }
            else
            {
                printf("Error: The file signature not match with .mp3 file\n");
                printf("Error: For viewing the tags, pass --> ./a.out -v file_name.mp3 or .mpeg\n");
                return mp3_failure;
            }
        }
    }
    else
    {
        printf("Error: For viewing the tags, pass --> ./a.out -v file_name.mp3 or .mpeg\n");
        return mp3_failure;
    }
    return mp3_success;
}
Status mp3_file_tag_view(char **argv, TagInfo *tag_info)
{
    printf("INFO: ## Start procedure to read mp3 file tags ...\n");
    if (read_and_validate_mp3_file(argv, tag_info) == mp3_success)
    {
       // printf("INFO: ## Read and Validate MP3 file successfully ##\n");
        display(tag_info);
        printf("%-10s : %s\n", "File Name",  tag_info->audio_file_name);
        fseek(tag_info->fptr_mp3, 10, SEEK_SET);
        Status result;
        result = read_and_display_data("Title", "TIT2", tag_info->frame_id, &tag_info->title_tag_size, tag_info->title_tag_id, tag_info->fptr_mp3);
        if (result == mp3_failure)
        {
            printf("Error: Failed to read title \n");
            fclose(tag_info->fptr_mp3);
            return mp3_failure;
        }
        result = read_and_display_data("Artist", "TPE1", tag_info->frame_id, &tag_info->artist_tag_size, tag_info->artist_tag_id, tag_info->fptr_mp3);
        if (result == mp3_failure)
        {
            printf("Error: Failed to read Artist tag \n");
            fclose(tag_info->fptr_mp3);
            return mp3_failure;
        }
        result = read_and_display_data("Album", "TALB", tag_info->frame_id, &tag_info->album_tag_size, tag_info->album_tag_id, tag_info->fptr_mp3);
        if (result == mp3_failure)
        {
            printf("Error: Failed to read Album tag \n");
            fclose(tag_info->fptr_mp3);
            return mp3_failure;
        }
        result = read_and_display_data("Year", "TYER", tag_info->frame_id, &tag_info->year_tag_size, tag_info->year_tag_id, tag_info->fptr_mp3);
        if (result == mp3_failure)
        {
            printf("Error: Failed to read MP3 year tag \n");
            fclose(tag_info->fptr_mp3);
            return mp3_failure;
        }
        result = read_and_display_data("Content", "TCON", tag_info->frame_id, &tag_info->content_type_size, tag_info->content_tag_id, tag_info->fptr_mp3);
        if (result == mp3_failure)
        {
            printf("Error: Failed to read MP3 Content tag \n");
            fclose(tag_info->fptr_mp3);
            return mp3_failure;
        }
        result = read_and_display_data("Comment", "COMM", tag_info->frame_id, &tag_info->comment_size, tag_info->comment_tag_id, tag_info->fptr_mp3);
        if (result == mp3_failure)
        {
            printf("Error: Failed to read MP3 Comment tag \n");
            fclose(tag_info->fptr_mp3);
            return mp3_failure;
        }
        printf("==========================================================\n");
        fclose(tag_info->fptr_mp3);
    }
    else
    {
        printf("Error: Failed to read and validate MP3 file\n");
        return mp3_failure;
    }
    return mp3_success;
}
Status read_and_display_data(const char *str_frame, const char *str_id, char *tag_id, uint *tag_size, char *tag, FILE *mp3_fptr)
{
    int temp_count;
    //printf("Mp3_ptr position = %ld.\n", ftell(mp3_fptr));

    temp_count = fread(tag_id, sizeof(char), 4, mp3_fptr);
    if (temp_count < 4)
    {
        printf("Error: Failed to read Tag id from MP3 file.\n");
        return mp3_failure;
    }
    // tag_id[4] = '\0';
    printf("%-10s : %s\n", "Tag", tag_id);

    if (strcmp(tag_id, str_id) == 0)
    {
        //printf("Comparision Done\n");
        
        temp_count = fread(tag_size, 4, 1, mp3_fptr);

        if (temp_count < 1)
        {
            printf("Error: Failed to read tag size from mp3 file\n");
            return mp3_failure;
        }
        char *pos = (char *)tag_size;
        for (int i = 0; i < ((sizeof(uint)) / 2); i++)
        {
            pos[i] = pos[i] ^ pos[(sizeof(uint)) - i - 1];
            pos[(sizeof(uint)) - i - 1] = pos[i] ^ pos[(sizeof(uint)) - i - 1];
            pos[i] = pos[i] ^ pos[(sizeof(uint)) - i - 1];
        }
        //printf("%s size is %u\n", str_frame, *tag_size);
        tag = (char *)malloc((*tag_size) * sizeof(char));
        if (tag == NULL)
        {
            printf("Error: Memory allocation failed.\n");
            return mp3_failure;
        }
        //printf("Poniter position %ld\n", ftell(mp3_fptr));
        fseek(mp3_fptr, 3L, SEEK_CUR);
        //printf("Poniter position %ld\n", ftell(mp3_fptr));

        temp_count = fread(tag, 1, (*tag_size - 1), mp3_fptr);
        if (temp_count < (*tag_size - 1))
        {
            printf("Error: Failed to read data from mp3 file.\n");
            free(tag);
            return mp3_failure;
        }
        tag[*tag_size - 1] = '\0';
        //printf("Poniter position %ld\n", ftell(mp3_fptr));
        printf ("%-10s : %s.\n", str_frame, tag);
    }
    else
    {
        printf("Error: Frame id Not found\n");
        return mp3_failure;
    }
    free(tag);
    tag = NULL;
    return mp3_success;
}

/*Function to display the project name*/
static void display()
{
    printf("\n==========================================================\n");
    printf("                  MP3 Tag Reader             \n");
    printf("==========================================================\n");
}