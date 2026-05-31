#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define MAGIC_CODE "%QUICKARCHIVE"
#define MAGIC_CODE_SIZE 14
#define ARGC_CREATE 3
#define ARGC_INSERT 4
#define MAJOR_VERSION 0
#define MINOR_VERSION 1
#define FILENAME_SIZE 128

typedef struct ArchiveMetadata {
    // %QUICKARCHIVE
    char magicCode[MAGIC_CODE_SIZE];
    unsigned int majorVersion;
    unsigned int minorVersion;
    unsigned int fileCount;
} ArchiveMetadata;

typedef struct FileMetadata {
    char filename[FILENAME_SIZE];
    unsigned int fileSize;
} FileMetadata;

void fprintArchiveMetadata(FILE *archiveFile, ArchiveMetadata *archiveMetadata)
{
    fprintf(archiveFile, "%s-%d.%d\n",
        archiveMetadata->magicCode,
        archiveMetadata->majorVersion,
        archiveMetadata->minorVersion);

    fwrite(&archiveMetadata->fileCount, sizeof(archiveMetadata->fileCount), 1, archiveFile);
}

void create(char *filename)
{
    FILE *archiveFile = fopen(filename, "wb");
    if (archiveFile == NULL) {
        printf("Cannot create archive '%s'\n", filename);
        exit(1);
    }

    ArchiveMetadata archiveMetadata;
    strcpy(archiveMetadata.magicCode, MAGIC_CODE);
    archiveMetadata.majorVersion = MAJOR_VERSION;
    archiveMetadata.minorVersion = MINOR_VERSION;
    archiveMetadata.fileCount = 0;

    fprintArchiveMetadata(archiveFile, &archiveMetadata);

    fclose(archiveFile);
}

void insert(int argc, char *argv[])
{
    FILE *archiveFile = fopen(argv[2], "rb");
    if (archiveFile == NULL) {
        printf("Cannot open archive '%s'\n", argv[2]);
        exit(1);
    }

    ArchiveMetadata archiveMetadata;
    strcpy(archiveMetadata.magicCode, MAGIC_CODE);
    fscanf(archiveFile, "%" MAGIC_CODE "-%d.%d\n",
        &archiveMetadata.majorVersion,
        &archiveMetadata.minorVersion);
    
    fread(&archiveMetadata.fileCount, sizeof(archiveMetadata.fileCount), 1, archiveFile);
    int newFileCount = archiveMetadata.fileCount + 1;

    FileMetadata *filesMetadata = (FileMetadata *)malloc(newFileCount * sizeof(FileMetadata));
    fread(filesMetadata, sizeof(FileMetadata), archiveMetadata.fileCount, archiveFile);

    char **filesData = (char **)malloc(newFileCount * sizeof(char *));
    for (int i = 0; i < archiveMetadata.fileCount; i++) {
        filesData[i] = (char *)malloc(filesMetadata[i].fileSize);
        fread(filesData[i], filesMetadata[i].fileSize, 1, archiveFile);
    }

    fclose(archiveFile);

    FileMetadata newFileMetadata;

    FILE *newFile = fopen(argv[3], "rb");
    if (newFile == NULL) {
        printf("Cannot new file '%s'\n", argv[3]);
        exit(1);
    }

    for (int i = 0; i < FILENAME_SIZE; i++) {
        newFileMetadata.filename[i] = '\0';
    }
    strcpy(newFileMetadata.filename, argv[3]);

    struct stat fileInfo;
    fstat(fileno(newFile), &fileInfo);

    newFileMetadata.fileSize = fileInfo.st_size;

    filesMetadata[newFileCount - 1] = newFileMetadata;
    filesData[newFileCount - 1] = (char *)malloc(fileInfo.st_size);
    fread(filesData[newFileCount - 1], fileInfo.st_size, 1, newFile);

    FILE *newArchiveFile = fopen(argv[2], "wb");
    archiveMetadata.fileCount = newFileCount;
    fprintArchiveMetadata(newArchiveFile, &archiveMetadata);
    fwrite(filesMetadata, sizeof(FileMetadata), newFileCount, newArchiveFile);

    for (int i = 0; i < newFileCount; i++) {
        fwrite(filesData[i], filesMetadata[i].fileSize, 1, newArchiveFile);
    }

    fclose(newArchiveFile);
}

int main(int argc, char *argv[])
{
    if (argc == ARGC_CREATE && strcmp(argv[1], "-c") == 0) {
        create(argv[2]);
    } else if (argc == ARGC_INSERT && strcmp(argv[1], "-i") == 0) {
        insert(argc, argv);
    } else {
        printf("Usage: %s -c <filename>.arc\n", argv[0]);
        printf("Usage: %s -i <filename>.arc <new_filename>\n", argv[0]);
        exit(1);
    }
}
