#include "hofmanHeader.h"

int main(int argc, char *argv[]) {

    char archivation = *argv[0];
    std::ifstream fileIn;
    std::ofstream fileOut;
    Node* root = nullptr;
    int count = 0;
    char buf = 0;
    
    if(archivation == '1') {
        fileIn.open(argv[1]);
        if (!fileIn.is_open()) {
            fprintf(stderr, "Error opening file: %s\n", argv[1]);
            return 1;
        }
        checkEmptyFile(fileIn, archivation, argv[1]);

        std::map<char, int> countMap;
        char sym;
        while((sym = fileIn.get()) != -1) {
            countMap[sym]++;
        }
        if(countMap.size() == 1) {
            if(countMap['0'] == 0) {
                countMap['0']++;
            } else {
                countMap['1']++;
            }
        }

        std::list<Node*> countList;
        std::map<char, int>::iterator iter;

        fileOut.open(argv[2], std::ios::binary);
        if (!fileOut) {
            fprintf(stderr, "Error opening file: %s\n", "outfile.bin");
            return 1;
        }
        int sizeMap = countMap.size();
        fileOut.write(reinterpret_cast<char*>(&sizeMap), sizeof(sizeMap));
        for (iter = countMap.begin(); iter != countMap.end(); iter++) {
            Node* temp = new Node;
            temp->symbol = iter->first;
            temp->count = iter->second;
            fileOut.write(reinterpret_cast<char*>(&temp->symbol), sizeof(temp->symbol));
            fileOut.write(reinterpret_cast<char*>(&temp->count), sizeof(temp->count));
            countList.push_back(temp);
        }

        Node::createTree(countList);
        root = countList.front();
        BuildTable(root);

        fileIn.clear();
        fileIn.seekg(0);
        while(!fileIn.eof()) {
            sym = fileIn.get();
            std::vector<bool> temp = table[sym];
            if(sym == -1) {
                fileOut << buf;
                fileOut << (char)(count + '0');
            }

            for(size_t i = 0; i < temp.size(); i++) {
                buf = buf | temp[i] << (7 - count);
                count++;
                if(count == 8) {
                    count = 0;
                    fileOut << buf;
                    buf = 0;
                }
            }
        }

        fileIn.close();
        fileOut.close();
    } else {

        fileIn.open(argv[2], std::ios::binary);
        if (!fileIn) {
            fprintf(stderr, "Error opening file: %s\n", "outfile.bin");
            return 1;
        }
        checkEmptyFile(fileIn, archivation, argv[1]);
        fileOut.open(argv[1]);
        if (!fileOut.is_open()) {
            fprintf(stderr, "Error opening file: %s\n", argv[1]);
            return 1;
        }
        root = Node::readTreeFromFile(fileIn);
        if (root == nullptr) {
            fprintf(stderr, "Error reading tree from file.\n");
            return 1;
        }

        Node* node = root;
        count = 0;
        int sizeBits = 0;
        std::streampos curPos = fileIn.tellg();
        fileIn.seekg(0, std::ios::end);
        int endPos = fileIn.tellg();
        fileIn.seekg(curPos);
        bool fl = checkLastByte(fileIn, endPos, sizeBits);
        buf = fileIn.get();
        while (!fileIn.eof()) {
            if(fl && count == sizeBits) {
                break;
            }
            bool bit = buf & (1 << (7 - count));

            if(bit) {
                node = node->right;
            } else {
                node = node->left;
            }
            if (node->right == nullptr && node->left == nullptr) {
                fileOut << node->symbol;
                node = root;
            }
            count++;
            if(count == 8) {
                count = 0;
                fl = checkLastByte(fileIn, endPos, sizeBits);
                buf = fileIn.get();
            }
        }

        fileIn.close();
        fileOut.close();
    }
    delete root;

    return 0;
}