#include "windows.h"

using namespace std;


string no_dict_windows(string prev, string nextp, string filename){
	ifstream fin(filename);
    if (!fin.is_open()){
        cerr << "анлак, не открывается файл: " << filename << "\n";
        return "";
    }

    vector<vector<string>> data;
    string line;

    //читаем заголовок
    getline(fin, line);
    
    //читаем все данные
    while (getline(fin, line)){
        stringstream ss(line);
        string fprev, fnextp, fcurrent, fcount;
        
        if (!getline(ss, fprev, ',')) continue;
        if (!getline(ss, fnextp, ',')) continue;
        if (!getline(ss, fcurrent, ',')) continue;
        if (!getline(ss, fcount)) continue;
        
        //чистим значения от мусора
        trim(fprev); 
        trim(fnextp); 
        trim(fcurrent); 
        trim(fcount);
        
        data.push_back({fprev, fnextp, fcurrent, fcount});
    }
    fin.close();

	int max = -1; string pos = "";

    for(const auto& row : data){
        if((row[0] == prev || prev == "UNKN") && (row[1] == nextp || nextp == "UNKN")){
            int count = stoi(row[3]);
            if(count > max){
                max = count;
                pos = row[2];
            }
            break;
        }
    }

	if(max == -1){
		pos = "UNKN";
	}
	return pos;
}

pair<string, string> omonim_windows(string token, string prev, string nextp, vector<Analysis> a, string filename1, string filename2){
    //открываем файл
    ifstream fin1(filename1);
    if (!fin1.is_open()){
        cerr << "анлак, не открывается файл: " << filename1 << "\n";
        return {"",""};
    }

    //читаем строки из файла в память
    vector<vector<string>> data;
    string line;
    
    //пропускаем заголовок
    getline(fin1, line);
    
    //читаем все данные
    while (getline(fin1, line)){
        stringstream ss(line);
        string fprev, fnextp, fcurrent, fcount;
        
        if (!getline(ss, fprev, ',')) continue;
        if (!getline(ss, fnextp, ',')) continue;
        if (!getline(ss, fcurrent, ',')) continue;
        if (!getline(ss, fcount)) continue;
        
        //чистим значения от мусора
        trim(fprev); 
        trim(fnextp); 
        trim(fcurrent); 
        trim(fcount);
        
        data.push_back({fprev, fnextp, fcurrent, fcount});
    }
    fin1.close();
    
    //ищем по всем частям речи
    int max = -1; 
    string pos = "";
    
    for(int i = 0; i < a.size(); i++){
        string current = a[i].pos;
        
        //ищем в загруженных данных
        for(const auto& row : data){
            if((row[0] == prev || prev == "UNKN") && (row[1] == nextp || nextp == "UNKN") && row[2] == current){
                int count = stoi(row[3]);
                if(count > max){
                    max = count;
                    pos = current;
                }
                break;
            }
        }
    }
	if(max == -1){
		pos = a[0].pos;
	}
    //ищем лемму
	ifstream fin2(filename2);
	if (!fin2.is_open()){
        cerr << "анлак, не открывается файл: " << filename2 << "\n";
        return {"",""};
    }
    string lemma = "";

    //пропускаем заголовок
    getline(fin2, line); 

    while (getline(fin2, line)){
        stringstream ss(line);
        string form, flemma, fpos;
        //проходимся по файлу
        if (!getline(ss, form, ',')) continue;
        if (!getline(ss, flemma, ',')) continue;
        if (!getline(ss, fpos)) continue;

        trim(form); trim(flemma); trim(fpos);

        if (form == token && fpos == pos){
            lemma = flemma;
            break;
        }
    }
    fin2.close();
    return {lemma, pos};
}