module SpecBlock2 where

import Block2
import Data.Functor.Compose
import Data.Int
import Data.Map qualified as Map
import MyIO
import Test.Prelude

tests :: NamedTests
tests = nameTests 2
  [ testCheck2
  , testallResults
  , testFuncEq
  , testReadInts
  , testSummerAppMain
  , testSumN
  ]

deriving stock instance Generic User
deriving via GenericArbitrary User instance Arbitrary User
deriving newtype instance Arbitrary UserId

testCheck2 :: Test
testCheck2 = TestLabel "check2" $ TestList
  [ let r = Map.fromList [(UserId 1, User "a" True)] in
    TestCase $ assertEqual "one user 1" Nothing $ check2 r (UserId 1) (UserId 2)
  , let r = Map.fromList [(UserId 2, User "a" True)] in
    TestCase $ assertEqual "one user 2" Nothing $ check2 r (UserId 1) (UserId 2)
  , let r = Map.fromList [(UserId 4, User "a" True)] in
    TestCase $ assertEqual "no users" Nothing $ check2 r (UserId 1) (UserId 2)
  , propertyToTest "all subscribed" \(users, u1, u2) -> fst u1 /= fst u2 ==>
      let r = Map.fromList (users ++ [u1, u2]) in
      Just (subscribed (snd u1) && subscribed (snd u2)) === check2 r (fst u1) (fst u2)
  ]

testallResults :: Test
testallResults = TestLabel "allResults&check" $ TestList
  [ TestCase $ assertEqual "список Just" (Just [3, 4]) (allResults [Just 3, Just 4])
  , TestCase $ assertEqual "список с Nothing" Nothing (allResults [Just 3, Nothing, Just 5])
  , TestCase $ assertEqual "список списков" [[1, 4], [1, 5], [2, 4], [2, 5], [3, 4], [3, 5]] (allResults [[1, 2, 3], [4, 5]])
  , TestCase $ assertEqual "список функций" [8, 10, 25] (allResults [(+ 3), (* 2), (^ 2)] 5)
  , TestCase $ assertEqual "бесконечный список" [8, 10, 25] $ take 3 $ allResults ([(+ 3), (* 2), (^ 2)] ++ infinite) 5
  , let r = Map.fromList [(UserId 1, User "a" True)] in
    TestCase $ assertEqual "пустой список" (Just True) $ check r []
  , let r = Map.fromList [(UserId 2, User "a" True)] in
    TestCase $ assertEqual "один пользователь" (Just True) $ check r [UserId 2]
  , let r = Map.fromList [(UserId 1, User "a" True)] in
    TestCase $ assertEqual "два пользователя, первый мимо" Nothing $ check r [UserId 1, UserId 2]
  , let r = Map.fromList [(UserId 2, User "a" True)] in
    TestCase $ assertEqual "два пользователя, второй мимо" Nothing $ check r [UserId 1, UserId 2]
  , let r = Map.fromList [(UserId 4, User "a" True)] in
    TestCase $ assertEqual "no users" Nothing $ check r [UserId 1, UserId 2]
  , propertyToTest "all subscribed" \(users, u1, u2) -> fst u1 /= fst u2 ==>
      let r = Map.fromList (users ++ [u1, u2]) in
      Just (subscribed (snd u1) && subscribed (snd u2)) === check r [fst u1, fst u2]
  ]

testFuncEq :: Test
testFuncEq = TestList
  [ TestCase $ assertBool "equal" $ (\(_ :: Int8) (_ :: Bool) -> 1) ==??! (\_ _ -> 1)
  , TestCase $ assertBool "on same differ" $ not $ (==??!)
      (curry \case (True, 1 :: Int8) -> 'a'; _ -> 'q')
      (curry \case (True, 1) -> 'b'; _ -> 'q')
  , TestCase $ assertBool "cross differ" $ not $ (==??!)
      (curry \case (True, 10 :: Int8) -> 'a'; _ -> 'q')
      (curry \case (True, 1) -> 'b'; _ -> 'q')
  ]

testReadInts :: Test
testReadInts = TestList
  [ TestCase $ assertEqual "read fibs" (take 22 fibs) $ evalUnsafeMyIO (readInts 22) (world fibs)
  , TestCase $ assertEqual "read nothing" [] $ evalUnsafeMyIO (readInts 0) (world fibs)
  ]
  where
    fibs = 0 : 1 : zipWith (+) fibs (tail fibs)

testSummerAppMain :: Test
testSummerAppMain = TestList
  [ TestCase $ assertEqual "summerAppMain: 3 и 4" (unsafeWorld [] [7]) $
      execUnsafeMyIO summerAppMain (world [3, 4])
  ]

testSumN :: Test
testSumN = TestList
  [ TestCase $ assertEqual "empty world 0: " (emptyWorld, Just 0) $ task 0 emptyWorld
  , TestCase $ assertEqual "empty world 1: " (emptyWorld, Nothing) $ task 1 emptyWorld
  , TestCase $ assertEqual "1 + 2 + eof: " (emptyWorld, Nothing) $ task 4 emptyWorld
  , TestCase $ assertEqual "1 + 2 + 3 + 4 + 5 ..." (emptyWorld, Just 21) $ task 6 (world [1 .. 6])
  , TestCase $ assertEqual "не прочитали лишнего" (world [7], Just 21) $ task 6 (world [1 .. 7])
  ]
  where
    task = unwrap . sumN
    unwrap = runUnsafeMyIO . getCompose
