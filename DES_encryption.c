#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#define LIMIT 1000
#define FULL 8
#define HALF 4

typedef struct Token {
    unsigned char a, b, c, d;
} token;
typedef struct Unit {
    token a, b;
} unit;

static const int INITIAL_PERMUTATION[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};

static const int FINAL_PERMUTATION[64] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25
};

static const int EXPENSION_PERMUTATION[8][6] = {
    { 32, 1, 2, 3, 4, 5 },
    { 4, 5, 6, 7, 8, 9 },
    { 8, 9, 10, 11, 12, 13 },
    { 12, 13, 14, 15, 16, 17 },
    { 16, 17, 18, 19, 20, 21 },
    { 20, 21, 22, 23, 24, 25 },
    { 24, 25, 26, 27, 28, 29 },
    { 28, 29, 30, 31, 32, 1 }
};

static const int PERMUTATION[8][4] = {
    { 16, 7, 20, 21 },
    { 29, 12, 28, 17 },
    { 1, 15, 23, 26 },
    { 5, 18, 31, 10 },
    { 2, 8, 24, 14 },
    { 32, 27, 3, 9 },
    { 19, 13, 30, 6 },
    { 22, 11, 4, 25 }
};

static const int SBOX1[4][16] = {
    { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 },
    { 0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8 },
    { 4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0 },
    { 15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 }
};
static const int SBOX2[4][16] = {
    { 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10 },
    { 3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5 },
    { 0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15 },
    { 13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 }
};
static const int SBOX3[4][16] = {
    { 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8 },
    { 13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1 },
    { 13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7 },
    { 1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 }
};
static const int SBOX4[4][16] = {
    { 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15 },
    { 13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9 },
    { 10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4 },
    { 3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 }
};

static const int SBOX5[4][16] = {
    { 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9 },
    { 14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6 },
    { 4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14 },
    { 11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 }
};

static const int SBOX6[4][16] = {
    { 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11 },
    { 10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8 },
    { 9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6 },
    { 4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 }
};

static const int SBOX7[4][16] = {
    { 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1 },
    { 13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6 },
    { 1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2 },
    { 6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 }
};

static const int SBOX8[4][16] = {
    { 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7 },
    { 1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2 },
    { 7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8 },
    { 2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 }
};

static const int PERMUTATION_CHOICE1[56] = {
    57, 49, 41, 33, 25, 17, 9,
    1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27,
    19, 11, 3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
    7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29,
    21, 13, 5, 28, 20, 12, 4
};

static const int PERMUTATION_CHOICE2[48] = {
     14, 17, 11, 24, 1, 5 ,
     3, 28, 15, 6, 21, 10 ,
     23, 19, 12, 4, 26, 8 ,
     16, 7, 27, 20, 13, 2 ,
     41, 52, 31, 37, 47, 55 ,
     30, 40, 51, 45, 33, 48 ,
     44, 49, 39, 56, 34, 53 ,
     46, 42, 50, 36, 29, 32 
};
static const int LEFT_SHIFTS[16] = { 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };

void tokenize_message(unit** unit_arr, int* size);
void initial_permutation(unit* unit_arr, int size);
void final_permutation(unit* unit_arr, int size);
int padding(unsigned char* message);
unit* generate_roundkeys(unit key);

void getbits(unsigned char* bits, unit unit_ele, int size);
void fillbits(unsigned char* bits, unit* unit_ele);
void setbit(unsigned char* message, unsigned char bit);
unit* get_tokens(unsigned char* message, int size, unit* unit_arr);
unsigned char* permutedarr_initial(unsigned char* bits);
unsigned char* permutedarr_final(unsigned char* bits);
void print_message(unit* unit_arr, int size);
void get_message(unsigned char* message);
unit generate_key();
token rotate_key_left(token t, int shift);


int main()
{
    unit* unit_arr;
    unit key;
    int size = 0;
	unit * round_keys;
    tokenize_message(&unit_arr, &size);
    key = generate_key();
	generate_roundkeys(key);
	//printf("%c%c%c%c%c\n",key.a.a, key.a.b, key.a.c, key.a.d, key.b.a);
    initial_permutation(unit_arr, size);
    print_message(unit_arr, size);
  //  key = permutation_c1(key);
    final_permutation(unit_arr, size);
    print_message(unit_arr, size);

    return 0;
}

void tokenize_message(unit** unit_arr, int* size)
{
    unsigned char message[LIMIT];
    *size = 0;
    get_message(message);
    printf("got message\n");
    *size = padding(message); //paddint the message for 64 bit block
    printf("size of message is %d\n", *size);
    *unit_arr = get_tokens(message, *size, *unit_arr);
}

