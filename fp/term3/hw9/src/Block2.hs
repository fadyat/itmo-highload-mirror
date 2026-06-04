-- | Рукописные парсеры с помощью рекурсивного спуска.
-- Локальный минимум: 1б.
module Block2 where

import Lexer (Token)
import MetaUtils
import Text.Read (readMaybe)

-- Простенький язык программирования, который мы будем парсить.

data BinOp = Plus | Minus deriving (Eq)

data Expr = Const Int | BinOp BinOp Expr Expr deriving (Eq)

instance Show BinOp where
  show = \case Plus -> "+"; Minus -> "-"

instance Show Expr where
  show = \case
    Const n -> show n
    BinOp op l r -> "(" ++ show l ++ " " ++ show op ++ " " ++ show r ++ ")"

-- Примеры простейших парсеров, которые можно переиспользовать.
-- Парсером будем называть функцию из списка токенов в Maybe пару из списка
-- оставшихся токенов и результата парсинга. Nothing обозначает неудачу.
-- Например:
-- parseInt :: [Token] -> Maybe ([Token], Int)
-- parseInt ["3", "+", "2"] ~> Just (["+", "2"], 3)
-- parseInt ["+", "2"] ~> Nothing

-- Парсит только первый токен из потока, если он удовлетворяет предикату.
-- Через этот парсер удобно выражать другие.
satisfy :: (Token -> Bool) -> [Token] -> Maybe ([Token], Token)
satisfy p ts = case ts of
  t : ts' | p t -> Just (ts', t)
  _ -> Nothing

token :: Token -> [Token] -> Maybe ([Token], Token)
token t = satisfy (== t)

anyToken :: [Token] -> Maybe ([Token], Token)
anyToken = satisfy (const True)

parseBinOp :: [Token] -> Maybe ([Token], BinOp)
parseBinOp ts = case token "+" ts of
  Just (ts', _) -> Just (ts', Plus)
  Nothing -> case token "-" ts of
    Just (ts', _) -> Just (ts', Minus)
    Nothing -> Nothing

-- 1. (0.5б)
-- Реализуйте функцию, разбирающую константные выражения (e.g. "1").

parseConst :: [Token] -> Maybe ([Token], Expr)
parseConst [] = Nothing
parseConst (x : xs) = case readMaybe x of
  Just n -> Just (xs, Const n)
  Nothing -> Nothing

-- 2. (1б)
-- Напишите рекурсивный парсер, который считывает выражениe в польской (префиксной, см. wiki)
-- нотации и строит по нему AST. Всё что вам нужно - паттерн-матчинг, рекурсия и готовые
-- парсеры, написанные выше.
-- Примеры:
--  * parsePolish (tokenize "+ 2 3")     ~> BinOp Plus (Const 2) (Const 3)
--  * parsePolish (tokenize "+ 2 * 3 5") ~> BinOp Plus (Const 2) (BinOp Mult (Const 3) (Const 5))
--  * parsePolish (tokenize "+ * 2 3 5") ~> BinOp Plus (BinOp Mult (Const 2) (Const 3)) (Const 5)
--  * parsePolish (tokenize "+ + 1")     ~> НЕУДАЧА

parsePolish :: [Token] -> Maybe ([Token], Expr)
parsePolish ts = case parseBinOp ts of
  Nothing -> parseConst ts
  Just (ts', op) -> case parsePolish ts' of
    Nothing -> Nothing
    Just (ts'', left) -> case parsePolish ts'' of
      Nothing -> Nothing
      Just (ts''', right) -> Just (ts''', BinOp op left right)

-- 3. (1.5б)
-- Ассоциативную бинарную операцию в инфиксной форме можно представить как
-- начальное значение, и список пар (оператор, правый операнд):
-- 2 + 3 - 5 => (2, [('+', 3), ('-', 5)])
-- Напишите функцию, которая парсит выражение из чисел и бинарных операций в такое представление.

-- Переписывать тип парсера мы уже устали, введём для него newtype.
newtype MyParser a = MyParser {runMyParser :: [Token] -> Maybe ([Token], a)}

parseAssoc :: MyParser (Expr, [(BinOp, Expr)])
parseAssoc = MyParser $ \tokens ->
  case runMyParser (MyParser parseConst) tokens of
    Nothing -> Nothing
    Just (tokens', first) ->
      let go ts = case parseBinOp ts of
            Nothing -> Just (ts, [])
            Just (ts', op) ->
              case runMyParser (MyParser parseConst) ts' of
                Nothing -> Nothing
                Just (ts'', expr) -> case go ts'' of
                  Nothing -> Nothing
                  Just (tsFinal, ops) -> Just (tsFinal, (op, expr) : ops)
       in case go tokens' of
            Nothing -> Nothing
            Just (tokensFinal, ops) -> Just (tokensFinal, (first, ops))

-- 4. (1б)
-- Фух, ну давайте допишем парсинг простых выражений без скобочек и отдохнём.
-- Проверьте в консоли:
-- ghci> runMyParser parseBinOpExpr $ tokenize "2 + 3 - 5"

parseBinOpExpr :: MyParser Expr
parseBinOpExpr = MyParser $ \tokens ->
  case runMyParser parseAssoc tokens of
    Nothing -> Nothing
    Just (tokens', (first, ops)) ->
      let expr = foldl (\acc (op, e) -> BinOp op acc e) first ops
       in Just (tokens', expr)
       
