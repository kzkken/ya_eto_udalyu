#ifndef PARSER_H
#define PARSER_H

#include <bits/stdc++.h>

//узел дерева разбора
struct Node{
    std::string type; //"rule" или "terminal"
    std::string value; //имя правила или значение терминала
    std::vector<std::shared_ptr<Node>> children;
    
    Node(std::string t, std::string v);
    void addChild(std::shared_ptr<Node> child);
};

// Результат проверки
struct ParseResult{
    bool success; //успешно или нет
    std::shared_ptr<Node> tree; //дерево разбора при успехе
    std::string error; //сообщение об ошибке при беде
    int error_position; //где ошибка
    
    ParseResult() : success(false), error_position(-1) {}
};

class Parser{
private:
    std::map<std::string, std::vector<std::string>> grammar;
    std::vector<std::string> tokens;
    size_t pos;
    
    std::vector<std::string> splitVariant(const std::string& variant);
    bool isNonTerminal(const std::string& part);
    bool isTerminal(const std::string& part);
    bool isOptional(const std::string& part);
    std::string cleanPart(const std::string& part);
    
    std::shared_ptr<Node> tryParseVariant(const std::string& ruleName, const std::string& variant,  size_t startPos, bool& success,  std::string& errorMsg, int& errorPos);
    
public:
    Parser(const std::map<std::string, std::vector<std::string>>& g);
    std::shared_ptr<Node> parseRule(const std::string& ruleName, std::string& errorMsg, int& errorPos);
    ParseResult parse(const std::vector<std::string>& lemmas);
};

std::map<std::string, std::vector<std::string>> loadGrammar(const std::string& filename);

void printTree(std::shared_ptr<Node> node, int indent = 0);

#endif