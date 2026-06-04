-- | Класс типов Traversable.
-- Локальный минимум: 2б.
module Block1 where

import Control.Applicative
import Data.Coerce
import Data.Foldable (traverse_)
import Data.Traversable
import MetaUtils
import MyIO

-- 1. (1б)
-- Напишите функцию, которая печатает `n` чисел Фибоначчи в "консоль".

printFibs :: Int -> MyIO ()
printFibs n = mapM_ writeInt (take n fibs)
  where
    fibs = 0 : 1 : zipWith (+) fibs (tail fibs)

-- 2. (1.5б)
-- Сделайте точку в трёхмерном пространстве представителем Applicative и Traversable.

data Point3 a = Point3 a a a
  deriving (Show, Eq, Functor)

instance Applicative Point3 where
  pure :: a -> Point3 a
  pure a = Point3 a a a

  (<*>) :: Point3 (a -> b) -> Point3 a -> Point3 b
  Point3 f g h <*> Point3 x y z = Point3 (f x) (g y) (h z)

instance Foldable Point3 where
  foldMap :: (Monoid m) => (a -> m) -> Point3 a -> m
  foldMap f (Point3 x y z) = f x <> f y <> f z

instance Traversable Point3 where
  traverse :: (Applicative f) => (a -> f b) -> Point3 a -> f (Point3 b)
  traverse f (Point3 x y z) = Point3 <$> f x <*> f y <*> f z

-- Напечатайте на "консоль" все компоненты двух переданных вам векторов, а затем
-- компоненты их суммы. Воспользуйтесь реализованными инстансами классов типов.

printSum :: Point3 Int -> Point3 Int -> MyIO ()
printSum p1 p2 =
  traverse_ writeInt p1
    *> traverse_ writeInt p2
    *> traverse_ writeInt (liftA2 (+) p1 p2)

-- 3. (1.5б)
-- Вдохновитесь вызовом `sequenceA (Point3 (Point3 1 2 3) (Point3 4 5 6) (Point3 7 8 9))`
-- и напишите транспонирование матриц, используя подходящие инстансы Applicative и Traversable
-- для списка. Ручное редуцирование, как всегда, - лучший способ понять что-то.

transpose :: [[a]] -> [[a]]
transpose = todo "transpose"

-- 4. (2б)
-- Дано обыкновенное бинарное дерево и три обёртки вокруг него. Реализуйте
-- для каждой обёртки экземпляр Traversable с соответствующим названию обёртки
-- порядком обхода дерева.
--  * Preorder -- обходится сначала вершина, затем всё левое поддерево, затем всё
--    правое поддерево.
--  * Inorder -- сначала обходится всё левое поддерево, затем вершина, затем всё
--    правое.
--  * Postorder -- сначала обходится всё левое поддерево, затем всё правое, затем
--    вершина.
-- В тестах есть пример.
-- Обратите внимание, что на реализации более слабых классов типов можно сэкономить.

data Tree1 a = Leaf1 | Node1 (Tree1 a) a (Tree1 a)
  deriving (Eq, Functor)

newtype PreorderTraversal a = PreorderTraversal (Tree1 a)
  deriving newtype (Eq, Functor)

newtype PostorderTraversal a = PostorderTraversal (Tree1 a)
  deriving newtype (Eq, Functor)

newtype InorderTraversal a = InorderTraversal (Tree1 a)
  deriving newtype (Eq, Functor)

instance Foldable PreorderTraversal where
  foldMap _ (PreorderTraversal Leaf1) = mempty
  foldMap f (PreorderTraversal (Node1 l x r)) =
    f x <> foldMap f (PreorderTraversal l) <> foldMap f (PreorderTraversal r)

instance Traversable PreorderTraversal where
  traverse f (PreorderTraversal t) = PreorderTraversal <$> go t
    where
      go Leaf1 = pure Leaf1
      go (Node1 l x r) = liftA3 (\x' l' r' -> Node1 l' x' r') (f x) (go l) (go r)

instance Foldable PostorderTraversal where
  foldMap _ (PostorderTraversal Leaf1) = mempty
  foldMap f (PostorderTraversal (Node1 l x r)) =
    foldMap f (PostorderTraversal l) <> foldMap f (PostorderTraversal r) <> f x

instance Traversable PostorderTraversal where
  traverse f (PostorderTraversal t) = PostorderTraversal <$> go t
    where
      go Leaf1 = pure Leaf1
      go (Node1 l x r) = liftA3 (\l' r' x' -> Node1 l' x' r') (go l) (go r) (f x)

instance Foldable InorderTraversal where
  foldMap _ (InorderTraversal Leaf1) = mempty
  foldMap f (InorderTraversal (Node1 l x r)) =
    foldMap f (InorderTraversal l) <> f x <> foldMap f (InorderTraversal r)

instance Traversable InorderTraversal where
  traverse f (InorderTraversal t) = InorderTraversal <$> go t
    where
      go Leaf1 = pure Leaf1
      go (Node1 l x r) = liftA3 (\l' x' r' -> Node1 l' x' r') (go l) (f x) (go r)

-- ~ красивая печаталка ~
deriving stock instance Generic (Tree1 a)

instance (Out a) => Out (Tree1 a)

instance (Show a) => Show (Tree1 a) where
  show = pretty . coerce @(Tree1 a) @(Tree1 (OutShow a))

deriving newtype instance (Show a) => Show (PreorderTraversal a)

deriving newtype instance (Show a) => Show (PostorderTraversal a)

deriving newtype instance (Show a) => Show (InorderTraversal a)
