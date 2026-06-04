-- | Реализация собственных парсер-комбинаторов и парсинг с их помощью.
-- Локальный минимум: 5б.
module Block3 where

import Control.Applicative hiding (Const (..))
import Data.Char qualified as Char
import Lexer (Token)
import MetaUtils
import Text.Read (readMaybe)

-- Простенький язык программирования, который мы будем парсить.

data BinOp = Plus | Minus | Mult deriving (Eq)

data Expr = Const Int | Var String | BinOp BinOp Expr Expr deriving (Eq)

instance Show BinOp where
  show = \case Plus -> "+"; Minus -> "-"; Mult -> "*"

instance Show Expr where
  show = \case
    Const n -> show n
    Var name -> name
    BinOp op l r -> "(" ++ show l ++ " " ++ show op ++ " " ++ show r ++ ")"

-- Заметьте, что писать парсеры вручную сложно из-за количества рутинной работы, которую
-- приходится повторять раз за разом.
-- Для этого реализуем несколько стандартных классов типов. Так, мы не только избавимся
-- от рутинного кода, но и сможем переиспользовать большое количество стандартных функций,
-- которые реализованы обобщённо для любых типов-представителей этих классов типов.

newtype MyParser a = MyParser {runMyParser :: [Token] -> Maybe ([Token], a)}

-- Обернём имеющиеся парсеры в коробочки, чтобы для них работали наши
-- реализации классов типов.
satisfy :: (Token -> Bool) -> MyParser Token
satisfy p = MyParser \ts -> case ts of
  t : ts' | p t -> Just (ts', t)
  _ -> Nothing

token :: Token -> MyParser Token
token t = satisfy (== t)

anyToken :: MyParser Token
anyToken = satisfy (const True)

-- 1. (1б)
-- Заметим, что нередко парсер парсит одно значение, а нам вместо него нужно другое.
-- Сделайте MyParser представителем Functor с семантикой: сделать дополнительное
-- преобразование над значением после парсинга.
-- Реализуйте parseConst через fmap, не используя конструктор MyParser.

type TestMyParserFunctor = Result2 '[Functor :> Done]

instance Functor MyParser where
  fmap f (MyParser p) = MyParser $ \ts -> case p ts of
    Just (ts', a) -> Just (ts', f a)
    Nothing -> Nothing

numberToken :: MyParser Token
numberToken = satisfy $ \t -> case readMaybe t :: Maybe Int of
  Just _ -> True
  Nothing -> False

parseConst :: MyParser Expr
parseConst = fmap (Const . read) numberToken

-- 2. (1б)
-- Реализуйте Applicative для MyParser с семантикой: получается парсер, который
-- сначала запускает первый данный парсер, если он закончился с успехом, запускает
-- второй и агрегирует результаты с помощью функции. Если хотя бы один парсер завершился
-- неудачей, то новый парсер тоже завершатся неудачей.
-- Используя полученные инстансы, реализуйте парсер, который разбирает выражения вида '1 + 2'.

type TestMyParserApplicative = Result2 '[Functor :> Done, Applicative :> Done]

instance Applicative MyParser where
  pure :: a -> MyParser a
  pure a = MyParser $ \ts -> Just (ts, a)

  (<*>) :: MyParser (a -> b) -> MyParser a -> MyParser b
  MyParser f <*> MyParser x = MyParser $ \ts -> case f ts of
    Nothing -> Nothing
    Just (ts', f') -> case x ts' of
      Nothing -> Nothing
      Just (ts'', x') -> Just (ts'', f' x')

parseTrivialBinOpExpr :: MyParser Expr
parseTrivialBinOpExpr = (\l op r -> BinOp op l r) <$> parseConst <*> parseBinOp <*> parseConst

-- 3. (1б)
-- Реализуйте Alternative для MyParser с семантикой: получается парсер, который
-- пробует запустить первый данный парсер. Если он завершился неудачей, то пытается
-- запустить второй на том же входе.
-- Используя новые инстансы и базовые парсеры, скомбинируйте их в parseBinOp.

type TestMyParserAlternative = Result2 '[Functor :> Done, Applicative :> Done, Alternative :> Done]

instance Alternative MyParser where
  empty = MyParser $ const Nothing

  (<|>) :: MyParser a -> MyParser a -> MyParser a
  MyParser p <|> MyParser p' = MyParser $ \ts -> case p ts of
    Just res -> Just res
    Nothing -> p' ts

parseBinOp :: MyParser BinOp
parseBinOp = (Plus <$ token "+") <|> (Minus <$ token "-") <|> (Mult <$ token "*")

-- 4. (1б)
-- Снова напишите парсер, который считывает выражениe в польской (префиксной, см. wiki)
-- нотации и строит по нему AST.

parsePolish :: MyParser Expr
parsePolish = parseConst <|> liftA3 BinOp parseBinOp parsePolish parsePolish

-- 5. (1б)
-- Напишите парсер полноценных выражений со скобочками: (1 + 2) * 3.

-- Принимает парсер и возвращает такой же, но который в начале разбора и в конце
-- ожидает по скобочке: '<something>' => '(<something>)'
inParens :: MyParser a -> MyParser a
inParens = todo "inParens"

-- Неделимая часть выражения. Что ни допиши справа, результат не изменится.
parseAtom :: MyParser Expr
parseAtom = todo "parseAtom"

parseParensExpr :: MyParser Expr
parseParensExpr = todo "parseParensExpr"

-- 6. (1б)
-- Напишите парсер выражений, в которых можно опускать скобочки
-- (учтите приоритеты операторов).

parseExpr :: MyParser Expr
parseExpr = todo "parseExpr"
