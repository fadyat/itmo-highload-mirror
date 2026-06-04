{-# OPTIONS_GHC -Wno-missing-signatures #-}

-- | Структуры данных в Haskell.
-- Локальный минимум: 3б.
module Block1 where

import Data.Foldable
import MetaUtils
import System.Random (mkStdGen, randomIO, setStdGen)

-- 1. (1б)
-- Реализуйте две функции:

-- * makeNat, которая по числу строит число в унарной записи

-- * foldNat, которая делает что-то n раз:

--   foldNat (1+) 0 (Suc (Suc (Suc Zero))) == 3

data Nat = Zero | Suc Nat
  deriving (Show) -- Магия даёт функцию show :: Nat -> String для печати

makeNat :: Int -> Nat
makeNat n
  | n == 0 = Zero
  | n > 0 = Suc (makeNat (n - 1))
  | otherwise = error "makeNat: negative number"

foldNat :: (a -> a) -> a -> Nat -> a
foldNat _ z Zero = z
foldNat f z (Suc n) = f (foldNat f z n)

-- 2. (1б)
-- Реализуйте тип данных IntTree, который может быть построен одним из двух способов:
--  * Leaf: В экземпляре нет дополнительных данных;
--  * Node: В экземпляре содержится число типа Int, а также список детей IntTree.

data IntTree = Leaf | Node Int [IntTree]

-- Когда определите структуру данных, замените эти две строки на
-- leaf = Leaf
-- node = Node
-- Это изощрение нужно, чтобы тестовый файл компилировался ещё до появления
-- конструкторов IntTree.

leaf = Leaf

node = Node

-- Напишите функцию, которая просуммирует все значения в дереве.

sumIntTree :: IntTree -> Int
sumIntTree Leaf = 0
sumIntTree (Node x xs) = x + foldr ((+) . sumIntTree) 0 xs

-- 3. (1.5б)
-- Дано полиморфное бинарное дерево.

data BinTree a = Empty | BinNode (BinTree a) a (BinTree a)
  deriving (Show, Eq)

-- Определите функцию, которая заменяет каждый элемент одного типа другим.
-- С помощью принимаемой функции.

mapBinTree :: (a -> b) -> BinTree a -> BinTree b
mapBinTree _ Empty = Empty
mapBinTree f (BinNode l x r) = BinNode (mapBinTree f l) (f x) (mapBinTree f r)

-- Напишите функцию, которая принимает дерево деревьев чисел и возвращает дерево деревьев строк.
-- Каждая строка состоит из соответствующего числа из исходного дерева и восклицательного знака после.
-- Напишите сигнатуру этой функции самостоятельно

bangAll :: BinTree (BinTree Int) -> BinTree (BinTree String)
bangAll = mapBinTree (mapBinTree (\n -> show n ++ "!"))

-- 4. (7б)
-- Реализуйте эффективный персистентный массив на основе
-- декартового дерева по неявному ключу:
--  * https://habr.com/ru/articles/240519/
--  * https://ru.algorithmica.org/cs/tree-structures/implicit/

data DNode a = DNode
  { treeSize :: Int,
    treeKey :: Int,
    treeElem :: a,
    treeLeft :: DTree a,
    treeRight :: DTree a
  }

type DTree a = Maybe (DNode a) -- root

-- Взятие случайного числа не является чистой операцией. Поэтому все операции
-- принимают ключ явно как параметр, а следующая магия умеет генерировать деревья
-- со случайными значениями:
-- ghci> tree <- treeOf [1, 2, 3]
treeOf :: [a] -> IO (DTree a)
treeOf xs = do
  setStdGen (mkStdGen 42)
  foldlM (\t x -> append t x . (`mod` 1000) <$> randomIO) empty xs

split :: Int -> DTree a -> (DTree a, DTree a)
split = todo "split"

merge :: DTree a -> DTree a -> DTree a
merge = todo "merge"

-- Принимает дерево и индекс для поиска элемента.
(!!?) :: DTree a -> Int -> Maybe a
(!!?) = todo "!!?"

empty :: DTree a
empty = todo "empty"

-- Принимает элемент и ключ.
singleton :: a -> Int -> DTree a
singleton = todo "singleton"

-- Принимает дерево, элемент, ключ.
append :: DTree a -> a -> Int -> DTree a
append = todo "append"

-- Принимает индекс, элемент, ключ, дерево.
insert :: Int -> a -> Int -> DTree a -> DTree a
insert = todo "insert"

remove :: DTree a -> Int -> DTree a
remove = todo "remove"

size :: DTree a -> Int
size = todo "size"

-- ~ сюда не смотрим ~
-- Всякий бойлерплейт для тестирования и печати.

deriving stock instance Generic (DNode a)

instance (Out a) => Out (DNode a)

instance (Out a) => Show (DNode a) where
  show = pretty
