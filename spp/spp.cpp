#include <iostream>

// sudoku pretty printer

using Printer = void (*)(std::string);

void print_minimal(std::string sudoku) {
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            std::cout << sudoku[r * 9 + c] << " ";
        }
        std::cout << std::endl;
    }
}

void print_spaced(std::string sudoku) {
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            char digit = sudoku[r * 9 + c];
            std::cout << " " << (digit == '0' ? '.' : digit);
            if (c % 3 == 2) std::cout << " ";
        }
        std::cout << std::endl;
        if (r == 2 || r == 5) std::cout << std::endl;
    }
}

void print_bordered(std::string sudoku) {
    std::cout << "+-------+-------+-------+" << std::endl;
    for (int r = 0; r < 9; ++r) {
        std::cout << "|";
        for (int c = 0; c < 9; ++c) {
            char digit = sudoku[r * 9 + c];
            std::cout << " " << (digit == '0' ? '.' : digit);
            if (c % 3 == 2) std::cout << " |";
        }
        std::cout << std::endl;
        if (r % 3 == 2) std::cout << "+-------+-------+-------+" << std::endl;
    }
}

int main(int argc, char** argv) {

    int skips = 0;
    std::string line;
    Printer printer = print_spaced;

    if (argc > 1) {
        if (argc > 2) {
            std::cerr << "Too many arguments, expected 0 or 1." << std::endl;
            return 1;
        }

        std::string format = argv[1];
        if (format == "minimal") {
            printer = print_minimal;
        } else if (format == "bordered") {
            printer = print_bordered;
        } else {
            std::cerr << "Usage: " << argv[0] << " [minimal | bordered]" << std::endl;
            return 1;
        }
    }

    while (std::getline(std::cin, line)) {
        if (line.length() < 81) {
            if (line.length())
                skips++;
            continue;
        }

        printer(line);
        std::cout << std::endl;
    }
    if (skips) {
        std::cerr << "Skipped " << skips << " non-empty lines containing less than 81 characters." << std::endl;
    }
}
