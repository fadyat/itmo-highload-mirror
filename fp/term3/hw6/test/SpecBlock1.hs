module SpecBlock1 where

import Block1
import Data.Int
import Data.List qualified as List
import Data.List.NonEmpty (NonEmpty (..))
import Data.List.NonEmpty qualified as NE
import Test.Prelude

tests :: NamedTests
tests = nameTests 1
  [ testIntTreeEq
  , testIntTreeOrd
  , testCycleEnum
  , testCycling
  , testMatrixShow
  , testMatrixRead
  , testChurchShowEq
  , testChurchEnum
  , testChurchNum
  ]

deriving via GenericArbitrary IntTree instance Arbitrary IntTree

testIntTreeEq :: Test
testIntTreeEq = TestList
  [ lawsToTest $ eqLaws $ Proxy @IntTree
  , TestCase $ assertEqual "different lefts"
      (IntNode IntLeaf (42, 2) IntLeaf)
      (IntNode IntLeaf (24, 2) IntLeaf)
  ]

testIntTreeOrd :: Test
testIntTreeOrd = TestList
  [ lawsToTest $ ordLaws $ Proxy @IntTree
  , propertyToTest "leaf less" \tree ->
      tree /= IntLeaf ==> l < tree
  , TestCase $ assertBool "first subtree first" $
      n (n l (5, 1) l) (1, 5) l < n (n l (5, 2) l) (1, 0) l
  , TestCase $ assertBool "elements then" $
      n (n l (5, 1) l) (1, 0) (n l (0, 0) l) < n (n l (5, 1) l) (1, 5) l
  , TestCase $ assertBool "second subtree last" $
      n l (0, 0) l < n l (0, 0) (n l (0, 0) l)
  ]
  where
    l = IntLeaf
    n = IntNode

testCycleEnum :: Test
testCycleEnum = TestList
  [ TestCase $ assertEqual "cycSucc False" True (cycSucc False)
  , TestCase $ assertEqual "cycSucc True" False (cycSucc True)
  , TestCase $ assertEqual "cycPred False" True (cycPred False)
  , TestCase $ assertEqual "cycPred True" False (cycPred True)
  , TestCase $ assertEqual "cycSucc 1" (2 :: Int8) (cycSucc 1)
  , TestCase $ assertEqual "cycSucc max" (minBound :: Int8) (cycSucc maxBound)
  , TestCase $ assertEqual "cycPred min" (maxBound :: Int8) (cycPred minBound)
  ]

testCycling :: Test
testCycling = TestList
  [ TestCase $ assertEqual "cycSucc One" (Cycling Two) (cycSucc $ Cycling One)
  , TestCase $ assertEqual "cycSucc Three" (Cycling One) (cycSucc $ Cycling Three)
  , TestCase $ assertEqual "cycPred One" (Cycling Three) (cycPred $ Cycling One)
  ]

data Ternary = One | Two | Three
  deriving stock (Eq, Ord, Enum, Bounded, Show)

deriving stock instance Generic (Matrix a)
deriving newtype instance Arbitrary a => Arbitrary (Matrix a)

testMatrixShow :: Test
testMatrixShow = TestList
  [ TestCase $ assertEqual "show empty matrix" "EMPTY" $ show $ Matrix ([] :: [NonEmpty Int])
  , TestCase $
      let expected = "[1,2,3]\n[4,5,6]\n[7,8,9]" in
      let actual = show $ Matrix $ NE.fromList <$> [[1 :: Int, 2, 3], [4, 5, 6], [7, 8, 9]] in
      let simplified = List.intercalate "\n" $ lines actual in
      assertEqual "show non-empty matrix" expected simplified
  , lawsToTest $ showLaws $ Proxy @(Matrix Int)
  , lawsToTest $ showLaws $ Proxy @(Matrix Char)
  ]

testMatrixRead :: Test
testMatrixRead = TestList
  [ TestCase $ assertEqual "read empty matrix" (Matrix ([] :: [NonEmpty Int])) (read "EMPTY")
  , TestCase $ assertEqual "read non-empty matrix"
      (Matrix $ NE.fromList <$> [[1 :: Int, 2, 3], [4, 5, 6], [7, 8, 9]]) (read m)
  , propertyToTest "show and read are agreed" \(m :: Matrix Int) ->
      m === read (show m)
  ]
  where
    m = "[1,2,3]\n[4,5,6]\n[7,8,9]"

