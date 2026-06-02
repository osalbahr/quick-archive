#include <string.h>
#include "quick-archive.h"

int main(int argc, char *argv[])
{
    if (argc == ARGC_CREATE && strcmp(argv[1], "-c") == 0) {
        create(argv[2]);
    } else if (argc == ARGC_INSERT && strcmp(argv[1], "-i") == 0) {
        insert(argc, argv);
    } else if (argc == ARGC_LIST && strcmp(argv[1], "-l") == 0) {
        list(argc, argv);   
    } else if (argc == ARGC_HELP && strcmp(argv[1], "-h") == 0) {
        help(argv[0]);
    } else if (argc == ARGC_DELETE  && strcmp(argv[1], "-d") == 0) {
        delete(argc, argv);
    } else if (argc == ARGC_EXTRACT && strcmp(argv[1], "-x") == 0) {
        extract(argc, argv);
    } else if (argc == ARGC_FILE_COUNT && strcmp(argv[1], "-n") == 0) {
        file_count(argc, argv);
    } else if (argc == ARGC_REPLACE && strcmp(argv[1], "-r") == 0) {
        replace(argc, argv);
    } else {
        help(argv[0]);
        exit(1);
    }
}
