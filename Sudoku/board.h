#pragma once

#include "types.h"

#define foreach_in_range(idx) \
   for(uint idx=0; idx < SQUARE_SIZE; idx++) \
   {

#define next_in_range \
   }

#define foreach_in_subrange(idx) \
   for(uint idx=0; idx < SUBSQUARE_SIZE; idx++) \
   {

#define next_in_subrange \
   }

#define foreach_row(r, b) \
   foreach_in_range(__row) \
   { \
      Board::LinearRange r = b->GetRow(__row);

#define next_row \
   }\
   next_in_range

#define foreach_column(c, b) \
   foreach_in_range(__column) \
   { \
      Board::LinearRange c = b->GetColumn(__column);

#define next_column \
   } \
   next_in_range

#define foreach_subsquare(ss, b) \
   foreach_in_subrange(__row) \
   { \
      foreach_in_subrange(__column) \
      { \
         Board::SubSquare ss = b->GetSubsquare(__row, __column);

#define next_subsquare \
      } \
      next_in_subrange; \
   } \
   next_in_subrange

struct Cell
{
   uint m_value : LOG2_SQUARE_SIZE;
   uint m_mask  : SQUARE_SIZE;
};

static const Cell empty_cell = { 0, FULL_MASK };

class Board
{
public:

   Board()
   {
      foreach_row(r, this)
      {
         foreach_in_range(idx)
         {
            r.ClearValue(idx);
         }
         next_in_range;
      }
      next_row;
   }

public:

   // These are some helper types to abstract some useful cross-sections of the board
   // data for reasoning logic and validation

   struct Range
   {
      enum Kind
      {
         ILLEGAL = 0,
         Row = 1,
         Column = 2
      };

      friend class Board;

   public:
      Range(Board * board) : m_board(board)
      {
      }
   
      virtual uint GetValue(uint variant_dimension) = 0;
      virtual uint GetMask(uint variant_dimension) = 0;
      virtual void SetValue(uint variant_dimension, uint value) = 0;
      virtual void ClearValue(uint variant_dimension) = 0;
      virtual bool HasValue(uint variant_dimension) = 0;

      uint AvailableValueMask()
      {
         uint value_mask = FULL_MASK;

         foreach_in_range(idx)
         {
            if (this->HasValue(idx))
            {
               value_mask ^= (1 << (this->GetValue(idx)-1));
            }
         }
         next_in_range;

         assert(value_mask <= FULL_MASK);

         return value_mask;
      }

      bool IsComplete()
      {
         foreach_in_range(idx)
         {
            if (!this->HasValue(idx))
            {
               return false;
            }
         }
         next_in_range;

         return true;
      }

      virtual void Display() = 0;
   
      bool Check()
      {
         uint values = 0;

         foreach_in_range(variant_dimension)
         {
            if (this->HasValue(variant_dimension))
            {
               uint cell = this->GetValue(variant_dimension);

               if (values & (1 << cell))
               {
                  // We've seen this value

                  return false;
               }

               // Mark the value of this cell to validate it doesn't occur twice

               values |= (1 << cell);
            }
         }
         next_in_range;

         return true;
      }

   protected:

      static void DisplayHeaderFooter()
      {
         std::cout << '+';
         foreach_in_subrange(idx)
         {
            std::cout << "--";
         }
         next_in_subrange;
         std::cout << '+';
      }

      bool SolveTrivialOnePlace();

      virtual void SetMask(uint variant_dimension, uint mask) = 0;

   protected:

      Board * m_board;
   };

   struct LinearRange : public Range
   {

   public:
      LinearRange(Board * board, Kind kind, uint fixed_dimension) : Range(board), m_FD(fixed_dimension), m_kind(kind)
      {
      }

      virtual uint GetValue(uint variant_dimension)
      {
         if (this->m_kind == Row)
         {
            return this->m_board->GetValue(this->m_FD, variant_dimension);
         }
         else
         {
            assert(this->m_kind == Column);
            return this->m_board->GetValue(variant_dimension, this->m_FD);
         }
      }
      
      virtual uint GetMask(uint variant_dimension)
      {
         if (this->m_kind == Row)
         {
            return this->m_board->GetMask(this->m_FD, variant_dimension);
         }
         else
         {
            assert(this->m_kind == Column);
            return this->m_board->GetMask(variant_dimension, this->m_FD);
         }
      }
      
      virtual void SetMask(uint variant_dimension, uint mask)
      {
         if (this->m_kind == Row)
         {
            this->m_board->SetMask(this->m_FD, variant_dimension, mask);
         }
         else
         {
            assert(this->m_kind == Column);
            this->m_board->SetMask(variant_dimension, this->m_FD, mask);
         }
      }
      
