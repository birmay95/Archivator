#include "header.h"

int main(int argc, char *argv[]) {
    char* files[100];
    int numFiles = 0;
    char archivePath[PATH_MAX] = ".";
    bool archivePathProvided = false;
    char archivation = '2';
    char methodArch = 'n';
    char passwordProvided = 'n';
    char nameBin[40] = "binary";
    int opt;
    Archive archive;

    static struct option long_options[] = {
            {"file", no_argument, 0, 'f'},
            {"path", optional_argument, 0, 'p'},
            {"archive", no_argument, 0, 'a'},
            {"unarchive", no_argument, 0, 'u'},
            {"hofman", no_argument, 0, 'h'},
            {"lzw", no_argument, 0, 'l'},
            {"lz77", no_argument, 0, 'L'},
            {"name", required_argument, 0, 'n'},
            {"help", no_argument, 0, 'H'},
            {"password", no_argument, 0, 'P'},
            {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "HauPhlLfp::n:", long_options, NULL)) != -1) {
        if(opt == 'H') {
            archive.help();
            return 0;
        }
        if(opt != 'a' && opt != 'u' && archivation == '2') {
            fprintf(stderr, "Error in command args: The first option should be -a or -u.\n");
            return 1;
        }
        if(opt != 'h' && opt != 'l' && opt != 'L' && methodArch == 'n' && opt != 'a' && opt != 'u' && archivation != '0') {
            methodArch = 'l';
        }
        if((methodArch == 'h' && (opt == 'l' || opt == 'L')) || (methodArch == 'l' && (opt == 'h' || opt == 'L')) || (methodArch == 'L' && (opt == 'h' || opt == 'l'))) {
            fprintf(stderr, "Error in command args: Should be only -h or -l or -L.\n");
            return 1;
        }
        if((archivation == '1' && opt == 'u') || (archivation == '0' && opt == 'a')) {
            fprintf(stderr, "Error in command args: Should be only -a or -u.\n");
            return 1;
        }
        switch (opt) {
            case 'f':
                if(archivation == '0' && argv[optind][0] != '-') {
                    char *extension = strstr(argv[optind], ".archive");
                    if (extension != NULL && strcmp(extension, ".archive") == 0) {
                        files[numFiles++] = argv[optind++];
                        break;
                    } else {
                        printf("Invalid file extension. Only .archive files are supported.\n");
                        return 1;
                    }
                }
                while (optind < argc && argv[optind][0] != '-') {
                    files[numFiles++] = argv[optind++];
                }
                if(numFiles > 0) {
                    optind--;
                }
                break;
            case 'p':
                if (argv[optind]) {
                    strcpy(archivePath, argv[optind]);
                    int lastEl = strlen(argv[optind]);
                    if(argv[optind][lastEl - 1] != '/') {
                        archivePath[lastEl] = '/';
                    }
                    archivePathProvided = true;
                } else {
                    archivePathProvided = false;
                }
                break;
            case 'a':
                archivation = '1';
                break;
            case 'u':
                archivation = '0';
                break;
            case 'P':
                passwordProvided = 'y';
                break;
            case 'h':
                methodArch = 'h';
                break;
            case 'l':
                methodArch = 'l';
                break;  
            case 'L':
                methodArch = 'L';
                break;  
            case 'n':
                strcpy(nameBin, optarg);
                break;  
            default:
                printf("Unknown option, that's why skip skip\n");
        }
    }

    if (!archivePathProvided) {
        printf("No path provided for archiving. Using default path.\n");
    }

    if (numFiles == 0) {
        fprintf(stderr, "No files provided for archiving.\n");
        return 1;
    }

    archive.init(files, numFiles, archivePath, methodArch, nameBin, passwordProvided);
    archive.getInfo();
    if (archivation == '1') {
        archive.scanArgs();
    }
    if(archivation == '0') {
        archive.outCompress();
    }

    return 0;
}
