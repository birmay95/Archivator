#include "header.h"
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <unistd.h>
#include <vector>

const int sizeOfWR = 1048576;
char Archive::methodArch;

Archive::Archive() {
    files = nullptr;
    numFiles = 0;
    path[0] = '\0';
    writeBinFile[0] = '\0';
}

Archive::~Archive() {
    for (int i = 0; i < numFiles; i++) {
        free(files[i]);
    }
    free(files);
}

void Archive::init(char** files, int numFiles, const char* path, char methodArch, char* nameBin) {
    this->files = (char**)malloc(numFiles * sizeof(char*));
    for (int i = 0; i < numFiles; i++) {
        this->files[i] = strdup(files[i]);
    }
    this->numFiles = numFiles;
    strcpy(this->path, path);
    strcpy(this->writeBinFile, this->path);
    strcat(this->writeBinFile, nameBin);
    strcat(this->writeBinFile, ".archive");
    this->methodArch = methodArch;
}

void Archive::getInfo() const {
    printf("Files: %d\n", numFiles);
    for (int i = 0; i < numFiles; i++) {
        printf("%s\n", files[i]);
    }
}


void Archive::showProgressBar(int progress) const {
    int barWidth = 50;
    float ratio = static_cast<float>(progress) / 100;
    int barProgress = static_cast<int>(ratio * barWidth);

    printf("[");
    for (int i = 0; i < barWidth; ++i) {
        if (i < barProgress) {
            printf("=");
        } else {
            printf(" ");
        }
    }
    printf("] %d%%\r", static_cast<int>(ratio * 100.0));
    fflush(stdout);
}

void* Archive::launchChild(void* args) {
    struct pthreadArgs* arg = (struct pthreadArgs*)args;

    pid_t pid = fork();

    if (pid == -1) {
        fprintf(stderr, "Error, Code of the error - %d\n", errno);
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        if (methodArch == 'l') {
            execl("./lzwArchiver", &arg->archivation, arg->filename.c_str(), arg->fileBin.c_str(), NULL);
        } else if (methodArch == 'h') {
            execl("./hofmanArchiver", &arg->archivation, arg->filename.c_str(), arg->fileBin.c_str(), NULL);
        } else if (methodArch == 'L') {
            execl("./lz77Archiver", &arg->archivation, arg->filename.c_str(), arg->fileBin.c_str(), NULL);
        }
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            printf("Child process exited abnormally\n");
        }
    }
    return NULL;
}

void Archive::scanArgs() const {
    std::ofstream binFile(this->writeBinFile);
    binFile.close();
    for(int i = 0; i < numFiles; i++) {
        std::string filename;
        if (opendir(files[i])) {
            std::string path(files[i]);
            size_t found = path.find_last_of("/");
            if (found != std::string::npos) {
                filename = path.substr(found + 1);
            }
        }
        fflush(stdout);
        printf("Scanning %s...\n", files[i]);
        for (int j = 0; j <= 100; ++j) {
            showProgressBar(j);
            usleep(5000);
        }
        printf("\n");
        scanDir(files[i], filename.empty()? nullptr : filename.c_str());
    }
}

void Archive::scanDir(const char* file, const char* nameDir) const {
    DIR* dir = opendir(file);
    if (!dir) {
        inCompress(file, false, nameDir);
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        std::string full_path = file;
        full_path += "/";
        full_path += entry->d_name;
        std::string dir_path;
        if (nameDir != nullptr) {
            dir_path = nameDir;
            dir_path += "/";
            dir_path += entry->d_name;
        }
        struct stat sb;
        if (lstat(full_path.c_str(), &sb) == -1) {
            perror("lstat");
            continue;
        }
        if (S_ISDIR(sb.st_mode)) {
            scanDir(full_path.c_str(), dir_path.empty()? nullptr : dir_path.c_str());
        } else if (S_ISREG(sb.st_mode)) {
            inCompress(full_path.c_str(), true, dir_path.empty()? nullptr : dir_path.c_str());
        }
    }
    closedir(dir);
}

