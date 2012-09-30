#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#if defined(WINDOWS)

#include <intrin.h>
#define NORETURN __declspec(noreturn)

#else

#define __rdtsc() 0
#define __debugbreak() exit(-5)
#define nullptr (const char*)0x0
#define __int64 long long
#define NORETURN

#endif

#define islower(c) ('a' <= (c)) && ('z' >= (c))
#define assert(b) if(!(b)) { __debugbreak(); }


#include "trie.h"
#include "board.h"
#include "path.h"

Tries::Trie *   g_dictionary;
Tries::Trie *   g_seen;
Boards::Board * g_board;

#if defined(DEBUG)
void TestTrie()
{
   Tries::Trie * t = new Tries::BasicTrie();
   
   t = t->AddChild("ab");
   t = t->AddChild("acb");
   t = t->AddChild("abd");
   t = t->AddChild("cbd");
   t = t->AddChild("abc");

   
   assert(t->Lookup("abc"));
   assert(t->Lookup("acb"));
   assert(t->Lookup("abd"));
   assert(t->Lookup("cbd"));
   assert(t->Lookup("ab"));

   assert(!t->Lookup("a"));
   assert(!t->Lookup("ac"));
   assert(!t->Lookup("ad"));
   assert(!t->Lookup("c"));
   assert(!t->Lookup("b"));
   
   char* s1 = t->ToString();
   char* s2 = t->GetChild('a')->GetChild('b')->ToString();


   delete s1;
   delete s2;

   delete t;
}
#endif //defined(DEBUG)

void cleanup()
{
   delete g_seen;
   delete g_board;
   delete g_dictionary;
}

NORETURN void usage(const char* error, const char* error2)
{
   if (error2 != nullptr)
   {
      printf("ERROR: %s \"%s\"\n", error, error2);
   }
   else
   {
      printf("ERROR: %s\n", error);
   }
   printf("game -d dict.txt -b \"a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p\" [-p]"
#if !defined(_ARM_) && !defined(THUMB2)
      " [-t]"
#endif
      "\n");
   fflush(stdout);

   cleanup();
   exit(1);
}

void LoadDictionary(char * filename)
{
   FILE* file = fopen(filename, "r");

   if (file == NULL)
   {
      usage("couldn't open dictionary file", filename);
   }

   g_dictionary = new Tries::BasicTrie();

   char line [ 256 ]; /* or other suitable maximum line size */       

   while ( fgets ( line, sizeof line, file ) != NULL ) /* read a line */      
   {
      int length = strlen(line);

      if (line[length-1] == '\n')
      {
         line[length-1] = '\0';
      }

      g_dictionary->AddChild(line);
   }

#if defined(DEBUG)
   assert(g_dictionary->Lookup("hello"));
   assert(g_dictionary->Lookup("world"));
   assert(g_dictionary->Lookup("how"));
   assert(g_dictionary->Lookup("you"));
   assert(g_dictionary->Lookup("doing"));
#endif //defined(DEBUG)

   if (fclose(file))
   {
      assert(false);
   }
}

static int found = 0;
static bool printWords = false;

#if !defined(_ARM_) && !defined(THUMB2)
static bool timeMatch = false;
#endif

void SolveRecursive(Path * p)
{
   int i, j;
   
   for(i=-1; i <= 1; i++)
   {
      for(j=-1; j <= 1; j++)
      {
         if ((i == 0) && (j == 0)) continue;

         Path * newPath = p->Move(i, j);

         if (newPath != NULL)
         {
            SolveRecursive(newPath);
         }

         delete newPath;
      }
   }

   if (p->IsWord())
   {
      char * str = p->ToString();

      if (!g_seen->Lookup(str))
      {
         g_seen->AddChild(str);

         if (printWords) printf("%.3d %s\n", p->value(), str);

         found++;
      }

      delete str;
   }
}

void Solve(int x, int y)
{
   Path* p = new Path(x, y, g_dictionary, g_board);
   
   SolveRecursive(p);

   delete p;
}

int main(int argc, char** argv)
{
#if defined(DEBUG)
   //TestTrie();
#endif //defined(DEBUG)

   int gridsize = 0;

   g_seen = new Tries::BasicTrie();

#if !defined(_ARM_) && !defined(THUMB2)
   unsigned __int64 start = __rdtsc();
#endif

   for(int i = 1; i < argc; i++)
   {
      if (strcmp("-d", argv[i]) == 0)
      {
         if (i+1 >= argc) usage("-d takes a filename argument", nullptr);

         // ISSUE-TODO-MattMoor-2012/-8/15
         // Delay load the dictionary so we can filter words 
         // with characters not present on the board.

         LoadDictionary(argv[++i]);

         // ISSUE-TODO-MattMoor-2011/12/16
         // We can optimize the dictionary's layout for optimal cache locality
         // now that we know the contents.

         // OptimizeDictionary();
      }
      else if (strcmp("-b", argv[i]) == 0)
      {
         if (i+1 >= argc) usage("-b takes a board layout", nullptr);

         g_board = new Boards::Board(argv[++i]);
      }
      else if (strcmp("-p", argv[i]) == 0)
      {
         printWords = true;
      }
#if !defined(_ARM_) && !defined(THUMB2)
      else if (strcmp("-t", argv[i]) == 0)
      {
         timeMatch = true;
      }
#endif
      else
      {
         usage("unknown option", argv[i]);
      }
   }

   if (g_dictionary == NULL) usage("-d is required", nullptr);
   if (g_board == NULL) usage("-b is required", nullptr);

   int i,j;

   for(i = 0; i < 4; i++)
   {
      for(j=0; j < 4; j++)
      {
         Solve(i, j);
      }
   }

#if !defined(_ARM_) && !defined(THUMB2)
   unsigned __int64 end = __rdtsc();

   if (timeMatch)
   {
     printf("found: %d in %d clocks", found, (int)(end-start));
   }
#endif
   
   cleanup();

   return 0;
}
