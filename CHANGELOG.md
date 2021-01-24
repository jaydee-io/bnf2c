Unreleased
----------
* Debug : print grammar rules in order of declaration
* Update compiler support:
  * drop xcode 6.4 : no more supported by travis
  * drop xcode 7.3 : `brew update` issue
  * update xcode 11.0 to 11.6
  * add xcode 12.2
  * add gcc 10
  * add clang 9.0, 10.0 & 11.0

Version 0.6 (2018-04-10)
------------------------
 * Some refactoring
 * Activate '-Wall' and fix warnings
 * Added VSCode workspace
 * Fix recursive rule issue
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

