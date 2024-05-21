#include "lzwHeader.h"

int main(int argc, char *argv[]) {

    char archivation = *argv[0];
    if (archivation == '1') {
        std::ifstream fileIn(argv[1]);
        if (!fileIn) {
            fprintf(stderr, "Error opening file: %s\n", argv[1]);
            return 1;
        }

        std::string inputData;
        char temp;
        while (fileIn.read(reinterpret_cast<char*>(&temp), sizeof(temp))) {
            inputData.push_back(temp);
        }

        std::vector<int> compressedData = LempelZivWelch::compress(inputData);

        std::ofstream fileOut(argv[2], std::ios::binary);
        if (!fileOut) {
            fprintf(stderr, "Error opening file: %s\n", "outfile.bin");
            return 1;
        }

        for (int code : compressedData) {
            fileOut.write(reinterpret_cast<char*>(&code), sizeof(code));
        }

        fileIn.close();
        fileOut.close();
    } else {
        std::ifstream fileIn(argv[2], std::ios::binary);
        if (!fileIn) {
            fprintf(stderr, "Error opening file: %s\n", "outfile.bin");
            return 1;
        }

        std::vector<int> compressedData;
        int temp;
        while (fileIn.read(reinterpret_cast<char*>(&temp), sizeof(temp))) {
            compressedData.push_back(temp);
        }

        std::string decompressedData = LempelZivWelch::decompress(compressedData);

        std::ofstream fileOut(argv[1]);
        if (!fileOut) {
            fprintf(stderr, "Error opening file: %s\n", argv[1]);
            return 1;
        }

        fileOut << decompressedData;

        fileOut.close();
        fileIn.close();
    }

    return 0;
}
