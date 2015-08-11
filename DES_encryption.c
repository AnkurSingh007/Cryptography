#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#define LIMIT 1000
#define FULL 8
#define HALF 4
typedef unsigned long long  ull;

const ull longlong1 = 1;
static const ull INITIAL_PERMUTATION[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};

static const ull FINAL_PERMUTATION[64] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25
};

static const ull EXPENSION_PERMUTATION[48] = {
     32, 1, 2, 3, 4, 5 ,
     4, 5, 6, 7, 8, 9 ,
     8, 9, 10, 11, 12, 13 ,
     12, 13, 14, 15, 16, 17 ,
     16, 17, 18, 19, 20, 21 ,
     20, 21, 22, 23, 24, 25 ,
     24, 25, 26, 27, 28, 29 ,
     28, 29, 30, 31, 32, 1
};

static const ull PERMUTATION[32] = {
     16, 7, 20, 21 ,
     29, 12, 28, 17,
    1, 15, 23, 26 ,
     5, 18, 31, 10 ,
     2, 8, 24, 14 ,
     32, 27, 3, 9 ,
     19, 13, 30, 6 ,
     22, 11, 4, 25
};

static  ull SBOX1[4][16] = {
    { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 },
    { 0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8 },
    { 4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0 },
    { 15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 }
};
static ull SBOX2[4][16] = {
    { 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10 },
    { 3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5 },
    { 0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15 },
    { 13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 }
};
static  ull SBOX3[4][16] = {
    { 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8 },
    { 13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1 },
    { 13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7 },
    { 1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 }
};
static ull SBOX4[4][16] = {
    { 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15 },
    { 13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9 },
    { 10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4 },
    { 3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 }
};

static ull SBOX5[4][16] = {
    { 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9 },
    { 14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6 },
    { 4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14 },
    { 11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 }
};

static ull SBOX6[4][16] = {
    { 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11 },
    { 10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8 },
    { 9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6 },
    { 4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 }
};

static ull SBOX7[4][16] = {
    { 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1 },
    { 13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6 },
    { 1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2 },
    { 6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 }
};

static ull SBOX8[4][16] = {
    { 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7 },
    { 1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2 },
    { 7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8 },
    { 2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 }
};

static  int PERMUTATION_CHOICE1[56] = {
    57, 49, 41, 33, 25, 17, 9,
    1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27,
    19, 11, 3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
    7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29,
    21, 13, 5, 28, 20, 12, 4
};

