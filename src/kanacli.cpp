#define BACKGROUND 1
#define FOREGROUND 2
#define RESET 0

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <random>

void moveCursor(int x, int y);
void clearScreen();
void changeColor(int colorType, int r, int g, int b);
void changeColor(int colorType);
void loadFile(std::string filePath, std::ifstream &inputFile);
void populateDictionary(std::ifstream &inputFile, std::vector<std::pair<std::string, std::string>> &kanaDictionary);
void shuffleKana(std::vector<std::pair<std::string, std::string>> &kanaDictionary);
void printInterface(std::ifstream &inputFile);
void printKana(int x, int y, std::vector<std::pair<std::string, std::string>> &kanaDictionary, size_t kanaIndex);
void checkInput(std::vector<std::pair<std::string, std::string>> kanaDictionary, size_t &kanaIndex);

int main(){
	std::vector<std::pair<std::string, std::string>> kanaDictionary;
	std::ifstream inputFile;
	
	clearScreen();
	
	loadFile("./assets/hiragana.kana", inputFile);
	populateDictionary(inputFile, kanaDictionary);
	inputFile.close();
	
	shuffleKana(kanaDictionary);
	
	loadFile("./assets/interface.kana", inputFile);
	printInterface(inputFile);
	inputFile.close();
	
	size_t kanaIndex = kanaDictionary.size()-1;
	
	while(kanaIndex > 0){
		printKana(3, 22, kanaDictionary, kanaIndex);
		checkInput(kanaDictionary, kanaIndex);
	}

	changeColor(RESET);
}

void moveCursor(int x, int y) {
	std::cout << "\033[" << x << ";" << y << "H";
}

void clearScreen() {
	std::cout << "\033[2J\033[H";
}

void changeColor(int colorType, int r, int g, int b) {
	if(colorType==FOREGROUND)
		std::cout << "\033[38;2;" << r << ";" << g << ";" << b << "m";
	else if(colorType==BACKGROUND){
		std::cout << "\033[48;2;" << r << ";" << g << ";" << b << "m";
	}
}

void changeColor(int colorType) {
	if(colorType==RESET)
		std::cout<<"\033[0m";
}

void loadFile(std::string filePath, std::ifstream &inputFile){
	inputFile.open(filePath); // Open the input file

	if (!inputFile.is_open()) {
		std::cerr << "Error opening file!" << std::endl;
	}
}

void populateDictionary(std::ifstream& inputFile, std::vector<std::pair<std::string, std::string>> &kanaDictionary){
	std::string line;
	std::string romaji;
	std::string asciiArt;

	while (getline(inputFile, line)) {
		// Detect romaji header lines such as [a], [i], etc.
		if (line[0] == '[') {
			if (!romaji.empty()) {
				kanaDictionary.emplace_back(romaji, asciiArt); // Store previous mapping
			}
			romaji = line.substr(1, line.find(']') - 1); // Extract romaji between brackets
			asciiArt.clear(); // Reset ASCII art for the next romaji
		} else {
			asciiArt += line + "\n"; // Collect ASCII art for the current romaji
		}
	}

	// Store the last romaji and its ASCII art
	if (!romaji.empty()) {
		kanaDictionary.emplace_back(romaji, asciiArt);
	}
}

void shuffleKana(std::vector<std::pair<std::string, std::string>> &kanaDictionary){
	std::random_device rd;
	std::default_random_engine rng(rd());
	std::shuffle(kanaDictionary.begin(), kanaDictionary.end(), rng);	
}
void printInterface(std::ifstream &inputFile){
	std::string line;

	changeColor(FOREGROUND, 255, 182, 193);
	
	while (getline(inputFile, line)) {
		std::cout<<line<<std::endl;
	}
	
	changeColor(RESET);
}

void printKana(int x, int y, std::vector<std::pair<std::string, std::string>> &kanaDictionary, size_t kanaIndex){
	const auto& [romaji, kanaArt] = kanaDictionary[kanaIndex];
	
	size_t start = 0;
	size_t end;
	int lineOffset = 0;
	
	changeColor(FOREGROUND, 119, 221, 119);
	while ((end = kanaArt.find('\n', start)) != std::string::npos) {
		std::string line = kanaArt.substr(start, end - start);
		moveCursor(x + lineOffset, y);
		std::cout << line;
		start = end + 1;
		++lineOffset;
	}
	changeColor(RESET);
}

void checkInput(std::vector<std::pair<std::string, std::string>> kanaDictionary, size_t &kanaIndex){
	const auto& [romaji, kanaArt] = kanaDictionary[kanaIndex];
	std::string guess;
	
	moveCursor(23, 37);
	std::cout<<"   ";
	moveCursor(23, 37);	
	std::cin>>guess;
	
	if(guess != romaji){
		moveCursor(25,36);
		std::cout<<"\033[31mWrong!\033[0m";
	}
	else{
			moveCursor(25, 36);
			std::cout<<"      ";
			kanaIndex--;
	}
}
