#include "parser.h"

using namespace std;

//реализация узла дерева разбора
Node::Node(string t, string v) : type(t), value(v){}

//добавление дочернего узла
void Node::addChild(shared_ptr<Node> child){
    children.push_back(child);
}

//конструктор парсера
Parser::Parser(const map<string, vector<string>>& g) : grammar(g), pos(0){}

//проверка, является ли часть варианта нетерминалом (в < >)
bool Parser::isNonTerminal(const string& part){
    return !part.empty() && part[0] == '<' && part.back() == '>';
}

//проверка, является ли часть варианта терминалом (в " ")
bool Parser::isTerminal(const string& part){
    return !part.empty() && part[0] == '"' && part.back() == '"';
}

//проверка, является ли часть варианта опциональной (в [ ])
bool Parser::isOptional(const string& part){
    return !part.empty() && part[0] == '[' && part.back() == ']';
}

//чистка от скобок и кавычек
string Parser::cleanPart(const string& part){
    if (part.empty()) return part;
    
    if (part[0] == '<' && part.back() == '>'){
        return part.substr(1, part.length() - 2);
    }
    else if (part[0] == '"' && part.back() == '"'){
        return part.substr(1, part.length() - 2);
    }
    else if (part[0] == '[' && part.back() == ']'){
        return part.substr(1, part.length() - 2);
    }
    
    return part;
}

//разбиение правила на части
vector<string> Parser::splitVariant(const string& variant){
    vector<string> parts;
    string current;
    bool inQuotes = false;
    int bracketLevel = 0;
    
    for (size_t i = 0; i < variant.length(); i++){
        char c = variant[i];
        
        //обработка кавычек
        if (c == '"'){
            inQuotes = !inQuotes;
            current += c;
        }
        //открытие угловой скобки 
        else if (c == '<' && !inQuotes){
            if (!current.empty() && bracketLevel == 0){
                parts.push_back(current);
                current.clear();
            }
            bracketLevel++;
            current += c;
        }
        //закрытие угловой скобки
        else if (c == '>' && !inQuotes){
            current += c;
            bracketLevel--;
            if (bracketLevel == 0 && !current.empty()){
                parts.push_back(current);
                current.clear();
            }
        }
        //открытие квадратной скобки
        else if (c == '[' && !inQuotes){
            if (!current.empty() && bracketLevel == 0){
                parts.push_back(current);
                current.clear();
            }
            //находим закрывающую скобку и берем всю опциональную часть целиком
            size_t end = variant.find(']', i);
            if (end != string::npos){
                string optional = variant.substr(i, end - i + 1);
                parts.push_back(optional);
                i = end;  //пропускаем всю опциональную часть
                current.clear();
                continue;
            }
        }
        //пробел - разделитель между частями
        else if (isspace(c) && !inQuotes && bracketLevel == 0){
            if (!current.empty()){
                parts.push_back(current);
                current.clear();
            }
        }
        //обычный символ
        else{
            current += c;
        }
    }
    
    if (!current.empty()){
        parts.push_back(current);
    }
    
    return parts;
}

//функция для удаления кавычек из строки
string removeQuotes(const string& s) {
    if (s.empty()) return s;
    if (s.front() == '"' && s.back() == '"') {
        return s.substr(1, s.length() - 2);
    }
    return s;
}

