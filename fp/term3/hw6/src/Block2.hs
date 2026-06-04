{-# LANGUAGE ScopedTypeVariables #-}
{-# OPTIONS_GHC -Wno-missing-signatures #-}
{-# OPTIONS_GHC -Wno-type-defaults #-}

-- | Стандартные классы типов для однопараметрических типов.
-- Локальный минимум: 3б.
module Block2 where

import Data.Typeable (Typeable, cast)
import MetaUtils
import Unsafe.Coerce (unsafeCoerce)

-- 1. (1б)
-- Класс типов StructEq предназначен для сравнения структур контейнеров.
-- Типы контейнеров однопараметрические, зависят от типа элементов контейнера.
-- Реализуйте StructEq для списков и деревьев.

class StructEq (f :: Type -> Type) where
  (==#) :: f a -> f b -> Bool

data Tree a = Leaf | Node a [Tree a]
  deriving (Eq, Show)

instance StructEq [] where
  [] ==# [] = True
  (_ : xs) ==# (_ : ys) = xs ==# ys
  _ ==# _ = False

instance StructEq Tree where
  Leaf ==# Leaf = True
  Node _ c ==# Node _ c' = length c == length c' && and (zipWith (==#) c c')
  _ ==# _ = False

-- 2. (1б)
-- Сделайте дерево представителем класса типов Functor.
-- Функция fmap - это как map для списков, только обобщённая по контейнеру.
-- Она принимает контейнер и трансформирует все его элементы. Она должна удовлетворять
-- главному закону: xs ==# fmap f xs.

instance Functor Tree where
  fmap _ Leaf = Leaf
  fmap f (Node x c) = Node (f x) (fmap (fmap f) c)

-- 3. (1б)
-- Сделайте данный тип представителем класса типов Functor.
-- Воспользуйтесь тем, что список, функциональная стрелка и пара уже являются функторами.

newtype PairReaderList e p a = PairReaderList
  { getPairReaderList :: [e -> (p, a)]
  }

type TestPRLFunctor = Result2 '[Functor :> Todo]

-- 4. (1б)
-- Сделайте пару представителем StructEq.

todoImpl ''StructEq ''(,)

-- 5. (3б)
-- По аналогии с тем, как классы типов реализованы в Haskell, реализуйте такую
-- структуру MyFunctor, чтобы следующий код компилировался.
-- Полный балл если заставите компилироваться 4й случай.

-- newtype MyFunctor

-- myListFmap = MkMyFunctor map
-- myMaybeFmap = MkMyFunctor $ \f m -> case m of
--  Nothing -> Nothing
--  Just a -> Just (f a)

case1 = todo "case1" -- myFmap myListFmap (^ 2) [1 .. 10]

case2 = todo "case2" -- myFmap myListFmap show [1 .. 10]

case3 = todo "case3" -- myFmap myMaybeFmap ('y' :) (Just "x")

case4 = todo "case4" -- go myListFmap where go d = (myFmap d (^ 2) [1 .. 10], myFmap d ('y' :) ["a"])
