#include <bits/stdc++.h>

#include "tokenizer.h"
#include "parser.h"
#include "lemmatizer.h"

using namespace std;

int main(){
    string grammarFile = "grammar.txt";
    string commandsFile = "test_commands.txt";
    ifstream fin("test_commands.txt");
    string testLine;
    auto grammar = loadGrammar("grammar.txt");
    Parser parser(grammar);

    while (getline(fin, testLine)){ 
        cout << "Запрос: " << testLine << "\n";
        auto tokens = tokenize(testLine);

        cout << "Лемматизированный запрос: ";
        for(int i = 0; i < tokens.size(); i++){
            tokens[i] = lemmatizeWord(tokens[i]);
            cout << tokens[i] << " ";
        }
        cout << "\n";

        ParseResult result; int flag = 1;
        while (tokens.size() > 0 && flag) {
            result = parser.parse(tokens);
            if (result.success){
                cout << "Дерево:\n";
                printTree(result.tree);
            } 
            else{
                cout << "Ошибка: " << result.error << "\n";
                flag = 0;
            }
            cout << "\n";
        }
    }
    
    return 0;
}