//This function assumes limit withing 1000
int padding(unsigned char* message)
{
    int count = 0;
    while (*message) {
        message++;
        count++;
    }
    if (count % 8 == 0)
        return count; //no padding required
    else {
        while (count % 8 != 0) {
            *message = '*'; //used for marker for dummy
            message++;
            count++;
        }
        *message = '\0';
    }
    return count / 8 + (count % 8 == 0 ? 0 : 1);
}

void initial_permutation(unit* unit_arr, int size)
{
    int i;
    int j, k;
    printf("entered here\n");
    for (i = 0; i < size; i++) {
        printf("doing for i = %d\n", i);
        unsigned char* bits = (unsigned char*)malloc(sizeof(unsigned char) * 64);
        unsigned char* result = (unsigned char*)malloc(sizeof(unsigned char) * 64);
        printf("memory allocated\n");
        getbits(bits, unit_arr[i], FULL);
        result = permutedarr_initial(bits);
        fillbits(result, &unit_arr[i]);

	//fprintf(stderr, "%d\n", __LINE__);
        free(bits);
        free(result);
		
	//fprintf(stderr, "%d\n", __LINE__);
    }
}

void final_permutation(unit* unit_arr, int size)
{
    int i, j, k;
    for (i = 0; i < size; i++) {
        unsigned char* bits = (unsigned char*)malloc(sizeof(unsigned char) * 64);
        unsigned char* result;// = (unsigned char*)malloc(sizeof(unsigned char) * 64);
        //Filling bits with bits of the structure to pass into swap function
        getbits(bits, unit_arr[i], FULL);
        result = permutedarr_final(bits);
        //printf("after permutation bits are\n");
        fillbits(result, &unit_arr[i]);

	//fprintf(stderr, "%d\n", __LINE__);
        free(bits);
        //free(result);

	//fprintf(stderr, "%d\n", __LINE__);
    }
}



//Helper Functions

unsigned char* permutedarr_initial(unsigned char* bits)
{
    int i;
    unsigned char* result = (unsigned char*)malloc(sizeof(unsigned char*) * 64);
    for (i = 0; i < 64; i++)
        result[i] = bits[INITIAL_PERMUTATION[i] - 1];
    return result;
}

unsigned char* permutedarr_final(unsigned char* bits)
{
    int i;
    unsigned char* result = (unsigned char*)malloc(sizeof(unsigned char*) * 64);
    for (i = 0; i < 64; i++)
        result[i] = bits[FINAL_PERMUTATION[i] - 1];
    return result;
}

//get bit at mentionged positition
void getbits(unsigned char* bits, unit unit_ele, int size)
{
    int i, j;
	unit tmp = unit_ele;
    unsigned char* temp = (unsigned char*) & tmp;
    for (i = 0; i < size; i++) {
        for (j = 0; j < 8; j++) {
            bits[i * 8 + j] = (*temp & (1 << (7 - j))) > 0 ? 1 : 0;
        }
        temp++;
    }
}



unit* get_tokens(unsigned char* message, int size, unit* unit_arr)
{
    unit_arr = (unit*)malloc(sizeof(unit) * size);
    int i = 0;
    for (i = 0; i < size; i++) {
        unit_arr[i].a.a = *message;
        message++;
        unit_arr[i].a.b = *message;
        message++;
        unit_arr[i].a.c = *message;
        message++;
        unit_arr[i].a.d = *message;
        message++;

        unit_arr[i].b.a = *message;
        message++;
        unit_arr[i].b.b = *message;
        message++;
        unit_arr[i].b.c = *message;
        message++;
        unit_arr[i].b.d = *message;
        message++;
    }
    return unit_arr;
}

void fillbits(unsigned char* bits, unit* unit_ele)
{
    int i, j;
    unsigned char* temp = (unsigned char*)unit_ele;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            if (bits[i * 8 + j])
                *temp |= 1 << (7 - j);
            else
                *temp &= ~(1 << (7 - j));
        }
        temp++;
    }
}



void print_message(unit* unit_arr, int size)
{
    int i, j;
    i = j = 0;
    unsigned char* temp = (unsigned char*)unit_arr;
    for (i = 0; i < size; i++) {
        for (j = 0; j < 8; j++) {
            if (*temp != '*')
                printf("%c", *temp);
            temp++;
        }
    }
    printf("\n");
}

