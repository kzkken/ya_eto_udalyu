#ifndef WINDOWS_H
#define WINDOWS_H

#include <bits/stdc++.h>
#include "dictionary.h"

std::pair<std::string, std::string> omonim_windows(std::string t, std::string prev, std::string nextp, std::vector<Analysis> a, std::string filename1, std::string filename2);
std::string no_dict_windows(std::string prev, std::string nextp, std::string filename);

#endif