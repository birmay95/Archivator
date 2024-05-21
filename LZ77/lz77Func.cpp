#include "lz77Header.h"

bool dataLeak = false;

void writeDecompressedData(const std::string& filename, const std::string& data) {
    std::ofstream fileOut(filename, std::ios::binary);
    fileOut << data;
    fileOut.close();
}

void readCompressedData(const std::string& filename, std::vector<Token>& compressedData) {
    std::ifstream fileIn(filename, std::ios::binary);
    if (!fileIn) {
        std::cerr << "Error: Unable to open input file." << std::endl;
        return;
    }
    Token token;
    while (fileIn.read(reinterpret_cast<char*>(&token), sizeof(token))) {
        compressedData.push_back(token);
    }
    fileIn.close();
}

void writeCompressedData(const std::string& filename, const std::vector<Token>& compressedData) {
    std::ofstream fileOut(filename, std::ios::binary);
    for (const auto& token : compressedData) {
        fileOut.write(reinterpret_cast<const char*>(&token), sizeof(token));
    }
    fileOut.close();
}

std::vector<Token> LZ77::compress(const std::string& input) {
    std::vector<Token> compressedData;
    int inputSize = input.size();
    int pos = 0;
    
    while (pos < inputSize) {
        int maxLength = 0;
        int maxOffset = 0;
        
        for (int i = 1; i <= pos; ++i) {
            int length = 0;
            while (pos + length < inputSize && input[pos - i + length] == input[pos + length]) {
                ++length;
            }
            if (length > maxLength) {
                maxLength = length;
                maxOffset = i;
            }
        }
        
        Token token;
        token.offset = maxOffset;
        token.length = maxLength;
        token.nextChar = input[pos + maxLength];
        compressedData.push_back(token);
        
        pos += (maxLength + 1);
    }

    return compressedData;
}

std::string LZ77::decompress(const std::vector<Token>& compressedData) {
    std::string decompressedData;
    for (const auto& token : compressedData) {
        int startPos = decompressedData.size() - token.offset;
        if (startPos < 0) {
            for (int i = 0; i < -startPos; ++i) {
                decompressedData += '\0';
            }
            startPos = 0;
            dataLeak = true;
        }
        for (int i = 0; i < token.length; ++i) {
            decompressedData += decompressedData[startPos + i];
        }
            decompressedData += token.nextChar;
    }
    if (!decompressedData.empty() && decompressedData.back() == 0) {
        decompressedData.pop_back();
    }
    return decompressedData;
}