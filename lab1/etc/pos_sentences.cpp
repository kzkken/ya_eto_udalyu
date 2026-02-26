#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "tinyxml2.h"

using namespace tinyxml2;
using namespace std;

int main(){
    XMLDocument doc;
    if (doc.LoadFile("annot.opcorpora.no_ambig.xml") != XML_SUCCESS){
        cerr << "Ошибка загрузки XML!" << "\n";
        return 1;
    }

    ofstream outfile("pos_sentences.txt");
    if (!outfile.is_open()){
        cerr << "Не открыть файл для записи!" << "\n";
        return 1;
    }

    XMLElement* root = doc.RootElement(); // <annotation>
    for (XMLElement* text = root->FirstChildElement("text"); text; text = text->NextSiblingElement("text")){
        for (XMLElement* paragraph = text->FirstChildElement("paragraphs")->FirstChildElement("paragraph"); 
             paragraph; 
             paragraph = paragraph->NextSiblingElement("paragraph")){

            for (XMLElement* sentence = paragraph->FirstChildElement("sentence"); 
                 sentence; 
                 sentence = sentence->NextSiblingElement("sentence")){

                vector<string> pos_list;
                pos_list.push_back("<START>");

                XMLElement* tokens = sentence->FirstChildElement("tokens");
                for (XMLElement* token = tokens->FirstChildElement("token"); token; token = token->NextSiblingElement("token")){
                    XMLElement* tfr = token->FirstChildElement("tfr");
                    if (!tfr) continue;
                    XMLElement* v = tfr->FirstChildElement("v");
                    if (!v) continue;
                    XMLElement* l = v->FirstChildElement("l");
                    if (!l) continue;
                    XMLElement* g = l->FirstChildElement("g");
                    if (!g) continue;

                    const char* pos = g->Attribute("v");
                    if (!pos) pos = "NI"; // неизвестное
                    pos_list.push_back(pos);
                }

                pos_list.push_back("<END>");

                //записываем предложение в файл
                for (size_t i = 0; i < pos_list.size(); ++i){
                    outfile << pos_list[i];
                    if (i != pos_list.size() - 1) outfile << " ";
                }
                outfile << "\n";
            }
        }
    }

    outfile.close();
    return 0;
}