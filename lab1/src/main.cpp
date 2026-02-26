#include <bits/stdc++.h>

#include "tokenizer.h"
#include "dictionary.h"
#include "windows.h"
#include "ngramms.h"
#include "guess_lemma.h"

using namespace std;

int main(){
    //загружаем словарь
    Dictionary dict;
    if (!dict.load("dict.csv")){
        cerr << "не удалось загрузить словарь\n";
        return 1;
    }

    string line;
    while (getline(cin, line)){ //считываем введенную строку
        auto tokens = tokenize(line); //токенизируем
        vector<string> prev_pos(2,"<START>"); //часть речи предыдущего слова

        for(int i = 0; i < tokens.size(); i++){ //проходимся по токенам
            auto& token = tokens[i];
            auto analyses = dict.get(token); // возвращает найденные в словаре леммы и части речи

            if(analyses.size() == 0){
                // слово не в словаре, используем все POS как кандидатов
                string prev2, prev1, nextp; string pos; //предыдущая и следующая части речи
                prev1 = prev_pos[prev_pos.size() - 1];
                prev2 = prev_pos[prev_pos.size() - 2];
                if(i == tokens.size() - 1){ //последнее слово в предложении
                    pos = no_dict_ngramms(prev2, prev1, "ngramms.csv"); //2грамма
                }
                else{ //иначе берем часть речи следующего слова
                    auto nextp_v = dict.get(tokens[i+1]);
                    if(nextp_v.size() == 1){ //если следующее слово однозначно определено
                        nextp = nextp_v[0].pos;
                        pos = no_dict_windows(prev1, nextp, "windows.csv"); //окно+-1
                    }
                    else{
                        if(i >= 1){
                            prev2 = prev_pos[prev_pos.size() - 2];
                        }
                        else{
                            prev2 = "<START>";
                        }
                        pos = no_dict_ngramms(prev2, prev1, "ngramms.csv"); //2грамма
                    }
                }

                if (pos == "NOUN" || pos == "ADJF" || pos == "ADJS" || pos == "VERB" || pos == "UNKN"){
                    pair<string, string> lemm_pos = guess_lemma(token, pos); //битва экстрасенсов по лемме
                    cout << token << "{" << lemm_pos.first << "=" << lemm_pos.second << "} "; //вывод
                }
                else{
                    cout << token << "{" << token << "=" << pos << "} "; //вывод
                }
                prev_pos.push_back(pos);
            } 
            else if(analyses.size() == 1){ //слово встречется в словаре 1 раз
                prev_pos.push_back(analyses[0].pos); // добавляем в массив часть речи
                cout << token << "{" << analyses[0].lemma << "=" << analyses[0].pos << "} "; //вывод
            } 
            else{ //слово встречается в словаре несколько раз (омонимы)
                string prev2, prev1, nextp; pair<string, string> ans; //предыдущая и следующая части речи
                prev1 = prev_pos[prev_pos.size() - 1]; //пререм часть речи предыдущего слова
                if(i == tokens.size() - 1){ //последнее слово в предложении
                    if(i >= 1){
                        prev2 = prev_pos[prev_pos.size() - 2];
                    }
                    else{
                        prev2 = "<START>";
                    }
                    ans = omonim_ngramms(token, prev2, prev1, analyses, "ngramms.csv", "dict.csv"); //2грамма
                }
                if(i != tokens.size() - 1){ //иначе берем часть речи следующего слова
                    auto nextp_v = dict.get(tokens[i+1]);
                    if(nextp_v.size() == 1){ //если следующее слово однозначно определено
                        nextp = nextp_v[0].pos;
                        ans = omonim_windows(token, prev1, nextp, analyses, "windows.csv", "dict.csv"); //окно+-1
                    }
                    else{
                        if(i >= 1){
                            prev2 = prev_pos[prev_pos.size() - 2];
                        }
                        else{
                            prev2 = "<START>";
                        }
                        ans = omonim_ngramms(token, prev2, prev1, analyses, "ngramms.csv", "dict.csv"); //2грамма
                    }
                }
                cout << token << "{" << ans.first << "=" << ans.second << "} ";//вывод
                prev_pos.push_back(ans.second);
            }
        }
        cout << "\n";
    }

    return 0;
}