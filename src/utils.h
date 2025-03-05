#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include <vector>

typedef int (*TestFunc)(void);

// convert inprintable characters to hexadecimal,
// '\n' to '\\n', etc.
auto EncodeString(const std::string &str) -> std::string;

// read all the contents of a file
auto ReadAll(const char *filename) -> std::string;

/* will only handle hexidemical(0x) numbers and decimal numbers
 * @throw std::invalid_argument if the string is not a valid number
 */
auto Atoi(const std::string &str) -> long;

// Remove the first ocurrence of `val` in `vec`.
template <typename T>
void RemoveFromVec(std::vector<T> &vec, T val) {
    typename std::vector<T>::iterator it = vec.begin();
    while (it != vec.end()) {
        if (*it == val) {
            vec.erase(it);
            break;
        }
        it ++;
    }
}

#endif
