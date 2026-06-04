-- | Лексический анализатор.
-- Существенно упрощает парсер.
module Lexer where

import Data.Char qualified as Char
import Data.List qualified as List

-- | Неделимая часть грамматики. Например, идентификатор, ключевое слово, оператор...
type Token = String

-- | Разбивает программу на токены, выкидывая незначащие символы:
-- Например: `"2     + 3"` превратиться в `["2", "+", "3"]`.
tokenize :: String -> [Token]
tokenize = concatMap (splitBySelector selector) . words
  where
    selector s
      | Just prefix <- List.find (`List.isPrefixOf` s) symbols = prefix
      | c:_ <- s, isDelimiter c = [c]
      | otherwise = []
    symbols = []
    delimiters = [Char.isMark, Char.isPunctuation, Char.isSymbol]
    isDelimiter c = any ($ c) delimiters

splitBySelector :: ([a] -> [a]) -> [a] -> [[a]]
splitBySelector selectFromPrefix xs = case breakSelected xs of
  (prefix, selected, suffix) -> consNotNull prefix $ consNotNull selected $
    if null suffix then [] else splitBySelector selectFromPrefix suffix
  where
    consNotNull xs
      | null xs = id
      | otherwise = (xs :)

    breakSelected xs = case (xs, selectFromPrefix xs) of
      ([], _) -> ([], [], [])
      (x:xs, []) -> case breakSelected xs of (prefix, ys, zs) -> (x:prefix, ys, zs)
      (xs, ys) -> ([], ys, drop (length ys) xs)
