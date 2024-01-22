#ifndef _COLORS_
#define _COLORS_

/* FOREGROUND */
#include "NumReply.hpp"
#define RST     "\x1B[0m"
#define KBLA    "\x1B[30m"
#define KRED    "\x1B[31m"
#define KGRN    "\x1B[32m"
#define KYEL    "\x1B[33m"
#define KBLU    "\x1B[34m"
#define KMAG    "\x1B[35m"
#define KCYN    "\x1B[36m"
#define KWHT    "\x1B[37m"

#define FBLA(x) KBLA x RST
#define FRED(x) KRED x RST
#define FGRN(x) KGRN x RST
#define FYEL(x) KYEL x RST
#define FBLU(x) KBLU x RST
#define FMAG(x) KMAG x RST
#define FCYN(x) KCYN x RST
#define FWHT(x) KWHT x RST

/* BACKGROUND */
#define KBBLA   "\x1B[41m"
#define KBRED   "\x1B[41m"
#define KBGRN   "\x1B[42m"
#define KBYEL   "\x1B[43m"
#define KBBLU   "\x1B[44m"
#define KBMAG   "\x1B[45m"
#define KBCYN   "\x1B[46m"
#define KBWHT   "\x1B[47m"

#define BBLA(x) KBBLA x RST
#define BRED(x) KBRED x RST
#define BGRN(x) KBGRN x RST
#define BYEL(x) KBYEL x RST
#define BBLU(x) KBBLU x RST
#define BMAG(x) KBMAG x RST
#define BCYN(x) KBCYN x RST
#define BWHT(x) KBWHT x RST

/* GRAPHIC */
#define BOLD(x) "\x1B[1m" x RST
#define ITL(x)  "\x1B[3m" x RST
#define UNDL(x) "\x1B[4m" x RST

class Color {
public:
    static std::string black(std::string str) {
        return KBLA + str + RST;
    };
    static std::string red(std::string str) {
        return KRED + str + RST;
    };
    static std::string green(std::string str) {
        return KGRN + str + RST;
    };
    static std::string yellow(std::string str) {
        return KYEL + str + RST;
    };
    static std::string blue(std::string str) {
        return KBLU + str + RST;
    };
    static std::string magenta(std::string str) {
        return KMAG + str + RST;
    };
    static std::string Cyan(std::string str) {
        return KCYN + str + RST;
    };
    static std::string white(std::string str) {
        return KWHT + str + RST;
    };
    class Background {
    public:
        static std::string black(std::string str) {
            return KBBLA + str + RST;
        }
    };
};
#endif
