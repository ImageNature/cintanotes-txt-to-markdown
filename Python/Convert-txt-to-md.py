import os
import re

import chardet


def get_encoding(file):
  with open(file, 'rb') as f:
    return chardet.detect(f.read())['encoding']


def txt_to_markdown(input_txt, output_md):
  encoding = get_encoding(input_txt)
  with open(input_txt, 'r', encoding=encoding) as f:
    lines = f.readlines()

  content_list = []
  temp_dict = {}
  link_text = []
  for line in lines:
    if re.match(r'\[\d{4}/\d{1,2}/\d{1,2} \d{1,2}:\d{1,2}\]', line):
      if 'url' in temp_dict:
        temp_dict['link_text'] = ' '.join(link_text)
        content_list.append(temp_dict.copy())
        link_text.clear()
      temp_dict.clear()
      temp_dict['date'] = re.findall(
          r'\[\d{4}/\d{1,2}/\d{1,2} \d{1,2}:\d{1,2}\]', line)[0]
      temp_dict['title'] = line.split(']')[1].strip()
    elif line.startswith('(https://'):
      temp_dict['url'] = line.strip()[1:-1]
    else:
      link_text.append(line.strip())

  if 'url' in temp_dict:
    temp_dict['link_text'] = ' '.join(link_text)
    content_list.append(temp_dict)

  with open(output_md, 'w', encoding='utf-8') as f:
    for i, item in enumerate(content_list):
      date_modified = item['date'][1:-1].replace('/', '-').replace(' ',
                                                                   '-').replace(
                                                                       ':', '-')
      markdown_item = f"{i + 1}. **{item['title']}**\n   {date_modified}\n   [{item['link_text']}]({item['url']})\n"
      f.write(markdown_item)


if __name__ == "__main__":
  path = "."
  for file_name in os.listdir(path):
    if file_name.endswith('.txt'):
      input_txt = file_name
      output_md = file_name.replace('.txt', '.md')
      txt_to_markdown(input_txt, output_md)
