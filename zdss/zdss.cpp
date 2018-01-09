#include <iostream>
#include <array>
#include <string>

#if _MSC_VER
#include <intrin.h>
#pragma intrinsic(_BitScanForward)
#define MSVC
#ifdef _DEBUG
#define DEBUG
#endif
#endif

#ifdef DEBUG
#include <bitset>
#endif

#ifdef TIMED
#include <chrono>
#endif

/**
 * ZDSS - the "zero-delay sudoku solver" utility
 *
 * Solves any 9x9-sudoku passed to stdin as a string of 81 characters and prints the solution (if it exists).
 * Your mileage may vary depending on the size of your dataset :)
 *
 * Example input:
 * 021000000000004900000000000030120000700000400000300000400006508000080020900000000
 * 030000000000195000008000060800060000400800001000020000060000280000419005000000070
 */

// COMPILATION OPTIONS
// NOVALIDATE   to suppress input validation
// NOREPRINT    to suppress reprinting the input
// TIMED        to enable timer


using Sudoku = std::array<int, 81>;
using SudokuData = std::array<int, 81>;

inline int first_set_index(int i) {
#ifdef MSVC
    unsigned long index;
    _BitScanForward(&index, i);
    return (int) index + 1;
#else
    return __builtin_ctz(i) + 1;
#endif
}

inline int set_bits_count(int i) {
#ifdef MSVC
    return __popcnt(i);
#else
    return __builtin_popcount(i);
#endif
}

void remove_adjacent(SudokuData &data, int field, int candidate) {
    int r = field / 9 * 9, c = field % 9, sq = field / 27 * 27 + c / 3 * 3;
    int mask = ~candidate;
    for (int i = 0; i < 9; ++i) {
        data[r + i] &= mask;
        data[c + i * 9] &= mask;
    }
    data[sq] &= mask; data[sq + 1] &= mask; data[sq + 2] &= mask;
    data[sq + 9] &= mask; data[sq + 10] &= mask; data[sq + 11] &= mask;
    data[sq + 18] &= mask; data[sq + 19] &= mask; data[sq + 20] &= mask;
}

void put(Sudoku &sudoku, SudokuData &data, int field, int candidate) {
    sudoku[field] = candidate;
    data[field] = 0;
    remove_adjacent(data, field, candidate);
}

// find single candidate fields
bool singles(Sudoku &sudoku, SudokuData &data) {
    bool found = false;

    for (int i = 0; i < 81; ++i) {
        if (sudoku[i])
            continue;
        int candidates = data[i];
        int count = set_bits_count(candidates);
        if (count == 0)
            return false;
        if (count == 1) {
            int c = candidates;
            put(sudoku, data, i, c);
#ifdef DEBUG
            std::cout << "[s] " << i << ": Found " << firstSetIndex(c) << "." << std::endl;
#endif
            found = true;
        }
    }
    return found;
}

// find candidates that occur exactly once in a block ("hidden singles")
// <0, 1, 0> / <0> corresponds to top row
// <0, 9, 0> / <0, 9> corresponds to leftmost column
// <0, 1, 6> corresponds to upper left square
template <int start, int inc = 1, int skip = 0>
bool hsinglesblock(Sudoku &sudoku, SudokuData &data) {

    int onceormore = 0, twiceormore = 0;
    int index = start;

    // unroll this please
    for (int i = 0; i < 3; ++i, index += skip) {
        for (int j = 0; j < 3; ++j, index += inc) {
            int candidates = data[index];
            twiceormore |= onceormore & candidates;
            onceormore |= candidates;
        }
    }

    int once = onceormore & ~twiceormore;
    if (!once)
        return false;

#ifdef DEBUG
    std::cout << "[-] Discovered single occurrences of " << std::bitset<9>(*(unsigned int *)&once)
        << " in block (" << start << "," << inc << "," << skip << ")" << std::endl;
#endif

    index = start;
    for (int i = 0; i < 3; ++i, index += skip) {
        for (int j = 0; j < 3; ++j, index += inc) {
            int intersect = data[index] & once;
            if (intersect) {
                put(sudoku, data, index, intersect);
#ifdef DEBUG
                std::cout << "[h] " << index << ": Found " << firstSetIndex(intersect) << "." << std::endl;
#endif
            }
        }
    }
    return true;
}

