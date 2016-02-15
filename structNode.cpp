#include <set>
#include <unordered_map>
#include <queue>
#include <sstream>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <stack>
#include <bitset>
using namespace std;
typedef pair < char, int > Pair;

struct Node{
	char letter;
	int value;
	Node *left, 
		*right;

	Node(char letter, int value, Node* left = nullptr, Node* right = nullptr){
		this->letter = letter;
		this->value = value;
		this->left = left;
		this->right = right;
	}
};

unordered_map<char, int>& insertLettersInMap(const char text[]){
	auto& letters = *new unordered_map<char, int>();
	for (int i = 0; text[i] != '\0'; i++){
		if (letters.find(text[i]) != letters.end()){
			letters[text[i]]++;
		}
		else{
			letters[text[i]] = 1;
		}
	}
	return letters;
}

class NodeComparer
{
public:
	bool operator() (const Node& first,const Node& second) const
	{
		return first.value > second.value;
	}
};

priority_queue<Node, vector<Node>, NodeComparer>& insertNodesInHeap(const unordered_map<char, int>& letters){
	
	auto& heap = *new priority_queue<Node, vector<Node>, NodeComparer>();

	for (pair<char, int> pair : letters){
		Node& node = *new Node(pair.first, pair.second);
		heap.push(node);
	}

	return heap;
}


//function to build the tree from heap with Nodes
Node* buildHuffmanTree(priority_queue<Node, vector<Node>, NodeComparer>& heap){
	Node* result;
	while (heap.size() > 1){
		Node* firstMin = new Node(heap.top());
		heap.pop();
		Node* secondMin = new Node(heap.top());
		heap.pop();

		int newValue = firstMin->value + secondMin->value;
		Node& newNode = *new Node('\0', newValue, firstMin, secondMin);
		heap.push(newNode);
	}
	result = new Node(heap.top());
	return result;
}

//recursive function to traverse the tree and generate codes
void generateCodes(Node* huffmanTree, unordered_map<char, string>& map, string code = ""){
	if (huffmanTree->left == nullptr && huffmanTree->right == nullptr){
		map.insert(pair<char, string>(huffmanTree->letter, code));
	}
	if (huffmanTree->left){
		generateCodes(huffmanTree->left, map, code + "0");
	}
	if (huffmanTree->right){
		generateCodes(huffmanTree->right, map, code + "1");
	}
}

//function to put letters and codes in map
unordered_map<char, string>& getCodesInMap(Node* huffmanTree){
	unordered_map<char, string>& codes = *new unordered_map < char, string > ;

	generateCodes(huffmanTree, codes);

	return codes;
}

//function to encode text to sequence of 0 and 1s
string& encodeText(const string& text, unordered_map<char, string>& codes){

	string& bytesString = *new string;
	for (char ch : text){
		bytesString.append(codes[ch]);
	}

	return bytesString;
}

//function to encode text file "sourcePath" to binary file "destPath" 
Node* encodeFile(const char* sourcePath, const char* destPath){
	ifstream input(sourcePath, ios::in);
	string text = { istreambuf_iterator<char>(input), istreambuf_iterator<char>() };

	auto map = insertLettersInMap(text.c_str());
	auto heap = insertNodesInHeap(map);
	Node* head = buildHuffmanTree(heap);
	unordered_map<char, string> codes = getCodesInMap(head);

	ofstream outstream(destPath, std::ios::binary);
	string& bits = encodeText(text, codes);

	auto start = bits.begin();
	auto end = start;
	while (start != bits.end()){
		for (size_t i = 0; i < 8 && end != bits.end(); i++){
			++end;
		}
		string byteString(start, end);
		//bitset<32> bitset(byte);
		char byte = '\0';
		for (int i = 0; i < byteString.length(); i++){
			char bit = byteString[i];
			if (bit == '1') {
				byte |= 1 << (7 - i);
			}
			else {
				byte &= ~(1 << (7 - i));
			}
		}
		//unsigned long number = bitset.to_ulong();
		outstream << byte;
		start = end;
	}

	return head;
}

void decodeFile(const char* sourcePath, const char* destPath, Node* huffmanTree){
	ifstream input(sourcePath, ios::in);
	if (input.eof()){
		cout << "The file is empty!" << endl;
		return;
	}

	ofstream outstream(destPath, ios::out);
	string bitsSequence;

	while (true){
		char byte = '\0';
		input >> noskipws >> byte;
		if (input.eof()) break;
		bitset<8> bitsOfChar(byte);
		bitsSequence.append(bitsOfChar.to_string());
	}
	auto iter = bitsSequence.begin();
	Node* start = huffmanTree;
	while (iter != bitsSequence.end()){
		if (!start->left && !start->right){
			outstream << start->letter;
			start = huffmanTree;
		}

		char bit = *iter;
		if (bit == '0')
			start = start->left;
		else
			start = start->right;

		++iter;
	}
	
}


int main(){
	Node* huffmanTree = encodeFile("source.txt", "dest.huffmancode");
	
	decodeFile("dest.huffmancode", "decoded.txt", huffmanTree);

	system("pause");
	return 0;
}
