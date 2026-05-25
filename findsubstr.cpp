#include <fstream>
#include <iostream>


const unsigned long long X = 257;
const int LINE_MAX_SIZE = 1026;  // 1024 chars + \0 + \n
const int FILE_BUFFER_SIZE = 16 * 1024 * 1024;  // 1 Mb


unsigned long long hash(const char* string, const int string_length) {

  unsigned long long result = 0;

  for (int i = 0; i < string_length; ++i) {
    result = result * X + static_cast<unsigned char>(string[i]);
  }

  return result;
}

unsigned long long pow_x(int power) {

  unsigned long long result = 1;

  for (int i = 0; i < power; ++i) {
    result *= X;
  }

  return result;
}

bool is_substrings_equal(
  const char* a,
  const char* b,
  const int start,
  const int b_length
) {

  for (int j = 0; j < b_length; ++j) {
    if (a[start + j] != b[j]) {
      return false;
    }
  }

  return true;
}

int calculate_length(const char* string) {

  int len = 0;

  while (string[len] != '\0') {
    ++len;
  }

  return len;
}

int count_substring(
  const char* string_src,
  const int string_src_length,
  const char* string_target,
  const int string_target_length,
  const unsigned long long target_hash,
  const unsigned long long x
) {

  if (string_target_length > string_src_length) {
    return 0;
  }

  unsigned long long current_hash = hash(string_src, string_target_length);
  int result = 0;
  const int limit = string_src_length - string_target_length;

  for (int i = 0; i <= limit; ++i) {

    if (
      current_hash == target_hash
      && string_src[i] == string_target[0]
      && string_src[i + string_target_length - 1] == string_target[string_target_length - 1]
      && is_substrings_equal(string_src, string_target, i, string_target_length)
    ) {
      ++result;
    }

    if (i < limit) {

      current_hash = (
        current_hash * X
        - static_cast<unsigned char>(string_src[i]) * x
        + static_cast<unsigned char>(string_src[i + string_target_length])
      );
    }
  }

  return result;
}

int main(int argc, char* argv[]) {

  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <target_string> <input_file>\n";
    return 1;
  }

  std::ifstream input_file(argv[2], std::ios::binary);

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

  const unsigned long long target_hash = hash(string_target, string_target_length);
  const unsigned long long x = pow_x(string_target_length);

  char read_buffer[FILE_BUFFER_SIZE];
  char line[LINE_MAX_SIZE];

  int line_length = 0;
  long long result = 0;

  while (input_file.read(read_buffer, FILE_BUFFER_SIZE) || input_file.gcount() > 0) {

    const std::streamsize bytes_read = input_file.gcount();

    for (std::streamsize i = 0; i < bytes_read; ++i) {

      const char ch = read_buffer[i];

      if (ch == '\n') {

        result += count_substring(
          line,
          line_length,
          string_target,
          string_target_length,
          target_hash,
          x
        );

        line_length = 0;

      } else if (ch != '\r') {

        line[line_length] = ch;
        ++line_length;
      }
    }
  }

  if (line_length > 0) {
    result += count_substring(
      line,
      line_length,
      string_target,
      string_target_length,
      target_hash,
      x
    );
  }

  std::cout << result << '\n';

  return 0;
}
