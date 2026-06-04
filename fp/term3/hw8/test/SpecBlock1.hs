{- HLINT ignore "Use <$>" -}

module SpecBlock1 where

import Block1
import Data.Bifunctor
import Data.Int
import Test.Prelude

tests :: NamedTests
tests = nameTests 1
  [ testHalfPair
  , testPoint
  , testPointN
  , testPointI
  , testMonoidal
  , testApplicativeVsMonoidal
  ]

testHalfPair :: Test
testHalfPair = TestList
  [ lawsToTest $ applicativeLaws $ Proxy @(HalfPair String)
  , TestCase $ assertEqual "exprHalfPair" (HalfPair "hello world") exprHalfPair
  ]

deriving stock instance Generic (HalfPair e a)
instance Arbitrary e => Arbitrary (HalfPair e a) where
  arbitrary = HalfPair <$> arbitrary
  shrink = genericShrink

testPoint :: Test
testPoint = TestList
  [ lawsToTest $ applicativeLaws $ Proxy @(TestPointApplicative Point)
  , TestCase $ assertEqual "stretchPoint" (Point 4 6) $ stretchPoint 2 (Point 2 3)
  , TestCase $ assertEqual "sumPoints" (Point 5 9) $ sumPoints (Point 1 2) (Point 3 4) (Point 1 3)
  ]

deriving stock instance Generic (Point a)
instance Arbitrary a => Arbitrary (Point a) where
  arbitrary = Point <$> arbitrary <*> arbitrary
  shrink = genericShrink

testPointN :: Test
testPointN = TestList
  [ lawsToTest $ filterLaws (/= "Homomorphism") $ applicativeLaws $ Proxy @(TestPointNApplicative PointN)
  , TestCase $ assertEqual "stretchPoint works" (PointN [4, 6]) $ stretchPointN 2 (PointN [2, 3])
  , TestCase $ assertEqual "sumPoints" (PointN [5, 9]) $ sumPointsN (PointN [1, 2]) (PointN [3, 4]) (PointN [1, 3])
  , TestCase $ assertEqual "sumPoints" (PointN [5, 9, 0]) $ sumPointsN (PointN [1, 2, 1]) (PointN [3, 4, -1]) (PointN [1, 3, 0])
  ]

deriving stock instance Generic (PointN a)
instance Arbitrary a => Arbitrary (PointN a) where
  arbitrary = fmap PointN $ choose (0, 4) >>= vector
  shrink = genericShrink

testPointI :: Test
testPointI = TestList
  [ lawsToTest $ functorLaws $ Proxy @(TestPointIApplicative (PointI Ordering))
  , lawsToTest $ applicativeLaws $ Proxy @(TestPointIApplicative (PointI Ordering))
  , TestCase $ assertEqual "stretchPoint" (PointI (* 4) :: PointI Int8 Int8) $ stretchPointI 2 (PointI (* 2))
  , TestCase $ assertEqual "sumPoints"
      (PointI ((+ 6) . (* 3)) :: PointI Int8 Int8) $
      sumPointsI (PointI (+ 1)) (PointI (+ 2)) (PointI (+ 3)) --  (i + 1) + (i + 2) + (i + 3)
  ]

instance (Enum i, Bounded i, Show i, Show a) => Show (PointI i a) where
  showsPrec _ (PointI p) = showList $ zip (domain :: [i]) $ p <$> domain

instance (Enum i, Bounded i, Eq a) => Eq (PointI i a) where
  PointI f == PointI g = map f domain == map g domain

deriving stock instance Generic (PointI i a)
instance (Function i, CoArbitrary i, Arbitrary a) => Arbitrary (PointI i a) where
  arbitrary = PointI . applyFun <$> arbitrary
  shrink = genericShrink

testMonoidal :: Test
testMonoidal = TestList
  [ testMonoidalLaws @Maybe
  , testMonoidalLaws @((->) Ordering)
  , testMonoidalLaws @((->) Bool)
  , testMonoidalLaws @((,) String)
  , testMonoidalLaws @((,) [Int])
  ]

testMonoidalLaws
  :: forall (f :: Type -> Type).
      ( forall a. Arbitrary a => Arbitrary (f a)
      , forall a. Show a => Show (f a)
      , forall a. Eq a => Eq (f a)
      , Monoidal f
      )
  => Test
testMonoidalLaws = TestList
  [ propertyToTest "левая единица моноидального функтора" \(x :: f Int) ->
      unit *&* x === fmap ((),) x
  , propertyToTest "правая единица моноидального функтора" \(x :: f Int) ->
      x *&* unit === fmap (,()) x
  , propertyToTest "моноидальный функтор натурален" \(Fun _ f :: Fun Int String, x :: f Int, y :: f Bool) ->
      fmap f x *&* y === fmap (first f) (x *&* y) .&&. y *&* fmap f x === fmap (second f) (y *&* x)
  ]

testApplicativeVsMonoidal :: Test
testApplicativeVsMonoidal = TestList
  [ testMonoidalLaws @(ApplicativeMonoidal Maybe)
  , testMonoidalLaws @(ApplicativeMonoidal [])
  , lawsToTest $ applicativeLaws $ Proxy @(MonoidalApplicative Maybe)
  , lawsToTest $ applicativeLaws $ Proxy @(MonoidalApplicative ((,) String))
  ]

deriving newtype instance Arbitrary (f a) => Arbitrary (ApplicativeMonoidal f a)
deriving newtype instance Arbitrary (f a) => Arbitrary (MonoidalApplicative f a)
