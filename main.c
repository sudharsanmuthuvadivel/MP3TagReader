/***************************************************************************
 * Title  : Program to read tags of mp3 file
 * Date   : 09-09-2024
 * Author : Sudharsan M
 * Input  : For option '-v'
 *           1. Filename.mp3
 * 
 * Output : Tag in mp3 files like(Song title, Author name, year, album name ect).
 *          
 *         For option '-e' 
 *          1. Tag to replace -t/-a/-A/-y/-m/-c  
 *          2. Replacement data
 *          3. filename.mp3 
 ***************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*include other header files*/
#include "types.h"
#include "view.h"
#include "edit.h"

static void help();// fucntion prototype


int main(int argc, char *argv[])
{
    TagInfo tag_info;//stucture variable for view option
    TagData tag_data; // stucture variable for edit option
    if (argc < 2)
    {
        printf("Error: Incorrct format of command line arguments\n");
        printf("INFO: Use\nFor help , pass -->  ./a.out --help\n");
    }
    else
    {
        operation_type op = check_operation_type(argv);  // fucntion call to get operation type
        if (op == mp3_view)
        {
            printf("INFO: ## View option selected ##\n");
            if (mp3_file_tag_view(argv, &tag_info) == mp3_success)  // fucntion call to view .mp3 file tags
            {
                printf("INFO: ## MP3 File Tags Readed successfull ##\n");
            }
            else
            {
                printf("Error: Failed To Read MP3 File Tags\n");
            }
        }
        else if (op == mp3_edit)
        {
            printf("INFO: Edit option selected ##\n");
            if (edit_tag(argv, &tag_data) == mp3_success) // fucntion call to edit something in mp3 file tags
            {
                printf("INFO: Tag Edited Successfully Done\n");
            }
            else{
                printf("Error: Failed To Edit Tag\n");
            }
        }
        else if (op == mp3_help)
        {
            printf("INFO: Help Option Selected \n");
            help();     // fucntion call to get help

        }
        else if (op == mp3_unsupported)  // invalid input from user
        {
            printf("\n==========================     Error    ================================\n");
            printf("Invalid option!\nUsage:\n");
            printf("To View, pass --> ./a.out -v mp3_file\n");
            printf("To Edit, pass --> ./a.out -e -t/-a/-A/-m/-y/-c repalcement_data mp3_file\n");
            printf("========================================================================\n");
        }
    }
    return 0;
} 

/*Function to check operation type based on user input
Options : 1. view 
          2. Edit
          3. Help*/
operation_type check_operation_type(char **argv)
{
    if (argv[1] != NULL && strcmp(argv[1], "-v") == 0 && argv[2] != NULL)
    {
        return mp3_view;
    }
    else if (argv[1] != NULL && strcmp(argv[1], "-e") == 0 && argv[2] != NULL && argv[3] != NULL && argv[4] != NULL)
    {
        return mp3_edit;
    }
    else if (argv[1] != NULL && strcmp(argv[1], "--help") == 0)
    {
        return mp3_help;
    }
    else
        return mp3_unsupported;
}

/*Help function for user reference*/
static void help()
{
    printf("===========================  View  =======================\n");
    printf("Comment To Use View Tags In MP3 Files\n");
    printf("Cmt : ./a.out -v audiofile_name.mp3\n");
    printf("\"-v\"  Represent View option\nFile Format Mustbe .MP3 or .mpeg\n");
    printf("================================================================\n");
    printf("\n=========================  Edit  ===========================\n");
    printf("Comment To Use Edit Tags In MP3 Files\n");
    printf("Cmt : ./a.out -e -t/-a/-A/-y/-m/-c audiofile_name.mp3(or).mpeg\n");
    printf("\n\"-v\" Represent Edit Option\n");

    printf("-t     --> Modify Title Tag\n-a     --> Modify Artist Tag\n-A     --> Modify Album Tag\n");
    printf("-y     --> Modify Year Tag\n-m     --> Modify Content Tag\n-c     --> Modify Comments Tag\n");

    printf("\nThanks For Viewing This Project!\n");

    printf("================================================================\n");
}