testChurchShowEq :: Test
testChurchShowEq = TestList
  [ propertyToTest "show" \(NonNegative (Small (n :: Int))) ->
      "Church " <> show n === show (wrap $ toChurch n)
  , propertyToTest "==" \(NonNegative (Small (n :: Int))) ->
      wrap (toChurch n) == wrap (toChurch n)
  , propertyToTest "/=" \(NonNegative (Small (n :: Int)), NonNegative (Small (m :: Int))) ->
      n /= m ==> wrap (toChurch n) /= wrap (toChurch m)
  , lawsToTest $ eqLaws $ Proxy @(TestChurchShowEq Church)
  ]
  where
    wrap = wrap1 @TestChurchShowEq

testChurchEnum :: Test
testChurchEnum = TestList
  [ lawsToTest $ boundedEnumLaws $ Proxy @(TestChurchEnum Church)
  , TestCase $ assertEqual "succ 0, " (wrap 1) (fmap fromChurch $ succ $ wrap ch0)
  , TestCase $ assertEqual "succ 1, " (wrap 2) (fmap fromChurch $ succ $ wrap ch1)
  , TestCase $ assertEqual "succ 2, " (wrap 3) (fmap fromChurch $ succ $ wrap ch2)
  , TestCase $ assertEqual "succ 3, " (wrap 4) (fmap fromChurch $ succ $ wrap ch3)
  , TestCase $ assertEqual "succ 4, " (wrap 5) (fmap fromChurch $ succ $ wrap ch4)
  , TestCase $ assertEqual "succ 5, " (wrap 6) (fmap fromChurch $ succ $ wrap ch5)
  , TestCase $ assertEqual "pred 1, " (wrap 0) (fmap fromChurch $ pred $ wrap ch1)
  , TestCase $ assertEqual "pred 2, " (wrap 1) (fmap fromChurch $ pred $ wrap ch2)
  , TestCase $ assertEqual "pred 3, " (wrap 2) (fmap fromChurch $ pred $ wrap ch3)
  , TestCase $ assertEqual "pred 4, " (wrap 3) (fmap fromChurch $ pred $ wrap ch4)
  , TestCase $ assertEqual "pred 5, " (wrap 4) (fmap fromChurch $ pred $ wrap ch5)
  , TestCase $ assertEqual "fromEnum 0, " 0 (fromEnum $ wrap ch0)
  , TestCase $ assertEqual "fromEnum 1, " 1 (fromEnum $ wrap ch1)
  , TestCase $ assertEqual "fromEnum 2, " 2 (fromEnum $ wrap ch2)
  , TestCase $ assertEqual "fromEnum 3, " 3 (fromEnum $ wrap ch3)
  , TestCase $ assertEqual "fromEnum 4, " 4 (fromEnum $ wrap ch4)
  , TestCase $ assertEqual "fromEnum 5, " 5 (fromEnum $ wrap ch5)
  , TestCase $ assertEqual "toEnum 3, " (wrap 3) (fromChurch <$> toEnum @(TestChurchEnum Church) 3)
  , propertyToTest "toEnum" \(NonNegative (Small (n :: Int))) ->
      wrap n === fmap fromChurch (toEnum n :: TestChurchEnum Church)
  , propertyToTest "toEnum&fromEnum" \(NonNegative (Small (n :: Int))) ->
      n === fromEnum (toEnum n :: TestChurchEnum Church)
  ]
  where
    wrap = wrap1 @TestChurchEnum

