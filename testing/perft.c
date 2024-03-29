#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "../src/magicBitBoard/MagicBitBoard.h"
#include "../src/MoveGenerator.h"
#include "../src/utils/FenString.h"
#include "../src/ChessGameEmulator.h"
#include "LogChessStructs.h"

#define TEST_ITERATION 100

int maximumDepth;
GameState* achievedStates;

bool debug = true;

u64 perft(int depth) {
  if (depth == 0) { return 1; }
  int nbMoveMade = maximumDepth - depth;
  GameState previousState = achievedStates[nbMoveMade];
  GameState previousStates[1] = { 0 };
  
  Move moves[MAX_LEGAL_MOVES + 1] = { [0 ... (MAX_LEGAL_MOVES)] = 0 };
  getValidMoves(moves, previousState, previousStates); // We do not care about draw by repetition
  int nbOfMoves, i;
  u64 nodes = 0;
  nbOfMoves = nbMovesInArray(moves);
  if (nbOfMoves == 1) {
    int flag = flagFromMove(moves[0]);
    if (flag == DRAW || flag == STALEMATE || flag == CHECKMATE) {
      // Game is finished, continuing to next move
      return 0;
    }
  }

  if (!debug && depth == 1) {
    return nbOfMoves;
  }

  for (i = 0; i < nbOfMoves; i++) {

    GameState newState = previousState;
    Move move = moves[i];
    makeMove(move, &newState); // Move is made
    
    if (depth != 1) { // when depth == 1 it would write to invalid indices
      achievedStates[nbMoveMade + 1] = newState;
    }

    u64 moveOutput = perft(depth - 1); // We generate the moves for the next perft
    if (debug && depth == maximumDepth) {
      printMoveToAlgebraic(move);
      printf(": %lu\n", moveOutput);
    }
    nodes += moveOutput;
  }
  
  return nodes;
}

bool isStringValidPerftNumber(char* string) {
  int index = 0;
  char currentChar;
  bool result = true;
  while ((currentChar = *(string + index))) {
    if (currentChar < '0' || currentChar > '9') {
      result = false;
      break;
    }
    index++;
  }
  return result;
}

/* Testing all the double pawn push case

rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 (Normal double pawn push) (ans: 20)
2r4k/8/8/8/3p4/8/2P5/2K5 w - - 0 1 (Pawn is pinned but still can double pawn push) (ans: 6)
7k/8/8/8/3p4/8/r1P1K3/8 w - - 0 1 (Pawn is pinned and cannot double pawn push) (ans: 7)
7k/8/8/4b3/8/8/3P4/K7 w - - 0 1 (King is in check and pawn can double push to get out of check) (ans: 3)

*/

/* Testing all the en-passant case

k6r/8/8/K1Pp4/8/8/8/8 w - d6 0 1 (Normal en-passant)
r7/7k/8/K1pP4/8/8/8/8 w - c6 0 1 (King is in check but en-passant is possible)
r3b3/7k/8/2pP4/1K6/8/8/2rb4 w - c6 0 1 (king is in check and en-passant will remove the check)
k7/b7/8/2Pp4/3K4/8/8/8 w - d6 0 1 (Pawn is pinned but en-passant is possible)
k7/8/8/K1Pp3r/8/8/8/8 w - d6 0 1 (Pawn is en-passant pinned)
8/8/3p4/KPp4r/1R2PpPk/8/8/ b - e3 0 1 (Pawn looks like it is en-passant pinned but it isn't) (ans: 16)
8/8/8/KPpP3r/1R3p1k/8/6P1/ w - c6 1 3 (Two pawns look en-passant pinned but both can do en-passant) (ans: 17)

*/

/**
 * Represents a position to perform a perft test on.
 * The nbTest parameter indicates for how many depths there is a results
 * perftResults returns the expected number of moves for a depth (the indices of said int)
*/
typedef struct testPosition {
  char* fenString;
  int nbTest;
  int* perftResults;
} TestPosition;

// These are the 8 ANSI color types
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

#define NUM_TEST_POSITIONS 6

