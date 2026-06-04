{-# OPTIONS_GHC "-Wno-missing-signatures" #-}

module SpecBlock2 where

import Block2
import Lexer
import Test.Prelude

tests :: NamedTests
tests = nameTests 2
  [ testParseTrivial
  , testParsePolish
  , testParseAssoc
  , testParseBinOpExpr
  ]

c = Const
(.+) = BinOp Plus
(.-) = BinOp Minus

testParseTrivial :: Test
testParseTrivial = TestList
  [ TestCase $ assertEqual "satisfy" (Just ([";", "b"], "a")) $ satisfy (== "a") $ t "a;b"
  , TestCase $ assertEqual "parseBinOp plus" (Just ([";", "b"], Plus)) $ parseBinOp $ t "+;b"
  , TestCase $ assertEqual "parseBinOp minus" (Just ([";", "b"], Minus)) $ parseBinOp $ t "-;b"
  , TestCase $ assertEqual "parseBinOp nothing" Nothing $ parseBinOp $ t "1;b"
  , TestCase $ assertEqual "parseConst just" (Just ([";", "kek"], Const 42)) $ parseConst $ t "42;kek"
  , TestCase $ assertEqual "parseConst nothing" Nothing $ parseConst $ t "a;kek"
  ]
  where
    t = tokenize

testParsePolish :: Test
testParsePolish = TestList
  [ TestCase $ assertEqual "polish '+ 2 3'" (Just (["q"], c 2 .+ c 3)) $ f "+ 2 3 q"
  , TestCase $ assertEqual "polish '1'" (Just ([], c 1)) $ f "1"
  , TestCase $ assertEqual "polish '+'" Nothing $ f "+"
  , TestCase $ assertEqual "polish '+'" Nothing $ f "+ 1"
  , TestCase $ assertEqual "polish '+ 2 - 3 5'" (Just ([], c 2 .+ (c 3 .- c 5))) $ f "+ 2 - 3 5"
  , TestCase $ assertEqual "polish '+ - 2 3 5'" (Just ([], (c 2 .- c 3) .+ c 5)) $ f "+ - 2 3 5"
  , TestCase $ assertEqual "polish '+ + 1'" Nothing $ f "+ + 1"
  ]
  where
    f = parsePolish . tokenize

testParseAssoc :: Test
testParseAssoc = TestList
  [ TestCase $ assertEqual "assoc '2 + 3 + 5'" (Just ([], (c 2, [(Plus, c 3), (Plus, c 5)]))) $ f "2 + 3 + 5"
  , TestCase $ assertEqual "assoc '2 + 3 - 5 q'" (Just (["q"], (c 2, [(Plus, c 3), (Minus, c 5)]))) $ f "2 + 3 - 5 q"
  , TestCase $ assertEqual "assoc '-'" Nothing $ f "- 2 + 3 + 5 q"
  ]
  where
    f = runMyParser parseAssoc . tokenize

testParseBinOpExpr :: Test
testParseBinOpExpr = TestList
  [ TestCase $ assertEqual "binOpExpr '2 + 3 + 5'" (Just ([], c 2 .+ c 3 .+ c 5)) $ f "2 + 3 + 5"
  , TestCase $ assertEqual "binOpExpr '2 + 3 - 5 q'" (Just (["q"], c 2 .+ c 3 .- c 5)) $ f "2 + 3 - 5 q"
  , TestCase $ assertEqual "binOpExpr '-'" Nothing $ f "- 2 + 3 + 5 q"
  ]
  where
    f = runMyParser parseBinOpExpr . tokenize
