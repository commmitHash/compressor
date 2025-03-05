#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <bitset>

using namespace std;

// A Tree node
struct Node {
    char ch;
    int freq;
    Node *left, *right;
};

// Function to allocate a new tree node
Node* getNode(char ch, int freq, Node* left, Node* right) {
    Node* node = new Node();
    node->ch = ch;
    node->freq = freq;
    node->left = left;
    node->right = right;
    return node;
}

// Comparison object to be used to order the heap
struct comp {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

// Traverse the Huffman Tree and store Huffman Codes in a map
void encode(Node* root, string str, unordered_map<char, string> &huffmanCode) {
    if (root == nullptr)
        return;

    if (!root->left && !root->right) {
        huffmanCode[root->ch] = str;
    }

    encode(root->left, str + "0", huffmanCode);
    encode(root->right, str + "1", huffmanCode);
}

// Traverse the Huffman Tree and decode the encoded string
void decode(Node* root, int &index, string str, ofstream &outFile) {
    if (root == nullptr) {
        return;
    }

    if (!root->left && !root->right) {
        outFile << root->ch;
        return;
    }

    index++;

    if (str[index] == '0')
        decode(root->left, index, str, outFile);
    else
        decode(root->right, index, str, outFile);
}

// Write encoded string as binary data to the compressed file
void writeEncodedStringToFile(const string &encodedStr, const unordered_map<char, string> &huffmanCode, const string &compressedFile) {
    ofstream outCompressed(compressedFile, ios::binary);
    if (!outCompressed.is_open()) {
        cerr << "Unable to open compressed file for writing.\n";
        return;
    }

    // Write Huffman codes to the file
    for (auto pair : huffmanCode) {
        outCompressed << pair.first << pair.second << '\n';
    }
    outCompressed << "END\n"; // Marker to separate codes from encoded data

    // Pack encoded string into bytes and write to file
    char byte = 0;
    int bitCount = 0;
    for (char bit : encodedStr) {
        byte = (byte << 1) | (bit == '1' ? 1 : 0);
        bitCount++;

        if (bitCount == 8) {
            outCompressed.put(byte);
            byte = 0;
            bitCount = 0;
        }
    }

    // Write remaining bits
    if (bitCount > 0) {
        byte <<= (8 - bitCount);
        outCompressed.put(byte);
    }

    outCompressed.close();
    cout << "Encoded string written to " << compressedFile << endl;
}

// Build Huffman Tree and process the input text
void buildHuffmanTree(const string &text, const string &compressedFile, const string &decompressedFile) {
    // Count frequency of appearance of each character
    unordered_map<char, int> freq;
    for (char ch : text) {
        freq[ch]++;
    }

    // Create a priority queue to store live nodes of Huffman tree
    priority_queue<Node*, vector<Node*>, comp> pq;

    // Create a leaf node for each character and add it to the priority queue
    for (auto pair : freq) {
        pq.push(getNode(pair.first, pair.second, nullptr, nullptr));
    }

    // Build the Huffman Tree
    while (pq.size() != 1) {
        Node *left = pq.top(); pq.pop();
        Node *right = pq.top(); pq.pop();
        int sum = left->freq + right->freq;
        pq.push(getNode('\0', sum, left, right));
    }

    // Root of the Huffman Tree
    Node* root = pq.top();

    // Generate Huffman Codes
    unordered_map<char, string> huffmanCode;
    encode(root, "", huffmanCode);

    // Write Huffman Codes to the console
    cout << "Huffman Codes are:\n";
    for (auto pair : huffmanCode) {
        cout << pair.first << " " << pair.second << '\n';
    }

    // Encode the input text
    string encodedStr;
    for (char ch : text) {
        encodedStr += huffmanCode[ch];
    }

    // Write the encoded string and Huffman codes to the compressed file
    writeEncodedStringToFile(encodedStr, huffmanCode, compressedFile);

    // Decode the encoded string and write to the decompressed file
    ofstream outDecompressed(decompressedFile);
    if (outDecompressed.is_open()) {
        int index = -1;
        while (index < (int)encodedStr.size() - 2) {
            decode(root, index, encodedStr, outDecompressed);
        }
        outDecompressed.close();
        cout << "Decoded string written to " << decompressedFile << endl;
    } else {
        cerr << "Unable to open decompressed file for writing.\n";
    }
}

// Main function
int main() {
    string inputFile = "C:\\Users\\Lenovo\\OOP VS code\\dsa project\\aaronhancy.txt";
    string compressedFile = "C:\\Users\\Lenovo\\OOP VS code\\dsa project\\compressed.bin";
    string decompressedFile = "C:\\Users\\Lenovo\\OOP VS code\\dsa project\\decompressed.txt";

    // Read the input text from the file
    ifstream inFile(inputFile);
    string text;
    if (inFile.is_open()) {
        text.assign((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
        inFile.close();
    } else {
        cerr << "Unable to open input file.\n";
        return 1;
    }

    // Build the Huffman Tree and process the files
    buildHuffmanTree(text, compressedFile, decompressedFile);

    return 0;
}