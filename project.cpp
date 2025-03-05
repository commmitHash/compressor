#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <vector>

using namespace std;

// Node structure for Huffman tree
struct Node {
    char ch;
    int freq;
    Node *left, *right;

    Node(char ch, int freq, Node* left = nullptr, Node* right = nullptr) {
        this->ch = ch;
        this->freq = freq;
        this->left = left;
        this->right = right;
    }
};

// Comparison function for priority queue
struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

// Generate Huffman codes recursively
void generateCodes(Node* root, string code, unordered_map<char, string>& huffmanCode) {
    if (!root)
        return;
    
    if (!root->left && !root->right) {
        huffmanCode[root->ch] = code;
    }
    
    generateCodes(root->left, code + "0", huffmanCode);
    generateCodes(root->right, code + "1", huffmanCode);
}

// Build Huffman Tree and return root
Node* buildHuffmanTree(const string& text, unordered_map<char, string>& huffmanCode) {
    unordered_map<char, int> freq;
    for (char ch : text) {
        freq[ch]++;
    }
    
    priority_queue<Node*, vector<Node*>, Compare> pq;
    for (auto& pair : freq) {
        pq.push(new Node(pair.first, pair.second));
    }
    
    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        pq.push(new Node('\0', left->freq + right->freq, left, right));
    }
    
    Node* root = pq.top();
    generateCodes(root, "", huffmanCode);
    return root;
}

// Compress text using Huffman coding
string compressText(const string& text, unordered_map<char, string>& huffmanCode) {
    string compressed = "";
    for (char ch : text) {
        compressed += huffmanCode[ch];
    }
    return compressed;
}

// Save compressed data to file
void saveCompressedFile(const string& filename, const string& compressedData, unordered_map<char, string>& huffmanCode) {
    ofstream file(filename, ios::binary);
    file << huffmanCode.size() << "\n";
    for (auto& pair : huffmanCode) {
        file << pair.first << " " << pair.second << "\n";
    }
    file << compressedData;
    file.close();
}

// Load Huffman codes from file
unordered_map<string, char> loadHuffmanCodes(ifstream& file) {
    unordered_map<string, char> reverseHuffmanCode;
    int size;
    file >> size;
    file.ignore();
    while (size--) {
        char ch;
        string code;
        file.get(ch);
        file.ignore();
        getline(file, code);
        reverseHuffmanCode[code] = ch;
    }
    return reverseHuffmanCode;
}

// Decompress Huffman encoded string
string decompressText(const string& compressedData, unordered_map<string, char>& reverseHuffmanCode) {
    string decoded = "", temp = "";
    for (char bit : compressedData) {
        temp += bit;
        if (reverseHuffmanCode.find(temp) != reverseHuffmanCode.end()) {
            decoded += reverseHuffmanCode[temp];
            temp = "";
        }
    }
    return decoded;
}

// Load compressed file and decompress it
void decompressFile(const string& compressedFilename, const string& outputFilename) {
    ifstream file(compressedFilename, ios::binary);
    auto reverseHuffmanCode = loadHuffmanCodes(file);
    string compressedData((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    
    string decompressedText = decompressText(compressedData, reverseHuffmanCode);
    ofstream outFile(outputFilename);
    outFile << decompressedText;
    outFile.close();
}

int main() {
    string inputFilename = "C:\\Users\\Lenovo\\OneDrive\\Desktop\\c^M^Mproject.docx";
    string compressedFilename = "C:\\Users\\Lenovo\\OneDrive\\Desktop\\compressed.huff";
    string decompressedFilename = "C:\\Users\\Lenovo\\OneDrive\\Desktop\\decompressed.docx";

    // Read input file in binary mode
    ifstream inputFile(inputFilename, ios::binary);
    if (!inputFile) {
        cout << "Error: Unable to open input file!" << endl;
        return 1;
    }

    string text((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();

    if (text.empty()) {
        cout << "Error: Input file is empty!" << endl;
        return 1;
    }

    unordered_map<char, string> huffmanCode;
    buildHuffmanTree(text, huffmanCode);

    string compressedData = compressText(text, huffmanCode);
    saveCompressedFile(compressedFilename, compressedData, huffmanCode);

    // Decompress the file
    decompressFile(compressedFilename, decompressedFilename);

    cout << "Compression and Decompression completed!" << endl;
    ifstream file("C:\\Users\\Lenovo\\OneDrive\\Desktop\\compressed.huff", ios::binary);
string data((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
cout << "Compressed file content: " << data << endl;
file.close();

    return 0;
}
