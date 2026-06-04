module SpecBlock2 where

import Block2
import Data.Map qualified as Map
import Data.Set qualified as Set
import MyIO
import Test.Prelude

tests :: NamedTests
tests = nameTests 2
  [ testBossBoss
  , testDubEven
  , testEdges
  , testCenterMass
  , testProbSolution
  , testProbShark
  ]

testBossBoss :: Test
testBossBoss = TestList
  [ TestCase $ assertEqual "no auth" Nothing $ bossBoss (UserId 1 `Set.delete` auth) users (UserId 1)
  , TestCase $ assertEqual "no user" Nothing $ bossBoss auth (UserId 1 `Map.delete` users) (UserId 1)
  , TestCase $ assertEqual "no boss" Nothing $ bossBoss auth (UserId 2 `Map.delete` users) (UserId 1)
  , TestCase $ assertEqual "boss boss" (Just $ UserId 3) $ userId <$> bossBoss auth users (UserId 1)
  ]
  where
    auth = Set.fromList [UserId 1, UserId 2, UserId 3, UserId 4]
    user1 = User (UserId 1) (UserName "Bob") (Just $ UserId 2)
    user2 = User (UserId 2) (UserName "Alice") (Just $ UserId 3)
    user3 = User (UserId 3) (UserName "Frank") (Just $ UserId 4)
    user4 = User (UserId 4) (UserName "Koka") (Just $ UserId 10)
    users = Map.fromList $ map (\user -> (userId user, user))
      [user1, user2, user3, user4]

testDubEven :: Test
testDubEven = TestList
  [ TestCase $ assertEqual "dibEven empty list" [] $ dubEven []
  , TestCase $ assertEqual "dibEven nonempty list" [0, 0, 1, 2, 2, 3, 4, 4, 5] $ dubEven [0, 1, 2, 3, 4, 5]
  ]

testEdges :: Test
testEdges = TestList
  [ TestCase $ assertEqual "edges1" result $ take (length result) edges1
  , TestCase $ assertEqual "edges2" result $ take (length result) edges2
  ]
  where
    result =
      [ (5, 4, 3)
      , (10, 8, 6)
      , (13, 12, 5)
      , (15, 12, 9)
      , (17, 15, 8)
      , (20, 16, 12)
      , (25, 20, 15)
      , (25, 24, 7)
      , (26, 24, 10)
      , (29, 21, 20)
      , (30, 24, 18)
      , (34, 30, 16)
      , (35, 28, 21)
      , (37, 35, 12)
      , (39, 36, 15)
      , (40, 32, 24)
      , (41, 40, 9)
      , (45, 36, 27)
      , (50, 40, 30)
      , (50, 48, 14)
      ]

testCenterMass :: Test
testCenterMass = TestList
  [ TestCase $ assertEqual "one object" (unsafeWorld [] [2, 3, 4]) $ fst $ runUnsafeMyIO centerMain $ world [1, 2, 3, 4]
  , TestCase $ assertEqual "three objs" (unsafeWorld [] [4, 10, 5]) $ fst $ runUnsafeMyIO centerMain $ world
      [ 3, 4, 42, 5
      , 2, 3, 10, 23
      , 10, 5, 1, 2
      ]
  ]

testProbSolution :: Test
testProbSolution = TestList
  [ TestCase $ assertBool "probSolution 10" $ cmp 0.138 $ probSolution 10
  , TestCase $ assertBool "probSolution 10" $ cmp 0.00434 $ probSolution 33
  ]
  where
    cmp q w = abs (q - w) < 0.00001

testProbShark :: Test
testProbShark = TestList
  [ TestCase $ assertDoubleEq "probShark 1" 1 $ probShark 1
  , TestCase $ assertDoubleEq "probShark 2" 0.75 $ probShark 2
  , TestCase $ assertDoubleEq "probShark 3" 0.4444444444 $ probShark 3
  , TestCase $ assertDoubleEq "probShark 4" 0.4453125 $ probShark 4
  , TestCase $ assertDoubleEq "probShark 5" 0.3766400000 $ probShark 5
  , TestCase $ assertDoubleEq "probShark 6" 0.3556884431 $ probShark 6
  ]
  where
    assertDoubleEq msg a b = assertBool msg (abs (a - b) < 0.000001)
