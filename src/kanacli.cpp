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
#include <unistd.h>

#ifdef _WIN32
#include <windows.h>
#endif

struct ColorRGB {
	int r, g, b;
};

// COLOR PALETTE
// https://coolors.co/palette/cdb4db-ffc8dd-ffafcc-bde0fe-a2d2ff
const ColorRGB THISTLE = {205, 180, 219};
const ColorRGB FAIRY_TALE = {255, 200, 221};
const ColorRGB CARNATION_PINK = {255, 175, 204};
const ColorRGB URANIAN_BLUE = {189, 224, 254};
const ColorRGB LIGHT_SKY_BLUE = {162, 210, 255};

void moveCursor(int x, int y);
void clearScreen();
void setupConsole();
void changeColor(int colorType, int r, int g, int b);
void changeColor(int colorType, const ColorRGB &color);
void changeColor(int colorType);
void loadFile(std::string filePath, std::ifstream &inputFile);
void populateDictionary(std::ifstream &inputFile, std::vector<std::pair<std::string, std::string>> &kanaDictionary);
void shuffleKana(std::vector<std::pair<std::string, std::string>> &kanaDictionary);
void printFeedback(int correctAnswers, int wrongAnswers);
void printInterface(std::ifstream &inputFile);
void printKana(int x, int y, std::vector<std::pair<std::string, std::string>> &kanaDictionary, size_t kanaIndex);
void checkInput(std::vector<std::pair<std::string, std::string>> kanaDictionary, size_t &kanaIndex, int &correctAnswers, int &wrongAnswers);

int main(){
	int correctAnswers = 0;
	int wrongAnswers = 0;
	//int totalKana = 8;
	
	std::vector<std::pair<std::string, std::string>> kanaDictionary;
	std::ifstream inputFile;

	setupConsole();
	clearScreen();

	loadFile("./assets/hiragana.kana", inputFile);
	populateDictionary(inputFile, kanaDictionary);
	inputFile.close();
	
	shuffleKana(kanaDictionary);
	
	loadFile("./assets/interface.kana", inputFile);
	printInterface(inputFile);
	inputFile.close();
	printFeedback(correctAnswers, wrongAnswers);
	
	size_t kanaIndex = 0;
	
	while(kanaIndex < kanaDictionary.size()){
		printKana(3, 22, kanaDictionary, kanaIndex);
		checkInput(kanaDictionary, kanaIndex, correctAnswers, wrongAnswers);
		printFeedback(correctAnswers, wrongAnswers);
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

void changeColor(int colorType, const ColorRGB &color){
	if(colorType==FOREGROUND)
		std::cout << "\033[38;2;" << color.r << ";" << color.g << ";" << color.b << "m";
	else if(colorType==BACKGROUND){
		std::cout << "\033[48;2;" << color.r << ";" << color.g << ";" << color.b << "m";
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

void printFeedback(int correctAnswers, int wrongAnswers){
	moveCursor(7,66);
	changeColor(FOREGROUND, URANIAN_BLUE);
	std::cout<<"Correct: " << correctAnswers;
	
	moveCursor(8, 66);
	changeColor(FOREGROUND, CARNATION_PINK);
	std::cout<<"Wrong:   " << wrongAnswers;
	
	changeColor(RESET);
}

void printInterface(std::ifstream &inputFile){
	std::string line;

	changeColor(FOREGROUND, THISTLE);
	
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
	
	changeColor(FOREGROUND, LIGHT_SKY_BLUE);
	
	while ((end = kanaArt.find('\n', start)) != std::string::npos) {
		std::string line = kanaArt.substr(start, end - start);
		moveCursor(x + lineOffset, y);
		std::cout << line;
		start = end + 1;
		++lineOffset;
	}
	changeColor(RESET);
}

void checkInput(std::vector<std::pair<std::string, std::string>> kanaDictionary, size_t &kanaIndex, int &correctAnswers, int &wrongAnswers){
	const auto& [romaji, kanaArt] = kanaDictionary[kanaIndex];
	std::string guess;
	
	moveCursor(23, 37);
	std::cout<<"   ";
	moveCursor(23, 37);	
	
	std::cin>>guess;
	
	if(guess != romaji ){
		wrongAnswers++;
		kanaIndex++;
	}
	else{
		correctAnswers++;
		kanaIndex++;
	}
}

void setupConsole(){
	#ifdef _WIN32
        SetConsoleOutputCP(65001);
        SetConsoleCP(65001);
    #endif	
}