#define _CRT_SECURE_NO_WARNINGS
// crt_fopen.c
// compile with: /W3
// This program opens two files. It uses
// fclose to close the first file and
// _fcloseall to close all remaining files.

#include <stdio.h>

FILE* stream, * stream2;

inline const int FILE_LEN = 30;

int main(void)
{
    printf("파일이름 입력해주세요.");
    char fileName[FILE_LEN];
    if (scanf("%s", fileName) != 1)
    {
        printf("잘못된 입력값\n");
    }

/*    int numclosed;

    // Open for read (will fail if file "crt_fopen.c" does not exist)
    if ((stream = fopen("crt_fopen.c", "r")) == NULL) // C4996
        // Note: fopen is deprecated; consider using fopen_s instead
        printf("The file 'crt_fopen.c' was not opened\n");
    else
        printf("The file 'crt_fopen.c' was opened\n");

    // Open for write
    if ((stream2 = fopen("data2", "w+")) == NULL) // C4996
        printf("The file 'data2' was not opened\n");
    else
        printf("The file 'data2' was opened\n");

    // Close stream if it is not NULL
    if (stream)
    {
        if (fclose(stream))
        {
            printf("The file 'crt_fopen.c' was not closed\n");
        }
    }

    // All other files are closed:
    numclosed = _fcloseall();
    printf("Number of files closed by _fcloseall: %u\n", numclosed);*/
}