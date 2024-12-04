#include "parseEmail.h"

string sanitizeRequest(const string request) {
    string sanitized;
    for (char c : request) {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            sanitized += c;
        }
    }
    return sanitized;
}

vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    size_t start = 0, end;
    while ((end = str.find(delimiter, start)) != string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(str.substr(start));
    return tokens;
}

vector<string> parseEmail(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Could not open file: " << filePath << endl;
        return {};
    }

    string line;
    string senderEmail;
    string contentPlainText;
    string boundary;
    string partContent;

    // Regular expressions for parsing email components
    regex senderRegex(R"(From:.*<([\w\.-]+@[\w\.-]+)>)");
    regex boundaryRegex(R"(boundary=\"?(.+?)\"?)");
    smatch match;

    // First pass: Extract sender email and boundary
    while (getline(file, line)) {
        if (regex_search(line, match, senderRegex)) {
            senderEmail = match[1];
        }
        if (regex_search(line, match, boundaryRegex)) {
            boundary = "--" + match[1].str();
        }
    }

    // Reset file stream
    file.clear();
    file.seekg(0, ios::beg);

    bool isPlainText = false;

    // Extract plain text content
    while (getline(file, line)) {
        if (!boundary.empty() && line.find(boundary) != string::npos) {
            if (isPlainText) {
                contentPlainText = partContent;
                break;
            }
            partContent.clear();
            isPlainText = false;
        }

        if (line.find("Content-Type: text/plain") != string::npos) {
            isPlainText = true;
            file.ignore();
         
        }
        else if (isPlainText) {
            partContent += line + "\n"; // Use newline to preserve readability
        }
    }

    file.close();

    // Split plain text content into IP and request, assuming a specific format
    vector<string> info = split(contentPlainText, '\n');
    if (info.size() < 2) {
        cerr << "Email content is incomplete or improperly formatted.\n";
        return {};
    }

    string ipAddr = info[0];
    string request = sanitizeRequest(info[1]);

    cout << "Sender: " << senderEmail << endl;
    cout << "IP Address: " << ipAddr << endl;
    cout << "Request: " << request << endl;

    return { senderEmail, ipAddr, request };
}
