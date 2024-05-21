#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>

class LempelZivWelch {
public:

    static std::vector<int> compress(const std::string& inputData);
    static std::string decompress(const std::vector<int>& compressedData);
};