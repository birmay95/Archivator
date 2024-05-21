#include "lz77Header.h"

int main(int argc, char *argv[]) {
    char archivation = *argv[0];

    if(archivation == '1') {
        std::ifstream fileIn(argv[1], std::ios::binary);
        if (!fileIn) {
            std::cerr << "Error: Unable to open input file." << std::endl;
            return 1;
        }

        std::string inputData;
        char temp;
        while(fileIn.read(reinterpret_cast<char*>(&temp), sizeof(temp))) {
            inputData.push_back(temp);
        }
        fileIn.close();

        std::vector<Token> compressedData = LZ77::compress(inputData);

        writeCompressedData(argv[2], compressedData);

    } else {
        std::vector<Token> compressedData;
        readCompressedData(argv[2], compressedData);

        std::string decompressedData = LZ77::decompress(compressedData);

        writeDecompressedData(argv[1], decompressedData);
        if (dataLeak) {
            std::cout << "File decompressed not successfully. There were some data leaks\n";
            return 1;
        }
    }

    return 0;
}