#ifndef TYPES_H
#define TYPES_H

/*creating alias to unsigned int data type*/
typedef unsigned int uint;

#define MAXI_FRAME_ID_SIZE 4
#define MP3_FLAG_SIZE 3

/*Status used as funtion return type*/
typedef enum
{
    mp3_success,
    mp3_failure
}Status;

/*Operation_type used to identify the opertion type*/
typedef enum
{
    mp3_view,
    mp3_edit,
    mp3_help,
    mp3_unsupported
}operation_type;

/*Function to find operation type*/
operation_type check_operation_type(char **agrv);

#endif