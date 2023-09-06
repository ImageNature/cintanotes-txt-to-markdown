#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <string>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

struct ContentItem {
    string date;
    string title;
    string link_text;
    string url;
};

bool startsWith(const string &str, const string &prefix) {
    return str.compare(0, prefix.size(), prefix) == 0;
}

int main() {
    for (const auto &entry : fs::directory_iterator(".")) {
        string file_name = entry.path().string();
        if (endsWith(file_name, ".txt")) {
            ifstream input(file_name);
            string output_md = file_name.substr(0, file_name.find_last_of('.')) + ".md";
            ofstream output(output_md);

            vector<ContentItem> content_list;
            ContentItem temp_item;
            string line, link_text;

            regex date_regex("\\[\\d{4}/\\d{1,2}/\\d{1,2} \\d{1,2}:\\d{1,2}\\]");

            while (getline(input, line)) {
                if (regex_search(line, date_regex)) {
                    if (!temp_item.url.empty()) {
                        temp_item.link_text = link_text;
                        content_list.push_back(temp_item);
                        link_text.clear();
                    }
                    temp_item = ContentItem();
                    temp_item.date = line.substr(0, line.find(']'));
                    temp_item.title = line.substr(line.find(']') + 1);
                } else if (startsWith(line, "(https://")) {
                    temp_item.url = line.substr(1, line.size() - 2);
                } else {
                    link_text += line + " ";
                }
            }

            if (!temp_item.url.empty()) {
                temp_item.link_text = link_text;
                content_list.push_back(temp_item);
            }

            for (size_t i = 0; i < content_list.size(); ++i) {
                output << i + 1 << ". **" << content_list[i].title << "**\n";
                output << "   " << content_list[i].date << "\n";
                output << "   [" << content_list[i].link_text << "](" << content_list[i].url << ")\n";
            }

            input.close();
            output.close();
        }
    }
    return 0;
}