      virtual void SetValue(uint variant_dimension, uint value)
      {
         if (this->m_kind == Row)
         {
            this->m_board->SetValue(this->m_FD, variant_dimension, value);
         }
         else
         {
            assert(this->m_kind == Column);
            this->m_board->SetValue(variant_dimension, this->m_FD, value);
         }
      }

      virtual void ClearValue(uint variant_dimension)
      {
         if (this->m_kind == Row)
         {
            this->m_board->ClearValue(this->m_FD, variant_dimension);
         }
         else
         {
            assert(this->m_kind == Column);
            this->m_board->ClearValue(variant_dimension, this->m_FD);
         }
      }

      virtual bool HasValue(uint variant_dimension)
      {
         if (this->m_kind == Row)
         {
            return this->m_board->HasValue(this->m_FD, variant_dimension);
         }
         else
         {
            assert(this->m_kind == Column);
            return this->m_board->HasValue(variant_dimension, this->m_FD);
         }
      }

      LinearRange GetPerpendicular(uint variant_dimension)
      {
         if (this->m_kind == Row)
         {
            return this->m_board->GetColumn(variant_dimension);
         }
         else
         {
            assert(this->m_kind == Column);
            return this->m_board->GetRow(variant_dimension);
         }
      }

      virtual void Display()
      {
         if ((this->m_FD % SUBSQUARE_SIZE) == 0)
         {
            std::cout << '\t';
            foreach_in_subrange(idx)
            {
               Range::DisplayHeaderFooter();
            }
            next_in_subrange;
            std::cout << std::endl;
         }

         std::cout << "\t";

         foreach_in_range(variant_dimension)
         {
            if ((variant_dimension % SUBSQUARE_SIZE) == 0)
            {
               std::cout << '|';
            }

            if (this->HasValue(variant_dimension))
            {
               std::cout << (char)('0' + this->GetValue(variant_dimension));
            }
            else
            {
               std::cout << ' ';
            }

            std::cout << ' ';

            if ((variant_dimension % SUBSQUARE_SIZE) == (SUBSQUARE_SIZE-1))
            {
               std::cout << '|';
            }
         }
         next_in_range;
         std::cout << std::endl;

         if ((this->m_FD % SUBSQUARE_SIZE) == (SUBSQUARE_SIZE-1))
         {
            std::cout << '\t';
            foreach_in_subrange(idx)
            {
               Range::DisplayHeaderFooter();
            }
            next_in_subrange;
            std::cout << std::endl;
         }
      }

      uint GetDimension() { return m_FD; }

      Kind GetKind() { return this->m_kind; }

   protected:

      uint m_FD;
      Kind m_kind;
   };

   struct SubSquare : public Range
   {
   public:
      SubSquare(Board * board, uint row, uint column) : Range(board), m_row(SUBSQUARE_SIZE*row), m_column(SUBSQUARE_SIZE*column)
      {
      }

      uint GetValue(uint row, uint column)
      {
         return this->m_board->GetValue(this->m_row + row, this->m_column + column);
      }
      
      uint GetMask(uint row, uint column)
      {
         return this->m_board->GetMask(this->m_row + row, this->m_column + column);
      }

      void SetValue(uint row, uint column, uint value)
      {
         this->m_board->SetValue(this->m_row + row, this->m_column + column, value);
      }

      void ClearValue(uint row, uint column)
      {
         this->m_board->ClearValue(this->m_row + row, this->m_column + column);
      }

      bool HasValue(uint row, uint column)
      {
         return this->m_board->HasValue(this->m_row + row, this->m_column + column);
      }

      virtual uint GetValue(uint variant_dimension)
      {
         return this->GetValue(variant_dimension / SUBSQUARE_SIZE, variant_dimension % SUBSQUARE_SIZE);
      }
      
      virtual uint GetMask(uint variant_dimension)
      {
         return this->GetMask(variant_dimension / SUBSQUARE_SIZE, variant_dimension % SUBSQUARE_SIZE);
      }

      virtual void SetValue(uint variant_dimension, uint value)
      {
         return this->SetValue(variant_dimension / SUBSQUARE_SIZE, variant_dimension % SUBSQUARE_SIZE, value);
      }

      virtual void ClearValue(uint variant_dimension)
      {
         return this->ClearValue(variant_dimension / SUBSQUARE_SIZE, variant_dimension % SUBSQUARE_SIZE);
      }

      virtual bool HasValue(uint variant_dimension)
      {
         return this->HasValue(variant_dimension / SUBSQUARE_SIZE, variant_dimension % SUBSQUARE_SIZE);
      }

