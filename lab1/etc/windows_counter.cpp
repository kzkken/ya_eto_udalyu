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
    string outfile = "windows.csv"; //куда сохраняем статистику

    auto sentences = readSentences(infile);

    //структура для подсчета: map<prev|next, map<current, count>>
    map<pair<string,string>, map<string,int>> windowCounts;

    for (auto& sent : sentences){
        for (int i = 1; i < sent.size() - 1; i++){
            string prev = sent[i-1];
            string next = sent[i+1];
            string curr = sent[i];

            windowCounts[{prev,next}][curr]++;
        }
    }

    //сохраняем в csv
    ofstream fout(outfile);
    fout << "prev,next,current,count\n";
    for (auto& w : windowCounts){
        string prev = w.first.first;
        string next = w.first.second;
        for (auto& c : w.second){
            fout << prev << "," << next << "," << c.first << "," << c.second << "\n";
        }
    }

    cout << "Статистика pos с окнами +-1 сохранена в " << outfile << "\n";
    return 0;
}