// В файле Archive.h
#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <linux/limits.h>
#include <getopt.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/wait.h>
#include <fstream>
#include <dirent.h>
#include <string.h>
#include <iostream>
#include <pthread.h>

struct pthreadArgs {
    std::string filename;
    std::string fileBin;
    char archivation;
};

class Archive {
private:
    char** files;
    int numFiles;
    char path[PATH_MAX];
    static char methodArch;
    static char passwordProvided;
    char writeBinFile[PATH_MAX];

    void scanDir(const char* file, const char* nameDir) const;
    static void* launchChild(void* args);
    void showProgressBar(int progress) const;

    void inCompress(const char* file, bool fromDir, const char* nameDir) const;
    std::vector<std::string> splitFile(const std::string& filename, size_t chunkSize) const;
    void compressChunks(const std::vector<std::string>& chunks) const;
    void combineChunks(const std::vector<std::string>& chunks, std::ofstream& outFile) const;
    void cleanup(const std::vector<std::string>& chunks) const;

    void createDirectories(const std::string& filePath) const;
    std::vector<std::string> splitArchive(std::ifstream& archiveFile, size_t chunkSize) const;
    void decompressChunks(const std::vector<std::string>& chunks) const;
    void combineDecompressedChunks(const std::vector<std::string>& chunks, const std::string& outputFile) const;
    void cleanupDecompess(const std::vector<std::string>& chunks) const;
public:

    Archive();
    ~Archive();

    void init(char** files, int numFiles, const char* path, char methodArch, char* nameBin, char pswrdProv);
    void getInfo() const;
    void scanArgs() const;
    void outCompress() const;
    void help();
};

#endif // ARCHIVE_H
