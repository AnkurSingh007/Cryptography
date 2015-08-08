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

static const int EXPENSION_PERMUTATION[48] = {
     32, 1, 2, 3, 4, 5 ,
     4, 5, 6, 7, 8, 9 ,
     8, 9, 10, 11, 12, 13 ,
     12, 13, 14, 15, 16, 17 ,
     16, 17, 18, 19, 20, 21 ,
     20, 21, 22, 23, 24, 25 ,
     24, 25, 26, 27, 28, 29 ,
     28, 29, 30, 31, 32, 1
};

static const int PERMUTATION[32] = {
     16, 7, 20, 21 ,
     29, 12, 28, 17,
    1, 15, 23, 26 ,
     5, 18, 31, 10 ,
     2, 8, 24, 14 ,
     32, 27, 3, 9 ,
     19, 13, 30, 6 ,
     22, 11, 4, 25
};

static  int SBOX1[4][16] = {
    { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 },
    { 0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8 },
    { 4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0 },
    { 15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 }
};
static  int SBOX2[4][16] = {
    { 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10 },
    { 3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5 },
    { 0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15 },
    { 13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 }
};
static  int SBOX3[4][16] = {
    { 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8 },
    { 13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1 },
    { 13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7 },
    { 1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 }
};
static  int SBOX4[4][16] = {
    { 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15 },
    { 13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9 },
    { 10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4 },
    { 3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 }
};

static  int SBOX5[4][16] = {
    { 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9 },
    { 14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6 },
    { 4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14 },
    { 11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 }
};

static  int SBOX6[4][16] = {
    { 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11 },
    { 10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8 },
    { 9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6 },
    { 4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 }
};

static int SBOX7[4][16] = {
    { 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1 },
    { 13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6 },
    { 1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2 },
    { 6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 }
};

