#ifndef _BIT_BOARD_H_
#define _BIT_BOARD_H_

#include "types.h"

class BitRow {
 public:
  // In C++, overloading "foo[n] = x" is typically achieved by returning a reference
  // from "operator[]", however, we cannot do this for sub-byte addressing, so instead
  // we return a delayed access with a pattern for get and set.
  class DelayBit {
   public:
    DelayBit(BitRow* row, uint32 index) : m_row(row), m_index(index) {
    }

    operator bool() const {
      const BitRow& row_ = *this->m_row;
      return row_[this->m_index];
    }

    void operator=(bool b) {
      this->m_row->set_bit(this->m_index, b);
    }

    DelayBit& operator^=(bool b) {
      if (b) {
	this->m_row->toggle_bit(this->m_index);
      }
      return *this;
    }

    DelayBit& operator&=(bool b) {
      if (!b) {
	this->m_row->clear_bit(this->m_index);
      }
      return *this;
    }

    DelayBit& operator|=(bool b) {
      if (b) {
	this->m_row->set_bit(this->m_index);
      }
      return *this;
    }

  private:
    BitRow * m_row;
    uint32   m_index;
  };

  DelayBit operator[](uint32 index) {
    return DelayBit(this, index);
  }

  bool operator[](uint32 index) const {
    return m_row & (1 << index);
  }

  void set_bit(uint32 index) {
    assert(index < 8);
    m_row |= (1 << index);
  }

  void clear_bit(uint32 index) {
    assert(index < 8);
    m_row &= ~(1 << index);
  }

  void toggle_bit(uint32 index) {
    assert(index < 8);
    m_row ^= (1 << index);
  }

  void set_bit(uint32 index, bool value) {
    if (value) {
      return this->set_bit(index);
    } else {
      return this->clear_bit(index);
    }
  }

  BitRow& operator^=(const BitRow & other) {
    *this = *this ^ other;
    return *this;
  }

  BitRow& operator&=(const BitRow & other) {
    *this = *this & other;
    return *this;
  }

  BitRow& operator|=(const BitRow & other) {
    *this = *this | other;
    return *this;
  }

  BitRow& operator-=(const BitRow & other) {
    // Force the bits on
    *this = *this | other;
    // Then toggle them off
    *this = *this ^ other;
    return *this;
  }

  void operator=(byte value) {
    this->m_row = value;
  }

  operator byte() const {
    return this->m_row;
  }

 private:
  byte m_row;

  // TODO(mattmoor): make this work with union...
  //DISABLE_COPY(BitRow)
};

union BOARD_T {
  BitRow rows[8];
  long bits;
};

class BitBoard {
 public:
  template <bool VALUE>
  class bit_iterator {
  public:
  bit_iterator(const BitBoard* board)
    : m_board(board) {
      this->m_coords.first = 0;
      this->m_coords.second = -1;
      // Find the first set bit
      this->MoveNext();
    }

    const std::pair<uint32, uint32>& operator*() const {
      return this->m_coords;
    }

    const std::pair<uint32, uint32>* operator->() const {
      return &this->m_coords;
    }

    bit_iterator& operator++() {
      this->MoveNext();
      return *this;
    }
    
    bool operator!=(const bit_iterator& rhs) const {
      const bit_iterator& lhs = *this;
      if (lhs.m_board != rhs.m_board) {
	return true;
      }
      if (lhs.m_coords.first != rhs.m_coords.first) {
	return true;
      }
      return lhs.m_coords.second != rhs.m_coords.second;
    }
    
    static bit_iterator End(const BitBoard* board) {
      bit_iterator iter(board);
      iter.m_coords.first = 8;
      iter.m_coords.second = 8;
      return iter;
    }

  private:
    void MoveNext() {
      const BitBoard& board = *(this->m_board);
      uint32 row = this->m_coords.first;
      // Finish this row
      for (uint32 col = this->m_coords.second+1; col < 8; ++col) {
	if (board[row][col] == VALUE) {
	  this->m_coords.first = row;
	  this->m_coords.second = col;
	  return;
	}
      }
      // For all other rows, start at the beginning.
      for (++row; row < 8; ++row) {
	for (uint32 col = 0; col < 8; ++col) {
	  if (board[row][col] == VALUE) {
	    this->m_coords.first = row;
	    this->m_coords.second = col;
	    return;
	  }
	}
      }
      // We didn't find a square, put ourselves in an END state.
      this->m_coords.first = 8;
      this->m_coords.second = 8;
    }

    std::pair<uint32, uint32> m_coords;
    const BitBoard* m_board;
  };

  template<bool VALUE>
  class bit_bucket {
   public:
    bit_bucket(const BitBoard* b)
    : m_board(b) {
    }
    
    bit_iterator<VALUE> begin() const {
      return bit_iterator<VALUE>(this->m_board);
    }
    
    bit_iterator<VALUE> end() const {
      return bit_iterator<VALUE>::End(this->m_board);
    }
    
  private:
    const BitBoard* m_board;
  };

 public:
  BitBoard() {
    *this = 0;
  }

  BitRow& operator[](uint32 index) {
    assert(index < 8);
    return m_board.rows[index];
  }

  const BitRow& operator[](uint32 index) const {
    assert(index < 8);
    return m_board.rows[index];
  }

  BitBoard& operator^=(const BitBoard & other) {
    *this = *this ^ other;
    return *this;
  }

  BitBoard& operator&=(const BitBoard & other) {
    *this = *this & other;
    return *this;
  }

  BitBoard& operator|=(const BitBoard & other) {
    *this = *this | other;
    return *this;
  }

  BitBoard& operator-=(const BitBoard & other) {
    // Force the bits on
    *this = *this | other;
    // Then toggle them off
    *this = *this ^ other;
    return *this;
  }

  void operator=(long value) {
    this->m_board.bits = value;
  }

  // TODO(mattmoor): uint64
  operator long() const {
    return this->m_board.bits;
  }

  bit_bucket<true> SetBits() const {
    return bit_bucket<true>(this);
  }

  bit_bucket<false> ClearBits() const {
    return bit_bucket<false>(this);
  }

 private:
  BOARD_T m_board;
  DISABLE_COPY(BitBoard);
};

inline std::ostream& operator<<(std::ostream& out, const BitRow& br) {
  for (uint32 i = 0; i < 8; i++) {
    out << (br[i] ? '1' : '0');
  }
  out << std::endl;
  return out;
}

inline std::ostream& operator<<(std::ostream& out, const BitBoard& bb) {
  for (uint32 i = 0; i < 8; i++) {
    out << bb[i];
  }
  out << std::endl;
  return out;
}

#endif  // _BIT_BOARD_H_
