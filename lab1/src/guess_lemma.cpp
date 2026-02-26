#include "guess_lemma.h"

using namespace std;

// Структура для хранения правил окончаний
struct EndingRule{
    string ending;           
    string pos;              
    string baseEnding;       
};

vector<EndingRule> rules;

void initRules(){
    //сущ
    rules.push_back({"а", "NOUN", ""});
    rules.push_back({"ам", "NOUN", ""});
    rules.push_back({"ами", "NOUN", ""});
    rules.push_back({"ах", "NOUN", ""});
    rules.push_back({"е", "NOUN", ""});
    rules.push_back({"ев", "NOUN", ""});
    rules.push_back({"ей", "NOUN", ""});
    rules.push_back({"ем", "NOUN", ""}); 
    rules.push_back({"и", "NOUN", ""}); 
    rules.push_back({"о", "NOUN", "о"});
    rules.push_back({"оев", "NOUN", "ой"});
    rules.push_back({"ой", "NOUN", ""});
    rules.push_back({"ом", "NOUN", ""});
    rules.push_back({"ою", "NOUN", "ой"}); 
    rules.push_back({"оя", "NOUN", "ой"});
    rules.push_back({"у", "NOUN", ""});
    rules.push_back({"ы", "NOUN", ""});
    rules.push_back({"ь", "NOUN", ""});
    rules.push_back({"ьев", "NOUN", ""});
    rules.push_back({"ью", "NOUN", ""});    
    rules.push_back({"ю", "NOUN", ""});  
    rules.push_back({"я", "NOUN", ""}); 
    rules.push_back({"ям", "NOUN", ""});     
    
    //прил
    rules.push_back({"ый", "ADJF", "ый"});
    rules.push_back({"ий", "ADJF", "ий"});
    rules.push_back({"ой", "ADJF", "ой"});
    rules.push_back({"ая", "ADJF", "ый"});
    rules.push_back({"яя", "ADJF", "ий"});
    rules.push_back({"ое", "ADJF", "ый"});
    rules.push_back({"ее", "ADJF", "ий"});
    rules.push_back({"ые", "ADJF", "ый"});
    rules.push_back({"ие", "ADJF", "ий"});
    rules.push_back({"ого", "ADJF", "ый"});
    rules.push_back({"его", "ADJF", "ий"});
    rules.push_back({"ому", "ADJF", "ый"});
    rules.push_back({"ему", "ADJF", "ий"});
    rules.push_back({"ым", "ADJF", "ый"});
    rules.push_back({"им", "ADJF", "ий"});
    rules.push_back({"ом", "ADJF", "ый"});
    rules.push_back({"ем", "ADJF", "ий"});
    
    //кр прил
    rules.push_back({"а", "ADJS", ""});
    rules.push_back({"о", "ADJS", ""});
    rules.push_back({"ы", "ADJS", ""});
    
    //гл
    rules.push_back({"ать", "VERB", "ать"}); 
    rules.push_back({"ять", "VERB", "ять"}); 
    rules.push_back({"ить", "VERB", "ить"});
    rules.push_back({"еть", "VERB", "еть"});
    rules.push_back({"оть", "VERB", "оть"}); 
    rules.push_back({"уть", "VERB", "уть"}); 
    rules.push_back({"чь", "VERB", "чь"}); 
    rules.push_back({"ю", "VERB", "ть"}); 
    rules.push_back({"ешь", "VERB", "еть"});
    rules.push_back({"ет", "VERB", "еть"}); 
    rules.push_back({"ем", "VERB", "еть"});
    rules.push_back({"ете", "VERB", "еть"});
    rules.push_back({"ют", "VERB", "еть"}); 
    rules.push_back({"у", "VERB", "ать"});
    rules.push_back({"ишь", "VERB", "ить"});
    rules.push_back({"ит", "VERB", "ить"});
    rules.push_back({"им", "VERB", "ить"});
    rules.push_back({"ите", "VERB", "ить"});
    rules.push_back({"ят", "VERB", "ить"}); 
    rules.push_back({"ат", "VERB", "ать"}); 
    rules.push_back({"л", "VERB", "ть"});
    rules.push_back({"ла", "VERB", "ть"});
    rules.push_back({"ло", "VERB", "ть"});
    rules.push_back({"ли", "VERB", "ть"});
    rules.push_back({"ся", "VERB", ""});
}
    
//поиск самого длинного подходящего окончания
pair<string, string> guess_lemma(string token, string pos){
    EndingRule bestRule = {"", pos, ""};
    int maxsize = 0;
    
    for (const auto& rule : rules){
        // Если указана предполагаемая часть речи, учитываем только её правила
        if (rule.pos != pos && rule.pos != "UNKN"){
            continue;
        }
        
        if (token.size() >= rule.ending.size()){
            string tokenEnding = token.substr(token.size() - rule.ending.size());
            
            if (tokenEnding == rule.ending){
                //выбираем по длине окончания
                if (rule.ending.size() >= maxsize){
                    maxsize = rule.ending.size();
                    bestRule = rule;
                }
            }
        }
    }
    
    token.replace(token.size() - bestRule.ending.size(), bestRule.ending.size(), bestRule.baseEnding);
    
    return {token, bestRule.pos};
}