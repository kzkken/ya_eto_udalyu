#include <bits/stdc++.h>

#include "tokenizer.h"
#include "dictionary.h"
#include "windows.h"
#include "ngramms.h"
#include "guess_lemma.h"

using namespace std;

//функция для нормализации POS (только строчные)
string normalize_pos(const string &pos){
    string res = pos;
    transform(res.begin(), res.end(), res.begin(), ::tolower);
    return res;
}

int main() {
    int k = 0;
    //загружаем словарь
    Dictionary dict;
    if (!dict.load("dict.csv")) {
        cerr << "Не удалось загрузить словарь\n";
        return 1;
    }

    ifstream fin("text_accuracy.txt"); // файл с текстом
    if (!fin.is_open()) {
        cerr << "Не удалось открыть файл\n";
        return 1;
    }

    long long correct = 0, total = 0;
    string textLine, posLine;

    while (getline(fin, textLine) && getline(fin, posLine)) { //читаем текст + POS
        auto tokens = tokenize(textLine);
        vector<string> pos_tokens;
        stringstream ss(posLine);
        k++;
        if(k%100==0){
            cout << k << '\n';
        }
        //cout<<correct <<" "<<total<<"\n";
        string posWord;
        //trim(posWord);
        while (ss >> posWord){
            pos_tokens.push_back(posWord);
            //cout <<posWord<<"\n";
        }

        if (tokens.size() != pos_tokens.size()) {
            //cerr << "Ошибка: количество слов и POS не совпадает!\n";
            continue;
        }

        vector<string> prev_pos(2, "<START>"); //предыдущие POS

        for (size_t i = 0; i < tokens.size(); i++) {
            total++;
            string file_pos = pos_tokens[i];

            auto &token = tokens[i];
            auto analyses = dict.get(token); //возвращает найденные в словаре леммы и части речи

            string pos; //определенная POS для текущего слова

            if (analyses.empty()) { //слово вне словаря
                string prev1 = prev_pos.size() >= 1 ? prev_pos[prev_pos.size() - 1] : "<START>";
                string prev2 = prev_pos.size() >= 2 ? prev_pos[prev_pos.size() - 2] : "<START>";
                string nextp = "<END>";

                if (i != tokens.size() - 1) {
                    auto nextv = dict.get(tokens[i + 1]);
                    if (nextv.size() == 1) nextp = nextv[0].pos;
                }

                if (i == tokens.size() - 1)
                    pos = no_dict_ngramms(prev2, prev1, "ngramms.csv");
                else
                    pos = no_dict_windows(prev1, nextp, "windows.csv");

                // можно лемматизировать
                if (pos == "NOUN" || pos == "ADJF" || pos == "ADJS" || pos == "VERB" || pos == "UNKN") {
                    auto lemm_pos = guess_lemma(token, pos);
                    token = lemm_pos.first;
                    pos = lemm_pos.second;
                }
            } 
            else if (analyses.size() == 1) { // слово встречается 1 раз
                pos = analyses[0].pos;
            } 
            else { //омонимы
                string prev1 = prev_pos.size() >= 1 ? prev_pos[prev_pos.size() - 1] : "<START>";
                string prev2 = prev_pos.size() >= 2 ? prev_pos[prev_pos.size() - 2] : "<START>";
                string nextp = "<END>";
                pair<string, string> ans;

                if (i != tokens.size() - 1) {
                    auto nextv = dict.get(tokens[i + 1]);
                    if (nextv.size() == 1) nextp = nextv[0].pos;
                }

                if (i == tokens.size() - 1)
                    ans = omonim_ngramms(token, prev2, prev1, analyses, "ngramms.csv", "dict.csv");
                else
                    ans = omonim_windows(token, prev1, nextp, analyses, "windows.csv", "dict.csv");

                token = ans.first;
                pos = ans.second;
            }

            //pos = normalize_pos(pos);
            prev_pos.push_back(pos);

            if (pos == file_pos) correct++;
            //cout <<pos<<"!"<<file_pos<<"\n";
        }
    }

    cout << "Точность: " << correct << "/" << total
         << " (" << fixed << setprecision(2)
         << (100.0 * correct / total) << "%)\n";

    return 0;
}