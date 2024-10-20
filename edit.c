#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*include types and edit header files*/
#include "edit.h"
#include "types.h"

// static void err_edit_display();
static Status mp3_flag_copy(TagData *tag_data);
static void swap_endian(void *data, size_t size);

/*function to read and validate comment line arguments and which tag user want to edit
Eg: -t = Title
    -a = Artist
    -A = Album name
    -y = Year
    -m = Content
    -c = Comments
Return values:
return : mp3_success   --> function executed without error
return : mp3_failure   --> function executed with error*/
Status read_and_validate_file(char **argv, TagData *tag_data)
{
    /*check 2nd argument vector to idenfy the  modifier*/
    if (argv[2] != NULL)
    {
        if (strcmp(argv[2], "-t") == 0)
            strcpy(tag_data->tag_id, "TIT2");
        else if (strcmp(argv[2], "-a") == 0)
            strcpy(tag_data->tag_id, "TPE1");
        else if (strcmp(argv[2], "-A") == 0)
            strcpy(tag_data->tag_id, "TALB");
        else if (strcmp(argv[2], "-y") == 0)
            strcpy(tag_data->tag_id, "TYER");
        else if (strcmp(argv[2], "-m") == 0)
            strcpy(tag_data->tag_id, "TCON");
        else if (strcmp(argv[2], "-c") == 0)
            strcpy(tag_data->tag_id, "COMM");
        else
        {
            printf("Invalid modifier or invalid comment line argument\n");
            printf("INFO: For Editing the Tags, pass ->  ./a.out -e <modifier> \"New_value\" <file_name.mp3 (or) .mpeg>\n");
            printf("Use Help option, pass -> \"./a.out --help\" , Read the instruction properly\n");
            return mp3_failure;
        }
    }
    else
    {
        printf("Error: There is no comment line arugument\n");
        // help function
        return mp3_failure;
    }

    if (argv[3] != NULL) // Find replacement data from user
    {
        tag_data->tag_id_size = strlen(argv[3]) + 1;
        printf("length of replacement_data size %d\n", tag_data->tag_id_size);
        if (tag_data->tag_id_size < sizeof(tag_data->tag_id_data))
        {
            strcpy(tag_data->tag_id_data, argv[3]);
            printf("Replacement data is : %s\n", tag_data->tag_id_data);
        }
        else
        {
            printf("Error: Length of replacement data size is too high\n");
            return mp3_failure;
        }
    }
    else
    {
        printf("Error: You are didn't mention replacement data\n");
        // help fucntion.
        return mp3_failure;
    }

    if (argv[4] != NULL) // check input .mp3 file is valid or not
    {
        if ((tag_data->fptr_mp3 = fopen(argv[4], "r")) != NULL)
        {
            fseek(tag_data->fptr_mp3, 0, SEEK_SET);
            char str[3];
            fread(str, sizeof(char), 3, tag_data->fptr_mp3);
            if (strcmp(str, "ID3") == 0)
            {
                printf("INFO: ## %s file Signature match with .mp3 file\n", argv[4]);
            }
            else
            {
                printf("Error: %s file signature not match with .mp3 file\n", argv[4]);
                printf("INFO: For Editing the Tags, pass ->  ./a.out -e <modifier> \"New_value\" <file_name.mp3 (or) .mpeg>\n");
                printf("Use Help option, pass -> \"./a.out --help\" , Read the instruction properly\n");
                fclose(tag_data->fptr_mp3);
                return mp3_failure;
            }
            fclose(tag_data->fptr_mp3);
        }
        else
        {
            printf("Error: Failed to open the %s file\n", argv[4]);
        }
    }

    return mp3_success;
}

