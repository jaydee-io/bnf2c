Unreleased
----------
 * Update to gcc 7.2 & clang 6.0

Version 0.5 (2017-03-14)
------------------------
 * Fix issue with state merge on lookaheads propagating
 * Fix issue in LR1/LALR1 parsing algorithms
 * Fix unknown option error message

Version 0.4 (2016-08-22)
------------------------
First _official_ release
 * Added 3 parsing algorithms:
   * LR0: Initial Left to right, Rightmost derivation parser without lookahead
   * LR1: More powerfull LR parser with a single lookahead terminal
   * LALR1: More compact Look-Ahead LR parser with a single lookahead terminal

