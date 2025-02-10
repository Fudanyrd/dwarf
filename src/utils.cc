#include "utils.h"
#include <sstream>
#include <unistd.h>
#include <fcntl.h>

auto EncodeString(const std::string &str) -> std::string {
    std::ostringstream ss;
    ss << "\"";
    for (const auto &ch : str) {
        switch (ch) {
        case ('\\'): {
            ss << "\\\\";
            break;
        }
        case ('\a'): {
            ss << "\\a";
            break;
        }

        case ('\b'): {
            ss << "\\b";
            break;
        }
        case ('\f'): {
            ss << "\\f";
            break;
        }

        case ('\n'): {
            ss << "\\n";
            break;
        }
        case ('\r'): {
            ss << "\\r";
            break;
        }
        case ('\t'): {
            ss << "\\t";
            break;
        }

        case ('\"'): {
            ss << "\\\"";
            break;
        }
        case ('\'') : {
            ss << "\\\'";
            break;
        }

        default: {
            ss << ch;
            break;
        }
        }
    }

    ss << "\"";
    return ss.str();
}

auto ReadAll(const char *filename) -> std::string {
    std::ostringstream ss;
    static char buf[512];

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        throw std::runtime_error("Failed to open file");
    }

    long nread = read(fd, buf, sizeof(buf));
    while (nread > 0) {
        ss.write(buf, nread);
        nread = read(fd, buf, sizeof(buf));
    }

    close(fd);
    return ss.str();
}


auto Atoi(const std::string &str) -> long {
    long ret = 0;
    long base = 10;
    // FIXME: handle negative number
    if (str[0] == '0') {
        if (str.size() == 1) {
            return 0;
        }
        if (str[1] != 'x' && str[1] != 'X') {
            throw std::invalid_argument("number should not begin with 0");
        }
        base = 16;

        for (size_t i = 2; i < str.size(); i++) {
            char ch = str[i];
        switch (ch) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            ret = ret * base + ch - '0';
            break;

        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            ret = ret * base + ch - 'a' + 10;
            break; 
        
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            ret = ret * base + ch - 'A' + 10;
            break;

        default: 
            throw std::runtime_error("Invalid hexadecimal character");
        }
        }
    } else {
        for (const auto &ch : str) {
            if (ch < '0' || ch > '9') {
                throw std::invalid_argument("number should be decimal");
            }
            ret *= base;
            ret += static_cast<long>(ch - '0');
        }
    }

    return ret;
}
