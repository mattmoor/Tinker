#include "board.h"

int main() {
  Othello::Board b;

  uint32 black, white;

  assertm(!b.Score(&black, &white), "Game Over at start!\n" << b);
  assertm(black == white, "Mismatched scores at start; black: "
	<< black << ", white: " << white);
  assertm(black == 2, "Each side should start with 2 tokens, saw: " << black);

  while (!b.Score(&black, &white)) {
    std::cout << "Black: " << black << ", White: " << white 
	      << std::endl << b;

    uint32 row, col;
    std::cin >> row >> col;

    if ((row < 8) && (col < 8)) {
      if (b.GetValidMoves()[row][col]) {
	b.Move(row, col);
      } else {
	std::cout << "INVALID MOVE!" << std::endl;
      }
    } else {
      // Treat out of bounds as an exit condition...
      break;
    }
  }

  return 0;
}