/*main function to edit tags data in user mentioned .mp3 file
Return values:
return : mp3_success   --> function executed without error
return : mp3_failure   --> function executed with error*/
Status edit_tag(char **argv, TagData *tag_data)
{

    if (read_and_validate_file(argv, tag_data) == mp3_failure) // function call to read and validate files and cmt line arguments
    {
        printf("Error: Failed To Read And Validate File Process\n");
        return mp3_failure;
    }
    tag_data->fptr_mp3 = fopen(argv[4], "r");
    tag_data->fptr_temp = fopen("temp_audio.mpeg", "w");

    fseek(tag_data->fptr_mp3, 0, SEEK_SET);
    fseek(tag_data->fptr_temp, 0, SEEK_SET);

    char arr[10];
    int count = fread(arr, sizeof(char), 10, tag_data->fptr_mp3);
    if (count < 10)
    {
        printf("Error: Failed To Read Header From MP3 file\n");
        return mp3_failure;
    }
    count = fwrite(arr, sizeof(char), count, tag_data->fptr_temp);
    if (count < 10)
    {
        printf("Error: Failed To Write Header From MP3 To Temp File\n");
        return mp3_failure;
    }

    char *tags[6] = {"TIT2", "TPE1", "TALB", "TYER", "TCON", "COMM"};
    for (int i = 0; i < 6; i++)
    {
        count = fread(arr, sizeof(char), 4, tag_data->fptr_mp3);
        if (count < 4)
        {
            printf("Error: Failed To Copy %s From MP3 File", tags[i]);
            return mp3_failure;
        }
        count = fwrite(arr, sizeof(char), 4, tag_data->fptr_temp);
        if (count < 4)
        {
            printf("Error: Failed To Write %s From MP3 File To Temp File", tags[i]);
            return mp3_failure;
        }
        int flag = 0;
        printf("MP3 File Poniter Position = %lu\n", ftell(tag_data->fptr_mp3));
        printf("Temp File Pointer Position %lu\n", ftell(tag_data->fptr_temp));
        if ((strcmp(tag_data->tag_id, tags[i])) == 0)
        {
            flag = 1;                                         // flag 1 means this is a correct tag to replace the exist data by user mentioned data
            if (copy_mp3_data(tag_data, flag) == mp3_failure) // fucntion call to edit tag data
            {
                printf("Error: Failed To Copy Updated Data From MP3 File To Temp File\n");
                return mp3_failure;
            }
        }
        else
        {
            if (copy_mp3_data(tag_data, flag) == mp3_failure)
            {
                printf("Error: Failed To Copy Data From Source To Destination File\n");
                return mp3_failure;
            }
        }
    }
    if (copy_reamaining_mp3_data(tag_data) == mp3_failure) // function call to copy remaining data from .mp3 file to temp file
    {
        printf("Error: Failed To Copy Reamaing Data From MP3 File\n");
        return mp3_failure;
    }

    if (copy_data_back_to_source(argv, tag_data) == mp3_failure) // function call to copy all data from temp file to orginal .mp3 file
    {
        printf("Error: Failed To Copy Data From Temp File to MP3 File\n");
        return mp3_failure;
    }

    return mp3_success;
}

