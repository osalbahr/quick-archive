#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAGIC_CODE_SIZE 15
#define ARC_CREATE 3
#define MAJOR_VERSION 0
#define MINOR_VERSION 1

typedef struct ArchiveMetadata {
    // %QUICK-ARCHIVE
    char magicCode[MAGIC_CODE_SIZE];
    unsigned int majorVersion;
    unsigned int minorVersion;
    unsigned int fileCount;
} ArchiveMetadata;

typedef struct FileMetadata {
    char filename[128];
    unsigned int fileSize;
} FileMetadata;

void create(char *filename)
{
    FILE *archiveFile = fopen(filename, "wb");
    if (archiveFile == NULL) {
        printf("Cannot create archive '%s'\n", filename);
        exit(1);
    }

    ArchiveMetadata archiveMetadata;
    strcpy(archiveMetadata.magicCode, "%QUICK-ARCHIVE");
    archiveMetadata.majorVersion = MAJOR_VERSION;
    archiveMetadata.minorVersion = MINOR_VERSION;
    archiveMetadata.fileCount = 0;

    fprintf(archiveFile, "%s-%d.%d\n",
        archiveMetadata.magicCode,
        archiveMetadata.majorVersion,
        archiveMetadata.minorVersion);

    char *remainingBytes = ((char *)&archiveMetadata) + MAGIC_CODE_SIZE;
    fwrite(remainingBytes, sizeof(ArchiveMetadata) - MAGIC_CODE_SIZE, 1, archiveFile);
    fclose(archiveFile);
}

int main(int argc, char *argv[])
{
    if (argc == ARC_CREATE && strcmp(argv[1], "-c") == 0) {
        create(argv[2]);
    } else {
        printf("Usage: %s -c <filename>.qar\n", argv[0]);
        exit(1);
    }
}
