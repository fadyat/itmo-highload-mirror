{-# OPTIONS_GHC -Wno-missing-signatures #-}
{-# OPTIONS_GHC -Wno-type-defaults #-}
{-# OPTIONS_GHC -Wno-unused-do-bind #-}
{-# OPTIONS_GHC -Wno-unused-local-binds #-}
{-# OPTIONS_GHC -Wno-unused-matches #-}

{- HLINT ignore "Use =<<" -}
{- HLINT ignore "Use join" -}

-- | Реализация монад.
-- Локальный минимум: 2б.
module Block1 where

import Control.Monad (ap, join)
import MetaUtils

-- 1. (0.5б)
-- Если известно, что двойное буррито --- это буррито и что из начинки буррито X
-- можно сделать буррито Y, то из буррито X можно сделать буррито Y.
-- Действительно: заменим начинку буррито X целиком на буррито Y; получим двойное буррито Y.
-- Но двойное буррито --- это буррито.
--
-- Выразите (>>=) через join и (<$>).

bind :: (Monad m) => m a -> (a -> m b) -> m b
bind ma f = join (f <$> ma)

-- 2. (0.5б)
-- Если известно, что можно получить буррито Y из буррито X, умея доготавливать начинку
-- буррито X до полноценного буррито, то всякое двойное буррито является буррито.
-- Действительно: возьмём какое-то двойное буррито Z и доготовим его начинку (обычное буррито Z)
-- до обычного буррито Z очень простым образом: просто не будем ничего делать с ним.
--
-- Выразите join через (>>=).

join' :: (Monad m) => m (m a) -> m a
join' m = m >>= id

-- 3. (1б)
-- Дан терм, перепишите его без do-нотации.
-- Воспользуйтесь правилами трансляции с лекции.

sugared a b = do
  a
  b
  if 3 < 5
    then do
      a
      pure 4
    else pure 5
  x <- a
  let y = 5
  [x, y] <- b
  [x, y, z] <- a
  x <- x
  x
  pure x

desugared a b =
  a
    >> b
    >> (if 3 < 5 then a >> pure 4 else pure 5)
    >> ( a >>= \x ->
           let y = 5
            in b >>= \v -> case v of
                 [x, y] ->
                   a >>= \v -> case v of
                     [x, y, z] -> x >>= \x -> pure x
                     _ -> fail "pattern mismatch"
                 _ -> fail "pattern mismatch"
       )

-- 4. (1б)
-- Дана структура данных 'Point', которая задаёт координату на плоскости.
-- Сделайте её монадой с такой семантикой, что вычисление проводится параллельно
-- над обеими компонентами. Например:
--    do a <- Point 3 4
--       b <- Point 10 12
--       pure (a + b)
--    ~> Point 13 16

data Point a = Point a a
  deriving (Show, Eq)

type TestPointMonad = Result2 '[Functor :> Done, Applicative :> Done, Monad :> Done]

instance Functor Point where
  fmap :: (a -> b) -> Point a -> Point b
  fmap f (Point x y) = Point (f x) (f y)

instance Applicative Point where
  pure :: a -> Point a
  pure x = Point x x

  (<*>) :: Point (a -> b) -> Point a -> Point b
  (Point f g) <*> (Point x y) = Point (f x) (g y)

instance Monad Point where
  (>>=) :: Point a -> (a -> Point b) -> Point b
  (Point x y) >>= f =
    let Point x' _ = f x
        Point _ y' = f y
     in Point x' y'

-- 5. (1.5б)
-- Реализуйте с помощью do-нотации функцию, которая складывает
-- первые компоненты точек и умножает вторые.

-- Определяет, по первой ли компоненте ведётся сейчас вычисление.
isFirstComponent :: Point Bool
isFirstComponent = Point True False

sumAndMult :: (Num a) => Point a -> Point a -> Point a
sumAndMult (Point x1 y1) (Point x2 y2) = do
  isFirst <- isFirstComponent
  pure $ if isFirst then x1 + x2 else y1 * y2