static  int SBOX8[4][16] = {
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
void get_message(unsigned char* message, int size);
unit generate_key();
token rotate_key_left(token t, int shift);

token fiestal(token message,  unit key);
uint64_t sbox(uint64_t input, int  box[][16]);

int main()
{
    unit* unit_arr;
    unit key;
    int size = 0;
    int i = 0;
    int j = 0;
	   unit * round_keys;
    tokenize_message(&unit_arr, &size);
    key = generate_key();
	  round_keys = generate_roundkeys(key);
    final_permutation(unit_arr, size);
    print_message(unit_arr, size);
    for(i = 0; i < size; i++){
        uint32_t * leftptr = (uint32_t *)&(unit_arr[i].a);
        uint32_t * rightptr = (uint32_t *)&(unit_arr[i].b);
        uint32_t * tempptr;
        uint32_t * holder;
        token temp;
        for(j = 15; j >= 0; j--){
              temp = fiestal(*(token *)rightptr, round_keys[j]);
              holder = (uint32_t * )&temp;
              *leftptr ^= *holder;
              //exchanging pointer
              tempptr = leftptr;
              leftptr = rightptr;
              rightptr= tempptr;
        }
        unit_arr[i].a = *(token *)leftptr;
        unit_arr[i].b = *(token *)rightptr;
    }
    initial_permutation(unit_arr, size);
    print_message(unit_arr, size);


    return 0;
}

void tokenize_message(unit** unit_arr, int* size)
{
    unsigned char message[LIMIT];
    *size = 0;
    scanf("%d",size);
    get_message(message, *size);
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
    for (i = 0; i < size; i++) {
        unsigned char* bits = (unsigned char*)malloc(sizeof(unsigned char) * 64);
        unsigned char* result = (unsigned char*)malloc(sizeof(unsigned char) * 64);
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

void get_message(unsigned char* message, int size)
{
  int i = 0;
  for(i = 0; i < size * 8; i++){
    message[i] = getchar();
  }
}


unit*  generate_roundkeys(unit key)
{
	int i = 0;
	int j = 0;
	unit * key_arr = (unit *)malloc(sizeof(unit) * 16);
	unit k = key;
	 uint64_t * tracekey = ( uint64_t *)&k;
	token t1, t2;
	uint32_t * trace = (uint32_t *)&t1;
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
  trace = (uint32_t *)&t1;
	//generating second token D0
  trace = (uint32_t *)&t2;
	for(i = 32; i < 64; i++){
		if(i < 60){
			if(*tracekey & (1L << (64 - PERMUTATION_CHOICE1[i - 4])))*trace |= (1L << (63 - i));
			else *trace &= ~(1L << (63 - i));
		}
		else{
			*trace &= ~(1L << (63 - i));
		}
	}
  trace = (uint32_t *)&t2;
	//tested till here successfully
	//working on producing round keys
	for(i = 0; i <= 16 ; i++){
			if(i == 0){
				t1 = rotate_key_left(t1, LEFT_SHIFTS[0]);
        trace = (uint32_t *)&t1;
				t2 = rotate_key_left(t2, LEFT_SHIFTS[0]);
        trace = (uint32_t *)&t2;
			}
			else{
					t1 = rotate_key_left(t1, LEFT_SHIFTS[i - 1]);
					trace = (uint32_t *)&t1;
          trace = (uint32_t *)&t1;
					t2 = rotate_key_left(t2, LEFT_SHIFTS[i - 1]);
					uint64_t * pointer;
          unit result = {0};
          pointer = (uint64_t *)&result;
          char arr[56];
          uint32_t * tokenpointer = (uint32_t *)&t1;
          for(j = 0; j < 28; j++){
            if(*tokenpointer & (1<<(31 - j)))arr[j] = 1;
            else arr[j]= 0;
          }
          tokenpointer = (uint32_t *)&t2;
          for(j = 28; j < 56; j++){
            if(*tokenpointer & (1<<(59 - j)))arr[j] = 1;
            else arr[j]= 0;
          }

					//temp now contains 56 bits
					//permutating
          char arr2[j];
					for(j = 0; j < 48; j++){
							if(arr[PERMUTATION_CHOICE2[j] -  1])
								arr2[j] = 1;
							else
								arr2[j] = 0;;
						}
            unsigned long long * ptr;
           ptr = (unsigned long long *)&result;
           for(j = 0; j < 48; j++){
               if(arr2[j]){
                 *ptr |= (1ull <<(63 - j));
              }
               else
                 *ptr &= ~(1ull <<(63 - j));
            }
            for(j = 48; j < 64; j++){
              *ptr &= ~(1ull <<(63 - j));
            }
  				   key_arr[i - 1] = result;
			}
	}
	return key_arr;
}



token rotate_key_left(token t, int shift){
	token here = t;
	uint32_t * trace = (uint32_t * )&here;
	unsigned int * shifts = (unsigned int * )malloc(sizeof(unsigned char) * shift);
	int i = 0;
	int j = 0;
	for(i  = 0; i < shift; i++){
		if(*trace & (1<<(31 - i)))shifts[i] = 1;
		else shifts[i] = 0;
	}
	*trace <<= shift;
	for(i = 0; i < shift; i++){
		if(shifts[i])*trace |= (1<<(3 + shift - i));
		else *trace &= ~(1<<(3 + shift - i));
	}
	free(shifts);
	return here;
}

token fiestal(token message,  unit key){
      //key_arr unit has 48 significant bits to use
      //expanding token
      int i = 0;
      unit temp ;
      token expended;
      expended = message;
      uint32_t * pointer1 = (uint32_t *)&expended;
      uint64_t * pointer2 = (uint64_t *)&key;
      uint64_t tempint;
      char arr[48];
      uint32_t uiholder = 1;
      uint64_t ullholder = 1;
      for(i = 0; i < 48; i++){
          if(*pointer1 & (uiholder << (32 - EXPENSION_PERMUTATION[i])))
            tempint |= (ullholder<<(63 - i));
          else
             tempint &= ~(ullholder<<(63 - i));
      }
      tempint ^= *pointer2;
      uint64_t final = 0;
      uint64_t holder;
      holder = sbox(tempint, SBOX1);
      holder >>=60;
      final |= holder;
      final <<= 4;
      holder = sbox(tempint, SBOX2);
      holder >>= 60;
      final |= holder;
      final |= holder;
      final<<=4;
      holder = sbox(tempint, SBOX3);
      holder >>= 60;
      final |= holder;
      final<<=4;
      holder = sbox(tempint, SBOX4);
      holder >>= 60;
      final |= holder;
      final<<=4;
      holder = sbox(tempint, SBOX5);
      holder >>= 60;
      final |= holder;
      final<<=4;
      holder = sbox(tempint, SBOX6);
      holder >>= 60;
      final |= holder;
      final<<=4;
      holder = sbox(tempint, SBOX7);
      holder >>= 60;
      final |= holder;
      final<<=4;
      holder = sbox(tempint, SBOX8);
      holder >>= 60;
      final |= holder;

      token returntoken;
      uint32_t * returnptr = (uint32_t *)&returntoken;
      //permutating final
      for(i = 0; i < 32; i++){
          if(holder & (uiholder << (31 - i)))*returnptr |= (uiholder << (31 - i));
          else *returnptr &= ~(uiholder << (31 - i));
      }
      return returntoken;
}


uint64_t sbox(uint64_t input, int  box[][16]){
    //input has 6 relevent bits
    int i = 0;
    uint64_t inputholder = input;
    uint32_t row = 0;
    uint32_t col = 0;
    uint32_t returnval = 0;
    uint32_t uiholder = 1;
    uint64_t ullhodler = 1;
    //getting row
    if(inputholder & (ullhodler << 63))row |= (uiholder<<1);
    if(inputholder & (ullhodler << 58))row |= (uiholder);
    //getting column
    if(inputholder & (ullhodler << 62))col |= (uiholder<<3);
    if(inputholder & (ullhodler << 61))col |= (uiholder<<2);
    if(inputholder & (ullhodler << 60))col |= (uiholder<<1);
    if(inputholder & (ullhodler << 59))col |= (uiholder);
    //putting 4 bits in return unit
    returnval = *(uint32_t *)&box[row][col];
    for(i = 0; i < 63; i++){
      if(i < 4){
        if(returnval & (uiholder << (3 - i)))inputholder |= (ullhodler << (63 - i));
        else inputholder &= ~(ullhodler << (63 - i));
      }
      else{
        inputholder &= ~(ullhodler << (63 - i));
      }
    }
    return inputholder;
}
