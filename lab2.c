#include <stdio.h>
#include <limits.h>

typedef struct uint1024_t { unsigned long arr[32]; } uint1024_t;

const short UINT_SIZE = sizeof(unsigned int);
const short MAX_DIGITS = 309;
char pow2_nums[1025][MAX_DIGITS];  //  1's are on index (MAX_DIGITS - 1)
int pow2_total_digits[1025];

// Custom

int get_greatest_bit_idx(uint1024_t x) { 
    // Returns greatest bit 1 index counting from greatest bit and starting from 0
    int n = -1;
    for (int i = 0; i < 32; i++) {       
        if (x.arr[i] == 0) { 
            continue;
        }
        for (int j = 0; j < 32; j++) {          
            if (x.arr[i] >> j & 0x01) { 
                n = j;                
            }
        }
        if (n != -1) { 
            return i * 32 + (31 - n);
        }
    }
    return -1;
}

void print_digits_arr(char digits[MAX_DIGITS]) {     
    // Find max decimal idx
    int i = -1;
    for (int x = 0; x < MAX_DIGITS; x++) {
        if (digits[x] > 0) {
            i = x;
            break;
        }
    }
    if (i == -1) {
        printf("0");
    } else {
        for (; i < MAX_DIGITS; i++)
            printf("%d", (int) digits[i]);
    }    
    printf("\n");
}

void read_pow2_nums() {
    for (int i = 0; i < 1024; i++) {
        for (int j = 0; j < 1024; j++) {
            pow2_nums[i][j] = 0;
        }
    }

    // Read decimal digits of 2^0 to 2^1024 from file
    FILE *input_file;
    input_file = fopen("pow2.txt", "r");
    if (input_file == NULL) {
        printf("Could't open pow2.txt");
        return;
    } 

    for (int i = 1; i < 1025; i++) {        
        char line[310] = {0};           
        fgets(line, 310, input_file);
        int digits = 0;        
        for (; digits < MAX_DIGITS; digits++) {
            if (line[digits] == '\n') 
                break;
        }
        pow2_total_digits[i] = digits;

        for (int j = 0; j < digits; j++) { 
            pow2_nums[i][MAX_DIGITS-digits+j] = line[j] - '0';
        }       
    }    
    fclose(input_file);    
}

unsigned long long ull_pow(unsigned long long base, int exp) {
    unsigned long long result = 1;
    while (exp) {
        if (exp % 2)
           result *= base;
        exp /= 2;
        base *= base;
    }
    return result;
}

unsigned long long ull_from_uint1024_t(uint1024_t x) { 
    // Result for x > ULLONG_MAX is undefined
    short digits[MAX_DIGITS]; 
    for (int i = 0; i <= MAX_DIGITS; i++) 
        digits[i] = 0; 
        
    for (int i = 31; i >= 0; i--) { 
        for (int j = 31; j >= 0; j--) { 
            int bit = x.arr[i] >> (31 - j) & 0x01;
            if (bit) {
                int power = 32 * (31 - i) + (32 - j); 
                for (int q = MAX_DIGITS-1; q >= 0; q--) {    
                    if (pow2_nums[power][q] != 0)                    
                    digits[q] += pow2_nums[power][q];
                    int k = q;
                    while (digits[k] > 9) {
                        digits[k] = digits[k] % 10;
                        digits[k-1] += 1;
                        k--;
                    }
                }
            }
        }        
    }

    unsigned long long res;
    int i = -1;
    for (int j = 0; j < MAX_DIGITS; j++) {
        if (digits[j] != 0) {
            i = j;
            break;
        }
    }        
    if (i != -1) 
        res = digits[MAX_DIGITS - 1];
    else
        return 0;
    for (int j = MAX_DIGITS - 2; j >= i; j--) {
        res += ull_pow(10, (MAX_DIGITS - 1 - j)) * digits[j];
    }
    return res;
}

int shift_left_safe(uint1024_t* x, int shift) {    
    if (shift == 0)
        return 1;
    // Returns 1 when shifted succcessfuly, -1 for overflow
    int greatest_bit_idx = get_greatest_bit_idx(*x);
    if (shift < 0 | shift > greatest_bit_idx)
        return -1;

    unsigned long long buf;
    for (int i = greatest_bit_idx / 32; i < 32; i++) {          
        buf = x->arr[i];
        x->arr[i] = 0;
        buf = buf << shift % 32;        
        x->arr[i - shift / 32] += buf & 0x00000000FFFFFFFF;
        x->arr[i - shift / 32 - 1] += buf >> 32;
    }

    return 1;
}

//  Assignment