      virtual void Display()
      {
         std::cout << '\t';
         Range::DisplayHeaderFooter();
         std::cout << std::endl;

         foreach_in_subrange(row)
         {
            std::cout << "\t|";

            foreach_in_subrange(column)
            {
               if (this->HasValue(row, column))
               {
                  std::cout << ('0' + this->GetValue(row, column)) << " ";
               }
               else
               {
                  std::cout << "  ";
               }
            }
            next_in_subrange;

            std::cout << '|' << std::endl;
         }
         next_in_subrange;
         
         std::cout << '\t';
         Range::DisplayHeaderFooter();
         std::cout << std::endl;
      }

      LinearRange GetCrossRow(uint idx)
      {
         return this->m_board->GetRow(this->m_row + idx);
      }

      LinearRange GetCrossColumn(uint idx)
      {
         return this->m_board->GetColumn(this->m_column + idx);
      }

      uint GetDimension(Range::Kind kind) { return this->GetDimension(kind, 0); }

      bool Contains(LinearRange & lr, uint idx)
      {
         uint ridx;
         uint cidx;

         if (lr.GetKind() == Row)
         {
            ridx = lr.GetDimension();
            cidx = idx;
         }
         else
         {
            assert(lr.GetKind() == Column);

            ridx = idx;
            cidx = lr.GetDimension();
         }

         ridx -= this->m_row;
         cidx -= this->m_column;

         if ((ridx < SUBSQUARE_SIZE) && (cidx < SUBSQUARE_SIZE))
         {
            return true;
         }
         else
         {
            return false;
         }
      }

      uint OverlapExclusiveAvailability(LinearRange & lr)
      {
         bool hasOverlap = false;
         uint xidx = lr.GetDimension();
         
         uint both_avail = 0;
         uint lrx_avail = 0;
         uint ssx_avail = 0;

         foreach_in_range(idx2)
         {
            if (this->Contains(lr, idx2))
            {
               // accumulate the availability of the intersection

               hasOverlap = true;

               both_avail |= lr.GetMask(idx2);
            }
            else
            {
               // accumulate the availability of the remainder of the linear range
               
               lrx_avail |= lr.GetMask(idx2);
            }

            if (xidx != this->GetDimension(lr.GetKind(), idx2))
            {
               // accumulate the availability of the remainder of the subsquare

               ssx_avail |= this->GetMask(idx2);
            }
         }
         next_in_range;

         assert(hasOverlap);

         uint lrx_excl = (both_avail & (~lrx_avail));
         uint ssx_excl = (both_avail & (~ssx_avail));

         return lrx_excl | ssx_excl;
      }

      bool RemoveExclusiveAvailabilities(LinearRange & lr, uint mask)
      {
         bool changed = false;

         if (mask != 0)
         {
            uint xidx = lr.GetDimension();

            foreach_in_range(idx2)
            {
               if (!this->Contains(lr, idx2))
               {
                  uint cur_mask = lr.GetMask(idx2);
                  
                  changed |= ((cur_mask & mask) != 0); // note any availability changes

                  cur_mask &= ~mask; // remove any available values that are exclusive to the overlap

                  lr.SetMask(idx2, cur_mask);
               }

               if (xidx != this->GetDimension(lr.GetKind(), idx2))
               {
                  uint cur_mask = this->GetMask(idx2);
                  
                  changed |= ((cur_mask & mask) != 0); // note any availability changes

                  cur_mask &= ~mask; // remove any available values that are exclusive to the overlap

                  this->SetMask(idx2, cur_mask);
               }
            }
            next_in_range;
         }

         return changed;
      }

   protected:

      void SetMask(uint row, uint column, uint mask)
      {
         this->m_board->SetMask(this->m_row + row, this->m_column + column, mask);
      }

      virtual void SetMask(uint variant_dimension, uint mask)
      {
         return this->SetMask(variant_dimension / SUBSQUARE_SIZE, variant_dimension % SUBSQUARE_SIZE, mask);
      }

      uint GetDimension(Range::Kind kind, uint idx)
      {
         if (kind == Row)
         {
            return this->m_row + (idx / SUBSQUARE_SIZE);
         }
         else
         {
            return this->m_column + (idx % SUBSQUARE_SIZE);
         }
      }

   private:
      uint    m_row;
      uint    m_column;
   };

public:

   uint GetValue(uint row, uint column)
   {
      assert(row < SQUARE_SIZE);
      assert(column < SQUARE_SIZE);

      return this->m_cells[row][column].m_value;
   }

