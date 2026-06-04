module SpecBlock1 where

import Block1
import Control.Exception
import Data.List qualified as List
import System.Random (uniform, mkStdGen)
import Test.Prelude

tests :: NamedTests
tests = nameTests 1
  [ testNat
  , testIntTree
  , testBinTree
  , testCartesianTree
  ]

deriving stock instance Eq Nat
deriving stock instance Generic Nat
deriving via GenericArbitrary Nat instance Arbitrary Nat

deriving stock instance Generic (BinTree a)
deriving via GenericArbitrary (BinTree a) instance
  (Arg (BinTree a) a, Arbitrary a) => Arbitrary (BinTree a)

testNat :: Test
testNat = TestList
  [ TestCase $ assertEqual "foldNat (+1) 0 2" 2 (foldNat (+ 1) 0 (Suc (Suc Zero)))
  , TestCase $ assertEqual "foldNat (+1) 0 1" 1 (foldNat (+ 1) 0 (Suc Zero))
  , TestCase $ assertEqual "foldNat (+1) 0 0" 0 (foldNat (+ 1) 0 Zero)
  , TestCase $ assertEqual "2^3 через foldNat" 8 (foldNat (* 2) 1 (Suc (Suc (Suc Zero))))
  , TestCase $ assertEqual "sssz через foldNat" "sssz" (foldNat ('s' :) "z" (Suc (Suc (Suc Zero))))
  , TestCase $ assertEqual "makeNat 0" Zero (makeNat 0)
  , TestCase $ assertEqual "makeNat 3" (Suc (Suc (Suc Zero))) (makeNat 3)
  , propertyToTest "fail-fast on contract not satisfied" $ \(Negative n) ->
      ioProperty $ assertThrows @ErrorCall (makeNat n)
  , propertyToTest "make-fold law" \(NonNegative n) -> foldNat (+1) 0 (makeNat n) === n
  , propertyToTest "fold-make law" \n -> makeNat (foldNat (+1) 0 n) === n
  ]

testIntTree :: Test
testIntTree = TestList
  [ TestCase $ assertEqual "sumIntTree с одним числом" 42 (sumIntTree oneNum)
  , TestCase $ assertEqual "sumIntTree tree" 15 (sumIntTree tree)
  ]
  where
    oneNum = node 42 [leaf, leaf, leaf, leaf]
    tree = node 1 [node 2 [node 5 [leaf]], leaf, node 3 [], leaf, node 4 []]

testBinTree :: Test
testBinTree = TestList
  [ propertyToTest "first functor law for mapBinTree"
      \(t :: BinTree Int) ->
        mapBinTree id t === t
  , propertyToTest "second functor law for mapBinTree"
      \(t :: BinTree Int, Fun _ f :: Fun Int String, Fun _ g :: Fun String Char) ->
        mapBinTree g (mapBinTree f t) === mapBinTree (g . f) t
  , TestCase $ assertEqual "mapBinTree" treeLengths (mapBinTree length treeLists)
  , TestCase $ assertEqual "bangAll" treeOfTrees' (bangAll treeOfTrees)
  ]
  where
    treeLists = BinNode
      (BinNode Empty [1] (BinNode Empty [2] Empty))
      [3, 4]
      (BinNode Empty [5, 6, 7] Empty)
    treeLengths = BinNode
      (BinNode Empty 1 (BinNode Empty 1 Empty))
      2
      (BinNode Empty 3 Empty)
    treeOfTrees = mapBinTree (const treeLengths) treeLengths
    treeOfTrees' = mapBinTree (const $ mapBinTree ((++ "!") . show) treeLengths) treeLengths

testCartesianTree :: Test
testCartesianTree = TestLabel "dtree" $ TestList
  [ propertyToTest "empty" \() ->
      ([] :: [Int]) =~= empty
  , propertyToTest "singleton" \(x :: Int, k :: Int) ->
      [x] =~= singleton x k
  , propertyToTest "append" \(xs :: [Int], seed) ->
      xs =~= treeOf xs seed
  , propertyToTest "insert" \(xs :: [Int], x, i, k, seed) ->
      let (l, r) = List.splitAt i xs in
      (l ++ x : r) =~= insert i x k (treeOf xs seed)
  , propertyToTest "remove" \(xs :: [Int], NonNegative i, seed) ->
      let (l, r) = List.splitAt i xs in
      (l ++ drop 1 r) =~= remove (treeOf xs seed) i
  , propertyToTest "size" \(xs :: [Int], seed) ->
      length xs === size (treeOf xs seed)
  ]
  where
    treeOf xs seed =
      let randoms = List.unfoldr (Just . uniform) (mkStdGen seed) in
      foldl (\t (x, k) -> append t x k) empty (zip xs randoms)
    xs =~= t =
      let size = length xs in
      let existing = (Just <$> xs) === ((t !!?) <$> [0..size-1]) in
      existing .&&. forAll arbitrary \i -> i < 0 || i >= size ==> t !!? i === Nothing
