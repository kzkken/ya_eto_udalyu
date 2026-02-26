#ifndef NGRAMMS_H
#define NGRAMMS_H

#include <bits/stdc++.h>
#include "dictionary.h"

std::pair<std::string, std::string> omonim_ngramms(std::string t, std::string prev2, std::string prev1, std::vector<Analysis> a, std::string filename1, std::string filename2);
std::string no_dict_ngramms(std::string prev2, std::string prev1, std::string filename);

#endif