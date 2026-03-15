/*----------------------------------------------------------------------
A basic lexer with a getToken function which assumes the input 
string consists of identifiers, numbers, and operators.

g++ -std=c++17 DFA.cpp simple_lexer_DFA.cpp
./a.out < teststream.txt 
more output.txt
------------------------------------------------------------------------*/
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "regex_compiler.h"
#include "DFA.h"
#include "NFA.h"

using namespace std;

// Token class
// no longer has hardcoded token types, user specifies the name of each token
class Token 
{
public:
    Token(string t, string v) : type(t), value(v) {};
    Token() : type("EOS"), value("") {};

    string type;
    string value;
};

// Lexer class
// now dynamically creates dfas (one per token) instead of just having a few
// hard coded in
class Lexer 
{
public:
    Lexer(const vector<Token>& tokens, const string& inputString);
    Token getToken();
private:
    string input;
    int pos;

    vector<string> tokenNames;
    vector<DFA> tokenDFAs;

};


Lexer::Lexer(const vector<Token>& tokens, const string& inputString) 
    : input(inputString), pos(0) {
    // dynamically makes dfas depending on the amount of tokens provided
    for (auto token : tokens) {
        tokenNames.push_back(token.type);

        string postfix = InfixToPostfix(token.value);
        NFA nfa = PostfixtoNFA(postfix);

        tokenDFAs.push_back(nfa.NFA2DFA());
    }

}


// return next token from the input string
// uses maximal munch then priority by order in which the tokens were given by user
Token Lexer::getToken()
{
    // skip any whitespace
    while (pos < input.size() && isspace(input[pos])) {
        pos++;
    }

	// return eos if at the end of the input string
    if (pos == input.size()) {
        return Token("EOS", "");
    }

	// reset state of dfas from previous token scan
    for (auto& dfa : tokenDFAs) {
        dfa.Reset();
    }

    int scanningPosition = pos;           // current position in the input string
    int lastAcceptedPosition = -1;            // highest accepted position
	int lastAcceptedTokenIndex = -1;          // which token type has the highest accepted position
    string lexeme = "";                   // longest match substring

    // dfas (tokens) that are still possible for longest match (havent rejected yet)
    vector<bool> active(tokenDFAs.size(), true);

    while (scanningPosition < input.size()) {

        bool anyActive = false;

        // feed current char to each active dfa
        for (int i = 0; i < tokenDFAs.size(); i++) {

            if (!active[i])
                continue;

            tokenDFAs[i].Move(input[scanningPosition]);

            // this dfa cant match any further, mark failed
            if (tokenDFAs[i].GetStatus() == FAIL) {
                active[i] = false;
            }
            else {
                anyActive = true;
            }

			// if dfa is still accepting, it is still a candidate for longest match, check if it is the longest so far
            if (tokenDFAs[i].GetAccepted()) {

                int newLen = scanningPosition + 1 - pos;
                int oldLen = lastAcceptedPosition - pos;

				// prefer longest match, OR if tie, prefer the one that appears first in the token list
                if (lastAcceptedTokenIndex == -1 ||
                    newLen > oldLen ||
                    (newLen == oldLen && i < lastAcceptedTokenIndex)) {

                    lastAcceptedPosition = scanningPosition + 1;
                    lastAcceptedTokenIndex = i;
                    lexeme = tokenDFAs[i].GetAcceptedLexeme();
                }
            }
        }

        // stop if all dfas have failed
        if (!anyActive)
            break;

        scanningPosition++;
    }

	// if a match was found, update the position and return the token
    if (lastAcceptedTokenIndex != -1) {
        pos = lastAcceptedPosition;
        return Token(tokenNames[lastAcceptedTokenIndex], lexeme);
    }

	// if no match was found, return invalid token for the current character and move forward
    return Token("INVALID", string(1, input[pos++]));

}


// parses the tokens from the given input
vector<Token> parseTokens(const string& input) {
    string inputTokens = input.substr(0, input.find('#'));
    vector<Token> tokens;

    stringstream ss(inputTokens);
    string oneToken;
    while (getline(ss, oneToken, ',')) {
        // erase whitespace from front and back
        while (!oneToken.empty() && isspace(oneToken.front())) {
            oneToken.erase(0, 1);
        }
        while (!oneToken.empty() && isspace(oneToken.back())) {
            oneToken.pop_back();
        }

        size_t delimiter = oneToken.find(' ');
        string tokenName = oneToken.substr(0, delimiter);
        string regex = oneToken.substr(delimiter + 1);

        Token newToken(tokenName , regex);
        tokens.push_back(newToken);
    }

    return tokens;
}


// main
int main() 
{
    string input;
    getline(cin, input);

    // function i made to parse the tokens easier
    vector<Token> tokens = parseTokens(input);

    // parses input string (removes quotes)
    string inputString;
    getline(cin, inputString);
    if (!inputString.empty() && inputString.front() == '"' && inputString.back() == '"') {
        inputString = inputString.substr(1, inputString.size() - 2);
    }

    // constructs lexer
    Lexer lexer(tokens, inputString);

    Token token;
    while ((token = lexer.getToken()).type != "EOS")
        cout << token.type << ", \"" << token.value << "\"" << endl;

    return 0;
}




