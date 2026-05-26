#include <fstream>
#include <ios>
#include <iostream>


const int FILE_BUFFER_SIZE = 4 * 1024 * 1024;  // 4 Mb


int calculate_length(const char* string) {

  int len = 0;

  while (string[len] != '\0') {
    ++len;
  }

  return len;
}

void build_prefix_function(const char* pattern, int pattern_length, int* prefix) {

  prefix[0] = 0;
  int matched_counter = 0;

  for (int i = 1; i < pattern_length; ++i) {

    while (matched_counter > 0 && pattern[i] != pattern[matched_counter]) {
      matched_counter = prefix[matched_counter - 1];
    }

    if (pattern[i] == pattern[matched_counter]) {
      ++matched_counter;
    }

    prefix[i] = matched_counter;
  }
}

int main(int argc, char* argv[]) {

  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <target_string> <input_file>\n";
    return 1;
  }

  std::ifstream input_file(argv[2], std::ios::in | std::ios::binary);

  if (!input_file.is_open()) {
    std::cerr << "Cannot open input file\n";
    return 2;
  }

  const char* string_target = argv[1];
  const int string_target_length = calculate_length(string_target);

  if (string_target_length == 0) {
    std::cout << 0 << '\n';
    return 0;
  }

  int* prefix = new int[string_target_length];
  build_prefix_function(string_target, string_target_length, prefix);

  char* read_buffer = new char[FILE_BUFFER_SIZE];
  long long result = 0;
  int matched_counter = 0;

  while (input_file.read(read_buffer, FILE_BUFFER_SIZE) || input_file.gcount() > 0) {

    const std::streamsize bytes_read = input_file.gcount();

    for (std::streamsize i = 0; i < bytes_read; ++i) {

      const char ch = read_buffer[i];

      if (ch == '\n') {
        matched_counter = 0;
        continue;
      }

      if (ch == '\r') {
        continue;
      }

      while (matched_counter > 0 && ch != string_target[matched_counter]) {
        matched_counter = prefix[matched_counter - 1];
      }

      if (ch == string_target[matched_counter]) {
        ++matched_counter;
      }

      if (matched_counter == string_target_length) {
        ++result;
        matched_counter = prefix[matched_counter - 1];
      }
    }
  }

  delete[] read_buffer;
  delete[] prefix;

  std::cout << result << '\n';
}