uint1024_t from_uint(unsigned int x) { 
    uint1024_t res;    
    for (int i = 0; i < 32; i++) { 
        res.arr[i] = 0;
    }
    res.arr[31] = x;    
    return res;
}

void printf_value(uint1024_t x) {
    if (pow2_nums[1024][0] == 0) { 
        read_pow2_nums();
    }
    char digits[MAX_DIGITS]; 
    for (int i = 0; i <= MAX_DIGITS; i++) 
        digits[i] = 0; 
        
    for (int i = 31; i >= 0; i--) { 
        for (int j = 31; j >= 0; j--) { 
            int bit = x.arr[i] >> (31 - j) & 0x01;
            if (bit) {
                int power = 32 * (31 - i) + (32 - j); 
                for (int q = MAX_DIGITS-1; q >= 0; q--) {                  
                    digits[q] += pow2_nums[power][q];
                    int k = q;
                    while (digits[k] > 9) {                        
                        digits[k] = digits[k] % 10;
                        digits[k-1] += 1;
                        k--;
                    }
                }
            }
        }        
    }

    print_digits_arr(digits);
}

uint1024_t add_op(uint1024_t x, uint1024_t y) { 
    uint1024_t res = from_uint(0);
    unsigned long long sum = 0;    
    for (int i = 31; i >= 1; i--) { 
        sum = (unsigned long long) x.arr[i] + y.arr[i] + res.arr[i];   
        res.arr[i] = sum & 0x00000000FFFFFFFF;
        res.arr[i - 1] = (sum & 0xFFFFFFFF00000000) >> 32;               
    }

    // Sum greatest 32 bits
    sum = (unsigned long long) x.arr[0] + y.arr[0] + res.arr[0];
    res.arr[0] = sum;

    return res;
}

uint1024_t subtr_op(uint1024_t x, uint1024_t y) { 
    uint1024_t res = from_uint(0);
    long long buf = (long long) x.arr[0] - y.arr[0];            
    if (buf < 0) {
        return res;  // x < y 
    }
    res.arr[0] = buf;

    for (int i = 1; i < 32; i++) {    
        buf = (long long) x.arr[i] - y.arr[i];                                             
        if (buf < 0) {            
            int n = i - 1;            
            while (res.arr[n] == 0) 
                n--;                        
            res.arr[n] -= 1;
            n++;            
            for (; n != i; n++) {
                res.arr[n] = ULONG_MAX;               
            }
            res.arr[i] = ULONG_MAX + buf + 1; 
        } 
        else { 
            res.arr[i] = buf;
        }
    }
}

uint1024_t mult_op(uint1024_t x, uint1024_t y) { 
    uint1024_t res = from_uint(0);

    // Find greatest 1 bit in y
    int y_max_bit_idx = get_greatest_bit_idx(y);    // lowest bit has index 1023  
    if (y_max_bit_idx == -1) {
        return res; // y = 0
    }
    
    uint1024_t buf = y;
    for (int i = 31; i >= 0; i--) { 
        for (int j = 0; j < 32; j++) { 
            int bit = x.arr[i] >> j & 0x01; // Iterating x bits right to left            
            if (bit) {                 
                int shift = (1023 - i * 32 - (31 - j));
                if (shift_left_safe(&buf, shift)) {
                    res = add_op(res, buf);
                    buf = y;                    
                } else {
                    printf("shift overflow\n");
                    return res; // Overflow
                }
            }
        }
    }

    return res;
}

