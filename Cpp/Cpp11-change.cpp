#include <dirent.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

using namespace std;

struct ContentItem {
  string date;
  string title;
  string link_text;
  string url;
};

bool startsWith(const string &str, const string &prefix) {
  return str.compare(0, prefix.size(), prefix) == 0;
}

bool endsWith(const string &str, const string &suffix) {
  return str.size() >= suffix.size() &&
         str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

string convertDateFormat(const string &date) {
  string newDate = date;
  replace(newDate.begin(), newDate.end(), '/', '-');
  newDate.erase(0, 1);  // Remove the opening bracket '['
  newDate.erase(newDate.find_last_not_of(' ') +
                1);  // Remove trailing whitespace
  return newDate;
}

int main() {
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(".")) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      string file_name = ent->d_name;
      if (endsWith(file_name, ".txt")) {
        ifstream input(file_name);
        string output_md =
            file_name.substr(0, file_name.find_last_of('.')) + ".md";
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
            temp_item.date = convertDateFormat(line.substr(0, line.find(']')));
            temp_item.title = line.substr(line.find(']') + 1).substr(1);
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
          output << "   [" << content_list[i].link_text << "]("
                 << content_list[i].url << ")\n";
        }

        input.close();
        output.close();
      }
    }
    closedir(dir);
  } else {
    cerr << "Could not open directory" << endl;
    return EXIT_FAILURE;
  }

  return 0;
}
