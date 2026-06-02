#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define MAGIC_CODE "%QUICKARCHIVE"
#define MAGIC_CODE_SIZE 14
#define ARGC_CREATE 3
#define ARGC_INSERT 4
#define ARGC_LIST 3
#define ARGC_REPLACE 5
#define ARGC_FILE_COUNT 3
#define ARGC_HELP 2
#define ARGC_DELETE 4
#define ARGC_EXTRACT 4
#define MAJOR_VERSION 0
#define MINOR_VERSION 1
#define FILENAME_SIZE 128
#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_RESET "\x1b[0m"

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
        printf("Cannot open file '%s'\n", argv[3]);
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

    printf(ANSI_GREEN "Inserted: %s\n" ANSI_RESET, newFileMetadata.filename);

    fclose(newArchiveFile);
}

void list(int argc, char *argv[])
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

    FileMetadata *filesMetadata = (FileMetadata *)malloc(archiveMetadata.fileCount * sizeof(FileMetadata));
    fread(filesMetadata, sizeof(FileMetadata), archiveMetadata.fileCount, archiveFile);

    for (int i = 0; i < archiveMetadata.fileCount; i++) {
        printf("%s\n", filesMetadata[i].filename);
    }
}

void file_count(int argc, char *argv[])
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

    printf("%d\n", archiveMetadata.fileCount);
}

void help(char *programName)
{
    printf("Quick Archive Tool\n\n");

    printf("Usage:\n");

    printf("  %s -c <archive.arc>\n", programName);
    printf("      Create archive\n\n");

    printf("  %s -i <archive.arc> <file>\n", programName);
    printf("      Insert file into archive\n\n");

    printf("  %s -l <archive.arc>\n", programName);
    printf("      List files inside archive\n\n");

    printf("  %s -d <archive.arc> <file>\n", programName);
    printf("      delete file from archive\n\n");

    printf("  %s -r <archive.arc> <old_file> <new_file>\n", programName);
    printf("      delete old file from archive and insert new file\n\n");

    printf("  %s -x <archive.arc> <file>\n", programName);
    printf("       extractfile from archive\n\n");

     printf("  %s -n <archive.arc> <file>\n", programName);
    printf("       view archive files \n\n");

    printf("  %s -h\n", programName);
    printf("      Show help\n\n");

    printf("   _____ ______ _____ __  __ ______ _   _ _______       _______ _____ ____  _   _ \n");
    printf("  / ____|  ____/ ____|  \\/  |  ____| \\ | |__   __|   /\\|__   __|_   _/ __ \\| \\ | |\n");
    printf(" | (___ | |__ | |  __| \\  / | |__  |  \\| |  | |     /  \\  | |    | || |  | |  \\| |\n");
    printf("  \\___ \\|  __|| | |_ | |\\/| |  __| | . ` |  | |    / /\\ \\ | |    | || |  | | . ` |\n");
    printf("  ____) | |___| |__| | |  | | |____| |\\  |  | |   / ____ \\| |   _| || |__| | |\\  |\n");
    printf(" |_____/|______\\_____|_|  |_|______|_| \\_|  |_|  /_/    \\_\\_|  |_____\\____/|_| \\_|\n");
    printf("\n");
    printf("                     _____         _    _ _   _______ \n");
    printf("                    |  ___|       | |  | | | |__   __|\n");
    printf("                    | |_ __ _ _   _| | | | |    | |   \n");
    printf("                    |  _/ _` | | | | | | | |    | |   \n");
    printf("                    | || (_| | |_| | |_| | |____| |   \n");
    printf("                    \\_| \\__,_|\\__,_|\\___/|______|_|   \n");
    printf("\n");

    printf("=====================================================\n");
    printf("          Developed by Team Segmentation Fault\n");
    printf("                  Quick Archive Tool\n");
    printf("                     Version 0.1\n");
    printf("            (C) 2026 All Rights Reserved\n");
    printf("=====================================================\n\n");
}

void delete(int argc, char *argv[])
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

    FileMetadata *filesMetadata =
        (FileMetadata *)malloc(archiveMetadata.fileCount * sizeof(FileMetadata));

    fread(filesMetadata, sizeof(FileMetadata), archiveMetadata.fileCount, archiveFile);

    char **filesData =
        (char **)malloc(archiveMetadata.fileCount * sizeof(char *));

    for (int i = 0; i < archiveMetadata.fileCount; i++) {
        filesData[i] = (char *)malloc(filesMetadata[i].fileSize);
        fread(filesData[i], filesMetadata[i].fileSize, 1, archiveFile);
    }

    fclose(archiveFile);

    int deleteIndex = -1;

    for (int i = 0; i < archiveMetadata.fileCount; i++) {
        if (strcmp(filesMetadata[i].filename, argv[3]) == 0) {
            deleteIndex = i;
            break;
        }
    }

    if (deleteIndex == -1) {
        printf("File not found: %s\n", argv[3]);

        for (int i = 0; i < archiveMetadata.fileCount; i++) {
            free(filesData[i]);
        }

        free(filesData);
        free(filesMetadata);
        return;
    }

    unsigned int oldFileCount = archiveMetadata.fileCount;

    archiveFile = fopen(argv[2], "wb");

    if (archiveFile == NULL) {
        printf("Cannot rewrite archive '%s'\n", argv[2]);
        exit(1);
    }

    archiveMetadata.fileCount = oldFileCount - 1;

    fprintArchiveMetadata(archiveFile, &archiveMetadata);

    for (int i = 0; i < oldFileCount; i++) {
        if (i != deleteIndex) {
            fwrite(&filesMetadata[i], sizeof(FileMetadata), 1, archiveFile);
        }
    }

    for (int i = 0; i < oldFileCount; i++) {
        if (i != deleteIndex) {
            fwrite(filesData[i], filesMetadata[i].fileSize, 1, archiveFile);
        }

        free(filesData[i]);
    }

    free(filesData);
    free(filesMetadata);

    fclose(archiveFile);

    printf(ANSI_GREEN "Deleted: %s\n" ANSI_RESET, argv[3]);
}

