#include "lemmatizer.h"

using namespace std;

//функция для выполнения команды в терминале
static string exec(const string& cmd){
    array<char, 4096> buffer;
    string result;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";
    while (fgets(buffer.data(), buffer.size(), pipe)){
        result += buffer.data();
    }
    pclose(pipe);
    return result;
}

//лемматизация
string lemmatizeWord(const string& word){
    //обработка чисел
    if (all_of(word.begin(), word.end(), ::isdigit)){
        return word;
    }

    string cmd = "echo \"" + word + "\" | mystem -l --format=text 2>/dev/null";
    string out = exec(cmd);
    
    string lemma = word;
    
    if (!out.empty()){
        //удаляем мусор
        out.erase(0, out.find_first_not_of(" \t\n\r"));
        out.erase(out.find_last_not_of(" \t\n\r") + 1);
        
        //удаляем фигурные скобки
        out.erase(0, out.find_first_not_of("{"));
        out.erase(out.find_last_not_of("}") + 1);
        
        //ищем лемму (до символа |)
        size_t pipe_pos = out.find('|');
        if (pipe_pos != string::npos){
            lemma = out.substr(0, pipe_pos);
            
            //удаляем вопросительные знаки
            lemma.erase(remove(lemma.begin(), lemma.end(), '?'), lemma.end());
        } 
        else if (!out.empty()){
            lemma = out;
            
            //удаляем вопросительные знаки
            lemma.erase(remove(lemma.begin(), lemma.end(), '?'), lemma.end());
        }
    }
    
    //если лемма пустая, возвращаем исходное слово
    if (lemma.empty()){
        lemma = word;
    }
    
    return lemma;
}