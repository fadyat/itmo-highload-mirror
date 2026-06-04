-- | Использование монад.
-- Локальный минимум: 5б.
module Block2 where

import Control.Applicative (liftA2)
import Control.Monad (guard, replicateM)
import Data.Functor ((<&>))
import Data.Int
import Data.List (genericLength)
import Data.Map (Map, (!?))
import Data.Map qualified as Map
import Data.Monoid (Sum (..))
import Data.Set (Set)
import Data.Set qualified as Set
import MetaUtils
import MyIO

-- 1. (1б)
-- По авторизованному сотруднику определите начальника его начальника.
-- В случае неудачи верните Nothing.

newtype UserId = UserId {getUserId :: Int64}
  deriving (Show, Eq, Ord)

newtype UserName = UserName {getUserName :: String}
  deriving (Show, Eq)

data User = User
  { userId :: UserId,
    userName :: UserName,
    userBoss :: Maybe UserId
  }
  deriving (Show, Eq)

type Auth = Set UserId

hasAuth :: UserId -> Auth -> Bool
hasAuth = Set.member

type UserDb = Map UserId User

bossBoss :: Auth -> UserDb -> UserId -> Maybe User
bossBoss auth db uid = do
  guard (hasAuth uid auth)
  user <- Map.lookup uid db
  boss <- userBoss user >>= (`Map.lookup` db)
  userBoss boss >>= (`Map.lookup` db)

-- 2. (0.5б)
-- Используя монаду списка, напишите функцию, которая каждое чётное число раздваивает.

dubEven :: [Int] -> [Int]
dubEven xs = xs >>= \x -> if even x then [x, x] else [x]

-- 3. (1б)
-- Напишите функцию, которая генерирует список троек чисел, в которых первая
-- компонента больше второй, а вторая больше третьей, а сами числа являются
-- сторонами прямоугольного треугольника (пифагоровы тройки). Порядок на этих
-- тройках должен быть лексикографический. Функция должна использовать монаду
-- списка.
-- Первые пять элементов списка, который должен генерироваться:
-- [(5,4,3),(10,8,6),(13,12,5),(15,12,9),(17,15,8)]

edges1 :: [(Int, Int, Int)]
edges1 = do
  a <- [1 ..]
  b <- [1 .. (a - 1)]
  c <- [1 .. (b - 1)]
  guard (a * a == b * b + c * c)
  return (a, b, c)

-- Напишите ту же функцию, но через list comprehensions.

edges2 :: [(Int, Int, Int)]
edges2 =
  [ (a, b, c)
    | a <- [1 ..],
      b <- [1 .. (a - 1)],
      c <- [1 .. (b - 1)],
      a * a == b * b + c * c
  ]

-- 4. (3б)
-- Пользователь печатает в "консоль" информацию о небесных телах:
-- масса, три координаты, масса следующего, три координаты следующего...
-- Для простоты, можно считать, что ввод пользователя всегда корректный.
-- Напишите приложение, которое печатает координаты центра масс всех тел.
-- Для деления используйте div.
--
-- Введением дополнительных data и определением классов типов для них
-- постарайтесь добиться наиболее обобщённого и лаконичного кода.
-- Используйте весь свой Хаскелл!
--
-- Изучите заготовку в app/Main.hs и запустите своё приложение из консоли!
--  $ stack run

data Vec3 a = Vec3 a a a
  deriving (Show, Eq)

instance Functor Vec3 where
  fmap f (Vec3 x y z) = Vec3 (f x) (f y) (f z)

instance Foldable Vec3 where
  foldMap f (Vec3 x y z) = f x <> f y <> f z

instance (Num a) => Semigroup (Vec3 a) where
  (Vec3 x1 y1 z1) <> (Vec3 x2 y2 z2) = Vec3 (x1 + x2) (y1 + y2) (z1 + z2)

instance (Num a) => Monoid (Vec3 a) where
  mempty = Vec3 0 0 0

data Body a = Body {mass :: a, pos :: Vec3 a}

centerOfMass :: (Integral a) => [Body a] -> Vec3 a
centerOfMass bodies =
  let totalMass = sum (map mass bodies)
      weightedPos = foldMap (\(Body m p) -> fmap (* m) p) bodies
   in fmap (`div` totalMass) weightedPos