// generate calls to hsinglesblock
template <int N = 0>
bool hsingles(Sudoku &sudoku, SudokuData &data) {
    return hsinglesblock<N * 9>(sudoku, data) ||
           hsinglesblock<N, 9>(sudoku, data) ||
           hsinglesblock<N / 3 * 27 + N % 3 * 3, 1, 6>(sudoku, data) ||
           hsingles<N + 1>(sudoku, data);
}

template <>
bool hsingles<9>(Sudoku &sudoku, SudokuData &data) { return false; }

bool solve(Sudoku &sudoku, SudokuData &data) {

    bool found = true;
    while (found) {
        found = singles(sudoku, data) || hsingles<>(sudoku, data);
    }

    int min = 10;
    int mindex = -1;

    // find empty field with lowest candidate count
    for (int i = 0; i < 81; ++i) {
        int count = set_bits_count(data[i]);
        if (!sudoku[i] && count < min) {
            min = count;
            mindex = i;
        }
    }

    // no field is empty
    if (mindex == -1)
        return true;

    // test all candidates
    Sudoku scopy = sudoku;
    SudokuData dcopy = data;
    for (int candidates = data[mindex]; candidates;) {

        int c = candidates & -candidates;
        put(sudoku, data, mindex, c);

#ifdef DEBUG
        std::cout << "[-] " << mindex << ": Testing " << firstSetIndex(c) << ". Candidate set is "
            << std::bitset<9>(*(unsigned int *)&candidates) << "." << std::endl;
#endif

        if (solve(sudoku, data))
            return true;

        data = dcopy;
        sudoku = scopy;
#ifdef DEBUG
        std::cout << "[-] " << mindex << ": Revoking " << firstSetIndex(c) << "." << std::endl;
#endif
        candidates &= ~c;
    }
#ifdef DEBUG
    std::cout << "[-] " << mindex << ": Search failed." << std::endl;
#endif
    return false;
}

int main() {

#ifdef TIMED
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
#endif

    std::array<char, 82> line;
    while (true) {

        // 81 digits + newline
        std::cin.read(line.data(), 82);
        if (std::cin.eof())
            break;

#ifndef NOVALIDATE
        if (!std::cin) {
            std::cerr << "I/O error." << std::endl;
            return 1;
        }
#endif

        std::streamsize count = std::cin.gcount();

#ifndef NOVALIDATE
        if (count != 82) {
            std::cerr << "A valid line has to contain exactly 81 characters and a line terminator. Found "
                      << count << ". Illegal line:\n";
            std::cerr.write(line.data(), count);
            std::cerr << std::endl;
            return 1;
        }
#endif

        Sudoku sudoku;
        SudokuData data;
        data.fill(0x1ff);

        bool unsolvable = false;

        for (int i = 0; i < 81; ++i) {
#ifndef NOVALIDATE
            if (line[i] > '9' || line[i] < '0') {
                std::cerr << "Illegal digit: " << line[i] << std::endl;;
                return 1;
            }
#endif
            int digit = line[i] - '0';
            int bit = digit ? 1 << (digit - 1) : 0;
            sudoku[i] = bit;
            if (digit) {
                if (~data[i] & bit) {
                    unsolvable = true;
                    break;
                }
                data[i] = 0;
                remove_adjacent(data, i, bit);
            }
        }

#ifndef NOREPRINT
        std::cout.write(line.data(), 82);
#endif

        if (unsolvable) {
            std::cout << "Instance unsolvable.\n";
        }
        else if (solve(sudoku, data)) {
            for (int i = 0; i < 81; ++i) {
                line[i] = sudoku[i] ? (char) first_set_index(sudoku[i]) + '0' : '0';
            }
            std::cout.write(line.data(), 82);
        }
        else {
            std::cout << "No solution found.\n";
        }
    }

#ifdef TIMED
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> delta = end - start;
    std::cerr << "Required " << delta.count() << " seconds." << std::endl;
#endif

    return 0;
}
