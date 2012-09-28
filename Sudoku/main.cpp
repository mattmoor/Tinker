
#include "board.h"
#include <vector>
#include <intrin.h>

std::vector<char *> boards;

void LoadBoards()
{
#define LOAD_BOARD(b_str) \
   { \
      char * tmp = (char*)malloc(strlen(b_str)+1); \
      \
      strcpy(tmp, b_str); \
      boards.push_back(tmp);\
   }
   
#include "boards.dat"

#undef LOAD_BOARD
}

#if !defined(RELEASE)
void StartupTest()
{
   Board b;
   uint  failures = 0;

   LoadBoards();

   size_t start_time = __rdtsc();

   for(auto bi = boards.begin(); bi != boards.end(); bi++)
   {
      b.Parse(*bi);
      b.Display();
      
      std::cout << std::endl;

      if (!b.Solve())
      {
         assert(b.Check());

         std::cout << "Failed to solve:" << std::endl;

         failures++;
      }
      else
      {
         assert(b.Check());

         std::cout << "Successfully solved:" << std::endl;
      }
      b.Display();

      std::cout << std::endl << std::endl << std::endl;
   }

   size_t end_time = __rdtsc();

   if (failures > 0) exit(failures);

   std::cout << "Startup tests completed successfully in " << (end_time - start_time) << " ticks" << std::endl << std::endl << std::endl;
}
#endif

void ReadBoardSolveBoard()
{
   Board b;

   char * board_str = (char*)malloc(SQUARE_SIZE*SQUARE_SIZE+SQUARE_SIZE+1);
   char * line_str = board_str;
   uint lines = 0;

   std::cout << "Enter a board:" << std::endl;

   do {
      std::cin.getline(line_str, SQUARE_SIZE+1, '\n');

      if(strlen(line_str) != SQUARE_SIZE)
      {
         std::cout << "Malformed line: " << line_str << std::endl;
         exit(-1);
      }
      line_str[SQUARE_SIZE] = '\n';
      line_str[SQUARE_SIZE+1] = '\0';

      line_str = &line_str[SQUARE_SIZE+1];
      lines++;

   } while(lines != SQUARE_SIZE);

   board_str[SQUARE_SIZE*SQUARE_SIZE + SQUARE_SIZE - 1] = '\0';

   b.Parse(board_str);

   if (!b.Check())
   {
      std::cout << "Malformed board: " << std::endl;
      b.Display();

      exit(-1);
   }

   std::cout << "Original board: " << std::endl;

   b.Display();
      
   std::cout << std::endl;

   size_t start_time = __rdtsc();

   bool result = b.Solve();

   size_t end_time = __rdtsc();

   if (!result)
   {
      assert(b.Check());

      std::cout << "Failed to solve:" << std::endl;
   }
   else
   {
      assert(b.Check());

      std::cout << "Successfully solved:" << std::endl;
   }
   b.Display();

   std::cout << std::endl << "Solution took " << (end_time - start_time) << " ticks" 
      << std::endl << std::endl << std::endl;
}

int main()
{
#if !defined(RELEASE)
   StartupTest();
#endif

   while(true)
   {
      ReadBoardSolveBoard();
   }

   return 0;
}