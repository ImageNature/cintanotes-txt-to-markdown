#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 500

typedef struct {
  char date[MAX_LINE_LENGTH];
  char title[MAX_LINE_LENGTH];
  char link_text[MAX_LINE_LENGTH *
                 5];  // Estimation to contain the combined lines
  char url[MAX_LINE_LENGTH];
} ContentItem;

ContentItem content_list[MAX_LINES];
int content_count = 0;

int starts_with(const char *str, const char *prefix) {
  return strncmp(str, prefix, strlen(prefix)) == 0;
}

void write_to_markdown(const char *output_md) {
  FILE *f = fopen(output_md, "w");
  if (f == NULL) {
    printf("Error opening file %s\n", output_md);
    return;
  }
  for (int i = 0; i < content_count; i++) {
    fprintf(f, "%d. **%s**\n", i + 1, content_list[i].title);
    fprintf(f, "   %s\n", content_list[i].date);
    fprintf(f, "   [%s](%s)\n", content_list[i].link_text, content_list[i].url);
  }
  fclose(f);
}

int main() {
  char input_txt[] = "input.txt";
  char output_md[] = "output.md";

  FILE *f = fopen(input_txt, "r");
  if (f == NULL) {
    printf("Error opening file %s\n", input_txt);
    return 1;
  }

  char line[MAX_LINE_LENGTH];
  char link_text_accum[MAX_LINE_LENGTH * 5] = "";
  while (fgets(line, sizeof(line), f)) {
    line[strcspn(line, "\n")] = 0;  // Strip newline
    if (strstr(line, "[") && strstr(line, "]") && strchr(line, '/')) {
      if (strlen(content_list[content_count].url) > 0) {
        strcpy(content_list[content_count].link_text, link_text_accum);
        content_count++;
      }
      strcpy(content_list[content_count].date, strtok(line, "]"));
      strcpy(content_list[content_count].title, strtok(NULL, "]"));
      memset(link_text_accum, 0, sizeof(link_text_accum));
    } else if (starts_with(line, "(https://")) {
      strcpy(content_list[content_count].url, line + 1);
      content_list[content_count]
          .url[strlen(content_list[content_count].url) - 1] =
          '\0';  // Remove trailing ')'
    } else {
      strcat(link_text_accum, line);
      strcat(link_text_accum, " ");
    }
  }
  if (strlen(content_list[content_count].url) > 0) {
    strcpy(content_list[content_count].link_text, link_text_accum);
    content_count++;
  }
  fclose(f);
  write_to_markdown(output_md);

  return 0;
}
