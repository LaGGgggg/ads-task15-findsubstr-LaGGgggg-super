#include <fstream>
#include <ios>
#include <iostream>


const int MAX_LINE_LENGTH = 1024;
const int ALPHABET_SIZE = 256;
const int FILE_BUFFER_SIZE = 4 * 1024 * 1024;  // 4 Mb


int calculate_length(const char* string) {

  int len = 0;

  while (string[len] != '\0') {
    ++len;
  }

  return len;
}

int max_int(int a, int b) {
  return a > b ? a : b;
}

void build_bad_characters_table(const char* pattern, int pattern_length, int* last_positions) {

  for (int i = 0; i < ALPHABET_SIZE; ++i) {
    last_positions[i] = -1;
  }

  for (int i = 0; i < pattern_length; ++i) {
    last_positions[static_cast<unsigned char>(pattern[i])] = i;
  }
}

void preprocess_good_suffix(
  const char* pattern, int pattern_length, int* shift, int* border_position
) {

  int i = pattern_length;
  int j = pattern_length + 1;

  border_position[i] = j;

  while (i > 0) {

    while (j <= pattern_length && pattern[i - 1] != pattern[j - 1]) {

      if (shift[j] == 0) {
        shift[j] = j - i;
      }

      j = border_position[j];
    }

    --i;
    --j;

    border_position[i] = j;
  }
}

void fill_prefix_suffix_shifts(int pattern_length, int* shift, const int* border_position) {

  int j = border_position[0];

  for (int i = 0; i <= pattern_length; ++i) {

    if (shift[i] == 0) {
      shift[i] = j;
    }

    if (i == j) {
      j = border_position[j];
    }
  }
}


void build_good_suffix_table(
  const char* pattern, int pattern_length, int* shift, int* border_position
) {

  for (int i = 0; i <= pattern_length; ++i) {
    shift[i] = 0;
  }

  preprocess_good_suffix(pattern, pattern_length, shift, border_position);
  fill_prefix_suffix_shifts(pattern_length, shift, border_position);
}


long long count_occurrences_in_line(
  const char* line,
  int line_length,
  const char* pattern,
  int pattern_length,
  const int* last_positions,
  const int* good_suffix_shifts
) {

  if (line_length < pattern_length) {
    return 0;
  }

  long long result = 0;
  int shift = 0;

  while (shift <= line_length - pattern_length) {

    int j = pattern_length - 1;

    while (j >= 0 && pattern[j] == line[shift + j]) {
      --j;
    }

    if (j < 0) {
      ++result;
      shift += good_suffix_shifts[0];
    } else {

      const unsigned char bad_char = static_cast<unsigned char>(line[shift + j]);

      const int bad_character_shift = j - last_positions[bad_char];
      const int good_suffix = good_suffix_shifts[j + 1];

      shift += max_int(1, max_int(bad_character_shift, good_suffix));
    }
  }

  return result;
}


int main(int argc, char* argv[]) {

  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <target_string> <input_file>\n";
    return 1;
  }

  const char* string_target = argv[1];
  const int string_target_length = calculate_length(string_target);

  if (string_target_length == 0) {
    std::cout << 0 << '\n';
    return 0;
  }

  std::ifstream input_file(argv[2], std::ios::in | std::ios::binary);

  if (!input_file.is_open()) {
    std::cerr << "Cannot open input file\n";
    return 2;
  }

  int last_positions[ALPHABET_SIZE];
  build_bad_characters_table(string_target, string_target_length, last_positions);

  int good_suffix_shift[MAX_LINE_LENGTH + 1];
  int border_position[MAX_LINE_LENGTH + 1];

  build_good_suffix_table(string_target, string_target_length, good_suffix_shift, border_position);

  char* read_buffer = new char[FILE_BUFFER_SIZE];
  char line[MAX_LINE_LENGTH + 1];

  int line_length = 0;
  long long result = 0;

  while (input_file.read(read_buffer, FILE_BUFFER_SIZE) || input_file.gcount() > 0) {

    const std::streamsize bytes_read = input_file.gcount();

    for (std::streamsize i = 0; i < bytes_read; ++i) {

      const char ch = read_buffer[i];

      if (ch == '\n') {

        result += count_occurrences_in_line(
          line,
          line_length,
          string_target,
          string_target_length,
          last_positions,
          good_suffix_shift
        );

        line_length = 0;

      } else if (ch != '\r') {
        line[line_length] = ch;
        ++line_length;
      }
    }
  }

  if (line_length > 0) {
    result += count_occurrences_in_line(
      line,
      line_length,
      string_target,
      string_target_length,
      last_positions,
      good_suffix_shift
    );
  }

  delete[] read_buffer;

  std::cout << result << '\n';

  return 0;
}
