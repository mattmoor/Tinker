
#include <stdio.h>

#pragma once

namespace Boards
{

class Board
{
public:

   Board(char * string)
   {
      int length = strlen(string);
      int i;
      int nextsquare = 0;

      this->m_board[0][nextsquare++] = string;

      for(i=0; i < length; i++)
      {
         if (string[i] == ',')
         {
            // terminate the boardsquare

            string[i] = '\0';

            // make the next board square point to the next character position

            this->m_board[0][nextsquare++] = string + i + 1;
         }
         else
         {
            assert(islower(string[i]));
         }
      }

      if (nextsquare != 16)
      {
         printf("ERROR: board must consist of 16 tiles: \"%s\"\n", string);
         exit(1);
      }
   }

   virtual ~Board()
   {
      // We do no allocations for the Board today
   }

   char* GetSquare(int x, int y)
   {
      assert((x >= 0) && (x < 4));
      assert((y >= 0) && (y < 4));

      return this->m_board[x][y];
   }

   char* GetSquare(int x)
   {
      assert((x >= 0) && (x < 16));

      return this->m_board[0][x];
   }

   bool CanMove(int x, int y, int xdelta, int ydelta)
   {
      assert(xdelta >= -1);
      assert(ydelta >= -1);
      assert(xdelta <= 1);
      assert(ydelta <= 1);
      assert((xdelta != 0) || (ydelta != 0));

      int newx = x + xdelta;
      int newy = y + ydelta;
      
      if ((newx < 0) || (newx >= 4))
      {
         return false;
      }

      if ((newy < 0) || (newy >= 4))
      {
         return false;
      }

      return true;
   }

protected:

   char* m_board[4][4];
};


} // Boards