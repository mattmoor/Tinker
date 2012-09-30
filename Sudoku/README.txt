This is a basic sudoku solver written in C++ by Matt Moore.


It can be built using the Visual C++ 2012 toolset via:

   cl -O2 -GL -DRELEASE#1 -DWINDOWS#1 main.cpp board.cpp

A debuggable version with various checking and startup-testing enabled can be built via:

   cl -Od /Zi -DWINDOWS#1 main.cpp board.cpp /link /profile /debug

TODO: Verify the above still works with the G++/clang++ fixes complete...
TODO: Change to output to sudoku.exe, to match *nix style below



It can be built using GCC 4.2 (i686-apple-darwin11-llvm-gcc-4.2) via:

   g++ -O2 -DRELEASE=1 main.cpp board.cpp -o sudoku


It can be built using Clang (clang 3.0) via:

   clang++ -O2 -DRELEASE=1 main.cpp board.cpp -o sudoku

TODO: Support debugging and timing for *nix platforms



The input format is a standard 9x9 sudoku board where blank cells are specified by either a space ' ' or a zero '0' character.  For example:

       39
     1  5
  3 5 8  
  8 9   6
 7   2   
1  4     
  9 8  5 
 2    6  
4  7     


The program displays the board it's read, then attempts to solve the puzzle.  Upon success, the solver displays some diagnostic data that indicates how challenging it found the puzzle as well as the completes board.  For the above input it yields:

Original board:
        +------++------++------+
        |      ||      ||  3 9 |
        |      ||    1 ||    5 |
        |    3 ||  5   ||8     |
        +------++------++------+
        +------++------++------+
        |    8 ||  9   ||    6 |
        |  7   ||    2 ||      |
        |1     ||4     ||      |
        +------++------++------+
        +------++------++------+
        |    9 ||  8   ||  5   |
        |  2   ||      ||6     |
        |4     ||7     ||      |
        +------++------++------+


Solve succeeded after 629 passes
Successfully solved:
        +------++------++------+
        |7 5 1 ||8 4 6 ||2 3 9 |
        |8 9 2 ||3 7 1 ||4 6 5 |
        |6 4 3 ||2 5 9 ||8 7 1 |
        +------++------++------+
        +------++------++------+
        |2 3 8 ||1 9 7 ||5 4 6 |
        |9 7 4 ||5 6 2 ||3 1 8 |
        |1 6 5 ||4 3 8 ||9 2 7 |
        +------++------++------+
        +------++------++------+
        |3 1 9 ||6 8 4 ||7 5 2 |
        |5 2 7 ||9 1 3 ||6 8 4 |
        |4 8 6 ||7 2 5 ||1 9 3 |
        +------++------++------+


Solution took 61351860 ticks


It then awaits further boards until a malformed entry is given, which terminates the program.