static int PERMUTATION_CHOICE2[48] = {
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

void initial_permutation(ull * ull_arr, int size);
void final_permutation(ull* ull_arr, int size);
ull * generate_roundkeys(ull key);

ull permutedarr_initial(ull  bits);
ull permutedarr_final(ull  bits);
void print_message(ull * ull_arr, int size);
void get_message(ull ** message1, int * size);
ull generate_key();
ull rotate_key_left(ull  t, int shift);

ull fiestal(ull message,  ull key);
ull sbox(ull input, ull  box[][16]);

int main()
{
    ull * test64;
    ull * checkchar;
    ull * ull_arr;
    ull  key;
    int size = 0;
    int i = 0;
    int j = 0;
    int k = 0;
	  ull * round_keys;
    get_message(&ull_arr, &size);
    for(i = 0; i < size; i++){
      for(j = 0; j < size; j++){
        
      }
    }
    key = generate_key();
    round_keys = generate_roundkeys(key);
    printf("testing generated keys\n");
    for(i = 0; i < size; i++){
        ull  leftint;
        ull  rightint;
        leftint = ull_arr[i];
        leftint >>= 32;
        leftint <<= 32;//took most significant 32 bits
        rightint = ull_arr[i];
        rightint <<= 32;
        rightint >>= 32;
        for(j = 0; j < 16; j++){
              ull temp;
              ull holder  = fiestal(rightint, round_keys[j]);
              leftint ^= holder;
              temp = leftint;
              leftint = rightint;
              rightint = temp;
        }
        ull_arr[i] = 0;
        ull_arr[i] |= rightint;
        ull_arr[i] <<= 32;
        ull_arr[i] |= leftint;
    }
    final_permutation(ull_arr, size);
    printf("%d\n",size);
    return 0;
}



void initial_permutation(ull * ull_arr, int size)
{
    int i;
    int j, k;
    ull result;
    for (i = 0; i < size; i++) {
        ull_arr[i] = permutedarr_initial(ull_arr[i]);
    }
}

void final_permutation(ull *  ull_arr, int size)
{
    int i, j, k;
    ull result;
    for (i = 0; i < size; i++)
        ull_arr[i] = permutedarr_final(ull_arr[i]);
}



//Helper Functions

ull permutedarr_initial(ull  bits)
{
    int i;
    ull temp = bits;
    for (i = 0; i < 64; i++){
        if(temp & (1<<(64 - INITIAL_PERMUTATION[i])))bits |= (1 << (63 - i));
        else bits &= ~(1 << (63 - i));
      }
    return bits;
}

ull  permutedarr_final(ull bits)
{
    int i;
    ull result = 0;
    for (i = 0; i < 64; i++){
        if(bits & (1 << (64 - FINAL_PERMUTATION[i])))result |= (1 << (63 - i));
        else result &= ~(1 << (63 - i));
    }
    return result;
}



//generate ull as 2 structures for key c0 and d0
ull generate_key()
{
    int i = 0;
    char c;
    ull temp = 0;
    ull* key = (ull*)malloc(sizeof(ull) * 8);
    for (i = 0; i < 8; i++) {
        key[i] = getchar();
    }
    c = getchar();
    for (i = 0; i < 8; i++) {
        temp |= key[i];
        temp << 8;
    }

	//fprintf(stderr, "%d\n", __LINE__);
    free(key);
	//fprintf(stderr, "%d\n", __LINE__);
    return temp;
}

void get_message(ull** message1, int * size)
{
    int i = 0;
    int j = 0;
    *size = 0;
    char dummychar;
    int message[1024];
    dummychar = getchar();
    while((dummychar = getchar()) != '\n'){
          if(dummychar - '0')message[(*size)++] = dummychar - '0';
          else message[(*size)++] = dummychar - '1';
    }
    (*size) = ((*size) % 64 ? (*size) / 64 : (*size) / 64 + 1);
    *message1 = (ull *)malloc(sizeof(ull) * (*size));
    for(i = 0; i < (*size) ; i++){
      for(j = 0; j < 63; j++){
        if(message[i * 64 + j])
            (*message1)[i] |= (longlong1 <<(63 - j));
        else
            (*message1)[i] &= (longlong1 << (63 - j));
      }
    }
}


ull*  generate_roundkeys(ull key)
{
	int i = 0;
	int j = 0;
	ull * key_arr = (ull *)malloc(sizeof(ull) * 16);
	ull k = key;
  ull c0 = 0;
  ull d0 = 0;
	//generating first token C0
	for(i = 0; i < 28; i++){
			if( key & (longlong1 << (64 - PERMUTATION_CHOICE1[i])))
				 c0 |= (longlong1 << (27 - i));
			else
			 c0 &= ~(longlong1 << (27 - i));
	}

	//generating second token D0
	for(i = 28; i < 56; i++){
			if(key & (longlong1 << (64 - PERMUTATION_CHOICE1[i]))) d0 |= (longlong1 << (55 - i));
			else d0 &= ~(longlong1 << (55 - i));
	}
	//working on producing round keys
	for(i = 0; i <= 16 ; i++){
			if(i == 0){
				c0 = rotate_key_left(c0, LEFT_SHIFTS[0]);
				d0 = rotate_key_left(d0, LEFT_SHIFTS[0]);
			}
			else{
					c0 = rotate_key_left(c0, LEFT_SHIFTS[i - 1]);
					d0 = rotate_key_left(d0, LEFT_SHIFTS[i - 1]);
      }
      key_arr[i] = 0;
      key_arr[i] |= c0;
      key_arr[i] << 28;
      key_arr[i] |= d0;
  }
	return key_arr;
}



ull rotate_key_left(ull t, int shift){
	ull here = t;
	int i = 0;
	int j = 0;
  unsigned int * shifts = (unsigned int *)malloc(sizeof(unsigned int) * shift);
	for(i  = 0; i < shift; i++){
		if(t & (longlong1 << (31 - i)))shifts[i] = 1;
		else shifts[i] = 0;
	}
  t <<= shift;
	for(i = 1; i <= shift; i++){
		if(shifts[i]) t |= (longlong1 << (shift - i));
		else t &= ~(longlong1 << (shift - i));
	}
	free(shifts);
	return t;
}

ull fiestal(ull message, ull key){
      //key_arr ull has 48 significant bits to use
      //message is 32 bit long
      int i = 0;
      ull shift6 = 6;
      ull temp = 0;
      ull expended;
      ull tempint = 0;
      ull final = 0;
      ull holder;
      expended = message;
      for(i = 0; i < 64; i++){
          if(message & (longlong1 << (32 - EXPENSION_PERMUTATION[i])))
            temp |= (longlong1 << (32 - i));
          else
             temp &= ~(longlong1 <<(32 - i));
      }
      temp ^= key;
      for(i = 5; i >= 0; i++){
        if(temp & (longlong1 << i))tempint |= (longlong1 << i);
        else tempint &= ~(longlong1 << i);
      }

      holder = sbox(temp, SBOX8);
      final |= holder;
      final <<= 4;
      temp >>= 6;
      tempint = 0;
      for(i = 5; i >= 0; i++){
        if(temp & (longlong1 << i))tempint |= (longlong1 << i);
        else tempint &= ~(longlong1 << i);
      }
      holder = sbox(tempint, SBOX7);
      final |= holder;
      final <<= 4;
      temp >>= 6;
      tempint = 0;
      for(i = 5; i >= 0; i++){
        if(temp & (longlong1 << i))tempint |= (longlong1 << i);
        else tempint &= ~(longlong1 << i);
      }
      holder = sbox(tempint, SBOX6);
      final |= holder;
      final <<= 4;
      temp >>= 6;
      tempint = 0;
      for(i = 5; i >= 0; i++){
        if(temp & (longlong1 << i))tempint |= (longlong1 << i);
        else tempint &= ~(longlong1 << i);
      }
      holder = sbox(tempint, SBOX5);
      final |= holder;
      final <<= 4;
      temp >>= 6;
      tempint = 0;
      for(i = 5; i >= 0; i++){
        if(temp & (longlong1 << i))tempint |= (longlong1 << i);
        else tempint &= ~(longlong1 << i);
      }
      holder = sbox(tempint, SBOX4);
      final |= holder;
      final <<= 4;
      temp >>= 6;
      tempint = 0;
      for(i = 5; i >= 0; i++){
        if(temp & (longlong1 << i))tempint |= (longlong1 << i);
        else tempint &= ~(longlong1 << i);
      }
      holder = sbox(tempint, SBOX3);
      final |= holder;
      final <<= 4;
      temp >>= 6;
      tempint = 0;
      for(i = 5; i >= 0; i++){
        if(temp & (longlong1 << i))tempint |= (longlong1 << i);
        else tempint &= ~(longlong1 << i);
      }
      holder = sbox(tempint, SBOX2);
      final |= holder;
      final <<= 4;
      temp >>= 6;
      tempint = 0;
      for(i = 5; i >= 0; i++){
        if(temp & (longlong1 << i))tempint |= (longlong1 << i);
        else tempint &= ~(longlong1 << i);
      }
      holder = sbox(tempint, SBOX1);
      final |= holder;

      return final;
}


ull sbox(ull input, ull  box[][16]){
    //input has 6 relevent bits
    int i = 0;
    ull returnval = 0;
    ull row = 0;
    ull col = 0;
    ull ullhodler = 1;
    //getting row
    if(input & (ullhodler << 63))row |= (longlong1<<1);
    if(input & (ullhodler << 58))row |= (longlong1);
    //getting column
    if(input & (ullhodler << 62))col |= (longlong1<<3);
    if(input & (ullhodler << 61))col |= (longlong1<<2);
    if(input & (ullhodler << 60))col |= (longlong1<<1);
    if(input & (ullhodler << 59))col |= (longlong1);
    //putting 4 bits in return ull
    returnval = box[row][col];
    return returnval;
}
