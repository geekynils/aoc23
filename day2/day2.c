/*
Question: https://adventofcode.com/2023/day/2
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

int max(int a, int b) {
    if (a >= b) { return a; }
    return b;
}

int min(int a, int b) {
    if (a <= b) { return a; }
    return b;
}

typedef struct {
    char *s;
    int sz;
} Str;

Str mkstr(char* ptr) {
    size_t sz = strlen(ptr);
    return (Str) {ptr, (int)sz};
}

/// Make Heap Str
Str mkhstr(char* ptr) {
    size_t sz = strlen(ptr);
    char *hptr = (char*) malloc(sz);
    memcpy(hptr, ptr, sz);
    return (Str){hptr, (int)sz};
}

int ezy_strstr(Str haystack, Str needle) {
    if (needle.sz > haystack.sz) { return -1;}
    int right[256];
    for (int i = 0; i < 256; ++i) { right[i] = -1; }
    for (int i = 0; i < needle.sz; ++i) { right[(int)needle.s[i]] = i; }

    int i = 0;
    while (i <= (haystack.sz - needle.sz)) {
        int skip = 0;
        for (int j = needle.sz - 1; j >= 0; --j) {
            char c = haystack.s[i + j];
            if (needle.s[j] != c) {
                skip = j - right[(int)c];
                skip = max(1, skip);
                break;
            }
        }
        if (skip == 0) {
            return i;
        }
        i += skip;
    }

    return -1;
}

bool ezy_eq(Str a, Str b) {
    if (a.sz != b.sz) { return false; }
    for (int i = 0; i < a.sz; ++i) {
        if (a.s[i] != b.s[i]) { return false; }
    }
    return true;
}

typedef struct {
    int num;
    int nChars; // Number of chars read.
} ReadNumRet;


/// Returns the number starting at *ptr or -1 if *ptr does not point to a digit char.
ReadNumRet readNum(const char *ptr) {
    if (!('0' <= *ptr && *ptr <= '9')) {
        return (ReadNumRet){-1, 1};
    }
    ReadNumRet ret = {0, 0};
    while ('0' <= *ptr && *ptr <= '9') {
        ret.num *= 10;
        ret.num += *ptr - '0';
        ++ret.nChars;
        ++ptr;
    }
    return ret;
}

/*
int main(int argc, const char * argv[]) {

    //               012345678901234567
    Str t = mkstr("the sun is shining");
    Str s0 = mkstr("un");
    Str s1 = mkstr("ng");
    Str s2 = mkstr("ni");
    Str s3 = mkstr("suun");

    int m0 = ezy_strstr(t, s0);
    int m1 = ezy_strstr(t, s1);
    int m2 = ezy_strstr(t, s2);
    int m3 = ezy_strstr(t, s3);
    
    printf("%s matches at %d\n", s0.s, m0);
    printf("%s matches at %d\n", s1.s, m1);
    printf("%s matches at %d\n", s2.s, m2);
    printf("%s matches at %d\n", s3.s, m3);

    return 0;
}

*/

#define MAX_MOVES 100

typedef enum {r=0, g=1, b=2} Color;

typedef struct {
    int num;
    Color color;
} Move;

typedef struct {
    int num;
    int numMoves;
    Move *ptr;
} Game;

typedef struct {
    Game *ptr;
    int sz;
} GameArray;

