{-# OPTIONS_GHC -Wno-missing-signatures #-}
{-# OPTIONS_GHC -Wno-type-defaults #-}
{- HLINT ignore "Use newtype instead of data" -}

module Examples where

import Data.Function (fix)
import Data.Kind

data MyOrd a = MyOrd { less :: a -> a -> Bool }

sort :: MyOrd a -> [a] -> [a]
sort _ [] = []
sort ord (x:xs) = insert x (sort ord xs)
  where
    insert x [] = [x]
    insert x xs@(y:ys)
      | less ord x y = x : xs
      | otherwise = y : insert x ys

intMyOrd = MyOrd { less = (<) }

listMyOrd :: MyOrd a -> MyOrd [a]
listMyOrd ord = MyOrd
  { less = fix \rec xs ys -> case (xs, ys) of
      (_, []) -> False
      ([], _) -> True
      (x:xs, y:ys) -> if eq x y then rec xs ys else less ord x y
  }
  where
    eq x y = not (less ord x y) && not (less ord y x)

example = sort (listMyOrd intMyOrd) [[3, 2], [], [1]]


class Profunctor (p :: Type -> Type -> Type) where
  dimap :: (a -> b) -> (c -> d) -> p b c -> p a d

instance Profunctor (->) where
  dimap g h f = h . f . g