std::vector<std::string> Archive::splitFile(const std::string& filename, size_t chunkSize) const {
    std::ifstream file(filename, std::ios::binary);
    std::vector<std::string> chunks;

    if (!file.is_open()) {
        fprintf(stderr, "Error opening file: %s\n", filename.c_str());
        return chunks;
    }

    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    size_t numChunks = fileSize / chunkSize;
    if(numChunks > 10) {
        numChunks = 9;
        chunkSize = fileSize / numChunks;
    }
    size_t dopChunk = fileSize % chunkSize;
    if(dopChunk != 0 || numChunks == 0) {
        numChunks++;
    }
    int progress = 100 / numChunks;

    fflush(stdout);
    printf("Splitting file into %lu chunks...\n", numChunks);
    for (size_t i = 0; i < numChunks; ++i) {
        for (int j = i * progress; j <= (int)((i + 1) * progress); ++j) {
            showProgressBar(j);
            usleep(5000);
            if(j == 99) {
                showProgressBar(100);
            }
        }
        std::ofstream outFile("chunk_" + std::to_string(i), std::ios::binary);
        size_t remainingSize = std::min(chunkSize, fileSize - i * chunkSize);
        char buffer[sizeOfWR];
        while (remainingSize > 0) {
            size_t bytesToRead = std::min(sizeof(buffer), remainingSize);
            file.read(buffer, bytesToRead);
            outFile.write(buffer, bytesToRead);
            remainingSize -= bytesToRead;
        }
        chunks.push_back("chunk_" + std::to_string(i));
        outFile.close();
    }
    file.close();
    printf("\n");

    return chunks;
}

void Archive::compressChunks(const std::vector<std::string>& chunks) const {
    std::vector<pthread_t> threads(chunks.size());
    std::vector<struct pthreadArgs> args(chunks.size());
    for (size_t i = 0; i < chunks.size(); i++) {
        args[i].filename = chunks[i];
        args[i].fileBin = chunks[i] + "Bin.bin";
        args[i].archivation = '1';
        pthread_create(&threads[i], NULL, launchChild, (void*)&args[i]);
    }
    int progress = 100 / chunks.size();

    fflush(stdout);
    printf("Working threads...\n");
    for (size_t i = 0; i < chunks.size(); i++) {
        for (int j = i * progress; j <= (int)(progress * (i + 1)); ++j) {
            showProgressBar(j);
            usleep(5000);
            if (j == 99) {
                showProgressBar(100);
            }
        }
        pthread_join(threads[i], NULL);
    }
    printf("\n");
}

void Archive::combineChunks(const std::vector<std::string>& chunks, std::ofstream& outFile) const {
    int progress = 100 / chunks.size();

    fflush(stdout);
    printf("Combine chunks into one file\n");
    for (size_t i = 0; i < chunks.size(); i++) {
        std::ifstream inFile(chunks[i] + "Bin.bin", std::ios::binary);
        inFile.seekg(0, std::ios::end);
        int sizeOfFIle = inFile.tellg();
        inFile.seekg(std::ios::beg);
        outFile.write(reinterpret_cast<char*>(&sizeOfFIle), sizeof(sizeOfFIle));
        char bytes[sizeOfWR];
        int k = 0;
        for(k = sizeOfWR; k < sizeOfFIle; k += sizeOfWR) {
            if(inFile.read(reinterpret_cast<char*>(&bytes), sizeOfWR)) {
                outFile.write(reinterpret_cast<char*>(&bytes), sizeOfWR);
            }
        }
        if(inFile.read(reinterpret_cast<char*>(&bytes), (sizeOfFIle + sizeOfWR - k))) {
            outFile.write(reinterpret_cast<char*>(&bytes), (sizeOfFIle + sizeOfWR - k));
        }
        inFile.close();
        for (int j = i * progress; j <= (int)(progress * (i + 1)); ++j) {
            showProgressBar(j);
            usleep(5000);
            if (j == 99) {
                showProgressBar(100);
            }
        }
    }
    printf("\n");
}

void Archive::cleanup(const std::vector<std::string>& chunks) const {
    fflush(stdout);
    printf("Cleaning up...\n");
    int progress = 100 / chunks.size();
    for (size_t i = 0; i < chunks.size(); i++) {
        std::remove((chunks[i] + ".Bin.bin").c_str());
        std::remove((chunks[i]).c_str());
        for (int j = i * progress; j <= (int)(progress * (i + 1)); ++j) {
            showProgressBar(j);
            usleep(2000);
        }
    }
    printf("\n");
}

