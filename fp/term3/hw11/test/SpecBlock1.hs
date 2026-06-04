module SpecBlock1 where

import Block1
import Data.Bifunctor
import Data.Map qualified as Map
import Test.Prelude

tests :: NamedTests
tests = nameTests 1
  [ testResult
  , testStateReader
  , testStateWriter
  , testMonadRandom
  ]

testResult :: Test
testResult = TestList
  [ lawsToTest $ functorLaws $ Proxy @(TestRes (Res Int))
  , lawsToTest $ applicativeLaws $ Proxy @(TestRes (Res Double))
  , lawsToTest $ monadLaws $ Proxy @(TestRes (Res Double))
  ]

testStateReader :: Test
testStateReader = TestList
  [ TestCase $ assertEqual "все функции StateReader разом" True $
      flip runStateReader [3, 4] do
        a <- stateLocal ([1, 2] <>) $ stateReader (sum :: [Int] -> Int)
        b <- stateAsks (length :: [Int] -> Int)
        c <- stateLocal (const [30]) (stateAsk :: StateReader [Int] [Int])
        pure $ a + b + product c == 42
  , TestCase $ assertEqual "keyMember ~> True" True $
      runStateReader (keyMember "Bob") e
  , TestCase $ assertEqual "keyMember ~> False" False $
      runStateReader (keyMember "Patric") e
  ]
  where
    e = Map.fromList [("Bob", 1), ("Alice", 2)]

testStateWriter :: Test
testStateWriter = TestList
  [ TestCase $ assertEqual "все функции StateWriter разом, " ("xax", "bcdefx") $
      runStateWriter $ do
        _ <- stateWriter ("a", "b")
        stateTell "c"
        stateTell "d"
        ((), _) <- stateListen $ stateTell "e" :: StateWriter String ((), String)
        () <- stateCensor (take 1) $ stateTell "ff"
        (_, s) <- stateListens (<> "ax") $ stateWriter ("i", "x")
        pure s
  ]

testMonadRandom :: Test
testMonadRandom = TestList
  [ TestCase $ assertBool "nextInt меняет состояние" $
      let ((v, v'), _) = runMonadRandom ((,) <$> nextInt <*> nextInt) 42 in
      v /= v'
  , let ((v, v'), _) = runMonadRandom
          (((,) <$> nextInt <* nextInt) <*> (reinit 42 *> nextInt)) 42 in
    TestCase $ assertEqual "reinit инициализирует состояние" v v'
  , TestCase $
      let (i, i', i'') = fst $ flip runMonadRandom 42 $
            (,,) <$> nextInt <*> nextInt <*> nextInt in
      do
        assertBool "i /= i'" (i /= i')
        assertBool "i' /= i''" (i' /= i'')
        assertBool "i /= i''" (i /= i'')
  , TestCase $ assertEqual "randomLists"
      ([19,98,35,76,2,20,30],[90,45,16,32,69])
      (bimap clamp clamp $ randomLists 5 2)
  ]
  where
    clamp = fmap (`mod` 100)

deriving stock instance Generic (Res e a)
instance (Arbitrary a, Arbitrary e) => Arbitrary (Res e a) where
  arbitrary = oneof [Err <$> arbitrary, Ok <$> arbitrary]
  shrink = genericShrink
