#include "board.h"

void Board::BuildCellAvailabilityMasks()
{
   // Build up first-order availability based on direct conflicts that exist with cells in the same
   // row, column or subsquare.

   foreach_row(r, this)
   {
      uint ravm = r.AvailableValueMask();

      foreach_column(c, this)
      {
         uint rn = r.GetDimension();
         uint cn = c.GetDimension();

         if (!r.HasValue(cn))
         {
            uint      cavm = c.AvailableValueMask(); // TODO: consider caching this to avoid 9x redundancy
            SubSquare ss = this->GetSubsquare(rn / SUBSQUARE_SIZE, cn / SUBSQUARE_SIZE); // TODO: same...  cache...
            uint      ssavm = ss.AvailableValueMask();

            uint avm = 0x1ff;

            avm &= ravm;    // mask out taken values in this row
            avm &= cavm;    // mask out taken values in this column
            avm &= ssavm;   // mask out taken values in this subsquare

            this->SetMask(rn, cn, avm);
         }
      }
      next_column;
   }
   next_row;

   // Build up second-order availability (deduced from availability information).
   // This leverages the multi-cell overlap between subsquares and rows/columns
   // to translate limited availability in one to restrict options in the other.
   //       ----------------------------
   //       | 3 a b |         |         |
   // For example, in the above, let's consider that cells "a,b" must hold "2,8" 
   // for their subsquare.  This means that none of the other cells in the row may 
   // contain "2,8".  There is a duality here with columns.  The inverse property
   // is also true for inferring limitations on subsquare cells from limited row/column
   // availability.

   bool changed = true;

   while (changed)
   {
      changed = false;

      foreach_subsquare(ss, this)
      {
         foreach_in_subrange(idx)
         {
            Board::LinearRange xrow = ss.GetCrossRow(idx);
            Board::LinearRange xcolumn = ss.GetCrossColumn(idx);
         
            uint roeavm = ss.OverlapExclusiveAvailability(xrow);
         
            changed |= ss.RemoveExclusiveAvailabilities(xrow, roeavm);

            uint coeavm = ss.OverlapExclusiveAvailability(xcolumn);
         
            changed |= ss.RemoveExclusiveAvailabilities(xcolumn, coeavm);
         }
         next_in_subrange;
      }
      next_subsquare;
   }
}

bool Board::SolveTrivialOneValue()
{
   // Solve the trivial case where there is only *one value* that can exist in a given cell based on 
   // values already occuring in the containing row, column, and subsquare.

   bool changed = false;

   foreach_row(r, this)
   {
      foreach_in_range(idx)
      {
         if (!r.HasValue(idx))
         {
            uint avm = r.GetMask(idx);

            if (ispow2(avm)) // if a single value remains, it must be the value of this cell
            {
               uint value=1;
               for(;avm != 1; avm>>=1, value++);

               r.SetValue(idx, value);
               changed = true;
            }
         }
      }
      next_in_range;
   }
   next_row;

   return changed;
}

bool Board::Range::SolveTrivialOnePlace()
{
   bool changed = false;
   uint avm = this->AvailableValueMask();

   for(uint value=1; avm != 0; avm >>= 1, value++)
   {
      if ((avm & 1) == 0) continue; // this value is taken by a cell.

      uint cell_mask = 0x1ff;

      foreach_in_range(idx)
      {
         if (this->HasValue(idx))
         {
            cell_mask ^= (1 << idx);
         }
         else
         {
            uint mask = this->GetMask(idx);

            assert(mask != 0);

            if ((mask & (1 << (value - 1))) == 0)
            {
               // This value isn't available at this index, so remove the index 
               // from the set of possible placements

               cell_mask ^= (1 << idx);
            }
         }
      }
      next_in_range;

      if (ispow2(cell_mask))
      {
         // There is only one place the value can go
         
         uint idx=0;
         for(;cell_mask != 1; cell_mask>>=1, idx++);

         this->SetValue(idx, value);

         changed = true;
      }
   }

   return changed;
}

bool Board::SolveTrivialOnePlace()
{
   // Solve the trivial case where there is only *one place* that a given value can go based on the 
   // available values for all of the cells within a row, column, or subsquare.

   bool changed = false;

   foreach_row(r, this)
   {
      changed |= r.SolveTrivialOnePlace();
   }
   next_row;

   foreach_column(c, this)
   {
      changed |= c.SolveTrivialOnePlace();
   }
   next_column;

   foreach_subsquare(ss, this)
   {
      changed |= ss.SolveTrivialOnePlace();
   }
   next_subsquare;

   return changed;
}

static bool found_multiple = false;

bool Board::SolveHelper(uint &passes)
{
   assert(this->Check());

   bool changed = true;

   // Employ basic deduction to fill in cell values and iteratively refine availability sets

   while(changed)
   {
      passes++;

      this->BuildCellAvailabilityMasks();

      if (this->HasImpossibleCell())
      {
         return false;
      }

      changed = this->SolveTrivialOneValue();

      if (!this->Check())
      {
         return false;
      }

      changed |= this->SolveTrivialOnePlace();

      if (!this->Check())
      {
         return false;
      }
   }

   if (this->IsComplete()) return true;

   // We cannot deduce further moves, therefore attempt to make progress by assuming a value and proving it 
   // either converges or reaches an impossible state.

   uint row, column;

   this->ChooseSpeculationCandidate(row, column);

   uint mask = this->GetMask(row, column);
   uint value = 1;

   Board soln;

   for(; mask != 0; mask >>= 1, value++)
   {
      if ((mask & 1) != 0)
      {
         // Copy our board, fill in the speculation value, and attempt to reach a known good state.

         Board b2(*this);

         b2.SetValue(row, column, value);

         assert(b2.Check());

         if (b2.SolveHelper(passes))
         {
            if (found_multiple)
            {
               *this = b2;
               return true;
            }

            if (soln.IsComplete())
            {
               *this = soln;
               found_multiple = true;
               return true;
            }

            soln = b2;
         }
      }
   }

   if (soln.IsComplete())
   {
      *this = soln;

      return true;
   }

   return false;
}

bool Board::Solve()
{
   uint passes = 0;

   found_multiple = false;

   this->SolveHelper(passes);
   
   if (found_multiple)
   {
      std::cout << "Found Multiple Solutions!" << std::endl;
   }

   std::cout << "Solve " << (this->IsComplete() ? "succeeded" : "failed") << " after " << passes << " passes" << std::endl;

   return this->IsComplete();
}