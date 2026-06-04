{-# LANGUAGE ImpredicativeTypes #-}
{-# LANGUAGE UndecidableInstances #-}

module SpecBlock2 where

import Block2
import Data.Function
import Test.Prelude

tests :: NamedTests
tests = nameTests 2
  [ testStructEq
  , testTreeFunctor
  , testPRL
  , testPairStructEq
  , testMyFunctor
  ]

testStructEq :: Test
testStructEq = TestList
  [ propertyToTest "list struct eq" \(l :: [Char], Fun _ f :: Fun Char Int) ->
      l ==# fmap f l
  , propertyToTest "list struct eq" \(l1 :: [()], l2 :: [()]) ->
      l1 /= l2 ==> not (l1 ==# l2)
  , TestCase $ assertBool "leafs struct eq" $ Leaf ==# Leaf
  , TestCase $ assertBool "nodes struct eq" $ Node 32 [Leaf, Leaf] ==# Node 44 [Leaf, Leaf]
  , TestCase $ assertBool "node and leaf not struct eq" $ not $ Leaf ==# Node 3 []
  , TestCase $ assertBool "lists are not struct eq" $ not $ Node 3 [Leaf] ==# Node 3 []
  , TestCase $ assertBool "subtrees are not struct eq" $ not $ Node 1 [Leaf] ==# Node 2 [Node 3 []]
  ]

deriving stock instance Generic (Tree a)

instance Arbitrary a => Arbitrary (Tree a) where
  arbitrary = sized $ fix \rec -> \case
    0 -> pure Leaf
    n -> oneof
      [ pure Leaf
      , choose (0, 3) >>= \len -> Node <$> arbitrary <*> vectorOf len (rec $ n - 1)
      ]
  shrink = genericShrink

testTreeFunctor :: Test
testTreeFunctor = TestList
  [ propertyToTest "functor law" \(t :: Tree Int, Fun _ f :: Fun Int Char) ->
      t ==# fmap f t
  , lawsToTest $ functorLaws $ Proxy @Tree
  ]

testPRL :: Test
testPRL = lawsToTest $ functorLaws $ Proxy @(TestPRLFunctor (PairReaderList Bool Int))

deriving instance Arbitrary a => Arbitrary (PairReaderList Bool Int a)

instance Show (PairReaderList e p a) where
  show _ = "<fun>"

instance Eq a => Eq (PairReaderList Bool Int a) where
  PairReaderList fs == PairReaderList gs = and $ zipWith (==) (fs <*> carrier) (gs <*> carrier)
    where
      carrier = [False ..]

testPairStructEq :: Test
testPairStructEq = TestList
  [ TestCase $ assertBool "struct equal pairs" $ (1, 10) ==# (1, 'c')
  , TestCase $ assertBool "struct not equal pairs" $ not $ (1, 20) ==# (2, 20)
  , propertyToTest "pair struct eq law" \(w1 :: Bool, w2 :: Bool, x :: Int, y :: Char) ->
      w1 == w2 ==> (w1, x) ==# (w2, y)
  ]

testMyFunctor :: Test
testMyFunctor = TestList
  [ TestCase $ assertEqual "MyFunctor case1" ((^ 2) <$> [1 .. 10]) case1
  , TestCase $ assertEqual "MyFunctor case2" (show <$> [1 .. 10]) case2
  , TestCase $ assertEqual "MyFunctor case3" (('y' :) <$> Just "x") case3
  , TestCase $ assertEqual "MyFunctor case4" ((^ 2) <$> [1 .. 10], ('y' :) <$> ["a"]) case4
  ]
