# quick-archive
Archive tool focused on speed

## Compilation

```console
$ gcc quick-archive.c -o quick-archive
```

## Usage

```console
Quick Archive Tool

Usage:
  ./quick-archive -c <archive.arc>
      Create archive

  ./quick-archive -i <archive.arc> <file>
      Insert file into archive

  ./quick-archive -l <archive.arc>
      List files inside archive

  ./quick-archive -d <archive.arc> <file>
      delete file from archive

  ./quick-archive -r <archive.arc> <old_file> <new_file>
      delete old file from archive and insert new file

  ./quick-archive -x <archive.arc> <file>
       extractfile from archive

  ./quick-archive -n <archive.arc> <file>
       view archive files 

  ./quick-archive -h
      Show help

   _____ ______ _____ __  __ ______ _   _ _______       _______ _____ ____  _   _ 
  / ____|  ____/ ____|  \/  |  ____| \ | |__   __|   /\|__   __|_   _/ __ \| \ | |
 | (___ | |__ | |  __| \  / | |__  |  \| |  | |     /  \  | |    | || |  | |  \| |
  \___ \|  __|| | |_ | |\/| |  __| | . ` |  | |    / /\ \ | |    | || |  | | . ` |
  ____) | |___| |__| | |  | | |____| |\  |  | |   / ____ \| |   _| || |__| | |\  |
 |_____/|______\_____|_|  |_|______|_| \_|  |_|  /_/    \_\_|  |_____\____/|_| \_|

                     _____         _    _ _   _______ 
                    |  ___|       | |  | | | |__   __|
                    | |_ __ _ _   _| | | | |    | |   
                    |  _/ _` | | | | | | | |    | |   
                    | || (_| | |_| | |_| | |____| |   
                    \_| \__,_|\__,_|\___/|______|_|   

=====================================================
          Developed by Team Segmentation Fault
                  Quick Archive Tool
                     Version 0.1
            (C) 2026 All Rights Reserved
=====================================================
```
