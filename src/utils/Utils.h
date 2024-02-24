#ifndef DB9C663F_7757_4E5F_9CDD_23E9A09D9515
#define DB9C663F_7757_4E5F_9CDD_23E9A09D9515

#include <stdint.h>

#define BOARD_SIZE 64
#define BOARD_LENGTH 8
#define MAX_LEGAL_MOVES 218

#define u64 uint64_t

// The x coordinates
#define file(square) (square & 7)
// The y coordinates
#define rank(square) (square >> 3)

int trailingZeros_64(const u64 x);
int numBitSet_64(u64 x);

u64 random_u64();

void (*loggingFFI)(char *);

void initializeFFILogging(void (*printCallback)(char *));

#endif /* DB9C663F_7757_4E5F_9CDD_23E9A09D9515 */
