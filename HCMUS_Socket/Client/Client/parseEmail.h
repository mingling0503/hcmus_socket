#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <sstream>
#include <vector>

using namespace std;

string sanitizeRequest(const string request);

std::vector <std::string> parseEmail(const std::string& filePath);

vector<string> split(const string& str, char delimiter);
