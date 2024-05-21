#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <list>

extern std::vector<bool> code;
extern std::map<char, std::vector<bool> > table;

class Node {
public:
    char symbol;
    int count;
    Node* left;
    Node* right;

    Node ();
    Node (Node* Left, Node* Right);
    ~Node ();

    static bool compare(const Node* a, const Node* b);
    static void createTree(std::list<Node*>& countList);
    static Node* readTreeFromFile(std::ifstream& inFile);
};

void checkEmptyFile(std::ifstream& file, char archivation, char* outFile);

void BuildTable(Node* root);

bool checkLastByte(std::ifstream& fileIn, int endPos, int& sizeBits);