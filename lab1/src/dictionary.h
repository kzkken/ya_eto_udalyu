#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <bits/stdc++.h>

struct Analysis {
    std::string lemma;
    std::string pos;

    Analysis() : lemma("UNKN"), pos("UNKN") {}
    Analysis(const std::string& l, const std::string& p): lemma(l), pos(p) {}
};

class Dictionary {
public:
    bool load(const std::string& filename);
    std::vector<Analysis> get(const std::string& word) const;

private:
    std::unordered_map<std::string, std::vector<Analysis>> data;
};

void trim(std::string& s);

#endif