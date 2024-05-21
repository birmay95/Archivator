#include <iostream>
#include <fstream>
#include <vector>
#include <string>

extern bool dataLeak;

struct Token {
    int offset;
    int length;
    char nextChar;
};

class LZ77 {
public:

    static std::vector<Token> compress(const std::string& input);
    static std::string decompress(const std::vector<Token>& compressedData);
};

void writeCompressedData(const std::string& filename, const std::vector<Token>& compressedData);

void readCompressedData(const std::string& filename, std::vector<Token>& compressedData);

void writeDecompressedData(const std::string& filename, const std::string& data);