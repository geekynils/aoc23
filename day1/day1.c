#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int one(char *buffer, size_t sz) {
    const int not_digit = 10;
    int first = not_digit, second = not_digit;
    int sum = 0;
    for (size_t i = 0; i < sz; ++i) {
        if (buffer[i] == '\n') {
            int value = first;
            if (second != not_digit) {
                value = 10*value + second;
            } else {
                value = 10*value + first;
            }
            sum += value;
            first = second = not_digit;
        }
        else if ('0' <= buffer[i] && buffer[i] <= '9') {
            if (first == not_digit) {
                first = buffer[i] - '0';
            } else {
                second = buffer[i] - '0';
            }
        }
    }
    return sum;
}

int argmin(size_t *nums, int end) {
    size_t min = INT_MAX;
    int min_i = 0;
    for (int i = 0; i < end; ++i) {
        if (nums[i] < min) {
            min = nums[i];
            min_i = i;
        }
    }
    return min_i;
}

int argmax(size_t *nums, int end) {
    size_t max = 0;
    int max_i = 0;
    for (int i = 0; i < end; ++i) {
        if (nums[i] > max) {
            max = nums[i];
            max_i = i;
        }
    }
    return max_i;
}

int two(const char *buffer, size_t sz) {
    
    char digit_words[9][6] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
    
#define MAX_NUM 20
    
    int numbers[MAX_NUM];
    size_t positions[MAX_NUM];
    size_t line_start = 0;
    int k = 0;
    
    int sum = 0;
    for (size_t i = 0; i < sz; ++i) {
        if ('0' <= buffer[i] && buffer[i] <= '9') {
            numbers[k] = buffer[i] - '0';
            positions[k] = i;
            ++k;
        }
        else if (buffer[i] == '\n') {
            size_t len = i - line_start;
            for (int j = 0; j < 9; ++j) {
                const char* ret = strnstr(buffer + line_start, digit_words[j], len);
                while (ret) {
                    size_t pos = ret - buffer;
                    numbers[k] = j + 1;
                    positions[k] = pos;
                    ++k;
                    // Note that the length changes here since we only look at
                    // the rest of the string.
                    ret = strnstr(ret + 1, digit_words[j], i - pos);
                }
            }
            
            int first_index = argmin(positions, k);
            int last_index = argmax(positions, k);
            
            int val = 10*numbers[first_index] + numbers[last_index];
            
            sum += val;
            
            printf("%d ", val);
            for (int k = 0; k < len; ++k) {
                printf("%c", buffer[line_start + k] );
            }
            printf("\n");
            
            line_start = i + 1;
            k = 0;
        }
    }
    return sum;
}

int main(int argc, const char * argv[]) {
    
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
    char *buffer = (char*)malloc(sz);
    fread(buffer, 1, sz, fp);
    fclose(fp);
    
    int sum;
    if (strcmp(what, "one") == 0) { sum = one(buffer, sz);}
    else { sum = two(buffer, sz); }
    
    printf("The sum of calibration values is %d\n", sum);
    
    return 0;
}
