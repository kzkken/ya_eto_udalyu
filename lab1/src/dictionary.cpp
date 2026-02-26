#include "dictionary.h"

using namespace std;

//обрезка пробелов и символов конца строки
void trim(string& s){
    size_t end = s.find_last_not_of(" \t\r\n");
    if (end != string::npos)
        s.erase(end + 1);
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start != string::npos)
        s.erase(0, start);
}

bool Dictionary::load(const string& filename){
    ifstream fin(filename);
    if (!fin.is_open()){
        cerr << "анлак, файл со словарем не открывается : " << filename << "\n";
        return false;
    }

    string line;
    while (getline(fin, line)){
        if (line.empty()) continue;

        string form, lemma, pos;
        stringstream ss(line);

        //берем значения из словаря
        if (!getline(ss, form, ',')) continue;
        if (!getline(ss, lemma, ',')) continue;
        if (!getline(ss, pos)) continue;

        trim(form);
        trim(lemma);
        trim(pos);

        if (form.empty() || lemma.empty() || pos.empty()){
            continue;
        }
        //проверяем что нет другой леммы с такой чатью речи (словарь шальной, на неомонимичное слово ставит несколько лемм)
        //омонимы одной части речи можно обработать по частоте встреч в тексте, но будем брать первую лемму в словаре по этой части речи, чтоб не разбирать не омонимичные слова
        auto it = data.find(form);
    
        if (it != data.end()){
            //ключ найден, проверяем наличие элемента с нужной чатью речи параметром
            auto& vec = it->second;
            
            //проверяем, есть ли элемент с такой частью речи
            auto found = std::find_if(vec.begin(), vec.end(),
                [&pos](const auto& item){  // захватываем pos
                    return item.pos == pos;  // сравниваем с pos
                });
            
            if (found == vec.end()){
                //данной части речи не было ранее в мапе с этим ключом
                data[form].emplace_back(lemma, pos);
            }
        } 
        else{ //если вооьще ключа в мапе нет
            data[form].emplace_back(lemma, pos);
        }
    }

    return true;
}

//берем леммы и части речи по слову в мапе
vector<Analysis> Dictionary::get(const string& word) const{
    auto it = data.find(word);
    if (it == data.end()){
        return{};
    }
    return it->second;
}