/*Function to copy the data from .mp3 file to temp file for editing purpose
Return values:
return : mp3_success   --> function executed without error
return : mp3_failure   --> function executed with error*/
Status copy_mp3_data(TagData *tag_data, int flag)
{
    if (flag == 1) // flag 1 means , this is a correct tag to edit
    {
        uint old_data_size, new_data_size;
        int count;
        count = fread(&old_data_size, sizeof(uint), 1, tag_data->fptr_mp3); // read  exist tag_id data size
        if (count < 1)
        {
            printf("ERROR: Unable To Read Old Data_Size From The MP3 File.\n");
            return mp3_failure;
        }
        // else

        swap_endian(&old_data_size, sizeof(old_data_size));

        printf("Old Tag Data Size = %d.\n", old_data_size);

        new_data_size = tag_data->tag_id_size;
        printf("New Tag Data Size = %X.\n", tag_data->tag_id_size); // 5

        swap_endian(&tag_data->tag_id_size, sizeof(tag_data->tag_id_size));
        printf("New Tag Data Size = %X.\n", tag_data->tag_id_size); // 50000000

        count = fwrite(&tag_data->tag_id_size, 4, 1, tag_data->fptr_temp); // write new data size to temp file
        if (count < 1)
        {
            printf("ERROR: Unable to write on the MP3 file.\n");
            return mp3_failure;
        }

        if (mp3_flag_copy(tag_data) == mp3_failure)
        {
            printf("Error: Failed to copy mp3 flag data\n");
        }
        // printf("\nNew Data Size %u\n", tag_data->tag_id_size); //5
        count = fwrite(tag_data->tag_id_data, sizeof(char), (new_data_size - 1), tag_data->fptr_temp); // write updated data to temp file
        if (count < (new_data_size - 1))
        {
            printf("ERROR: Unable To Write Updated Data on the Temp MP3 file.\n");
            return mp3_failure;
        }
        fseek(tag_data->fptr_mp3, (long)(old_data_size - 1), SEEK_CUR);
        printf("New Data = %s.\n", tag_data->tag_id_data);
        printf("MP3 indexition = %lu.\n", ftell(tag_data->fptr_mp3));
        printf("Temp indexition = %lu.\n", ftell(tag_data->fptr_temp));
    }
    else // if that current tag not need to edit just copy old data from .mp3 file to temp file
    {
        uint size;
        int count;
        count = fread(&size, sizeof(unsigned int), 1, tag_data->fptr_mp3);
        if (count < 1)
        {
            printf("ERROR: Unable To Read  Size Of Content From The MP3 File.\n");
            return mp3_failure;
        }

        count = fwrite(&size, sizeof(unsigned int), 1, tag_data->fptr_temp);
        if (count < 1)
        {
            printf("ERROR: Unable to write on the MP3 file.\n");
            return mp3_failure;
        }

        swap_endian(&size, sizeof(size));
        printf("Size of the Frame ID content = %d.\n", size);

        if (mp3_flag_copy(tag_data) == mp3_failure)
        {
            printf("Error: Failed to copy mp3 flag data\n");
        }

        char arr[size - 1];
        count = fread(arr, sizeof(char), (size - 1), tag_data->fptr_mp3);
        if (count < (size - 1))
        {
            printf("ERROR: Unable To Read Data From The MP3 File.\n");
            return mp3_failure;
        }
        else
        {
            arr[size - 1] = '\0';
            printf("Frame ID content = %s.\n", arr);
            count = fwrite(arr, 1, (size - 1), tag_data->fptr_temp);
            if (count < (size - 1))
            {
                printf("ERROR: Unable To Write Data On The Temp MP3 File.\n");
                return mp3_failure;
            }
        }

        printf("MP3  pointer indexition = %lu.\n", ftell(tag_data->fptr_mp3));
        printf("Temp  pointer indexition = %lu.\n", ftell(tag_data->fptr_temp));
    }

    return mp3_success;
}
/*Function to Reversal of the Bytes after reading to get the correct value of the 'Size'. */
static void swap_endian(void *data, size_t size)
{
    char *index = (char *)data;
    for (size_t i = 0; i < size / 2; i++)
    {
        index[i] = index[i] ^ index[size - i - 1];
        index[size - i - 1] = index[i] ^ index[size - i - 1];
        index[i] = index[i] ^ index[size - i - 1];
    }
}

/*Function to copy mp3 flag bytes from .mp3 file to temp file
Return values:
return : mp3_success   --> function executed without error
return : mp3_failure   --> function executed with error*/
static Status mp3_flag_copy(TagData *tag_data)
{
    char flag[MP3_FLAG_SIZE];
    int count = fread(flag, sizeof(char), MP3_FLAG_SIZE, tag_data->fptr_mp3);
    if (count < MP3_FLAG_SIZE)
    {
        printf("Error: Failed to read flags from mp3 file\n");
        return mp3_failure;
    }
    count = fwrite(flag, sizeof(char), MP3_FLAG_SIZE, tag_data->fptr_temp);
    if (count < MP3_FLAG_SIZE)
    {
        printf("Error: Failed to paste flag bytes to temp.mp3\n");
        return mp3_failure;
    }
    return mp3_success;
}