   void SetValue(uint row, uint column, uint value)
   {
      assert(row < SQUARE_SIZE);
      assert(column < SQUARE_SIZE);
      assert(value >= 1);
      assert(value <= SQUARE_SIZE);

      Cell c;

      c.m_value = value;
      c.m_mask = 0;

      this->m_cells[row][column] = c;
   }

   void ClearValue(uint row, uint column)
   {
      assert(row < SQUARE_SIZE);
      assert(column < SQUARE_SIZE);
      
      this->m_cells[row][column] = empty_cell;
   }

   uint HasValue(uint row, uint column)
   {
      return this->GetValue(row, column) != 0;
   }
   
   bool IsComplete()
   {
      foreach_row(r, this)
      {
         if (!r.IsComplete())
         {
            return false;
         }
      }
      next_row;

      return true;
   }

   LinearRange GetRow(uint row)
   {
      LinearRange r(this, Range::Row, row);

      return r;
   }

   LinearRange GetColumn(uint column)
   {
      LinearRange c(this, Range::Column, column);

      return c;
   }
   
   SubSquare GetSubsquare(uint row, uint column)
   {
      assert(row < SUBSQUARE_SIZE);
      assert(column < SUBSQUARE_SIZE);

      SubSquare ss(this, row, column);

      return ss;
   }

   void Display()
   {
      foreach_row(r, this)
      {
         r.Display();
      }
      next_row;

      std::cout << std::endl;
   }

   bool Check()
   {
      foreach_row(r, this)
      {
         if (!r.Check())
         {
            return false;
         }
      }
      next_row;

      foreach_column(c, this)
      {
         if (!c.Check())
         {
            return false;
         }
      }
      next_column;

      foreach_subsquare(ss, this)
      {
         if (!ss.Check())
         {
            return false;
         }
      }
      next_subsquare;

      return true;
   }

   uint GetMask(uint row, uint column)
   {
      assert(row < SQUARE_SIZE);
      assert(column < SQUARE_SIZE);

      return this->m_cells[row][column].m_mask;
   }

public:

   void Parse(char* board_string)
   {
      char** row_ptrs = new char*[SQUARE_SIZE];
      uint   length = strlen(board_string);
      uint   row_count = 0;

      char * prev_start = board_string;

      for(uint idx = 0; idx < length+1; idx++)
      {
         if ((board_string[idx] == '\n') 
            || (board_string[idx] == '\0'))
         {
            board_string[idx] = '\0';
            
            assert(row_count < SQUARE_SIZE);
            row_ptrs[row_count++] = prev_start;
            prev_start = &board_string[idx+1];
         }
      }

      assert(row_count == SQUARE_SIZE);

      uint row_idx = 0;

      foreach_row(r, this)
      {
         char * row_str = row_ptrs[row_idx];
         uint   row_length = strlen(row_str);

         assert(row_length == SQUARE_SIZE);

         for(uint idx = 0; idx < row_length; idx++)
         {
            char c = row_str[idx];
            if ((c == ' ') || (c == '0'))
            {
               r.ClearValue(idx);
            }
            else
            {
               assert(SQUARE_SIZE <= 9);

               if ((c < '0') || (c > '9'))
               {
                  std::cout << "Bad input character: " << c << std::endl;
                  exit(-1);
               }

               r.SetValue(idx, c-'0');
            }
         }

         row_idx++;
      }
      next_row;

      assert(this->Check());

      delete row_ptrs;
   }

   bool Solve();

private:

   bool SolveHelper(uint &passes);

   bool HasImpossibleCell()
   {
      foreach_row(r, this)
      {
         foreach_in_range(idx)
         {
            if (r.HasValue(idx))
            {
            }
            else if (r.GetMask(idx) == 0)
            {
               return true;
            }
         }
         next_in_range;
      }
      next_row;

      return false;
   }

   void ChooseSpeculationCandidate(uint &row, uint &column)
   {
      uint least_options = 10;

      foreach_row(r, this)
      {
         foreach_in_range(idx)
         {
            if (r.HasValue(idx)) continue;

            uint mask = r.GetMask(idx);
            uint options = 0;

            assert(mask != 0);

            for(; mask != 0; mask >>= 1, options += (mask&1));

            if (options < least_options)
            {
               least_options = options;
               row = r.GetDimension();
               column = idx;
            }
         }
         next_in_range;
      }
      next_row;

      assert(least_options != 10);
   }

   void SetMask(uint row, uint column, uint mask)
   {
      this->m_cells[row][column].m_mask = mask;
   }

   void BuildCellAvailabilityMasks();

   bool SolveTrivialOneValue();

   bool SolveTrivialOnePlace();

private:

   Cell m_cells[SQUARE_SIZE][SQUARE_SIZE];
};