//generate unit as 2 structures for key c0 and d0
unit generate_key()
{
    int i = 0;
    char c;
    unit temp ;
    unsigned char* key = (unsigned char*)malloc(sizeof(unsigned char) * 8);
    unsigned char* tempchar = (unsigned char*)&temp;
    printf("Enter the 64 bit key\n");
    for (i = 0; i < 8; i++) {
        key[i] = getchar();
    }
    for (i = 0; i < 8; i++) {
        *tempchar = key[i];
        tempchar++;
    }

	//fprintf(stderr, "%d\n", __LINE__);
    free(key);
	//fprintf(stderr, "%d\n", __LINE__);
    return temp;
}

void get_message(unsigned char* message)
{
    unsigned char dummychar;
    printf("Enter the message.\nMaximum 1000 unsigned characters allowed\n");
    scanf("%[^\n]s", message);
    dummychar = getchar(); //to cut out the newline key pressed at the end
}


unit*  generate_roundkeys(unit key)
{
	int j = 0;
	unit * key_arr = (unit *)	malloc(sizeof(unit) * 16);
	unit k = key;
	int64_t * tracekey = (int64_t *)&k;
	printf("working on roundkey function\n");
	int i = 0;
	token t1, t2;
	int32_t * trace = (int32_t *)&t1;
	//generating first token C0
	for(i = 0; i < 32; i++){			
		if(i < 28){
			if(*tracekey & (1L << (64 - PERMUTATION_CHOICE1[i])))
				*trace |= (1 << (31 - i));
			else 
				*trace &= ~(1 << (31 - i));
		}
		else{
			*trace &= ~(1 << (31 - i));
		}
	}
	
	//generating second token D0
	trace = (int32_t *)&t2;
	for(i = 32; i < 64; i++){
		if(i < 60){
			if(*tracekey & (1L << (64 - PERMUTATION_CHOICE1[i - 4])))*trace |= (1L << (64 - i));
			else *trace &= ~(1L << (64 - i));
		}
		else{
			*trace &= ~(1L << (64 - i));
		}
	}
	
//tested till here successfully
	//working on producing round keys
	for(i = 0; i <= 16 ; i++){
			if(i == 0){
				t1 = rotate_key_left(t1, LEFT_SHIFTS[0]);
				trace = (unsigned int *)&t2;
				t2 = rotate_key_left(t2, LEFT_SHIFTS[0]);
			}
			else{
					t1 = rotate_key_left(t1, LEFT_SHIFTS[0]);
					trace = (int32_t *)&t2;
					t2 = rotate_key_left(t2, LEFT_SHIFTS[0]);
					int64_t * pointer;
					unit temp;
					temp.a= t1;temp.b = t2;
					pointer = (unsigned long *)&temp;
					int32_t * tokenpointer = (int32_t *)&t1;
					for(j = 0; j < 28; j++){
						if(*tokenpointer & (1 <<(31 - j)))*pointer |= (1L << (63 - j));
						else *pointer &= ~(1 << (63 - j));
					}
					tokenpointer = (unsigned int *)&t2;
					for(j = 28; j < 56; j++){
						if(*tokenpointer & (59 - j))*pointer |= (1L << (63 - j));
						else *pointer &= ~(1 << (63 - j));
					}
					//temp now contains 56 bits
					//permutating
					unit result;
					unsigned long * resultpointer = (unsigned long * )&result;
					for(j = 0; j < 56; j++){
						if(j < 48){
							if(*tokenpointer & (1<<(56 - PERMUTATION_CHOICE2[j])))
								*resultpointer |= (1L << (63 - j));
							else 	
								*resultpointer &= ~(1 << (63 - j));
						}
						else{
							*resultpointer &= ~(1 << (63 - j));
						}
					 }
				key_arr[i - 1] = result;
			}		
	}
	return key_arr;
}

token rotate_key_left(token t, int shift){
	token here = t;
	 int32_t * trace = ( int32_t * )&here;
	printf("%d %d %d %d\n",here.a, here.b, here.c, here.d);
	unsigned int * shifts = (unsigned int * )malloc(sizeof(unsigned char) * shift);
	int i = 0;
	int j = 0;
	for(i = 0; i < 32; i++){
		if(*trace & (1<<(31 - i)))printf("1");
		else printf("0");
	}	
	printf("\n");
	
	for(i  = 0; i < shift; i++){
		if(*trace & (1<<(31 - i)))shifts[i] = 1;
		else shifts[i] = 0;
	}
	for(j = 0; j < shift; j++)
		printf("%d",shifts[j]);
	*trace <<= shift;
	printf("after shift\n");
	printf("%d %d %d %d\n",here.a, here.b, here.c, here.d);
	for(i = 0; i < shift; i++){
		if(shifts[i])*trace |= (1<<(28 - shift + i));
		else *trace &= ~(1<<(28 - shift + i));
	}
	free(shifts);
	return here;
}
