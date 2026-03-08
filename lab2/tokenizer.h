#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <bits/stdc++.h>

std::vector<std::string> tokenize(const std::string& sentence); // <-- const &

std::string normalize(const std::string& word);
bool isPunctuation(char c);

#endif