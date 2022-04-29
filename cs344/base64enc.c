#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int power(int base, int exp) {
    int result = 1;
    while (exp) {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        base *= base;
    }
    return result;
}

int main(int argc,char* argv[]) {
    static char const alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                               "abcdefghijklmnopqrstuvwxyz"
                               "0123456789+/=";
    char* string_to_use = "";
    char* path_to_file =  argv[0];
    // if there is an argument, set path_to_file to it
    if (argc > 1) {
        path_to_file = argv[1];
    }
    int length = 0;

    // if ther is no argument, get input from stdin
    if (argc == 1) {
        // get input from stdin
        char* input = malloc(sizeof(char) * 100);
        fgets(input, 100, stdin);
        // get the length of the input
        length = strlen(input);
        // set the string to use to the input
        string_to_use = input;
    }

    else {
        
        if (path_to_file[0] != '-'){
            // open file
            FILE* file = fopen(path_to_file, "r");
            // if file is not found, return
            if (file == NULL) {
                return 0;
            }
            // get length of file
            fseek(file, 0, SEEK_END);
            length = ftell(file);
            fseek(file, 0, SEEK_SET);
            // store file contents in string
            char* file_contents = malloc(length);
            fread(file_contents, 1, length, file);
            file_contents[length] = '\0';
            // empty string_to_use
            string_to_use = file_contents;
            // close file
            fclose(file);
        }
        
        // if second argument begins with "-", remove leading "-" from string_to_use
        if (path_to_file[0] == '-'){
            string_to_use = path_to_file + 1;
            length = strlen(string_to_use) + 1;
        }
        
    }
    // printf("string: %s\n", string_to_use);
    // length of string
    
    // print len
    // printf("length: %d\n", length);
    int full_length = 8 * length * 3;
    char* full_binary_string = malloc(full_length);
    char* equals_sign[1];
    equals_sign[0] = "=";
    // show full length
    // printf("full length: %d\n", full_length);
    for (int i = 0; i < full_length; i++) {
        full_binary_string[i] = equals_sign[0][0];
    }
    for (int i = 0; i < length; i++){
        // printf("string_to_use char[%i]: %c\n",i, string_to_use[i]);
    }

    // fill full binary string with binary representation of string_to_use
    for (int i = 0; i < length; i++) {
        // get binary representation of char
        char* binary_string = malloc(8);
        char current_char = string_to_use[i];
        for (int j = 0; j < 8; j++) {
            binary_string[j] = '0';
        }
        int value_of_char = current_char;
        // show value of char
        // printf("value of char: %d\n", value_of_char);
        // access each bit of char
        for (int j = 0; j < 8; j++) {
            // get bit
            int bit = value_of_char % 2;
            // show bit
            // printf("bit: %d\n", bit);
            // set bit in binary string
            binary_string[7 - j] = bit + '0';
            
            // shift value of char
            value_of_char = value_of_char / 2;
        }
        // show binary string
        // printf("binary string: %s\n", binary_string);
        // add binary string to full binary string if not all bits are zero
        if (strcmp(binary_string, "00000000") != 0) {
            for (int j = 0; j < 8; j++) {
                full_binary_string[i * 8 + j] = binary_string[j];
            }
        }

    }

    // print full binary string
    // printf("**full binary string: %s\n", full_binary_string);
    // find index of first "="
    int index_of_first_equals = 0;
    for (int i = 0; i < full_length; i++) {
        if (full_binary_string[i] == '=') {
            index_of_first_equals = i;
            break;
        }
    }

    int final_length = index_of_first_equals/6;
    // access each 6 bit group
    char* six_bit_group = malloc(6);
    int value_of_six_bit_group = 0;
    char* final_output = malloc(index_of_first_equals/6 + 2);
    for (int i = 0; i < final_length; i++) {
        // get 6 bit group
        for (int j = 0; j < 6; j++) {
            six_bit_group[j] = full_binary_string[i * 6 + j];
        }
        // show 6 bit group
        // printf("6 bit group: %s\n", six_bit_group);
        // get value of 6 bit group in decimal
        value_of_six_bit_group = 0;
        for (int j = 5; j >= 0; j--) {
            if (six_bit_group[j] == '1') {
                value_of_six_bit_group += power(2, 5 - j);
            }
        }
        // show value of 6 bit group
        // printf("value of 6 bit group: %d\n", value_of_six_bit_group);
        // set char to add to item at index in alphabet
        char char_to_add = alphabet[value_of_six_bit_group];
        // show char to add
        // printf("char to add: %c\n", char_to_add);
        // add char to final output
        final_output[i] = char_to_add;
    }
    // print final output
    // printf("final output: %s\n", final_output);
    int i = 0;
    while (i < final_length) {
        // wrap for every 76 characters
        if (i % 76 == 0 && i != 0) {
            printf("\n");
        }
        printf("%c", final_output[i]);
        i++;
    }
    // TODO: fix leftover bits
    // if there are leftover bits
    if (index_of_first_equals % 6 != 0) {
        // clear 6 bit group
        for (int i = 0; i < 6; i++) {
            six_bit_group[i] = '0';
        }
        // get number of leftover bits
        int leftover_bits = index_of_first_equals % 6;
        // get index of first leftover bit
        int index_of_first_leftover_bit = index_of_first_equals - leftover_bits;
        // add leftover bits to 6 bit group
        for (int i = 0; i < leftover_bits; i++) {
            six_bit_group[i] = full_binary_string[index_of_first_leftover_bit + i];
        }
        // get character from 6 bit group
        value_of_six_bit_group = 0;
        for (int j = 5; j >= 0; j--) {
            if (six_bit_group[j] == '1') {
                value_of_six_bit_group += power(2, 5 - j);
            }
        }
        printf("%c", alphabet[value_of_six_bit_group]);
        i++;
    }
    // char* leftover_bits[6];
    // update leftover bits
    // for (int i = 0; i < 6; i++) {
    //     leftover_bits[i] = full_binary_string[index_of_first_equals + i];
    // }
    // print leftover bits
    // printf("leftover bits: %s\n", leftover_bits);

    // while i is not divisible by 8, add "="
    while (i % 4 != 0) {
        printf("=");
        i++;
    }
    return 0;
}


