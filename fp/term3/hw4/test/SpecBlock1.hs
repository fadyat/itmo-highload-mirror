module SpecBlock1 where

import Block1
import Prelude hiding (fst, snd, either, pred)
import Test.QuickCheck
import Test.Prelude

tests :: NamedTests
tests = nameTests 1
  [ testPair
  , testVariant
  , testPred
  , testTypes
  ]

testPair :: Test
testPair = TestList
  [ propertyToTest "fst law satisfied" \(x :: Char, y :: Int) -> fst (pair x y) === x
  , propertyToTest "snd law satisfied" \(x :: Int, y :: Char) -> snd (pair x y) === y
  ]

testVariant :: Test
testVariant = TestList
  [ propertyToTest "variant inl law satisfied"
      \(x :: Char, Fun _ f :: Fun Char Int, Fun _ g :: Fun Int Int) ->
        either f g (inl x) === f x
  , propertyToTest "variant inr law satisfied"
      \(x :: Int, Fun _ f :: Fun Char Int, Fun _ g :: Fun Int Int) ->
        either f g (inr x) === g x
  ]

testPred :: Test
testPred = TestList
  [ propertyToTest "pred 0 law satisfied"
      \(Fun _ f :: Fun Int Int, ini :: Int) ->
        pred zero f ini === ini
  , propertyToTest "pred suc law satisfied"
      \(Positive n :: Positive Int, Fun _ f, ini :: Int) ->
        pred (toChurch n) f ini === toChurch (n - 1) f ini
  ]
  where
    zero _ z = z
    toChurch n s z
      | n == 0 = z
      | otherwise = s (toChurch (n - 1) s z)

testTypes :: Test
testTypes = TestCase $ assertBool "task 1.4 solved" solved4
