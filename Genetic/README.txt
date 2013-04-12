This implements a generic genetic algorithm package.  The entities being optimized are represented by Individual<T> and are seeded and evolved by God<T>.

individual.cc	-- unit testing for individual.h
god.cc		-- unit testing for god.h (uses individual.cc)

This can be build via "clang++ -O2 -std=c++11 foo.cc", but it intended to be included and specialized for new scenarios.
