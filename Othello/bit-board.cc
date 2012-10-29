#include "bit-board.h"

// BitRow and BitBoard unit testing

void init1(BitRow* br) {
  *br = 0;
  BitRow& br_ = *br;

  for (uint32 i = 0; i < 8; i++) {
    br_[i] ^= (bool)(i & 1);
  }
}

void init2(BitRow* br) {
  *br = 0;
  BitRow& br_ = *br;

  for (uint32 i = 0; i < 8; i++) {
    br_[i] ^= !(bool)(i & 1);
  }
}

void init1(BitBoard* bb) {
  BitBoard& bb_ = *bb;
  for (uint32 i = 0; i < 8; i++) {
    for (uint32 j = 0; j < 8; j++) {
      bb_[i][j] = ((i+j)&1);
    }
  }
}

void init2(BitBoard* bb) {
  BitBoard& bb_ = *bb;
  for (uint32 i = 0; i < 8; i++) {
    for (uint32 j = 0; j < 8; j++) {
      bb_[i][j] = !((i+j)&1);
    }
  }
}

void test_BitRow() {
  BitRow br1;
  BitRow br2;

  assert(sizeof(BitRow) == 1);

  init1(&br1);
  init2(&br2);

  br1 ^= br2;

  assert(br1 == (byte)-1);

  init1(&br1);
  init2(&br2);

  br1 |= br2;

  assert(br1 == (byte)-1);

  init1(&br1);
  init2(&br2);

  br1 &= br2;

  assert(br1 == 0);

  init1(&br1);
  init1(&br2);

  br1 &= br2;

  assert(br1 == 170);

  init1(&br1);
  init1(&br2);

  br1 |= br2;

  assert(br1 == 170);

  init1(&br1);
  init1(&br2);

  br1 ^= br2;

  assert(br1 == 0);
}

void test_BitBoard_Iterator() {
  // TODO(mattmoor): test iterator once implemented

  assert(!"NYI");
}

void test_BitBoard() {
  BitBoard bb1;
  BitBoard bb2;

  assert(sizeof(BitBoard) == 8);

  init1(&bb1);
  init2(&bb2);

  bb1 ^= bb2;

  assert(bb1 == -1);

  init1(&bb1);
  init2(&bb2);

  bb1 |= bb2;

  assert(bb1 == -1);

  init1(&bb1);
  init2(&bb2);

  bb1 &= bb2;

  assert(bb1 == 0);

  init1(&bb1);
  init1(&bb2);

  bb1 &= bb2;

  assert(bb1 == 6172840429334713770);

  bb1 |= bb2;

  assert(bb1 == 6172840429334713770);

  bb1 ^= bb2;

  assert(bb1 == 0);

  test_BitBoard_Iterator();
}

int main() {
  test_BitRow();
  test_BitBoard();
}