void scanf_value(uint1024_t* x) { 
    if (pow2_nums[1024][0] == 0) { 
        read_pow2_nums();
    }

    *x = from_uint(0);   
    char input_buf[MAX_DIGITS+1], buf[MAX_DIGITS];
    for (int i = 0; i < 309; i++) {
        input_buf[i] = -1;
        buf[i] = -1;
    }        
    scanf("%309s", input_buf);        

    int digits = 0;
    if (input_buf[0] - '0' <= 0) {          
        return;
    }    
    // Convert char codes to int's and count total decimal numbers
    for (int i = 0; i < 309; i++) {                               
        input_buf[i] -= '0';
        if (input_buf[i] < 0) {
            digits = i;
            break;
        }
    }
    // Put numbers to the end of buf array        
    for (int i = 0; i < digits; i++) {   
        buf[MAX_DIGITS-digits+i] = input_buf[i];
    }                        

    while (digits != 0) {        
        int left = 1, right = 1024, mid;
        while (left <= right) {
            mid = (left + right) / 2;
            if (digits > pow2_total_digits[mid]) {
                left = mid;
            }
            else if (digits < pow2_total_digits[mid]) {
                right = mid;
            }
            else {
                while (mid <= 1024 & pow2_total_digits[mid] == digits) {
                    mid += 1;
                }
                mid -= 1;
                break;            
            }
        }        

        // Iterate nums to find number <= buf
        int pow2_idx = -1;        
        for (int i = mid;; i--) {
            int good_num = 0;
            for (int j = 309 - digits;; j++) {
                if (j == 309) {
                    good_num = 1;   // buf equal to pow2
                }
                if (buf[j] == pow2_nums[i][j]) {
                    continue;
                }
                if (buf[j] > pow2_nums[i][j]) {
                    good_num = 1;
                    break;
                } 
                else {
                    break;
                }                 
            }
            if (good_num) {
                pow2_idx = i;
                break; 
            }  
        }

        // Add corresponding bit to x
        uint1024_t uint1024_pow2 = from_uint(1);
        if (!shift_left_safe(&uint1024_pow2, pow2_idx - 1)) {
            printf("shift overflow");
            return;
        }
        *x = add_op(*x, uint1024_pow2);        

        // buf -= pow2
        for (int i = 309 - digits; i < 309; i++) {
            buf[i] -= pow2_nums[pow2_idx][i];                
            if (buf[i] < 0) {
                for (int j = i - 1; j >= 0; j--) {                    
                    if (buf[j] != 0) {
                        buf[j] -= 1;
                        for (int k = j + 1; k < i; k++) {
                            buf[k] = 9;
                        }
                        break;
                    }
                }             
                buf[i] += 10;   
            }
        }            

        // Recompute digits
        digits = 0;
        for (int i = 0; i < 309; i++) {                               
            if (buf[i] > 0) {
                digits = 309 - i;
                break;
            }
        }
    }
    
}

// Tests

void test_add_op() { 
    printf("Testing add_op:\n");
    // Testging on numbers <= ULLONG_MAX    
    uint1024_t x = from_uint(0);
    unsigned long long check_sum = 0, ull_x;
    for (int i = 0; i < 100000; i++) { 
        check_sum += 65535;
        uint1024_t y = from_uint(65535);
        x = add_op(x, y);
        ull_x = ull_from_uint1024_t(x);

        if (check_sum != ull_x) { 
            printf("Test add_op: FAILED\n");
            printf("%d: check_sum(%llu) != x(%llu)\n", i, check_sum, ull_x);
            printf_value(x);            
            return;
        }
    }
    printf("\tPassed test on numbers <= ULLONG_MAX.\n");

    // Testging on numbers > ULLONG_MAX
    printf("\tTesting on numbers >= ULLONG_MAX:\n");
    printf("\t\tx = ");
    printf_value(x);
    // x = 6553500000    
    for (int i = 0; i < 100; i++) { 
        x = add_op(x, x);
    }
    printf("\t\tfollowing 2 numbers should be equal:\n");
    printf("\t\t8307548208595701382708644456431616000000\n\t\t");
    printf_value(x); // x should be 6553500000 * 2^100 = 8307548208595701382708644456431616000000
    printf("\n");
}

void test_subtr_op() { 
    printf("Testing subtr_op:\n");
    printf("\tx = 2y\n\n");
    uint1024_t x = from_uint(2), y = from_uint(1);
    for (int i = 0; i < 5; i++) {        
        printf("\tx =     ");
        printf_value(x);
        printf("\ty =     ");
        printf_value(y);
        printf("\tx - y = ");
        printf_value(subtr_op(x, y));
        printf("\n");

        shift_left_safe(&x, 40);        
        shift_left_safe(&y, 40);
    }    
}

void test_mult_op() { 
    printf("Testing mult_op:\n");
    uint1024_t x = from_uint(2), y = from_uint(10);
    for (int i = 0; i < 5; i++) {        
        printf("\tx =     ");
        printf_value(x);
        printf("\ty =     ");
        printf_value(y);
        printf("\tx * y = ");
        printf_value(mult_op(x, y));
        printf("\n");

        shift_left_safe(&x, 30);        
        shift_left_safe(&y, 30);        
    }    
}

void test_printf_and_scanf() { 
    printf("Testing printf_value and scanf_value:\n");
    printf("Press Ctrl + C to exit\n\n");
    uint1024_t x = from_uint(0);
    while (1) {
        printf("\tEnter unsigned number: ");
        scanf_value(&x);
        printf("\tYou entered:           ");
        printf_value(x);
    }
}

int main() {
    read_pow2_nums(); 

    printf("sizeof(uint1024_t) = %d bits\n\n", sizeof(uint1024_t) * 8);
    test_mult_op();
    test_subtr_op();
    test_add_op();
    test_printf_and_scanf();

    return 0;
}
