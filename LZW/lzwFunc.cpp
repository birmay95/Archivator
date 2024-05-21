#include "lzwHeader.h"

std::vector<int> LempelZivWelch::compress(const std::string& inputData) {
    int dictSize = 256;
    std::unordered_map<std::string, int> dictionary;
    for (int i = 0; i < dictSize; ++i) {
        dictionary[std::string(1, static_cast<char>(i))] = i;
    }

    std::string foundChars = "";
    std::vector<int> result;
    for (char character : inputData) {
        std::string charsToAdd = foundChars + character;
        if (dictionary.find(charsToAdd) != dictionary.end()) {
            foundChars = charsToAdd;
        } else {
            result.push_back(dictionary[foundChars]);
            dictionary[charsToAdd] = dictSize++;
            foundChars = std::string(1, character);
        }
    }
    if (!foundChars.empty()) {
        result.push_back(dictionary[foundChars]);
    }
    return result;
}

std::string LempelZivWelch::decompress(const std::vector<int>& compressedData) {
    int dictSize = 256;
    std::unordered_map<int, std::string> dictionary;
    for (int i = 0; i < dictSize; ++i) {
        dictionary[i] = std::string(1, static_cast<char>(i));
    }

    std::string characters = std::string(1, static_cast<char>(compressedData[0]));
    std::string result = characters;
    for (size_t i = 1; i < compressedData.size(); ++i) {
        int code = compressedData[i];
        std::string entry = (dictionary.find(code) != dictionary.end())
            ? dictionary[code]
            : characters + characters[0];
        result += entry;
        dictionary[dictSize++] = characters + entry[0];
        characters = entry;
    }
    return result;
}