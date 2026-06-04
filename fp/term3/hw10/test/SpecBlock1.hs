module SpecBlock1 where

import Block1
import Test.Prelude

tests :: NamedTests
tests = nameTests 1
  [ testBind
  , testJoin
  , testDesugared
  , testPointMonad
  , testSumAndMult
  ]

testBind :: Test
testBind = TestList
  [ TestCase $ assertEqual "bind works for just" (Just 42) $ Just 1 `bind` \x -> Just (x + 41)
  , TestCase $ assertEqual "bind works for noting (1)" Nothing $ Nothing `bind` \x -> Just (x + 41)
  , TestCase $ assertEqual "bind works for noting (2)" Nothing $ Just 1 `bind` const (Nothing @Int)
  ]

testJoin :: Test
testJoin = TestList
  [ TestCase $ assertEqual "join works for just" (Just 42) $ join' $ Just (Just 42)
  , TestCase $ assertEqual "join works for noting (1)" Nothing $ join' $ Just (Nothing @Int)
  , TestCase $ assertEqual "join works for noting (2)" (Nothing @Int) $ join' Nothing
  ]

testDesugared :: Test
testDesugared = TestList
  [ propertyToTest "desugaring works for Maybe monad" \(a :: Maybe [Maybe (Maybe Int)], b :: Maybe [Char]) ->
      sugared a b === desugared a b
  , propertyToTest "desugaring works for Either String monad"
      \(a :: EitherString [EitherString (EitherString Int)], b :: EitherString [Char]) ->
        sugared a b === desugared a b
  ]

newtype IgnoreEq a = IgnoreEq a deriving newtype Show
deriving newtype instance Arbitrary a => Arbitrary (IgnoreEq a)
instance Eq (IgnoreEq a) where
  _ == _ = True

type EitherString = Either (IgnoreEq String)
instance MonadFail EitherString where
  fail = Left . IgnoreEq

testPointMonad :: Test
testPointMonad = TestList
  [ lawsToTest $ functorLaws $ Proxy @(TestPointMonad Point)
  , lawsToTest $ applicativeLaws $ Proxy @(TestPointMonad Point)
  , lawsToTest $ monadLaws $ Proxy @(TestPointMonad Point)
  , TestCase $ assertEqual "point example works" (wrap $ Point 13 16) do
      a <- wrap $ Point 3 4
      b <- wrap $ Point 10 12
      pure (a + b)
  ]
  where
    wrap = wrap2 @(TestPointMonad Point)

deriving stock instance Generic (Point a)
instance Arbitrary a => Arbitrary (Point a) where
  arbitrary = Point <$> arbitrary <*> arbitrary

testSumAndMult :: Test
testSumAndMult = TestList
  [ propertyToTest "sumAndMult" \(x1 :: Int, y1, x2, y2) ->
      Point (x1 + x2) (y1 * y2) === sumAndMult (Point x1 y1) (Point x2 y2)
  ]