/// Crates a game with moves on the stack.
bool readGame(Str line, Game *game) {
    // Each line starts with: Game NUM
    const int start = 5;
    if (line.sz < start) {
        return false;
    }
    ReadNumRet ret = readNum(line.s + start);
    *game = (Game){ .num = ret.num };
    bool expectNum = true;
    char *r = "red", *g = "green", *b = "blue";
    Str colorStrings[3] = {mkstr(r), mkstr(g), mkstr(b)};
    Move moves[MAX_MOVES];
    int moveIdx = 0;
    for (int i = start + ret.nChars; i < line.sz;) {        
        if (expectNum) {
            ReadNumRet ret = readNum(line.s + i);
            // Read until we arrive at the number:
            if (ret.num != -1) {
                moves[moveIdx].num = ret.num;
                expectNum = !expectNum;
            } 
            i += ret.nChars;
        } else {
            bool colorFound = false;
            for (int j = 0; j < 3; ++j) {
                int sz = min(colorStrings[j].sz, line.sz - i);
                if (ezy_eq(colorStrings[j], (Str){line.s + i, sz})) {
                    moves[moveIdx].color = j;
                    ++moveIdx;
                    expectNum = !expectNum;
                    i += sz;
                    colorFound = true;
                    break;
                }
            }
            if (!colorFound) {
                ++i; // Advance if we cannot find a color string.
            }
        }
    }
    game->numMoves = moveIdx;
    game->ptr = malloc(sizeof(Move)*moveIdx);
    memcpy(game->ptr, moves, sizeof(Move)*moveIdx);
    return true;
}

/// Allocates and reads games.
GameArray readGames(Str input) {
    GameArray games = {};
    int nlines = 0;
    for (int i = 0; i < input.sz; ++i) {
        if (input.s[i] == '\n') { nlines += 1; }
    }
    games.ptr = (Game*)malloc(sizeof(Game)*nlines);
    
    int lineStart = 0;
    int j = 0;
    for (int i = 0; i < input.sz; ++i) {
        if (input.s[i] == '\n') {
            Str gameStr = {input.s + lineStart, i - lineStart};
            if (gameStr.sz > 6) {
                Game game;
                if (readGame(gameStr, &game)) {
                    games.ptr[j] = game;
                    ++j;
                    lineStart = i + 1;
                }
            }
        }
    }
    games.sz = j;
    return games;
}

bool isPossible(Game game, int *colors) {
    for (int i = 0; i < game.numMoves; ++i) {
        Move move = game.ptr[i];
        if (move.num > colors[move.color]) {
            return false;
        }
    }
    return true;
}

int findMinPowerSet(Game game) {
    int maxs[] = {0, 0, 0};
    for (int i = 0; i < game.numMoves; ++i) {
        Move move = game.ptr[i];
        maxs[move.color] = max(maxs[move.color], move.num);
    }
    int powerSet = 1;
    for (int i = 0; i < 3; ++i) {
        powerSet *= maxs[i];
    }
    return powerSet;
}


#undef MAX_MOVES

int main(int argc, const char* argv[]) {
    
    if (argc < 3) {
        fprintf(stderr, "Provide input data as first argument and one or two as the second argument!\n");
        return 1;
    }
    
    const char* what = argv[2];
    
    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "Cannot open %s\n", argv[1]);
        return 1;
    }
    
    fseek(fp, 0L, SEEK_END);
    size_t sz = ftell(fp);
    rewind(fp);
    Str input = {(char*)malloc(sz), (int)sz };
    fread(input.s, 1, sz, fp);
    fclose(fp);
    if (strcmp(what, "one") == 0) {
        GameArray games = readGames(input);
        int sum = 0;
        int marbles[] = {12, 13, 14};
        for (int i = 0; i < games.sz; ++i) {
            Game game = games.ptr[i];
            // printf("%d, %d\n", game.num, game.numMoves);
            if (isPossible(games.ptr[i], marbles)) { sum += game.num; }
            free(game.ptr);
        }
        free(games.ptr);
        printf("Sum of game numbers: %d\n", sum);
    } else {
        GameArray games = readGames(input);
        int sum = 0;
        for (int i = 0; i < games.sz; ++i) {
            Game game = games.ptr[i];
            int minPowerSet = findMinPowerSet(game);
            sum += minPowerSet;
            free(game.ptr);
        }
        free(games.ptr);
        printf("Sum of min power sets: %d\n", sum);
    }
    free(input.s);
}


