-- | Рекурсия в Haskell.
-- Локальный минимум: 4б.
module Block2 where

-- Преподаватели заботливо оставляют импорты модулей, которые могут пригодиться.
-- Ознакомьтесь с ними с помощью Hoogle.
import Data.Function (fix)
import MetaUtils

-- 1. (0.5б)
-- Реализуйте взаимно-рекурсивную пару функций:
-- isEven(n) = True, если n = 0
--             isOdd(n + 1), если n < 0
--             isOdd(n - 1), если n > 0
-- isOdd(n) = False, если n = 0
--            isEven(n + 1), если n < 0
--            isEven(n - 1), если n > 0
-- При реализации этой функции используйте guard'ы (см. лекцию).

isEven :: Integer -> Bool
isEven n
  | n == 0 = True
  | n < 0 = isOdd (n + 1)
  | otherwise = isOdd (n - 1)

isOdd :: Integer -> Bool
isOdd n
  | n == 0 = False
  | n < 0 = isEven (n + 1)
  | otherwise = isEven (n - 1)

-- 2. (0.5б)
-- Найдите двойной факториал, то есть произведение всех чисел меньше данного,
-- имеющих ту же чётность. Например: 6!! = 6 * 4 * 2 = 48.
-- +0.5б за:
--    Сделайте это через комбинатор неподвижной точки (Data.Function.fix).
--    Для избавления от синтаксического шума воспользуйтесь оператором ($).

facfac :: Integer -> Integer
facfac = fix $ \f n -> if n <= 0 then 1 else n * f (n - 2)

-- 3. (1б)
-- Реализуйте функцию, находящую элементы следующей рекуррентной последовательности:
-- b[0] = 1; b[1] = 2; b[2] = 3; b[k + 3] = b[k + 2] - 2 * b[k + 1] + 3 * b[k]
-- Постарайтесь сделать так, чтобы Ваша функция работала за линейное время.

itemAt :: Integer -> Integer
itemAt = go 1 2 3
  where
    go f s t x
      | x == 0 = f
      | otherwise = go s t (t - 2 * s + 3 * f) (x - 1)

-- 4. (1.5б)
-- Реализуйте проверку на простоту числа.

isPrime :: Integer -> Bool
isPrime n
  | n < 2 = False
  | otherwise = go 2
  where
    go x
      | x * x > n = True
      | mod n x == 0 = False
      | otherwise = go (x + 1)

-- 5. (1.5б)
-- Известно, что множество задаётся своей характеристической функцией:
-- то есть функцией, которая возвращает False на значении, если такого элемента
-- в множестве нет, и True - если есть.
-- Реализуйте функции для работы с множеством строк, заданным такой функцией:
--    emptySet - задаёт пустое множество: emptySet "something" == False
--    (+++) - оператор добавления элемента в множество: (set +++ "elem") "elem" == True
--      Он должен быть левоассоциативным с 5м приоритетом.
--    (///) - оператор удаления элемента из множества.
--      Он должен быть левоассоциативным с 5м приоритетом.
-- Пример множества {"a", "b", "c"}: emptySet +++ "a" +++ "b" +++ "c".

emptySet :: String -> Bool
emptySet = const False

infixl 5 +++

(+++) :: (String -> Bool) -> String -> (String -> Bool)
set +++ x = \y -> y == x || set y

infixl 5 ///

(///) :: (String -> Bool) -> String -> (String -> Bool)
set /// x = \y -> y /= x && set y

-- 6. (1.5б)
-- Реализуйте функцию, которая по числу возвращает количество цифр в его модуле (abs), а так же их сумму.
-- Вычисление должно быть хвостово-рекурсивным. Используйте параметры-аккумуляторы.

nSumDigits :: Integer -> (Integer, Integer)
nSumDigits n
  | n == 0 = (1, 0)
  | otherwise = go (abs n) 0 0
  where
    go 0 count sum = (count, sum)
    go x count sum = go (div x 10) (count + 1) (sum + mod x 10)
