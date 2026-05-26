#include <fstream>
#include <iostream>


const int ALPHABET_SIZE = 256;


int calculate_length(const char* string) {

  int length = 0;

  while (string[length] != '\0') {
    ++length;
  }

  return length;
}


void build_shifts_table(const char* pattern, int pattern_length, int* shifts) {

  for (int i = 0; i < ALPHABET_SIZE; ++i) {
    shifts[i] = pattern_length;
  }

  for (int i = 0; i < pattern_length - 1; ++i) {
    shifts[static_cast<unsigned char>(pattern[i])] = pattern_length - 1 - i;
  }
}


unsigned long long count_boyer_moore_horspool(
  const char* text,
  std::streamsize text_length,
  const char* pattern,
  int pattern_length,
  const int* shift
) {

  if (text_length < pattern_length) {
    return 0;
  }

  unsigned long long counter = 0;

  const int last_pattern_index = pattern_length - 1;
  const char last_pattern_character = pattern[last_pattern_index];

  std::streamsize current_position = 0;
  const std::streamsize last_possible_position = text_length - pattern_length;

  while (current_position <= last_possible_position) {

    const char* current_text = text + current_position;
    const char current_last_character = current_text[last_pattern_index];

    if (current_last_character == last_pattern_character) {

      int j = last_pattern_index - 1;

      while (j >= 0 && current_text[j] == pattern[j]) {
        --j;
      }

      if (j < 0) {
        ++counter;
      }
    }

    current_position += shift[static_cast<unsigned char>(current_last_character)];
  }

  return counter;
}


int main(int argc, char* argv[]) {

  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <pattern> <filename>\n";
    return 1;
  }

  const char* pattern = argv[1];
  const char* input_filename = argv[2];

  const int pattern_length = calculate_length(pattern);

  if (pattern_length == 0) {
    std::cout << 0 << '\n';
    return 0;
  }

  std::ifstream input_file(input_filename, std::ios::binary | std::ios::ate);

  if (!input_file.is_open()) {
    std::cerr << "Cannot open file\n";
    return 1;
  }

  const std::streamsize input_file_size = input_file.tellg();
  input_file.seekg(0, std::ios::beg);

  char* input_file_data = new char[input_file_size];

  if (!input_file.read(input_file_data, input_file_size)) {

    delete[] input_file_data;
    input_file.close();

    std::cerr << "Cannot read file\n";

    return 2;
  }

  input_file.close();

  unsigned long long total_count = 0;

  int shifts[ALPHABET_SIZE];

  build_shifts_table(pattern, pattern_length, shifts);

  total_count = count_boyer_moore_horspool(
    input_file_data, input_file_size, pattern, pattern_length, shifts
  );

  delete[] input_file_data;

  std::cout << total_count << '\n';
}