void extract(int argc, char *argv[])
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

    FileMetadata *filesMetadata =
        (FileMetadata *)malloc(archiveMetadata.fileCount * sizeof(FileMetadata));

    fread(filesMetadata, sizeof(FileMetadata), archiveMetadata.fileCount, archiveFile);

    for (int i = 0; i < archiveMetadata.fileCount; i++) {
        char *filesData = (char *)malloc(filesMetadata[i].fileSize);

        fread(filesData, filesMetadata[i].fileSize, 1, archiveFile);


        if (strcmp(filesMetadata[i].filename, argv[3]) == 0) {
            FILE *outputFile = fopen(argv[3], "wb");

            if (outputFile == NULL) {
                printf("Cannot create output file '%s'\n", argv[3]);
                free(filesData);
                free(filesMetadata);
                fclose(archiveFile);
                exit(1);
            }

            fwrite(filesData, filesMetadata[i].fileSize, 1, outputFile);

            fclose(outputFile);
            free(filesData);
            free(filesMetadata);
            fclose(archiveFile);

            printf("File extracted: %s\n", argv[3]);
            return;
        }

        free(filesData);
    }

    printf("File not found: %s\n", argv[3]);

    free(filesMetadata);
    fclose(archiveFile);
}

void replace(int argc, char *argv[]){    
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

    FileMetadata *filesMetadata =
        (FileMetadata *)malloc(archiveMetadata.fileCount * sizeof(FileMetadata));

    fread(filesMetadata, sizeof(FileMetadata), archiveMetadata.fileCount, archiveFile);

    char **filesData =
        (char **)malloc(archiveMetadata.fileCount * sizeof(char *));

    for (int i = 0; i < archiveMetadata.fileCount; i++) {
        filesData[i] = (char *)malloc(filesMetadata[i].fileSize);
        fread(filesData[i], filesMetadata[i].fileSize, 1, archiveFile);
    }

    fclose(archiveFile);

    int replaceindex = -1;

    for (int i = 0; i < archiveMetadata.fileCount; i++) {
        if (strcmp(filesMetadata[i].filename, argv[3]) == 0) {
            replaceindex = i;
            break;
        }
    }

    if (replaceindex == -1) {
        printf("File not found: %s\n", argv[3]);

        for (int i = 0; i < archiveMetadata.fileCount; i++) {
            free(filesData[i]);
        }

        free(filesData);
        free(filesMetadata);
        return;
    }

    archiveFile = fopen(argv[2], "wb");
    if (archiveFile == NULL) {
        printf("Cannot rewrite archive '%s'\n", argv[2]);
        exit(1);
    }

    FileMetadata replaceFileMetadata;

    FILE *replaceFile = fopen(argv[4], "rb");
    if (replaceFile == NULL) {
        printf("Cannot new file '%s'\n", argv[4]);
        exit(1);
    }

    for (int i = 0; i < FILENAME_SIZE; i++) {
        replaceFileMetadata.filename[i] = '\0';
    }
    strcpy(replaceFileMetadata.filename, argv[4]);

    struct stat fileInfo;
    fstat(fileno(replaceFile), &fileInfo);

    replaceFileMetadata.fileSize = fileInfo.st_size;

    char *replacefileData = (char *)malloc(replaceFileMetadata.fileSize);
    fread(replacefileData, replaceFileMetadata.fileSize, 1, replaceFile);

    int file_count = archiveMetadata.fileCount;

    fprintArchiveMetadata(archiveFile, &archiveMetadata);

    for (int i = 0; i < file_count; i++) {
        if (i != replaceindex) {
            fwrite(&filesMetadata[i], sizeof(FileMetadata), 1, archiveFile);
        }
        else {
            fwrite(&replaceFileMetadata, sizeof(FileMetadata), 1, archiveFile);
        }
    }

    for (int i = 0; i < file_count; i++) {
        if (i != replaceindex) {
            fwrite(filesData[i], filesMetadata[i].fileSize, 1, archiveFile);
        }
        else {
            fwrite(replacefileData, replaceFileMetadata.fileSize, 1, archiveFile);
        }

        free(filesData[i]);
    }

    free(replacefileData);
    free(replaceFile);
    free(filesData);
    free(filesMetadata);

    fclose(archiveFile);

    printf("Replaced: %s\n", argv[3]);
}
