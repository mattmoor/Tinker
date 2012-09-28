
#include "trie.h"
#include "board.h"

#pragma once

static int g_char_values[] = {
   2, //a
   6, //b
   5, //c
   2, //d
   1, //e
   6, //f
   3, //g
   2, //h
   2, //i
   8, //j
   5, //k
   3, //l
   3, //m
   2, //n
   2, //o
   4, //p
   9, //q
   2, //r
   2, //s
   1, //t
   4, //u
   5, //v
   3, //w
   8, //x
   4, //y
   15 // z
};
//ll
//qu
//ti 7
//nd
//se 6
//ss 6
//ty


class Path
{
public:
   Path(int x, int y, Tries::Trie * dictionary, Boards::Board * board)
   {
      this->Initialize(x, y, dictionary, board);
   }

   virtual ~Path()
   {
      // We do no allocations for the Path today
   }

   bool IsWord() const
   {
      if (this->m_isDud) return false;

      assert(this->m_currentWord != NULL);

      return this->m_currentWord->IsTerminal();
   }

   char* ToString() const
   {
      return this->m_currentWord->ToString();
   }

   int value() const
   {
      const Tries::Trie * iter = this->m_currentWord;
      int value = 0;
      int length = 0;

      while(true)
      {
         char* str = iter->Element();
         int len = strlen(str);

         if (len == 0)
         {
            switch(length)
            {
               case 16: return value + 500;
               case 15: return value + 450;
               case 14: return value + 400;
               case 13: return value + 350;
               case 12: return value + 300;
               case 11: return value + 250;
               case 10: return value + 200;
               case 9: return value + 150;
               case 8: return value + 100;
               case 7: return value + 50;
               case 6: return value + 20;
               default: return value;
            }
         }
         else if (len == 1)
         {
            assert(g_char_values[str[0]-'a'] != 0);
            value += g_char_values[str[0]-'a'];
         }
         else
         {
            assert(false);
         }

         delete str;
         iter = iter->GetParent();
         length++;
      }
   }

   Path* Move(int xdelta, int ydelta)
   {
      if (this->m_isDud) return NULL;

      if (!this->m_board->CanMove(this->X(), this->Y(), xdelta, ydelta)) return NULL;
      
      int x = this->X() + xdelta;
      int y = this->Y() + ydelta;
      int position = x*4+y;
      int bit = (1 << position);

      if((this->m_visitedSquares & bit) != 0) return NULL;

      return new Path(this, x, y);
   }
   
   int X() const { return this->m_currentPosition[0]; }
   int Y() const { return this->m_currentPosition[1]; }

protected:

   Path(Path* p, int x, int y)
   {
      this->Initialize(x, y, p->m_currentWord, p->m_board);

      int position = x*4+y;
      int bit = (1 << position);

      assert((p->m_visitedSquares & bit) == 0);

      this->m_visitedSquares |= p->m_visitedSquares;
   }

   void Initialize(int x, int y, Tries::Trie * dictionary, Boards::Board * board)
   {
      this->m_currentPosition[0] = x;
      this->m_currentPosition[1] = y;
      this->m_isDud = false;

      int position = x*4+y;

      this->m_visitedSquares = (1 << position);
      this->m_board = board;

      char* square = board->GetSquare(x, y);

      assert(square != NULL);
      assert(square[0] != '\0');

      int i=0;

      while(square[i] != '\0')
      {
         if (!dictionary->HasChild(square[i]))
         {
            // Adding this character eliminates all possible words.

            this->m_isDud = true;
            return;
         }
         dictionary = dictionary->GetChild(square[i]);
         i++;
      }

      assert(dictionary != NULL);

      this->m_currentWord = dictionary;
   }

   int   m_currentPosition[2];
   short m_visitedSquares;
   bool  m_isDud;

   Tries::Trie *   m_currentWord;
   Boards::Board * m_board;
};