/*function to copy remaining datas from .mp3 file (apart from tag_id and tag_id datas)
Return values:
return : mp3_success   --> function executed without error
return : mp3_failure   --> function executed with error*/
Status copy_reamaining_mp3_data(TagData *tag_data)
{
    char arr_ch[1024];
    int count;

    long start = ftell(tag_data->fptr_mp3);
    fseek(tag_data->fptr_mp3, 0l, SEEK_END);
    long end = ftell(tag_data->fptr_mp3);

    printf("Starting And End indexition Of MP3 File Pointer %lu %lu\n", start, end);
    printf("Temp File indexition %lu\n", ftell(tag_data->fptr_temp));

    fseek(tag_data->fptr_mp3, start, SEEK_SET);

    while ((ftell(tag_data->fptr_mp3)) != end) // loop to copy all the data utill reach end
    {
        count = fread(arr_ch, sizeof(char), sizeof(arr_ch), tag_data->fptr_mp3);
        if (count < 0)
        {
            printf("Error: Failed To Read Remaining Data From MP3 File\n");
            return mp3_failure;
        }
        fwrite(arr_ch, sizeof(char), count, tag_data->fptr_temp);
    }

    printf("MP3 indexition = %lu.\n", ftell(tag_data->fptr_mp3));
    printf("Temp indexition = %lu.\n", ftell(tag_data->fptr_temp));
    fclose(tag_data->fptr_mp3);
    fclose(tag_data->fptr_temp);
    return mp3_success;
}

/*Fucntion to copy all the data from temp.mp3 file to orginal .mp3 file
Return values:
return : mp3_success   --> function executed without error
return : mp3_failure   --> function executed with error            */
Status copy_data_back_to_source(char **argv, TagData *tag_data)
{
    printf("Copy Back To Source File\n");
    tag_data->fptr_mp3 = fopen(argv[4], "w");
    tag_data->fptr_temp = fopen("temp_audio.mpeg", "r");
    if (tag_data->fptr_mp3 == NULL || tag_data->fptr_temp == NULL)
    {
        printf("Error: Failed To Open The Files\n");
        return mp3_failure;
    }
    int count;
    long start, end;
    char arr_ch[1024];

    fseek(tag_data->fptr_temp, 0, SEEK_END);
    end = ftell(tag_data->fptr_temp);
    fseek(tag_data->fptr_temp, 0, SEEK_SET);
    start = ftell(tag_data->fptr_temp);
    fseek(tag_data->fptr_mp3, 0, SEEK_SET);

    printf("MP3 File Starting indexition %lu\nTemp File Starting  And End indexition %lu %lu\n", ftell(tag_data->fptr_mp3), start, end);

    while ((ftell(tag_data->fptr_temp)) != end) // loop to copy all the data until reach end
    {
        count = fread(arr_ch, sizeof(char), sizeof(arr_ch), tag_data->fptr_temp);
        if (count < 0)
        {
            printf("Error: Failed To Read Data From Temp File\n");
            return mp3_failure;
        }
        fwrite(arr_ch, sizeof(char), count, tag_data->fptr_mp3);
    }
    /*print both file size to verify file size is same */
    printf("MP3 indexition = %lu\n", ftell(tag_data->fptr_mp3));
    printf("Temp File indexition = %lu\n", ftell(tag_data->fptr_temp));

    /*close the file to prevent unwanted errors*/
    fclose(tag_data->fptr_mp3);
    fclose(tag_data->fptr_temp);
    mp3_success;
}
