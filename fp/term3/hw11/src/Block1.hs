-- | Реализация монад.
-- Локальный минимум: 2.5б.
module Block1 where

import Control.Monad (ap, replicateM)
import Control.SimpleMonad
import Data.Functor ((<&>))
import Data.Map (Map)
import Data.Map qualified as Map
import Debug.Trace (trace)
import MetaUtils
import System.Random

-- 0.
-- Прочитайте строчка за строчкой файл Control.SimpleMonad. Если какая-то
-- монада непонятна, сотрите её определение и попробуйте написать своё. По
-- опыту, это единственный действенный способ научиться монадам.
-- А ещё очень пригодится на экзамене.

-- 1. (0.5б)
-- Сделайте следующий тип представителем Monad.
-- Какой семантикой разумно было бы наделить реализацию?

data Res e a = Err e | Ok a
  deriving (Show, Eq)

type TestRes = Result2 '[Functor :> Done, Applicative :> Done, Monad :> Done]

instance Functor (Res e) where
  fmap :: (a -> b) -> Res e a -> Res e b
  fmap f (Ok x) = Ok (f x)
  fmap _ (Err e) = Err e

instance Applicative (Res e) where
  pure :: a -> Res e a
  pure = Ok

  (<*>) :: Res e (a -> b) -> Res e a -> Res e b
  Ok f <*> Ok x = Ok (f x)
  Err e <*> _ = Err e
  _ <*> Err e = Err e

instance Monad (Res e) where
  (>>=) :: Res e a -> (a -> Res e b) -> Res e b
  Ok x >>= f = f x
  Err e >>= _ = Err e

-- 2. (1б)
-- Коробочка State заключает дополнительный аргумент функции и дополнительное
-- возвращаемое значение:
-- f :: String -> State (Map String Int) Int
-- эквивалентно
-- f :: String -> Map String Int -> (Int, Map String Int)
--
-- Коробочка Reader заключает только дополнительный аргумент функции,
-- который с помощью монадической машинерии везде передаётся неявно.
-- Реализуйте интерфейс Reader через State.
--
-- Имеет смысл сначала воспользоваться низкоуровневым интерфейсом State
-- (state, runState), а потом переписать на высокоуровневый для понимания его работы.

type StateReader e a = State e a

runStateReader :: StateReader e a -> e -> a
runStateReader = evalState

stateAsk :: StateReader e e
stateAsk = get

stateLocal :: (e -> e) -> StateReader e a -> StateReader e a
stateLocal f sr = do
  old <- get
  res <- withState f sr
  put old
  pure res

stateReader :: (e -> a) -> StateReader e a
stateReader = gets

stateAsks :: (e -> a) -> StateReader e a
stateAsks = stateReader

-- Функция, которая проверяет, что в передающемся как окружение словаре
-- присутствует информация о данном ключе.
keyMember :: (Ord k) => k -> StateReader (Map k v) Bool
keyMember k = stateAsks (Map.member k)

-- 3. (1.5б)
-- С помощью функциональности монады State реализуйте функциональность монады Writer.
-- Обратите внимание на то, что моноиды не обязательно являются коммутативными.
--
-- Имеет смысл начать с низкоуровнего интерфейса State (state, runState)
-- а потом переписать на высокоуровневый для понимания его работы.

type StateWriter w a = State w a

runStateWriter :: (Monoid w) => StateWriter w a -> (a, w)
runStateWriter sw = runState sw mempty

stateWriter :: (Monoid w) => (a, w) -> StateWriter w a
stateWriter (a, w) = do
  modify (<> w)
  pure a

stateTell :: (Monoid w) => w -> StateWriter w ()
stateTell w = modify (<> w)

stateListen :: (Monoid w) => StateWriter w a -> StateWriter w (a, w)
stateListen sw = do
  old <- get
  let (a, new) = runState sw mempty
  put (old <> new)
  pure (a, new)

stateListens :: (Monoid w) => (w -> b) -> StateWriter w a -> StateWriter w (a, b)
stateListens f sw = do
  old <- get
  let (a, new) = runState sw mempty
  put (old <> new)
  pure (a, f new)

stateCensor :: (Monoid w) => (w -> w) -> StateWriter w a -> StateWriter w a
stateCensor f sw = do
  old <- get
  let (a, new) = runState sw mempty
  put (old <> f new)
  pure a

-- 4. (1.5б)
-- Случайность - не такое простое явление в детерминированном мире компьютеров.
-- Существует два основных способа получать в программе случайные числа.
--
-- Первый - использовать сервис операционной системы, который добывает случайность
-- из действий пользователя и оборудования. Но взаимодействие с ОС требует IO
-- и порочит полученное значение как "нечистое" до конца дней его.
--
-- Второй - использовать псевдослучайные числа. Это детерминированные
-- последовательности чисел, каждое следующее вычисляется сложным образом по
-- по состоянию генератора.
--
-- В Haskell функциональность псевдослучайных чисел предоставляет библиотека
-- System.Random.
--
-- Функция `mkStdGen` возвращает проинициализированный генератор псевдослучайных
-- чисел по подаваемому первым аргументом зерну. Зерно (seed) генератора
-- (псевдо-)случайных чисел -- это некоторое числовое значение, которое определяет
-- начальное состояние генератора.
--
-- Функция `uniform` принимает на вход генератор, генерирует с его помощью
-- случайное число и возвращает пару из полученного числа и нового генератора.
-- Все функции чистые, так что исходный генератор не меняется, его можно вызывать
-- сколько угодно раз и получать одно и то же число:
--   GHCi> :{
--   let gen = mkStdGen 42
--    in (map fst [uniform gen, uniform gen, uniform gen]) :: [Int]
--   :}
--   [1275548033995301424,1275548033995301424,1275548033995301424]
--
-- Чтобы получить череду случайных значений с таким интерфейсом, надо пользоваться
-- генератором как-то так:
--    let (i1, g1) = uniform g0 in
--    let (i2, g2) = uniform g1 in ...
-- Видим явный монадический паттерн передачи новых генераторов в каждое следующее
-- вычисление случайного значения.
--
-- Сделайте следующий тип монадой и реализуйте функции для работы с ним.
-- Так, мы получим предметный императивный язык для работы со случайными числами
-- с неявным эффектом передачи генераторов из вычисления в вычисление

newtype MonadRandom a = MonadRandom (State StdGen a)
  deriving newtype (Functor, Applicative, Monad)

-- Просим компилятор сгенерировать реализации, которые просто переупаковывают
-- newtype коробочки, делегируя реализацию хранимому типу.

-- Функция, которая выполняет данное вычисление в `MonadRandom`.
runMonadRandom ::
  MonadRandom a -> -- Выполняемый код в `MonadRandom`.
  Int -> -- Зерно для начального значения генератора.
  (a, StdGen) -- Пара из результирующего значения и состояния генератора
  -- на момент завершения вычисления.
runMonadRandom = todo "runMonadRandom"

-- Функция, которая реинициализирует генератор случайных чисел, используя
-- новое зерно.
reinit :: Int -> MonadRandom ()
reinit = todo "reinit"

-- Вычисление, которое возвращает следующее случайное число из равномерного
-- распределения.
nextInt :: MonadRandom Int
nextInt = todo "nextInt"

-- Функция принимает два зерна генератора.
-- Используя первое зерно, она вычисляет два случайных числа n и m.
-- Используя второе зерно, генерирует два случайных списка длин
-- n `mod` 10 и m `mod` 10 соответственно.
-- Используйте полученный ранее язык случайностей.
randomLists :: Int -> Int -> ([Int], [Int])
randomLists = todo "randomLists"
