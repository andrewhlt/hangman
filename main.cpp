#include <iostream> 
#include <fstream> 
#include <sstream> 
#include <utility> 
#include <string>
#include <vector>
#include <unordered_map>

// Method that reads dictionary text file into a unordered_map mapping from word length to vectors of words of that length. 
void readDictionary(std::unordered_map<int, std::vector<std::string> > &dictionaryMap) {
  // list over set: although unique values and order does not matter, since iterating over everything later it will be O(N) anyway
  // list has less memory usage than a set. Not searching for particular words, not inserting in middle, not deleting. 
  std::string currWord;
  std::ifstream inputFile("dictionary.txt");

  while (std::getline(inputFile, currWord)) {
    int wordLen = currWord.length();
    if (dictionaryMap.find(wordLen) == dictionaryMap.end()) {
      dictionaryMap[wordLen] = std::vector<std::string>();    
    }
    dictionaryMap[wordLen].push_back(currWord);
  }
}

template <class T>
T getUserSpecifiedValues(std::string message, std::unordered_map<int, std::vector<std::string> > &dictionaryMap, bool dictFind) {
  T requested;
  std::stringstream ss; 

  while (true) {
    std::string userInputWordLength;
    std::cout << message;
    std::cin >> userInputWordLength; 

    ss << userInputWordLength;
    ss >> requested;

    if (!ss.fail()) {
      if (dictFind) {
        if (dictionaryMap.find(requested) != dictionaryMap.end()) {
          return requested;
        } else {
          std::cout << "Sorry, we don't have any words of that length. Try again" << std::endl;
        }
      } else {
        return requested;
      }
    }

    ss.str(std::string());
    ss.clear();
  }
}

std::string generatePositions(char currGuess, std::string currWord,  std::string currState) {
  std::string buildup;

  for (size_t i = 0; i < currWord.length(); i++) {
    if (currWord[i] == currGuess) {
      buildup += currGuess;  
    } else {
      buildup += currState[i];
    }
  }

  return buildup;
}

int countCharsLeft(std::string currState) {
  int numDashes;
  for (size_t i = 0; i < currState.length(); i++) {
    if (currState[i] == '-') numDashes++;
  }
  return numDashes;
}

void evilHangman(std::string currState, std::vector<std::string> &initialWordBank, int numWordsRemaining, int numGuesses, std::unordered_map<int, std::vector<std::string> > &dictionaryMap) {
  std::unordered_map<std::string, std::pair<int, std::vector<std::string> > > wordFamiliesMap;
  std::vector<std::string> currWordBank = initialWordBank;

  while (numGuesses > 0)  { 
    int currMax = INT_MIN; 
    std::string currMaxKey; 

    std::cout << "The current state: \n" << currState << '\n' << std::endl;
    char currGuess = getUserSpecifiedValues<char>("Guess a letter: ", dictionaryMap, false);

    if (isalpha(currGuess)) {
      currGuess = tolower(currGuess);
    }
    std::cout << "You guessed: " << currGuess << std::endl;

    // go through list of words
    // create unordered_map mapping from "--E-" to vector {FLEX, IBEX}
    for (std::string &currWord: currWordBank) {
      std::string positionKey = generatePositions(currGuess, currWord, currState);

      if (wordFamiliesMap.find(positionKey) == wordFamiliesMap.end()) {
        wordFamiliesMap[positionKey] = std::make_pair(0, std::vector<std::string>());    
      }
      wordFamiliesMap[positionKey].first += 1;
      wordFamiliesMap[positionKey].second.push_back(currWord);
    }

    for (auto &wordFamily: wordFamiliesMap) {
      if (wordFamily.second.first > currMax) {
        currMax = wordFamily.second.first;
        currMaxKey = wordFamily.first;
      } else if (wordFamily.second.first == currMax) {
        if (countCharsLeft(wordFamily.first) > countCharsLeft(currMaxKey)) {
          currMax = wordFamily.second.first;
          currMaxKey = wordFamily.first;
        }
      }
    }

    currState = currMaxKey; 

    currWordBank = wordFamiliesMap[currMaxKey].second;
    numGuesses--;

    if (countCharsLeft(currState) == 0) {
      std::cout << "Hey! You won! The word was: " << currState << std::endl;
      return;
    }
    if (numGuesses == 0) {
      std::cout << "Womp womp! You lost...the word was: " << currWordBank[0] << std::endl;
      return;
    }
    wordFamiliesMap.clear();
  }
}

void playOneRound(std::unordered_map<int, std::vector<std::string> > &dictionaryMap) {
  int requestedWordLen = getUserSpecifiedValues<int>("How long do you want your word to be?: ", dictionaryMap, true);
  int requestedNumGuesses = getUserSpecifiedValues<int>("How many guesses do you want?: ", dictionaryMap, false);

  std::cout << "Your chosen length is: " << requestedWordLen << ".\n";
  std::cout << "Your chosen # of guesses is: " << requestedNumGuesses << ". Let us begin!\n" << "==============================================\n" << std::endl;

  std::vector<std::string> &initialWordBank = dictionaryMap[requestedWordLen];
  int numWordsRemaining = initialWordBank.size();
  std::string initialState(requestedWordLen, '-');

  evilHangman(initialState, initialWordBank, numWordsRemaining, requestedNumGuesses, dictionaryMap);
}

// Generates user responses to either start or exit program. If user opts to start, plays one round of the game. 
void initializeGame(std::unordered_map<int, std::vector<std::string> > &dictionaryMap) {
  // Each iteration of the while loop corresponds to one round of the game. 
  while (true) {
    std::string userResponse;

    std::cout << "Shall we play? (y/n): " << std::endl;
    std::cin >> userResponse;

    if (userResponse == "y") {
      playOneRound(dictionaryMap);
    } else if (userResponse == "n") {
      std::cout << "See you next time!" << std::endl;
      return; 
    } else {
      std::cout << "Please respond with either yes (y) or no (n)." << std::endl;
    }
  }
}

int main(int argc, char* argv[]) {
  std::unordered_map<int, std::vector<std::string> > dictionaryMap; // "Master" data structure mapping from word length to all words of that length. 

  readDictionary(dictionaryMap);
  initializeGame(dictionaryMap);

  return 0; 
}