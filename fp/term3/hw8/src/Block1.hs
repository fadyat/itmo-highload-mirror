{-
                   ___  ___
         \  \  /`\ \  \ \  \ \  \
          \__\ \__\ \__' \__' \__\
           \  \ \  \ \    \     \
            \  \ \  \ \    \     \
                    ___         ___  ___
\  \  /`\ \    \    \  \ \    \ \    \    \.  \
 \__\ \__\ \    \    \  \ \  . \ \__  \__  \`\ \
  \  \ \  \ \    \    \  \ \ |`\\ \    \    \ `\\
   \  \ \  \ \___ \___ \__\ \|  `\ \___ \___ \  `\
-}

-- | Реализация представителей аппликативных функторов.
-- Локальный минимум: 4б.
module Block1 where

import Control.Applicative
import Data.Functor.Identity
import MetaUtils

-- 1. (0.5б)
-- Сделайте следующий тип представителем Applicative в предположении, что левый тип - моноид.
-- Заметьте, что один из типовых параметров не используется. Эта идиома называется "Фантомные типы".

newtype HalfPair e a = HalfPair {runHalfPair :: e}
  deriving stock (Eq, Show, Functor)

instance (Monoid e) => Applicative (HalfPair e) where
  pure :: a -> HalfPair e a
  pure _ = HalfPair mempty

  (<*>) :: HalfPair e (a -> b) -> HalfPair e a -> HalfPair e b
  HalfPair a <*> HalfPair b = HalfPair (a <> b)

-- Изучите реализацию Applicative Identity с помощью Hoodle.
-- Сравните - аппликатив Identity не добавляет никакой семантики к чистому вычислению:
--   GHCi> exprIdentity
--   Identity 3
-- В то время как наш HalfPair только производит побочные эффекты и вовсе не производит вычислений:
--   GHCi> exprHalfPair
--   HalfPair {runHalfPair = "hello world"}

exprIdentity :: (Num n) => Identity n
exprIdentity = (+) <$> Identity 1 <*> Identity 2

exprHalfPair :: (Num n) => HalfPair String n
exprHalfPair = (+) <$> HalfPair "hello " <*> HalfPair "world"

-- 2. (1б)
-- У вас имеется вектор в двумерном пространстве. Объявите его представителем Applicative.
-- Реализуйте функции растяжения вектора и сложения трёх векторов. Проверьте решение в GHCi.
-- Тривиальный инстанс функтора уже генерирует для вас компилятор с помощью DerivingFunctor.

data Point a = Point a a
  deriving stock (Show, Eq, Functor)

type TestPointApplicative = Result2 '[Applicative :> Done]

instance Applicative (Point) where
  pure :: a -> Point a
  pure a = Point a a

  (<*>) :: Point (a -> b) -> Point a -> Point b
  Point f f' <*> Point x x' = Point (f x) (f' x')

stretchPointF :: (Functor f, Num n) => n -> f n -> f n
stretchPointF k = fmap (k *)

sumPointsF :: (Applicative f, Num n) => f n -> f n -> f n -> f n
sumPointsF x y z = pure (\a b c -> a + b + c) <*> x <*> y <*> z

-- Воспользуйтесь одним из классов типов. Пересобрать пару вручную запрещено.
stretchPoint :: (Num n) => n -> Point n -> Point n
stretchPoint = stretchPointF

-- Воспользуйтесь одним из классов типов. Пересобрать пару вручную запрещено.
sumPoints :: (Num n) => Point n -> Point n -> Point n -> Point n
sumPoints = sumPointsF

-- 3. (1б)
-- Сделайте уже серьёзные точки в многомерном пространстве аппликативами.
-- Убедитесь в GHCi, что работает.

newtype PointN a = PointN [a]
  deriving stock (Show, Eq, Functor)

type TestPointNApplicative = Result2 '[Applicative :> Done]

instance Applicative PointN where
  pure :: a -> PointN a
  pure a = PointN (repeat a)

  (<*>) :: PointN (a -> b) -> PointN a -> PointN b
  PointN fs <*> PointN xs = PointN (zipWith ($) fs xs)

stretchPointN :: (Num n) => n -> PointN n -> PointN n
stretchPointN = stretchPointF

sumPointsN :: (Num n) => PointN n -> PointN n -> PointN n -> PointN n
sumPointsN = sumPointsF

-- 4. (1б)
-- Сделайте совсем серьёзные точки аппликативами.
-- Компоненты индексируются элементами произвольных типов.
-- Напишите instance Functor в этот раз самостоятельно.

newtype PointI i a = PointI (i -> a)

pointIExample :: PointI String Int
pointIExample = PointI \case
  -- а ля $ \i -> case i of
  "x" -> 3
  "y" -> 4
  unknown -> error $ "Unexpected component index: " <> unknown

type TestPointIApplicative = Result2 '[Functor :> Done, Applicative :> Done]

instance Functor (PointI i) where
  fmap :: (a -> b) -> PointI i a -> PointI i b
  fmap f (PointI g) = PointI (\i -> f (g i))

instance Applicative (PointI i) where
  pure :: a -> PointI i a
  pure x = PointI (\_ -> x)

  (<*>) :: PointI i (a -> b) -> PointI i a -> PointI i b
  PointI f <*> PointI g = PointI (\i -> f i (g i))

stretchPointI :: (Num n) => n -> PointI i n -> PointI i n
stretchPointI = stretchPointF

sumPointsI :: (Num n) => PointI i n -> PointI i n -> PointI i n -> PointI i n
sumPointsI = sumPointsF

-- 5. (1б)
-- Есть класс типов, по выразительности эквивалентный аппликативным функторам - моноидальные функторы.
-- Реализуйте его инстансы для Maybe, стрелки и пары.

class (Functor f) => Monoidal f where
  -- | Оборачивает скучное значение в контекст исполнения.
  unit :: f ()

  -- | Объединяет два значения в контексте в пару значений в контексте.
  (*&*) :: f a -> f b -> f (a, b)

instance Monoidal Maybe where
  unit :: Maybe ()
  unit = Just ()

  (*&*) :: Maybe a -> Maybe b -> Maybe (a, b)
  Nothing *&* _ = Nothing
  _ *&* Nothing = Nothing
  Just x *&* Just y = Just (x, y)

instance Monoidal ((->) r) where
  unit :: r -> ()
  unit = \_ -> ()

  (*&*) :: (r -> a) -> (r -> b) -> r -> (a, b)
  f *&* g = \r -> (f r, g r)

instance (Monoid c) => Monoidal ((,) c) where
  unit :: (c, ())
  unit = (mempty, ())

  (*&*) :: (c, a) -> (c, b) -> (c, (a, b))
  (c1, x) *&* (c2, y) = (c1 <> c2, (x, y))

-- 6. (1б)
-- Покажите, что каждый аппликативный функтор является моноидальным.
-- Покажите, что каждый моноидальный функтор является аппликативным.

newtype ApplicativeMonoidal f a = ApplicativeMonoidal (f a)
  deriving (Eq, Ord, Show)

newtype MonoidalApplicative f a = MonoidalApplicative (f a)
  deriving (Eq, Ord, Show)

todoImpl ''Functor ''ApplicativeMonoidal
todoImpl ''Monoidal ''ApplicativeMonoidal

todoImpl ''Functor ''MonoidalApplicative
todoImpl ''Applicative ''MonoidalApplicative
