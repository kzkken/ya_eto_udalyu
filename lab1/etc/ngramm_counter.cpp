#include <bits/stdc++.h>

using namespace std;

struct Context{
    string prev;
    string current;
    string next;
};

//читаем предложения из файла
vector<vector<string>> readSentences(const string& filename){
    vector<vector<string>> sentences;
    ifstream fin(filename);
    string line;
    while (getline(fin, line)){
        if (line.empty()) continue;
        vector<string> sentence;
        istringstream iss(line);
        string pos;
        while (iss >> pos){
            sentence.push_back(pos);
        }
        sentences.push_back(sentence);
    }
    return sentences;
}

int main(){
    string infile = "pos_sentences.txt"; //файл с pos-последовательностями
    string outfile = "ngramms.csv"; //куда сохраняем статистику

    auto sentences = readSentences(infile);

    //структура для подсчета: map<prev2|prev1, map<current, count>>
    map<pair<string,string>, map<string,int>> windowCounts;

    for (auto& sent : sentences){
        for (int i = 1; i < sent.size() - 1; i++){
            string prev2;
            if(i == 1){
                prev2 = "<START>";
            }
            else{
                prev2 = sent[i - 2];
            }
            string prev1 = sent[i - 1];
            string curr = sent[i];

            windowCounts[{prev2,prev1}][curr]++;
        }
    }

    // сохраняем в CSV
    ofstream fout(outfile);
    fout << "prev2,prev1,current,count\n";
    for (auto& w : windowCounts){
        string prev2 = w.first.first;
        string prev1 = w.first.second;
        for (auto& c : w.second){
            fout << prev2 << "," << prev1 << "," << c.first << "," << c.second << "\n";
        }
    }

    cout << "Статистика pos с n-граммами сохранена в " << outfile << "\n";
    return 0;
}