void test() {
  magicBitBoardInitialize();
  int maxDepth = 5;
  
  int startingPosResults[6] = {1, 20, 400, 8902, 197281, 4865609};
  TestPosition startingPosition = {
    .fenString = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 
    .nbTest = 6, 
    .perftResults = startingPosResults
  };
  
  int pos2Result[5] = {1, 48, 2039, 97862, 4085603};
  TestPosition pos2 = {
    .fenString = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
    .nbTest = 5,
    .perftResults = pos2Result
  };
  
  int pos3Result[6] = {1, 14, 191, 2812, 43238, 674624};
  TestPosition pos3 = {
    .fenString = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
    .nbTest = 6,
    .perftResults = pos3Result
  };

  int pos4Result[5] = {1, 6, 264, 9467, 422333};
  TestPosition pos4 = {
    .fenString = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
    .nbTest = 5,
    .perftResults = pos4Result
  };

  int pos5Result[5] = {1, 44, 1486, 62379, 2103487};
  TestPosition pos5 = {
    .fenString = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
    .nbTest = 5,
    .perftResults = pos5Result
  };

  int pos6Result[5] = {1, 46, 2079, 89890, 3894594};
  TestPosition pos6 = {
    .fenString = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
    .nbTest = 5,
    .perftResults = pos6Result
  };

  TestPosition testPositions[NUM_TEST_POSITIONS] = { 
    startingPosition, 
    pos2,
    pos3,
    pos4,
    pos5,
    pos6, 
  };
  
  u64 perftResult;
  double timeSpent;
  clock_t begin, end;
  achievedStates = malloc(sizeof(GameState) * (maxDepth));
  
  char* testPassed = GRN "✓" RESET;
  char *testFailedPrefix = RED "❌" RESET " Test failed (expected ";
  clock_t fullTestBegin = clock();
  
  for (int i = 0; i < NUM_TEST_POSITIONS; i++) {
    TestPosition testPosition = testPositions[i];

    GameState startingState = { 0 }; 
    setGameStateFromFenString(testPosition.fenString, &startingState);
    achievedStates[0] = startingState;

    printf(RESET "Running test for fen string: %s\n", testPosition.fenString);

    for (int depth = 0; depth < testPosition.nbTest; depth++) {
      maximumDepth = depth;
      begin = clock();
      perftResult = perft(depth);
      end = clock();
      timeSpent = (double)(end - begin) / CLOCKS_PER_SEC;
      
      printf(RESET "Depth: " GRN "%d " RESET "ply  " RESET "Result: " RED "%lu" RESET "  Time: " BLU "%f " RESET "ms ", depth, perftResult, timeSpent * 1000);
      if (perftResult == (u64) testPosition.perftResults[depth]) {
        printf("%s" RESET "\n", testPassed);
      } else {
        printf("%s " RED "%d" RESET ")\n", testFailedPrefix, testPosition.perftResults[depth]);
      }

    }

    printf("\n");
  }

  clock_t fullTestEnd = clock();
  double fullTestTimeSpent = (double)(fullTestEnd - fullTestBegin) / CLOCKS_PER_SEC;
  printf(RESET "The full test took " BLU "%f " RESET "ms" RESET "\n", fullTestTimeSpent * 1000);
  free(achievedStates);
  magicBitBoardTerminate();
}

// To compile and run the program: ./perft
// To check for memory leaks that program: valgrind --leak-check=full --track-origins=yes -s ./perftTesting <args>
int main(int argc, char* argv[]) {
  if (argc == 1) {
    printf("Usage: ./%s <mode (debug, time, test)> [position (fen string)] [depth (positive integer)]\n", argv[0]);
    printf("The `position` and `depth` argument only apply for the debug and time mode\n");
    printf("If `mode` is not provided it will default to debug mode\n");
    printf("If `position` is not provided it will default to the starting position\n");
    printf("`depth` needs to be provided\n");
    exit(EXIT_FAILURE);
  }

  char* fenString = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  maximumDepth = -1;

  { // I am wrapping this code in a scope to not "leak out" the firstArg variable  
    char* firstArg = argv[1];
    if (strcmp(firstArg, "test") == 0) {
      debug = false;
      test();
      return 0;
    }
    if (strcmp(firstArg, "time") == 0) {
      debug = false;
    } else {
      // No parameter is provided, so it is either a fen string of a depth
      if (isStringValidPerftNumber(firstArg)) {
        // No fen string is provided, so the first argument is just the depth
        maximumDepth = atoi(firstArg);
      } else {
        // The first argument is a fen string
        fenString = firstArg;
      }
    }
  }

  if (argc >= 3) {
    char* secondArg = argv[2];
    if (isStringValidPerftNumber(secondArg)) {
      // No fen string is provided, so the first argument is just the depth
      maximumDepth = atoi(secondArg);
    } else {
      // The first argument is a fen string
      fenString = secondArg;
    }
  }

  if (argc >= 4) {
    char* thirdArgument = argv[3];
    // This argument needs to be the depth
    if (!isStringValidPerftNumber(thirdArgument)) {
      printf("The argument %s is not a valid perft number\n", thirdArgument);
      exit(EXIT_FAILURE);
    } else {
      maximumDepth = atoi(thirdArgument);
    }
  }

  if (maximumDepth < 0) {
    printf("You did not provide a valid depth for the mode `%s`\n", debug ? "debug" : "time");
    exit(EXIT_FAILURE);
  }

  magicBitBoardInitialize();
  
  achievedStates = malloc(sizeof(GameState) * maximumDepth);
  
  GameState startingState = { 0 }; 
  
  if (!setGameStateFromFenString(fenString, &startingState)) {
    printf("The fen string \"%s\" is invalid!\n", fenString);
    exit(EXIT_FAILURE);
  }

  achievedStates[0] = startingState;

  u64 perftResult;

  if (debug) {
    printBoard(startingState.board);
    perftResult = perft(maximumDepth);
    printf("Perft depth %d returned a total number of moves of %lu\n", maximumDepth, perftResult);
  } else {
    double averageExecutionTime = 0;
    clock_t begin, end;
    for (int iterations = 0; iterations < TEST_ITERATION; iterations++) {
      begin = clock();
      perftResult = perft(maximumDepth);
      end = clock();
      double timeSpent = (double)(end - begin) / CLOCKS_PER_SEC;
      averageExecutionTime += timeSpent;
      printf("ITERATION #%d, Time: %fs, Perft: %lu\n", iterations, timeSpent, perftResult);
    }
    averageExecutionTime /= TEST_ITERATION;
    printf("Perft depth %d took on average %fms (%fs)\n", maximumDepth, averageExecutionTime * 1000, averageExecutionTime);
  }
  free(achievedStates);
  magicBitBoardTerminate();
  return 0;
}