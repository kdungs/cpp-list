# cpp-list
CoW functional list in C++11.

## Functionality
Functionality loosely follows all the nice stuff you find in Haskell ([Prelude](http://hackage.haskell.org/package/base-4.7.0.2/docs/Prelude.html)). Here's a list of functions that have been implemented so far. If the name is different in Haskell, it is given in italic.

 * Constructing lists
  * `cons` _(:)_
  * `makeList` _[...]_
  * `append` _does not exist in Haskell in this form..._
 * List operations
  * `map`
  * `join` _(++)_
  * `filter`
  * `head`
  * `last`
  * `tail`
  * `init`
  * `null`
  * `length`
  * `at` _(!!)_
  * `reverse` (implemented in terms of `foldl` therefore found at the end of the code)
 * Reducing lists (folds)
  * `foldl`
  * `foldl1`
  * `foldr`
  * `foldr1`
 * Special folds
  * `concat`
  * `concatMap`
 * Zipping and unzipping lists
  * `zipWith`
 * Other
  * `apply` _does not exist in Haskell..._

## To Do

 * Implement more functionality
 * Write actual (unit) tests
 * More documentation
 * Performance?!


## Contact
If you have any questions or suggestions feel free to write an issue report. You can also send me an email to kevin at my domain.