testChurchNum :: Test
testChurchNum = TestList
  [ TestCase $ assertEqual "signum 0" (wrap 0) (fmap fromChurch $ signum $ wrap ch0)
  , TestCase $ assertEqual "signum 1" (wrap 1) (fmap fromChurch $ signum $ wrap ch1)
  , TestCase $ assertEqual "signum 2" (wrap 1) (fmap fromChurch $ signum $ wrap ch2)
  , TestCase $ assertEqual "signum 5" (wrap 1) (fmap fromChurch $ signum $ wrap ch5)
  , TestCase $ assertEqual "abs 0" (wrap 0) (fmap fromChurch $ abs $ wrap ch0)
  , TestCase $ assertEqual "abs 1" (wrap 1) (fmap fromChurch $ abs $ wrap ch1)
  , TestCase $ assertEqual "abs 2" (wrap 2) (fmap fromChurch $ abs $ wrap ch2)
  , TestCase $ assertEqual "abs 5" (wrap 5) (fmap fromChurch $ abs $ wrap ch5)
  , TestCase $ assertEqual "0 * 0" (wrap 0) (fromChurch <$> (*) (wrap ch0) (wrap ch0))
  , TestCase $ assertEqual "0 * 1" (wrap 0) (fromChurch <$> (*) (wrap ch0) (wrap ch1))
  , TestCase $ assertEqual "0 * 2" (wrap 0) (fromChurch <$> (*) (wrap ch0) (wrap ch2))
  , TestCase $ assertEqual "1 * 0" (wrap 0) (fromChurch <$> (*) (wrap ch1) (wrap ch0))
  , TestCase $ assertEqual "1 * 1" (wrap 1) (fromChurch <$> (*) (wrap ch1) (wrap ch1))
  , TestCase $ assertEqual "1 * 2" (wrap 2) (fromChurch <$> (*) (wrap ch1) (wrap ch2))
  , TestCase $ assertEqual "2 * 0" (wrap 0) (fromChurch <$> (*) (wrap ch2) (wrap ch0))
  , TestCase $ assertEqual "2 * 1" (wrap 2) (fromChurch <$> (*) (wrap ch2) (wrap ch1))
  , TestCase $ assertEqual "2 * 2" (wrap 4) (fromChurch <$> (*) (wrap ch2) (wrap ch2))
  , TestCase $ assertEqual "0 - 0" (wrap 0) (fromChurch <$> (-) (wrap ch0) (wrap ch0))
  , TestCase $ assertEqual "1 - 0" (wrap 1) (fromChurch <$> (-) (wrap ch1) (wrap ch0))
  , TestCase $ assertEqual "1 - 1" (wrap 0) (fromChurch <$> (-) (wrap ch1) (wrap ch1))
  , TestCase $ assertEqual "2 - 0" (wrap 2) (fromChurch <$> (-) (wrap ch2) (wrap ch0))
  , TestCase $ assertEqual "2 - 1" (wrap 1) (fromChurch <$> (-) (wrap ch2) (wrap ch1))
  , TestCase $ assertEqual "2 - 2" (wrap 0) (fromChurch <$> (-) (wrap ch2) (wrap ch2))
  , TestCase $ assertEqual "0 + 0" (wrap 0) (fromChurch <$> (+) (wrap ch0) (wrap ch0))
  , TestCase $ assertEqual "0 + 1" (wrap 1) (fromChurch <$> (+) (wrap ch0) (wrap ch1))
  , TestCase $ assertEqual "0 + 2" (wrap 2) (fromChurch <$> (+) (wrap ch0) (wrap ch2))
  , TestCase $ assertEqual "1 + 0" (wrap 1) (fromChurch <$> (+) (wrap ch1) (wrap ch0))
  , TestCase $ assertEqual "1 + 1" (wrap 2) (fromChurch <$> (+) (wrap ch1) (wrap ch1))
  , TestCase $ assertEqual "1 + 2" (wrap 3) (fromChurch <$> (+) (wrap ch1) (wrap ch2))
  , TestCase $ assertEqual "2 + 0" (wrap 2) (fromChurch <$> (+) (wrap ch2) (wrap ch0))
  , TestCase $ assertEqual "2 + 1" (wrap 3) (fromChurch <$> (+) (wrap ch2) (wrap ch1))
  , TestCase $ assertEqual "2 + 2" (wrap 4) (fromChurch <$> (+) (wrap ch2) (wrap ch2))
  , TestCase $ assertEqual "2 * 3 + 14 - 3" (wrap 17) $
      fmap fromChurch $ wrap ch2 * wrap ch3 + 14 - wrap ch3
  , TestCase $ assertEqual "list of nums is ok" [4, 6 .. 14] (fromChurch <$> from4To15)
  ]
  where
    wrap = wrap1 @TestChurchNum

instance Bounded Church where
  minBound = toChurch 0
  maxBound = toChurch 64

instance Arbitrary Church where
  arbitrary = do
    NonNegative (Small (n :: Int)) <- arbitrary
    pure $ toChurch n

toChurch :: Int -> Church
toChurch n = Church $ \s z -> go n s z
  where
    go n s z
      | n == 0 = z
      | otherwise = s (go (n - 1) s z)

fromChurch :: Church -> Int
fromChurch (Church n) = n succ (0 :: Int)

ch0, ch1, ch2, ch3, ch4, ch5 :: Church
ch0 = Church $ \_ z -> z
ch1 = Church $ \s z -> s z
ch2 = Church $ \s z -> s (s z)
ch3 = Church $ \s z -> s (s (s z))
ch4 = Church $ \s z -> s (s (s (s z)))
ch5 = Church $ \s z -> s (s (s (s (s z))))
