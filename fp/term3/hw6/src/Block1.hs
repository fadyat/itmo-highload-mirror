-- | Стандартные классы типов.
-- Локальный минимум: 5б.
module Block1 where

import Data.Int (Int8)
import Data.List qualified as List
import Data.List.NonEmpty (NonEmpty (..), nonEmpty, toList)
import Data.List.NonEmpty qualified as NE
import MetaUtils

-- У нас есть система, которая позволяет компилировать тесты до того,
-- как вы напишите свой инстанс класса типов.
-- Как только приступаете к решению определённого задания, выключайте
-- систему для него:
--  * Строчки вида todoImpl ''Eq ''IntTree удаляйте;
--  * В строчках вида
--    type TestChurchEnum = Result1 '[Show :> Todo, Eq :> Todo, Enum :> Todo]
--    заменяйте Todo на Done.

-- 1. (0.5б)
-- Сделайте тип IntTree представителем класса типов Eq так, чтобы левая
-- компонента пары не влияла на результат.
-- Теперь можно использовать оператор (==) деревьях IntTree!
-- Обратите внимание на секцию minimal complete definition в документации класса типов.
-- Можете ознакомиться с тем, как это задание тестируется в test/SpecBlock1.hs.
-- https://hackage.haskell.org/package/quickcheck-classes-base-0.6.2.0/docs/Test-QuickCheck-Classes-Base.html#v:eqLaws

data IntTree = IntLeaf | IntNode IntTree (Int, Int) IntTree

instance Eq IntTree where
  IntLeaf == IntLeaf = True
  IntNode l (_, v2) r == IntNode l' (_, v2') r' = v2 == v2' && l == l' && r == r'
  _ == _ = False

-- 2. (0.5б)
-- Чтобы тип IntTree можно было использовать как ключ в словаре, полезно
-- определить порядок на его элементах с помощью класса типов Ord.
-- Заметьте, что декларация Ord обязывает предварительно реализовать Eq и
-- они должны быть совместимы ((t1 <= t2) /\ (t1 >= t2) -> t1 = t2).
-- Реализуем Ord следующим образом:
--  * Листья всегда равны;
--  * Лист всегда меньше вершины;
--  * У вершин сначала сравниваются левые поддеревья, если они равны, то элементы, а потом правые поддеревья.

instance Ord IntTree where
  IntLeaf <= IntLeaf = True
  IntLeaf <= IntNode {} = True
  IntNode {} <= IntLeaf = False
  IntNode l1 (_, v2) r1 <= IntNode l1' (_, v2') r1' =
    l1 < l1' || (l1 == l1' && v2 < v2') || (l1 == l1' && v2 == v2' && r1 <= r1')

-- 3. (1б)
-- Реализуйте класс типов CycleEnum. Обе его функции ведут себя как succ и pred,
-- но при достижении максимального и минимального значений соответственно, начинают сначала
-- (то есть с минимального и максимального соответственно).
-- Сделайте Bool представителем CycleEnum. Сделайте Int8 представителем CycleEnum.

class CycleEnum a where
  cycSucc :: a -> a
  cycPred :: a -> a

instance CycleEnum Bool where
  cycSucc True = False
  cycSucc False = True
  cycPred True = False
  cycPred False = True

instance CycleEnum Int8 where
  cycSucc x
    | x == maxBound = minBound
    | otherwise = x + 1
  cycPred x
    | x == minBound = maxBound
    | otherwise = x - 1

-- 4. (1б)
-- Для newtype-обёртки Cycling, реализуйте CycleEnum при условии, что оборачиваемый
-- тип удовлетворяет минимальному набору ограничений.

newtype Cycling a = Cycling a
  deriving newtype (Show, Eq) -- Делегировать реализации оборачиваемому типу.

instance (Eq a, Enum a, Bounded a) => CycleEnum (Cycling a) where
  cycSucc (Cycling x)
    | x == maxBound = Cycling minBound
    | otherwise = Cycling (succ x)
  cycPred (Cycling x)
    | x == minBound = Cycling maxBound
    | otherwise = Cycling (pred x)

-- 5. (1б)
-- Сделайте тип матриц произвольных элементов представителем класса типов Show.
-- Пример:
--   GHCi> Matrix [[1,2,3],[4,5,6],[7,8,9]]
--   [1,2,3]
--   [4,5,6]
--   [7,8,9]
--   GHCi> Matrix []
--   EMPTY
-- Обратите внимание, что наивная реализация не очень асимптотически оптимальна, почему?
-- +1б, если добьётесь линейной сложности с помощью https://wiki.haskell.org/Difference_list.

newtype Matrix a = Matrix [NonEmpty a]
  deriving (Eq)

instance (Show a) => Show (Matrix a) where
  show (Matrix []) = "EMPTY"
  show (Matrix rows) = unlines $ map (show . toList) rows

-- 6. (1.5б)
-- Объявите Matrix представителем класса типов Read совместимо с реализацией Show.
-- Используйте функцию readsPrec :: Int -> String -> [(Matrix a, String)]
--  * Первое число игнорируйте;
--  * Получите значение типа Matrix a, и заверните его в сложный результирующий тип
--    с помощью данной функции wrap (смысл этого типа мы поймём позже).
-- Можно ожидать, что на вход подаются только корректные строковые представления матриц.

wrap :: Matrix a -> [(Matrix a, String)]
wrap = (: []) . (,"")

instance (Read a) => Read (Matrix a) where
  readsPrec _ s
    | s == "EMPTY" = wrap (Matrix [])
    | otherwise =
        case mapM (nonEmpty . read) (lines s) of
          Nothing -> []
          Just rows -> wrap (Matrix rows)

-- 7. (0.5б)
-- Сделайте числа Чёрча представителем классов типов Show ("Church N") и Eq.

newtype Church = Church (forall a. (a -> a) -> a -> a)

type TestChurchShowEq = Result1 '[Show :> Done, Eq :> Done]

churchToInt :: Church -> Int
churchToInt (Church n) = n (+ 1) 0

instance Eq Church where
  a == b = churchToInt a == churchToInt b

instance Show Church where
  show c = "Church " ++ show (churchToInt c)

-- 8. (0.5б)
-- Сделайте числа чёрча представителем класса типов Enum.

type TestChurchEnum = Result1 '[Show :> Done, Eq :> Done, Enum :> Done]

intToChurch :: Int -> Church
intToChurch n
  | n <= 0 = Church (\_ x -> x)
  | otherwise = Church (\f x -> f ((\(Church c) -> c) (intToChurch (n - 1)) f x))

instance Enum Church where
  toEnum = intToChurch
  fromEnum = churchToInt

-- 9. (1б)
-- Сделайте числа чёрча представителем класса типов Num.
-- К сожалению, из-за отсутствия в природе отрицательных чисел Чёрча, наша реализация не будет
-- удовлетворять всем законам класса типов Num. Однако реализация, которая получится, всё равно
-- будет полезной.

type TestChurchNum = Result1 '[Show :> Todo, Eq :> Todo, Num :> Todo]

-- Напишите функцию, которая возвращает список чисел Чёрча от 4х до 14ти включительно через одно.
-- Как вам в этом могут помочь классы типов Num и Enum?

from4To15 :: [Church]
from4To15 = todo "from4To15"

-- ~ сюда не смотрим ~
-- Всякий бойлерплейт для тестирования и печати.

deriving stock instance Generic IntTree

instance Out IntTree

deriving via ShowOut IntTree instance Show IntTree
