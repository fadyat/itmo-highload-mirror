{- HLINT ignore "Use <$>" -}
{-# OPTIONS_GHC "-Wno-missing-signatures" #-}

module SpecBlock3 where

import Control.Applicative hiding (Const (..))
import Block3
import Lexer
import Test.Prelude

tests :: NamedTests
tests = nameTests 3
  [ testMyParserFunctor
  , testMyParserApplicative
  , testMyParserAlternative
  , testParsePolish
  , testParensExpr
  , testParseExpr
  ]

c = Const
v = Var
(.+) = BinOp Plus
(.-) = BinOp Minus
(.*) = BinOp Mult
infixl 7 .*
infixl 6 .+
infixl 6 .-

testMyParserFunctor :: Test
testMyParserFunctor = TestList
  [ TestCase $ assertEqual "Functor MyParser" (Just ([";", "bb"], 3)) $ f (unwrap2 $ length <$> wrap anyToken) "aac;bb"
  , TestCase $ assertEqual "parseConst just" (Just ([";", "kek"], Const 42)) $ f parseConst "42;kek"
  , TestCase $ assertEqual "parseConst nothing" Nothing $ f parseConst "a;kek"
  ]
  where
    f parser = runMyParser parser . tokenize
    wrap = wrap2 @(TestMyParserFunctor MyParser)

testMyParserApplicative :: Test
testMyParserApplicative = TestList
  [ TestCase $ assertEqual "Applicative 1 fails" Nothing $ f (unwrap2 $ pure (,) <*> p1 <*> p2) "1 b"
  , TestCase $ assertEqual "Applicative 2 fails" Nothing $ f (unwrap2 $ pure (,) <*> p1 <*> p2) "a 2"
  , TestCase $ assertEqual "Applicative works" (Just (["lol"], ("a", "b"))) $ f (unwrap2 $ pure (,) <*> p1 <*> p2) "a b lol"
  , TestCase $ assertEqual "parseTrivialBinOpExpr" (Just (["+", "2"], c 1 .+ c 3)) $ f parseTrivialBinOpExpr "1 + 3 + 2"
  ]
  where
    wrap = wrap2 @(TestMyParserApplicative MyParser)
    p1 = wrap $ token "a"
    p2 = wrap $ token "b"
    f parser = runMyParser parser . tokenize

testMyParserAlternative :: Test
testMyParserAlternative = TestList
  [ TestCase $ assertEqual "Alternative 1 ok" (Just (["b"], "a")) $ f (un $ p1 <|> p2) "a b"
  , TestCase $ assertEqual "Alternative 2 ok" (Just (["a"], "b")) $ f (un $ p1 <|> p2) "b a"
  , TestCase $ assertEqual "Alternative not ok" Nothing $ f (un $ p1 <|> p2) "1 b"
  , TestCase $ assertEqual "parseBinOp plus" (Just ([";", "b"], Plus)) $ f parseBinOp "+;b"
  , TestCase $ assertEqual "parseBinOp minus" (Just ([";", "b"], Minus)) $ f parseBinOp "-;b"
  , TestCase $ assertEqual "parseBinOp nothing" Nothing $ f parseBinOp "1;b"
  ]
  where
    un = unwrap2
    wrap = wrap2 @(TestMyParserAlternative MyParser)
    p1 = wrap $ token "a"
    p2 = wrap $ token "b"
    f parser = runMyParser parser . tokenize

testParsePolish :: Test
testParsePolish = TestList
  [ TestCase $ assertEqual "polish '+ 2 3'" (Just (["q"], c 2 .+ c 3)) $ f "+ 2 3 q"
  , TestCase $ assertEqual "polish '1'" (Just ([], c 1)) $ f "1"
  , TestCase $ assertEqual "polish '+'" Nothing $ f "+"
  , TestCase $ assertEqual "polish '+'" Nothing $ f "+ 1"
  , TestCase $ assertEqual "polish '+ 2 - 3 5'" (Just ([], c 2 .+ (c 3 .- c 5))) $ f "+ 2 - 3 5"
  , TestCase $ assertEqual "polish '+ * 2 3 5'" (Just ([], (c 2 .* c 3) .+ c 5)) $ f "+ * 2 3 5"
  , TestCase $ assertEqual "polish '+ + 1'" Nothing $ f "+ + 1"
  ]
  where
    f = runMyParser parsePolish . tokenize

testParensExpr :: Test
testParensExpr = TestList
  [ TestCase $ assertEqual "test with parens 1" (out ((v "name" .+ c 1) .+ c 2)) $ f "((name + 1) + 2)"
  , TestCase $ assertEqual "test with parens 2" (out ((v "name" .+ c 2) .+ c 1)) $ f "((name) + 2) + 1"
  , TestCase $ assertEqual "test with parens 3" (out (v "name" .+ c 1)) $ f "((name + (((1)))))"
  , TestCase $ assertEqual "test with parens 4" (out (((v "name" .+ c 42) .+ c 1) .* (v "result"))) $
      f "((((((name) + 42)))) + ((((1))))) * ((((result))))"
  ]
  where
    out = Just . ([],)
    f = runMyParser parseParensExpr . tokenize

testParseExpr :: Test
testParseExpr = TestList
  [ TestCase $ assertEqual "число парсится" (out $ c 42) (p "42")
  , TestCase $ assertEqual "число в скобках парсится" (out $ c 42) (p "(42)")
  , TestCase $ assertEqual "число с пробелами парсится" (out $ c 42) (p " (  42 )  ")
  , TestCase $ assertEqual "сложение парсится (1)" (out $ c 7 .+ c 5) (p "7+ 5")
  , TestCase $ assertEqual "сложение парсится (2)" (out $ c 7 .+ c 5) (p "7 +5")
  , TestCase $ assertEqual "сложение парсится (3)" (out $ c 7 .+ c 5) (p "7+5")
  , TestCase $ assertEqual "умножение парсится (1)" (out $ c 7 .* c 5) (p "  7   * 5 ")
  , TestCase $ assertEqual "умножение парсится (2)" (out $ c 7 .* c 5) (p "7 * (5) ")
  , TestCase $ assertEqual "умножение парсится (3)" (out $ c 7 .* c 5) (p " 7*5")
  , TestCase $ assertEqual "умножение парсится (4)" (out $ c 4 .* v "hello") (p "4 * hello")
  , TestCase $ assertEqual "умножение парсится (5)" (out $ v "x" .* v "y") (p "x*y")
  , TestCase $ assertEqual "умножение парсится (6)" (out $ c 1 .* c 9) (p " 1*  9 ")
  , TestCase $ assertEqual "умножение парсится (7)" (out $ c 10 .* c 15) (p "((10)*(15))")
  , TestCase $ assertEqual "приоритет правильный"
      (out $ c 1 .+ c 2 .* v "ab" .+ v "x" .+ c 5 .* (c 6 .+ c 7))
      (p "1 + 2* ab +x + 5 * (6 + 7)")
  , TestCase $ assertEqual "ассоциативность правильная (1)" (out $ c 1 .+ c 2 .- c 3) (p "1 + 2 - 3")
  , TestCase $ assertEqual "ассоциативность правильная (3)"
      (out $ c 1 .+ c 2 .* v "ab" .+ v "x" .- c 2 .- v "wow" .+ c 5 .* (c 6 .+ c 7))
      (p "1 + 2* ab +x -2 -wow + 5 * (6 + 7)")
  , TestCase $ assertEqual "сложное выражение"
      (out $ c 9 .+ (c 6 .+ (((c 1 .+ c 7 .* c 2) .* c 3) .* c 5)))
      (p " 9 + (6+(((((1+7*2)*3)*5))))")
  ]
  where
    out = Just . ([],)
    p = runMyParser parseExpr . tokenize
