#include "tokenizer.h"

using namespace std;

//проверяем на знаки препинания
bool isPunctuation(char c){
    if(c == ',' || c == '.' || c == '!' || c == '?'){
        return true;
    }
    else{
        return false;
    }
}

vector<string> tokenize(const string& sentence){
    //мапа заглавные в строчные и Ёё в е
    static map<string, string> upper_map ={
       {"А","а"},{"Б","б"},{"В","в"},{"Г","г"},{"Д","д"},{"Е","е"},
       {"Ж","ж"},{"З","з"},{"И","и"},{"Й","й"},{"К","к"},{"Л","л"},
       {"М","м"},{"Н","н"},{"О","о"},{"П","п"},{"Р","р"},{"С","с"},
       {"Т","т"},{"У","у"},{"Ф","ф"},{"Х","х"},{"Ц","ц"},{"Ч","ч"},
       {"Ш","ш"},{"Щ","щ"},{"Ъ","ъ"},{"Ы","ы"},{"Ь","ь"},{"Э","э"},
       {"Ю","ю"},{"Я","я"},{"Ё","е"},{"ё","е"}
    };

    vector<string> tokens;
    string current;

    //идем по предложению
    for (int i = 0; i < sentence.size();){
        unsigned char c = sentence[i];

        // Разделители
        if (isspace(c)/* || isPunctuation(c)*/){
            if (!current.empty()){
                tokens.push_back(current);
                current.clear();
            }
            i++;
            continue;
        }

        //символ в ASCII
        if (c < 128){
            if (isalpha(c)) current += tolower(c);
            else current += c;
            i++;
            continue;
        }

        //двухбайтовые кириллические символы
        if (i + 1 < sentence.size()){
            string chr = sentence.substr(i,2);

            //проверяем мапу
            auto it = upper_map.find(chr);
            if (it != upper_map.end()){
                current += it->second;
            } 
            else{
                current += chr; // остальная кириллица
            }
            i += 2;
            continue;
        }

        //одиночный байт
        current += sentence[i];
        i++;
    }

    if (!current.empty()){
        tokens.push_back(current);
    }
    return tokens;
}