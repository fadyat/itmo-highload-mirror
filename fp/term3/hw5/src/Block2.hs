-- | Списки в Haskell.
-- Локальный минимум: 4б.
module Block2 where

import MetaUtils

-- 1. (1б)
-- Реализуйте функцию, которая сравнивает два списка лексикографически:
--    1. Если список A является префиксом B и не равен B, то A < B;
--    2. Если есть такой индекс i, что i-ый элемент A отличается от i-ого элемента
--      B, а все предыдущие элементы равны, то меньше тот список, у которого i-ый
--      элемент меньше.
-- Для сравнения элементов можно использовать операторы (<), (<=), (>), (>=),
-- (==), (/=) и функцию compare.
-- Ознакомьтесь с тем, как эта функция тестируется в test/SpecBlock2.hs.

lexLess :: (Ord a) => [a] -> [a] -> Bool
lexLess [] [] = False
lexLess [] _ = True
lexLess _ [] = False
lexLess (x : xs) (y : ys)
  | x < y = True
  | x > y = False
  | otherwise = lexLess xs ys

-- 2. (1б)
-- Сформируйте список цифр заданного неотрицательного целого числа.

digits :: Int -> [Int]
digits x
  | x < 0 = error "digits: negative number"
  | x < 10 = [x]
  | otherwise = digits (x `div` 10) ++ [x `mod` 10]

-- 3. (1б)
-- Реализуйте функцию, которая подсчитывает количество элементов списка,
-- которые являются числами Фибоначчи.
-- Сделайте это с помощью списков, а не google магической формулы.

fibs :: [Int]
fibs = 0 : 1 : zipWith (+) fibs (tail fibs)

isFib :: Int -> Bool
isFib n = n `elem` takeWhile (<= n) fibs

countFibs :: [Int] -> Int
countFibs xs = length (filter isFib xs)

-- 4. (1.5б)
-- Реализуйте функцию, которая будет применять к каждому элементу списка
-- заданную функцию, а затем, если все элементы являются Just, "снимать"
-- Just'ы и возвращать список элементов, а если есть хотя бы один Nothing,
-- то возвращать Nothing.

applyMaybeList :: (a -> Maybe b) -> [a] -> Maybe [b]
applyMaybeList _ [] = Just []
applyMaybeList f (x : xs) =
  case f x of
    Nothing -> Nothing
    Just y ->
      case applyMaybeList f xs of
        Just ys -> Just (y : ys)
        Nothing -> Nothing

-- 5. (2б)
-- Реализуйте функцию, которая проходит по списку с "окном" заданного размера:
-- [1..6] `withWindow` 3 == [[1, 2, 3], [2, 3, 4], [3, 4, 5], [4, 5, 6]].
-- Длина каждого списка в результирующем списке должна быть равна окну.
-- Поведение с окном неположительного размера не проверяется.
-- Напоминание: изменение данных сигнатур функций запрещено.

withWindow :: [a] -> Int -> [[a]]
withWindow xs n
  | n <= 0 = error "window size must be positive"
  | length xs < n = []
  | otherwise = take n xs : withWindow (tail xs) n
