module SpecBlock2 where

import Block2
import Data.Digits (digits)
import Data.Either
import Data.Numbers.Primes qualified as Primes
import Data.List (genericLength)
import Data.Set qualified as Set
import Test.HUnit
import Test.QuickCheck
import Test.Prelude

tests :: NamedTests
tests = nameTests 2
  [ testEvenOdd
  , testFacfac
  , testItemAt
  , testIsPrime
  , testSet
  , testNSumDigits
  ]

testEvenOdd :: Test
testEvenOdd = TestList
  [ propertyToTest "isEven works" \(Small n) -> isEven n === even n
  , propertyToTest "isOdd works" \(Small n) -> isOdd n === odd n
  ]

testFacfac :: Test
testFacfac = TestList
  [ TestCase $ assertEqual "1!!" 1 (facfac 1)
  , TestCase $ assertEqual "2!!" 2 (facfac 2)
  , TestCase $ assertEqual "3!!" 3 (facfac 3)
  , TestCase $ assertEqual "4!!" 8 (facfac 4)
  , TestCase $ assertEqual "6!!" 48 (facfac 6)
  , TestCase $ assertEqual "7!!" 105 (facfac 7)
  ]

testItemAt :: Test
testItemAt = TestList $ uncurry mkTest <$> zip [0..] items
  where
    mkTest i x = TestCase $ assertEqual ("b[" ++ show i ++ "]") x (itemAt i)
    items = [1,2,3,2,2,7,9,1,4,29,24,-22,17,133,33,-182,151,614,-234,-1009]

testSet :: Test
testSet = TestList
  [ propertyToTest "emptySet is empty" \(s :: String) -> not (emptySet s)
  , propertyToTest "adding adds" \(ss :: [String], new :: String) ->
      (setOf ss +++ new) new
  , propertyToTest "removing removes" \(ss :: [String], old :: String) ->
      not $ (setOf ss /// old) old
  , propertyToTest "non empty set contains its elements" \(ss :: [String]) ->
      let set = setOf ss in
      forAll (oneof [elements ss, arbitrary]) \s ->
        elem s ss === set s
  , propertyToTest "remove works correctly" \(NonEmpty ini, actions :: [Either String String]) ->
      let expected = foldr (either Set.delete Set.insert) (Set.fromList ini) actions in
      let actual = foldr (either (flip (///)) (flip (+++))) (setOf ini) actions in
      let existing = elements $ "" : Set.toList expected in
      let removed = elements $ "" : lefts actions in
      forAll (oneof [existing, removed, arbitrary]) \s ->
        Set.member s expected === actual s
  ]
  where
    setOf :: [String] -> (String -> Bool)
    setOf = foldl (+++) emptySet

testIsPrime :: Test
testIsPrime = TestList
  [ propertyToTest "isPrime works" \(NonNegative n) -> isPrime n === Primes.isPrime n
  ]

testNSumDigits :: Test
testNSumDigits = TestList
  [ propertyToTest "nSumDigits works well" \n ->
      let ds = digits 10 (abs n) in
      nSumDigits n === (max 1 (genericLength ds), sum ds)
  ]