//рекурсивный разбор грамматики
shared_ptr<Node> Parser::parseRule(const string& ruleName, string& errorMsg, int& errorPos){
    
    //проверка на терминал
    if (grammar.find(ruleName) == grammar.end()){
        if (pos < tokens.size() && tokens[pos] == ruleName){
            return make_shared<Node>("terminal", tokens[pos++]);
        }
        errorMsg = "Ожидалось: '" + ruleName + "'";
        errorPos = pos;
        return nullptr;
    }
    
    size_t startPos = pos;
    
    //пробуем все варианты
    for (const auto& variant : grammar.at(ruleName)){
        pos = startPos;
        
        auto node = make_shared<Node>("rule", ruleName);
        bool variantOk = true;
        string variantError;
        int variantErrorPos = -1;
        
        auto parts = splitVariant(variant);
        
        for (const auto& part : parts){
            bool optional = isOptional(part);
            string clean = cleanPart(part);
            
            //проверяем, является ли часть нетерминалом
            bool isStillNonTerminal = isNonTerminal(clean);
            
            if (isNonTerminal(part) || isStillNonTerminal){
                //нетерминал
                size_t beforePos = pos;
                string ruleToParse = clean;
                //чистим <>
                if (isStillNonTerminal) {
                    ruleToParse = cleanPart(clean);
                }
                
                auto child = parseRule(ruleToParse, variantError, variantErrorPos);
                
                if (child){
                    node->addChild(child);
                } 
                else{
                    if (optional){
                        pos = beforePos;
                    } 
                    else{
                        variantOk = false;
                        break;
                    }
                }
            } 
            else{
                //терминал
                string tokenToFind = clean;
                //убираем кавычки
                if (tokenToFind.front() == '"' && tokenToFind.back() == '"') {
                    tokenToFind = tokenToFind.substr(1, tokenToFind.length() - 2);
                }
                
                if (pos < tokens.size() && tokens[pos] == tokenToFind){
                    node->addChild(make_shared<Node>("terminal", tokens[pos++]));
                } 
                else{
                    if (optional){
                        //ничего не делаем
                    } 
                    else{
                        variantOk = false;
                        break;
                    }
                }
            }
        }
        
        if (variantOk){
            return node;
        }
    }
    
    pos = startPos;
    errorMsg = "Не удалось разобрать <" + ruleName + ">";
    errorPos = pos;
    return nullptr;
}

//парсинг
ParseResult Parser::parse(const vector<string>& lemmas){
    ParseResult result;
    
    tokens = lemmas;
    pos = 0;
    
    if (tokens.empty()){
        result.error = "Пустой запрос";
        return result;
    }
    
    string errorMsg;
    int errorPos;
    auto tree = parseRule("Запрос", errorMsg, errorPos);
    
    //проверяем, все ли токены разобраны
    if (tree && pos == tokens.size()){
        result.success = true;
        result.tree = tree;
    } 
    else{
        result.success = false;
        result.error = errorMsg;
        result.error_position = errorPos;
        
        if (pos < tokens.size()){
            result.error = "Слово вне грамматики или не на своем месте: '" + tokens[pos] + "'";
            result.error_position = pos;
        } 
    }
    
    return result;
}

//загрузка грамматики
map<string, vector<string>> loadGrammar(const string& filename){

    map<string, vector<string>> grammar;
    ifstream file(filename);
    
    if (!file.is_open()){
        throw runtime_error("Не удалось открыть файл грамматики: " + filename);
    }
    
    string line;
    int lineNum = 0;
    
    //читаем файл построчно
    while (getline(file, line)){
        lineNum++;
        
        //ищем знак равенства
        size_t eq = line.find('=');
        if (eq == string::npos) continue;
        
        //извлекаем имя правила
        string name = line.substr(0, eq);
        
        //убираем пробелы вокруг имени
        name.erase(0, name.find_first_not_of(" \t"));
        name.erase(name.find_last_not_of(" \t") + 1);
        
        //убираем угловые скобки < > если они есть
        if (name.front() == '<' && name.back() == '>'){
            name = name.substr(1, name.length() - 2);
        }
        
        //часть после =
        string rules = line.substr(eq + 1);
        vector<string> variants;
        stringstream ss(rules);
        string var;
        
        //разделяем варианты по символу |
        while (getline(ss, var, '|')){
            //убираем пробелы в начале и конце варианта
            var.erase(0, var.find_first_not_of(" \t"));
            var.erase(var.find_last_not_of(" \t") + 1);
            
            if (!var.empty()){
                variants.push_back(var);
            }
        }
        
        //сохраняем правило в грамматику
        grammar[name] = variants;
    }
    return grammar;
}

//вывод дерева разбора
void printTree(shared_ptr<Node> node, int indent){
    if (!node) return;
    
    string indentStr(indent * 2, ' ');
    
    if (node->type == "rule"){
        cout << indentStr << "<" << node->value << ">" << "\n";
        for (auto child : node->children){
            printTree(child, indent + 1);
        }
    } 
    else{
        cout << indentStr << "'" << node->value << "'" << "\n";
    }
}