module SpecBlock2 where

import Block2
import Data.List qualified as List
import Data.List.NonEmpty qualified as NE
import Data.Map qualified as Map
import Test.Prelude

tests :: NamedTests
tests = nameTests 2
  [ testWordCount
  , testWordCountShow
  , testInvertedIndex
  , testFindMaxFrequency
  ]

testWordCount :: Test
testWordCount = TestList
  [ TestCase $ assertEqual "no words" Map.empty $ wordCount []
  , TestCase $ assertEqual "empty lines" Map.empty $ wordCount ["", ", :"]
  , TestCase $
      let expected = Map.fromList [("s", 1), ("alice", 1)] in
      let actual = wordCount ["Alice's"] in
      assertEqual "puctuation in words" expected actual
  , TestCase $
      let expected = Map.fromList [("text", 2), ("line1", 1), ("line2", 1)] in
      let input = ["text: line1.", "TexT, line2"] in
      assertEqual "non-trivial word count example" expected (wordCount input)
  ]

testWordCountShow :: Test
testWordCountShow = TestList
  [ TestCase $ assertEqual "empty word count show" "" $ wordCountShow stdWcShow Map.empty
  , TestCase $ assertEqual "small word count show" "bcdf : 2\n" $ wordCountShow stdWcShow $ Map.fromList [("a", 1), ("bcdf", 2)]
  , TestCase $
      let expected = "submarine : 6\nyellow : 6\nlive : 2\nsame : 2\ntesting : 2\n" in
      let actual = wordCountShow stdWcShow { topSize = 5 } $ wordCount submarine in
      assertEqual "word count show works on alice" expected actual
  ]
  where
    submarine =
      [ "We all live in a yellow submarine"
      , "Yellow submarine, yellow submarine"
      , "We all live in a yellow submarine"
      , "Yellow submarine, yellow submarine"
      , "testing testing same same words"
      ]

testInvertedIndex :: Test
testInvertedIndex = TestList
  [ TestCase $ assertEqual "no documents" Map.empty $ buildInvertedIndex []
  , TestCase $
      let (a, b) = (Document "A" ["me you"], Document "B" ["you test"]) in
      let expected = Map.fromList $ fmap (NE.fromList . List.sort) <$>
            [ ("me", [IndexNode a 0.5])
            , ("you", [IndexNode a 0.5, IndexNode b 0.5])
            , ("test", [IndexNode b 0.5])
            ] in
      assertEqual "small documents list" expected $ NE.sort <$> buildInvertedIndex [a, b]
  ]

testFindMaxFrequency :: Test
testFindMaxFrequency = TestList
  [ TestCase $ assertEqual "no documents" Nothing $ find [] ""
  , TestCase $ assertEqual "no word" Nothing $ find [Document "A" ["y"]] "x"
  , TestCase $
      let docs = [Document "A" ["a a", "a", "a a"], Document "B" ["a a", "b a", "a a a", "a"]] in
      assertEqual "most used word" (Just "A") $ find docs "a"
  ]
  where
    find = findMostRelevant . buildInvertedIndex

deriving stock instance Ord Document
deriving stock instance Ord IndexNode