void Archive::inCompress(const char* file, bool fromDir, const char* nameDir) const {
    fflush(stdout);
    printf("Compressing file %s...\n", file);

    std::ofstream binFile(this->writeBinFile, std::ios::app);
    std::string path(file);
    size_t found = path.find_last_of("/");
    std::string filename;
    if (!fromDir && found != std::string::npos) {
        filename = path.substr(found + 1);
    } else if (fromDir && nameDir != nullptr) {
        filename = nameDir;
    } else {
        filename = file;
    }
    int sizeofNameFile = filename.size();

    binFile.write((char*)&methodArch, sizeof(methodArch));
    binFile.write(reinterpret_cast<char*>(&sizeofNameFile), sizeof(sizeofNameFile));
    binFile.write(filename.c_str(), sizeofNameFile);

    std::vector<std::string> chunks = splitFile(file, 1048576);
    if (chunks.empty()) {
        binFile.close();
        std::remove(this->writeBinFile);
        return;
    }
    compressChunks(chunks);
    int countFiles = chunks.size();
    binFile.write(reinterpret_cast<char*>(&countFiles), sizeof(countFiles));
    combineChunks(chunks, binFile);
    cleanup(chunks);

    binFile.close();
    fflush(stdout);
    printf("File %s compressed\n", file);
}

std::vector<std::string> Archive::splitArchive(std::ifstream& archiveFile, size_t chunkSize) const {
    std::vector<std::string> chunks;
    int countFiles = 0;
    archiveFile.read(reinterpret_cast<char*>(&countFiles), sizeof(countFiles));
    int progress = 100 / countFiles;

    fflush(stdout);
    printf("Splitting archive into %d chunks...\n", countFiles);
    for (int i = 0; i < countFiles; ++i) {
        int sizeOfFile = 0;
        archiveFile.read(reinterpret_cast<char*>(&sizeOfFile), sizeof(sizeOfFile));
        std::string chunkName = "chunk_" + std::to_string(i) + "Bin.bin";
        std::ofstream tempFile(chunkName);
        char bytes[sizeOfWR];
        int j = 0;

        for(j = sizeOfWR; j < sizeOfFile; j += sizeOfWR) {
            if (archiveFile.read(reinterpret_cast<char*>(&bytes), sizeOfWR)) {
                tempFile.write(reinterpret_cast<char*>(&bytes), sizeOfWR);
            }
        }
        if (archiveFile.read(reinterpret_cast<char*>(&bytes), (sizeOfFile + sizeOfWR - j))) {
            tempFile.write(reinterpret_cast<char*>(&bytes), (sizeOfFile + sizeOfWR - j));
        }
        tempFile.close();
        chunks.push_back("chunk_" + std::to_string(i));

        for(int j = i * progress; j <= (int)((i + 1) * progress); ++j) {
            showProgressBar(j);
            usleep(5000);
            if(j == 99) {
                showProgressBar(100);
            }
        }
    }
    printf("\n");

    return chunks;
}

void Archive::decompressChunks(const std::vector<std::string>& chunks) const {
    std::vector<pthread_t> threads(chunks.size());
    std::vector<struct pthreadArgs> args(chunks.size());
    for (size_t i = 0; i < chunks.size(); i++) {
        args[i].filename = chunks[i];
        args[i].fileBin = chunks[i] + "Bin.bin";
        args[i].archivation = '0';
        pthread_create(&threads[i], NULL, launchChild, (void*)&args[i]);
    }
    int progress = 100 / chunks.size();

    fflush(stdout);
    printf("Working threads...\n");
    for (size_t i = 0; i < chunks.size(); i++) {
        for (int j = i * progress; j <= (int)(progress * (i + 1)); ++j) {
            showProgressBar(j);
            usleep(5000);
            if (j == 99) {
                showProgressBar(100);
            }
        }
        pthread_join(threads[i], NULL);
    }
    printf("\n");
}

