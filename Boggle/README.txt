This is a basic Boggle-style word game solver written by Matt Moore.

It can be built using Visual C++ 2012 via:

	cl -O2 -GL game.cpp

A typical usage would be:

	game.exe -d dict.txt -p -b a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p | sort

Which yields the point-ordered list of words:

003 ae
004 in
004 no
004 on
005 lo
005 mi
006 op
007 be
007 ef
007 nim
008 ab
008 ba
008 fa
008 if
009 fie
009 ink
009 lop
009 pol
010 fin
010 jo
011 kop
012 fino
012 glop
012 jin
012 mink
013 knop
015 fink
016 knife
016 plonk
017 jink


The ordering is done by "sort" as the words are displayed as they are found.