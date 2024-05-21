#include "hofmanHeader.h"

std::vector<bool> code;
std::map<char, std::vector<bool>> table;

void checkEmptyFile(std::ifstream& file, char archivation, char* outFile) {
    std::streampos checkSizeBeg = file.tellg();

    file.seekg(0, std::ios::end);

    std::streampos checkSizeEnd = file.tellg();

    if (checkSizeBeg == checkSizeEnd) {
        if (archivation == '1') {
            std::ofstream fileOut("outfile.bin");
            fileOut.close();
        } else {
            std::ofstream fileOut(outFile);
            fileOut.close();
        }
        file.close();
        exit(0);
    } else {
        file.seekg(checkSizeBeg);
    }
}

void BuildTable(Node* root) {
    if(root->left != nullptr) {
        code.push_back(0);
        BuildTable(root->left);
    }
    if(root->right != nullptr) {
        code.push_back(1);
        BuildTable(root->right);
    }
    if(root->symbol) {
        table[root->symbol] = code;
    }
    code.pop_back();
}

bool checkLastByte(std::ifstream& fileIn, int endPos, int& sizeBits) {
    char buf;
    int curPosInt = fileIn.tellg();
    int bytesLeft = endPos - curPosInt;
    if (bytesLeft == 2) {
        fileIn.seekg(1, std::ios::cur);
        buf = fileIn.get();
        sizeBits = buf - '0';
        fileIn.seekg(-2, std::ios::cur);
        return true;
    }
    return false;
}

Node::Node(): symbol(0), count(0), left(nullptr), right(nullptr) {}

Node::Node(Node* Left, Node* Right) {
    left = Left;
    right = Right;
    count = Left->count + Right->count;
    symbol = 0;
}

Node::~Node () {
    delete left;
    delete right;
}

bool Node::compare(const Node* a, const Node* b) {
    return a->count < b->count;
}

void Node::createTree(std::list<Node*>& countList) {
    while (countList.size() != 1) {
        countList.sort(compare);

        Node* sonLeft = countList.front();
        countList.pop_front();

        Node* sonRight = countList.front();
        countList.pop_front();

        Node* parent = new Node(sonLeft, sonRight);
        countList.push_back(parent);
    }
}

Node* Node::readTreeFromFile(std::ifstream& inFile) {
    int countEL;
    std::list<Node*> countList;

    inFile.read(reinterpret_cast<char*>(&countEL), sizeof(countEL));
    for (int i = 0; i < countEL; i++) {
        Node* temp = new Node;
        inFile.read(reinterpret_cast<char*>(&temp->symbol), sizeof(temp->symbol));
        inFile.read(reinterpret_cast<char*>(&temp->count), sizeof(temp->count));
        countList.push_back(temp);
    }

    createTree(countList);

    return countList.front();
}