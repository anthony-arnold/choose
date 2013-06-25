/*
    Choose
    Copyright (C) 2013  Anthony Arnold

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <vector>
#include <string>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <algorithm>

// Alias the largest integer type
typedef long long big_int;
typedef unsigned long long big_uint;


// Global input array
std::vector<std::string> INPUT_VECTOR;

// Global string defines output separator
static std::string OUTPUT_SEPARATOR = ",";

// Global integer defines the choose number (K)
static big_uint CHOOSE_K = 0;


// Print usage
static void print_usage() {
    std::cerr << "usage: choose [-t | -c] <item> [<item> ...] k" << std::endl;
}

// Read the separator argument (if it exists)
static bool read_separator(const std::string &arg) {
    
    if (arg == "-t") {
        OUTPUT_SEPARATOR = "\t";
        return true;
    } else if (arg == "-c") {
        OUTPUT_SEPARATOR = ",";
        return true;
    }
    
    return false;
}

// Read the K (for n choose k) value from command line
static bool read_k(const std::string &arg) {
    std::size_t converted;
    
    
    try {
        CHOOSE_K = std::stoull(arg, &converted);
    } catch (const std::invalid_argument &) {
        std::cerr << "Invalid argument k" << std::endl;
        return false;
    } catch (const std::out_of_range &) {
        std::cerr << "Out of range k" << std::endl;
        return false;
    }
    
    if (converted < arg.length()) {
        // the whole thing must be a number
        return false;
    }
    
    return true;
}

// When a string is found, input here.
static void input_string(const std::string &str) {
    INPUT_VECTOR.emplace_back(str);
}

// Generate a range of strings representing integers
// Range can be descending.
struct range_generator {
    
    range_generator(big_int start, big_int end) : 
        start_(start), inc_(start < end ? 1 : -1)
    {    
    }
    
    std::string operator()() {
        big_int next = start_;
        start_ += inc_;
        return std::to_string(next);
    }
    
private:
    big_int start_;
    int inc_;
};


// Add range [first,last] to the input vector
static void input_range(big_int first, big_int last) {
    // +1 because range is inclusive
    big_int count = (first < last ? last - first : first - last) + 1;

    auto start = INPUT_VECTOR.size();
    INPUT_VECTOR.resize(start + count);
    std::generate(INPUT_VECTOR.begin() + start, INPUT_VECTOR.end(), 
                                                range_generator(first, last));
      
}

// Read a potential range element
static bool read_range(const std::string &left, const std::string &right) {
    if (!left.length() || !right.length()) {
        // Not a range
        return false;
    }

    
    auto dash = left.find('-');
    if (dash != std::string::npos && dash != 0 /* minus */) {
        // Not a range. Number must be in the form "\-?[0-9]+"
        return false;
    }
    
    dash = right.find('-');
    if (dash != std::string::npos && dash != 0 /* minus */) {
        // Not a range. Number must be in the form "\-?[0-9]+"
        return false;
    }
    
    // Get the values of left and right size
    try {
        size_t pos;
        big_int first = std::stoll(left, &pos);
        if (pos != left.length()) {
            // The whole thing must be a number
            return false;
        }
        
        big_int last = std::stoll(right, &pos);
        if (pos != right.length()) {
            // The whole thing must be a number
            return false;
        }
       
        // Add the range to the correct collection
        input_range(first, last);
    
    } catch (...) {
        // One side is not a number
        return false;
    }
    
    return true;
}

// Read in each element argument and add to the global vector
static void read_elements(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        std::string arg(argv[i]);
        
        std::size_t dash_pos;
        std::size_t last_dash_pos = arg.rfind('-');
        std::size_t first_dash_pos = arg.find('-');
        
        if (last_dash_pos != first_dash_pos) {
            // more than one dash.
            if (first_dash_pos == 0) {
                // first one is a minus symbol
                dash_pos = last_dash_pos;
                
                // second might be minus symbol too
                if (arg[dash_pos - 1] == '-') {
                    dash_pos--;
                }
            } else {
                // first one is the dash
                dash_pos = first_dash_pos;
            }
            
        } else {
            dash_pos = last_dash_pos;
        }
        
        
        if (dash_pos != std::string::npos && dash_pos != 0) {
            // Element may be a range.
            std::string left(arg.begin(), arg.begin() + dash_pos);
            std::string right(arg.begin() + dash_pos + 1, arg.end());
            
            if (!read_range(left, right)) {
                // Element is not a range.
                input_string(arg);
            }
            
        } else {
            // Element is not a range.
            input_string(arg);
        }
    }
}

// Recusively print combinations
template <typename I>
static void print_combination(std::vector<std::string*> &combos, 
                                                    I begin, I end, big_int k) 
{
    if (!k) {
        for(auto it = combos.begin(); it != combos.end() - 1; ++it) {
            std::cout << **it << OUTPUT_SEPARATOR;
        }
        std::cout << *combos.back() << std::endl;
        
        return;
    }

    while (std::distance(begin, end) >= k) {
        
        combos.push_back(&*begin);
        print_combination(combos, ++begin, end, k - 1);
        combos.pop_back();
        
    }
}

// Prints all combinations based on the container
template <typename Container>
static void print_all(Container &e) {
    std::vector<std::string *> combos;
    print_combination(combos, e.begin(), e.end(), CHOOSE_K);
}

int main (int argc, char **argv) {

    // Check input
    if (argc < 3) {
        print_usage();
        return 1;
    }
    
    // Set the output separator
    argc--;
    argv++;
    if (read_separator(*argv)) {
        argc--;
        argv++;
    }
    
    // Make sure there are enough arguments left.
    if (argc < 2) {
        print_usage();
        return 2;
    }

    // Read the K value
    argc--;
    if (!read_k(*(argv + argc))) {
        print_usage();
        return 3;
    }
    
    // n choose k for all k == 0 is 1: an empty set
    if (CHOOSE_K == 0) {
        return 0;
    }
    
    // Single element of '-' means get elements from stdin
    if (argc == 1 && (*argv)[0] == '-' && (*argv)[1] == '\0') {
        std::string word;
        while (std::cin >> word) {
            input_string(word);
        }
    } else {    
        // Fill the element vector
        read_elements(argc, argv);
    }
    
    // Print all the combinations
    print_all(INPUT_VECTOR);
    
    return 0;
}
