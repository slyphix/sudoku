#include <iostream>
#include <array>
#include <string>

/**
 * ZDSV - the "zero-delay sudoku verifier" utility
 *
 * Verifies any 9x9-sudoku passed to stdin as a string of 81 characters and reports any errors to stderr.
 *
 * Example input:
 * 021000000000004900000000000030120000700000400000300000400006508000080020900000000
 *   (valid)
 * 821593647357264981649871253534127896718659432296348175472936518163485729900000000
 *   (incomplete solution)
 * 821593647357264981649871253534127896718659432296348175472936518163485729999999999
 *   (erroneous solution)
 */

// COMPILATION OPTIONS
// NOVALIDATE    to suppress input validation
// ALLOWEMPTY    to allow empty fields and check for contradictions only
// FINDALL       to prevent the program from terminating after discovering the first contradiction
// REPRINT       to enable reprinting the input and suppress affirmation of correct input

// REPRINT + FINDALL  can be used to filter any invalid sudoku instances.
// ALLOWEMPTY         can also be used to only check if the input format is valid.

using Sudoku = std::array<int, 81>;

template <int start, int inc = 1, int stride = 0>
bool validate_block(Sudoku &sudoku) {

    int once = 0;
    int twice = 0;
    int offset = start;

    for (int i = 0; i < 3; ++i, offset += stride) {
        for (int j = 0, index = offset; j < 3; ++j, index += inc) {
            twice |= once & sudoku[index];
            once |= sudoku[index];
        }
    }
#ifdef ALLOWEMPTY
    return twice == 0;
#else
    return once == 0x1ff;
#endif
}

template <int N = 0>
bool validate(Sudoku &sudoku) {
    return validate_block<N * 9>(sudoku) &&
           validate_block<N, 9>(sudoku) &&
           validate_block<N / 3 * 27 + N % 3 * 3, 1, 9>(sudoku) &&
           validate<N + 1>(sudoku);
}

template <>
bool validate<9>(Sudoku &sudoku) { return true; }

int main(int argc, char** argv) {

    bool ok = true;
    std::string line;

    while (std::getline(std::cin, line)) {

        if (line.empty())
            continue;

#ifndef NOVALIDATE
        if (line.length() != 81) {
            std::cerr << "A valid line has to contain exactly 81 characters and a line terminator. Found "
                      << line.length() << ". Offending line:\n" << line << std::endl;
            return 1;
        }
#endif

        Sudoku sudoku;
        for (int i = 0; i < 81; ++i) {
#ifndef NOVALIDATE
            if (line[i] > '9' || line[i] < '0') {
                std::cerr << "Illegal digit: " << line[i] << std::endl;;
                return 1;
            }
#endif
            int digit = line[i] - '0';

#ifndef ALLOWEMPTY
            if (!digit) {
                std::cerr << "Empty field detected. Offending line:\n" << line << std::endl;
                ok = false;
#ifndef FINDALL
                return 2;
#endif
            }
#endif

            int bit = 1 << (digit - 1);
            sudoku[i] = bit;
        }

        if (!validate<>(sudoku)) {
            std::cerr << "Contradiction detected. Offending line:\n" << line << std::endl;
            ok = false;
#ifndef FINDALL
            return 2;
#endif
        }

#ifdef REPRINT
        std::cout << line << "\n";
#endif
    }

#ifndef REPRINT
#ifdef FINDALL
    if (ok)
#endif
        std::cout << "No contradictions detected." << std::endl;
#endif

    return 0;
}
