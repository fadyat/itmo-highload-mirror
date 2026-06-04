{-# OPTIONS_GHC -Wno-unused-imports #-}

-- | Свёртки, полугруппы, моноиды.
-- Локальный минимум: 4б.
module Block1 where

import Control.Applicative (liftA2, (<|>))
import Data.Bool (bool)
import Data.List (unfoldr)
import Data.Monoid (First (..), Sum (..))
import MetaUtils

-- 1. (0.5б)
-- Используя `foldr`, напишите функцию, которая проверяет, есть ли заданный элемент в списке.
-- Решение должно работать на бесконечных списках.

elem' :: (Eq a) => a -> [a] -> Bool
elem' x = foldr (\y res -> y == x || res) False

-- 2. (1б)
--  * `uncurry` является свёрткой для пары
--  * `either` является свёрткой для `Either`
--  * `foldNat` является свёрткой для `Nat`

-- Значит, с помощью этих функций можно выразить что угодно, что можно было бы
-- сделать через сопоставление с образцом по данным типам.
-- Убедитесь в этом: напишите функцию, которая принимает на вход `Either (Nat, Nat) String` и в случае,
-- когда в экземпляре лежит пара, находит сумму её элементов, а когда строка, то её длину.
-- При этом нельзя делать сопоставление с образцом или использовать функции помимо `foldr`, `uncurry`,
-- `either`, `foldNat` и стандартных комбинаторов (`const`, `(.)` и всего такого).

data Nat = Z | S Nat deriving (Eq, Show, Ord)

foldNat :: a -> (a -> a) -> Nat -> a
foldNat z _ Z = z
foldNat z s (S n) = s (foldNat z s n)

sumOrLength :: Either (Nat, Nat) String -> Nat
sumOrLength =
  either
    (uncurry (\x y -> foldNat y S x))
    (foldr (const S) Z)

-- 3. (1б)
-- Используя `unfoldr`, реализуйте `map`, не конструируя список явно через его конструкторы.
-- Решение должно работать на бесконечных списках.

map' :: (a -> b) -> [a] -> [b]
map' f =
  unfoldr
    ( \xs -> case xs of
        [] -> Nothing
        (x : xt) -> Just (f x, xt)
    )

-- 4. (1б)
-- Напишите функцию, которая подсчитывает количество элементов,
-- соответствующих предикату, используя `foldMap` с подходящим моноидом.

countMatching :: (Foldable t, Num n) => (a -> Bool) -> t a -> n
countMatching f x =
  getSum (foldMap (\x -> if f x then Sum 1 else Sum 0) x)

-- 5. (1б)
-- Сделайте следующую структуру данных представителем класса типов Foldable так,
-- чтобы сначала в обход попадал корень, а затем поддеревья по порядку.

data Tree a = Leaf | Node a [Tree a]
  deriving (Eq, Show)

type TestTreeFoldable = Result2 '[Foldable :> Done]

instance Foldable Tree where
  foldMap _ Leaf = mempty
  foldMap f (Node x ts) = f x <> foldMap (foldMap f) ts

-- 6. (1б)
-- Как известно, словарь - это частичная функция (определена не на всём домене).
-- Реализуйте следующие операции над словарём MyMap, заданным частичной функцией.

newtype MyMap k v = MyMap {myLookup :: k -> Maybe v}

myEmpty :: MyMap k v
myEmpty = MyMap (const Nothing)

myInsert :: (Eq k) => k -> v -> MyMap k v -> MyMap k v
myInsert key value (MyMap lookup) =
  MyMap (\k -> if k == key then Just value else lookup k)

myInsertMany :: (Eq k, Foldable t) => MyMap k v -> t (k, v) -> MyMap k v
myInsertMany m pairs = foldl (\m (k, v) -> myInsert k v m) m pairs

myDelete :: (Eq k) => k -> MyMap k v -> MyMap k v
myDelete key (MyMap lookup) = MyMap (\k -> if k == key then Nothing else lookup k)

myDeleteMany :: (Eq k, Foldable t) => MyMap k v -> t k -> MyMap k v
myDeleteMany m keys = foldl (\m k -> myDelete k m) m keys

-- 7. (1б)
-- Сделайте MyMap представителем полугруппы и моноида относительно объединения словарей.

type TestMyMapMonoid = Result1 '[Semigroup :> Done, Monoid :> Done]

instance Semigroup (MyMap k v) where
  MyMap lookup <> MyMap lookup' = MyMap $ \k ->
    case lookup k of
      Just v -> Just v
      Nothing -> lookup' k

instance Monoid (MyMap k v) where
  mempty = myEmpty

-- 8. (1б)
-- Сделайте MyMap представителем Foldable в случае, если тип ключа является представителем Enum и Bounded.

type TestMyMapFoldable = Result2 '[Foldable :> Done]

instance (Enum k, Bounded k) => Foldable (MyMap k) where
  foldMap f (MyMap lookup) = foldMap (\k -> maybe mempty f (lookup k)) [minBound .. maxBound]

-- 9. (2б)
-- Сделайте экземпляром `Foldable` дерево в представлении "левый ребёнок, правый сосед":
--
--    Node 1 ---------------- Node 2 ------------------------- Node 3 -- Leaf
--   /                          /                               /
-- Node 4 -- Leaf            Node 5 ------- Node 6 -- Leaf   Node 7 -- Leaf
--   /                        /                /               /
-- Leaf                    Node 8 -- Leaf   Node 9 -- Leaf   Leaf
--                          /                /
--                        Leaf             Node 10 -- Leaf
--                                         /
--                                       Leaf
--
-- Node 1 (Node 4 Leaf Leaf)
--        (Node 2 (Node 5 (Node 8 Leaf Leaf)
--                        (Node 6 (Node 9 (Node 10 Leaf Leaf) Leaf) Leaf))
--                (Node 3 (Node 7 Leaf Leaf) Leaf))
--
-- Порядок обхода: сначала все вершины на первом уровне, потом все вершины на
-- втором и так далее. На представленном в примере дереве требуемый порядок обхода
-- совпадает с порядком на вершинах.

data NeighborTree a
  = NLeaf
  | NNode
      a -- value
      (NeighborTree a) -- left child
      (NeighborTree a) -- right neighbor
  deriving (Eq)

type TestNeighborTreeFoldable = Result2 '[Foldable :> Todo]

-- ~ сюда не смотрим ~
-- Всякий бойлерплейт для тестирования и печати.

deriving stock instance Generic (NeighborTree a)

deriving stock instance Functor NeighborTree

instance (Out a) => Out (NeighborTree a)

instance (Show a) => Show (NeighborTree a) where
  show = pretty . fmap OutShow