readBodies :: [Body Int] -> MyIO [Body Int]
readBodies acc = do
  mm <- readIntMaybe
  maybe
    (pure (reverse acc))
    ( \m -> do
        mxyz <- mapM (const readIntMaybe) [(), (), ()]
        case sequenceA mxyz of
          Just [x, y, z] -> readBodies (Body m (Vec3 x y z) : acc)
          _ -> pure (reverse acc)
    )
    mm

centerMain :: MyIO ()
centerMain = do
  bodies <- readBodies []
  let Vec3 x y z = centerOfMass bodies
  writeInt x
  writeInt y
  writeInt z

-- 5. (1б)
-- n-гранный кубик кинули трижды. Известно, что сумма всех выпавших значений -
-- число чётное. Найдите вероятность того, что выпало число 14, используя монаду
-- списков.

probability ::
  (outcome -> Bool) -> -- Какие элементарные исходы желаемые
  [outcome] -> -- Всевозможные исходы
  Double -- Вероятность желаемого исхода
probability p outcomes = genericLength (filter p outcomes) / genericLength outcomes

allEvenOutcomes :: Int -> [Int]
allEvenOutcomes n =
  [ a + b + c
    | a <- [1 .. n],
      b <- [1 .. n],
      c <- [1 .. n],
      even (a + b + c)
  ]

probSolution :: Int -> Double
probSolution = probability (== 14) . allEvenOutcomes

-- 6. (2б)
-- Акула захотела поиграть в покер, однако, будучи малоразумным животным,
-- не может ни у кого спросить, какие у этой игры правила. Более того, так как
-- акула --- животное морское, в карты играть не выйдет: они размокнут, да и само
-- предположение, что акула тасовала бы карты, звучит абсурдно.
--
-- В итоге акула играет в покер по таким правилам: сначала она кидает k-гранные
-- кости (игральные) один раз и получает число n от 1 до k. Затем акула кидает
-- кости ещё n раз и получает числа m_1, m_2, ..., m_n. Наконец, акула
-- подсчитывает сумму всех выпавших ей значений (n, m_1, ..., m_n), и если
-- число оказывается простым, то акула считает себя победителем и может
-- полакомиться проигравшим.
--
-- С какой вероятностью акула победит? Определите, используя монаду списков.

-- Функция, которая перебирает все возможные варианты развития событий
-- и, если сумма значений оказалась простым числом, возвращает пару из
-- вероятности такого варианта и сам вариант в виде списка всех полученных
-- значений, а если нет, то игнорирует данный вариант.
--
-- Например, для k = 2 результат был бы такой, если бы мы не выкидывали
-- все составные числа:
-- [(Sum {getSum = 0.250},[1,1]),
--  (Sum {getSum = 0.250},[1,2]),
--  (Sum {getSum = 0.125},[2,1,1]),
--  (Sum {getSum = 0.125},[2,1,2]),
--  (Sum {getSum = 0.125},[2,2,1]),
--  (Sum {getSum = 0.125},[2,2,2])]
--
-- Так как мы оставляем лишь простые числа, результат будет таким:
-- [(Sum {getSum = 0.250},[1,1]),
--  (Sum {getSum = 0.250},[1,2]),
--  (Sum {getSum = 0.125},[2,1,2]),
--  (Sum {getSum = 0.125},[2,2,1])]

isPrime :: Int -> Bool
isPrime n
  | n < 2 = False
  | otherwise = null [x | x <- [2 .. isqrt n], n `mod` x == 0]
  where
    isqrt = floor . sqrt . (fromIntegral :: Int -> Double)

allOutcomes :: Int -> [(Sum Double, [Int])]
allOutcomes k = do
  n <- [1 .. k]
  ms <- replicateM n [1 .. k]
  let outcome = n : ms
      p = 1 / fromIntegral (k ^ (n + 1))
  guard (isPrime (sum outcome))
  pure (Sum p, outcome)

-- Проверяется только эта функция, остальные вспомогательные.
-- Если есть идеи получше, можете игнорировать то, что выше.
probShark :: Int -> Double
probShark k = sum [p | (Sum p, _) <- allOutcomes k]