void Archive::combineDecompressedChunks(const std::vector<std::string>& chunks, const std::string& outputFile) const {
    std::ofstream outFile(outputFile, std::ios::binary);
    int progress = 100 / chunks.size();

    fflush(stdout);
    printf("Combine chunks into one file %s\n", outputFile.c_str());    
    for (size_t i = 0; i < chunks.size(); i++) {
        std::ifstream inFile(chunks[i], std::ios::binary);
        inFile.seekg(0, std::ios::end);
        int sizeFile = inFile.tellg();
        inFile.seekg(0, std::ios::beg);
        char bytes[sizeOfWR];
        int j = 0;

        for(j = sizeOfWR; j < sizeFile; j += sizeOfWR) {
            if (inFile.read(reinterpret_cast<char*>(&bytes), sizeOfWR)) {
                outFile.write(reinterpret_cast<char*>(&bytes), sizeOfWR);
            }
        }
        if (inFile.read(reinterpret_cast<char*>(&bytes), (sizeFile + sizeOfWR - j))) {
            outFile.write(reinterpret_cast<char*>(&bytes), (sizeFile + sizeOfWR - j));
        }
        inFile.close();

        for (int j = i * progress; j <= (int)(progress * (i + 1)); ++j) {
            showProgressBar(j);
            usleep(5000);
            if (j == 99) {
                showProgressBar(100);
            }
        }
    }
    printf("\n");
    outFile.close();
}

void Archive::cleanupDecompess(const std::vector<std::string>& chunks) const {
    int progress = 100 / chunks.size();

    fflush(stdout);
    printf("Cleaning up...\n");
    for (size_t i = 0; i < chunks.size(); i++) {
        std::remove((chunks[i]).c_str());
        std::remove((chunks[i] + "Bin.bin").c_str());

        for (int j = i * progress; j <= (int)(progress * (i + 1)); ++j) {
            showProgressBar(j);
            usleep(5000);
            if (j == 99) {
                showProgressBar(100);
            }
        }
    }
    printf("\n");
}

void Archive::createDirectories(const std::string& filePath) const{
    std::string dirPath;
    size_t pos = 0;

    printf("Creating directories...\n");
    for (int i = 0; i <= 50; i++) {
        showProgressBar(i);
        usleep(5000);
    }

    while (pos != std::string::npos) {
        pos = filePath.find('/', pos + 1);
        if (pos != std::string::npos) {
            std::string subdir = filePath.substr(0, pos);
            if (access(subdir.c_str(), F_OK) == -1) {
                if (mkdir(subdir.c_str(), 0777) == -1) {
                    fprintf(stderr, "Failed to create directory: %s", subdir.c_str());
                    return;
                }
            }
        }
    }

    for (int i = 51; i <= 100; i++) {
        showProgressBar(i);
        usleep(5000);
    }
    printf("\n");
}

void Archive::outCompress() const {
    fflush(stdout);
    printf("Decompressing files from %s...\n", this->files[0]);
    
    std::ifstream binFile(this->files[0]);
    if (!binFile.is_open()) {
        fprintf(stderr, "Error opening file: %s\n", this->files[0]);
        return;
    }
    while(!binFile.eof()) {
        int sizeOfNameFIle = 0;
        if (!binFile.read((char *)&methodArch, sizeof(methodArch))) {
            break;
        }
        binFile.read(reinterpret_cast<char*>(&sizeOfNameFIle), sizeof(sizeOfNameFIle));
        char nameFile[sizeOfNameFIle + 1];
        binFile.read(reinterpret_cast<char*>(&nameFile), sizeOfNameFIle);
        nameFile[sizeOfNameFIle] = '\0';

        char fileOut[PATH_MAX];
        strcpy(fileOut, this->path);
        strcat(fileOut, nameFile);

        createDirectories(fileOut);
        std::vector<std::string> chunks = splitArchive(binFile, 1048576);
        decompressChunks(chunks);
        combineDecompressedChunks(chunks, fileOut);
        cleanupDecompess(chunks);

    }
    binFile.close();
    fflush(stdout);
    printf("Files decompressed\n");
}

void Archive::help() {
    printf("Usage: program [OPTIONS]\n\n");
    printf("Options:\n");
    printf("  -f, --file FILE\tSpecify the file to be archived/unarchived.\n");
    printf("  -p, --path PATH\tSpecify the path where the archive/unarchive should be placed. Default is current directory.\n");
    printf("  -a, --archive\t\tArchive the specified file(s).\n");
    printf("  -u, --unarchive\tUnarchive the specified file(s).\n");
    printf("  -h, --hofman\t\tUse Hofman method for archivation.\n");
    printf("  -l, --lzw\t\tUse LZW method for archivation.\n");
    printf("  -L, --lz77\t\tUse LZ77 method for archivation.\n");
    printf("  -n, --name NAME\tSpecify the name of the archive.\n");
    printf("  -H, --help\t\tDisplay this help and exit.\n");
}