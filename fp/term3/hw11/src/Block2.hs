-- | Использование стандартных монад.
-- Локальный минимум: 4б.
module Block2 where

import Control.Monad (ap, when, zipWithM_)
import Control.SimpleMonad
import Data.Bool (bool)
import Data.Functor ((<&>))
import Data.IORef
import Data.Semigroup ((<>))
import Data.Set (Set)
import Data.Set qualified as Set
import GHC.IO.Handle
import MetaUtils
import Options.Applicative
import System.Environment (getArgs)
import System.IO

-- 1. (1б)
-- Дан тип лямбда-термов. Используя монаду `Reader` и, в частности, функцию
-- `local`, реализуйте программу, которая возвращает множество свободных переменных
-- в лямбда-терме. Для этого в качестве контекста `Reader` надо использовать (и
-- поддерживать с помощью `local`) множество переменных, связанных снаружи.
-- Итоговая реализация должна быть без использования функции `reader`.

data Lam a = Var a | App (Lam a) (Lam a) | Lam a (Lam a)
  deriving (Eq, Show)

freeVarsReader :: (Ord a) => Lam a -> Set a
freeVarsReader lam = runReader (go lam) Set.empty
  where
    go :: (Ord b) => Lam b -> Reader (Set b) (Set b)
    go (Var x) = do
      bound <- ask
      pure $ if x `Set.member` bound then Set.empty else Set.singleton x
    go (App f e) = liftA2 (<>) (go f) (go e)
    go (Lam x body) = local (Set.insert x) (go body)

-- 2. (1б)
-- Решите предыдущую задачу, но вместо монады `Reader` используйте `Writer`
-- с функцией `censor`. В качестве накапливаемого моноида держите множество
-- свободных переменных, которое надо вернуть.
-- Итоговая реализация должна быть без функции `writer`.

freeVarsWriter :: (Ord a) => Lam a -> Set a
freeVarsWriter lam = execWriter (go lam)
  where
    go :: (Ord b) => Lam b -> Writer (Set b) ()
    go (Var x) = tell (Set.singleton x)
    go (App f e) = go f >> go e
    go (Lam x body) = censor (Set.delete x) (go body)

-- 3. (1б)
-- Дан код на Си:
--   int fib(int n) {
--   	 int prev = 0;
--   	 int curr = 1;
--   	 while (n > 0) {
--   	 	 int tmp = prev + curr;
--   	 	 prev = curr;
--   	 	 curr = tmp;
--   	 	 --n;
--   	 }
--   	 return prev;
--   }
-- Хотим его оттранслировать в Haskell напрямую. Для этого воспользуемся `IORef`.

-- Функция, которая выполняет указанное вычисление до тех пор, пока условие не
-- перестанет быть истинным.
whileM_ ::
  (Monad m) =>
  m Bool -> -- Вычисление, возвращающее условие цикла.
  m a -> -- Тело цикла.
  m () -- Эффекты вычисления.
whileM_ cond action = do
  c <- cond
  when c $ action >> whileM_ cond action

-- Функция, которая уменьшает значение в заданной ячейке памяти на единицу.
decrementIORef :: (Num n) => IORef n -> IO ()
decrementIORef ref = modifyIORef' ref (subtract 1)

-- Функция, вычисляющая n-ое число Фибоначчи с использованием `IORef`.
fibIO :: Int -> IO Integer
fibIO n0 = do
  prev <- newIORef 0
  curr <- newIORef 1
  nRef <- newIORef n0

  let cond = do
        n <- readIORef nRef
        pure (n > 0)

      body = do
        p <- readIORef prev
        c <- readIORef curr
        let tmp = p + c
        writeIORef prev c
        writeIORef curr tmp
        decrementIORef nRef

  whileM_ cond body
  readIORef prev

-- 4. (2б)
-- Напишите аналог cat из Unix.
-- Если передан файл, напечатайте его содержимое, иначе - читайте stdin.
-- Поддержите ключик -n для печати номеров строчек.
-- Можно особо не заморачиваться с парсингом аргументов.
-- Дополнительный балл за использование optparse-applicative.
-- Протестируйте ваше приложение через stack run -- <аргументы>.

data CatOptions = CatOptions
  { numberLines :: Bool,
    files :: [FilePath]
  }

catParser :: Parser CatOptions
catParser =
  CatOptions
    <$> switch (short 'n' <> help "Show line numbers")
    <*> many (argument str (metavar "FILES"))

mainCat :: IO ()
mainCat = do
  CatOptions {..} <- execParser $ info (catParser <**> helper) fullDesc
  if null files
    then processHandle numberLines stdin
    else mapM_ (\f -> withFile f ReadMode (processHandle numberLines)) files
  where
    processHandle :: Bool -> Handle -> IO ()
    processHandle numberLines h = do
      ls <- lines <$> hGetContents h
      if numberLines
        then zipWithM_ (\i line -> putStrLn (show i ++ "\t" ++ line)) ([1 ..] :: [Int]) ls
        else mapM_ putStrLn ls

-- 5. (3б)
-- Иногда для написания приложения требуется более могущественный язык, а котором
-- есть не только один эффект (e.g. состояние), а сразу куча разных эффектов.
-- Далее в курсе мы разберём способы инкрементального конструирования такого
-- языка их нескольких готовых языков. Однако пока мы можем пойти другим путём
-- и завести одну большую супер-монаду, которая умеет всё, что нужно!
--
-- Сделайте язык (data + Monad instance), в котором можно общаться с операционной
-- системой, завершаться с ошибкой и читать из неизменяемой конфигурации.
-- Реализуйте cat с его помощью. Протестируйте реализацию вручную.

newtype SuperMonad env err a = SuperMonad
  { runSuperMonad :: env -> IO (Either err a)
  }

liftIO :: IO a -> SuperMonad env err a
liftIO = todo "liftIO"

superAsks :: (env -> a) -> SuperMonad env err a
superAsks = todo "superAsks"

superLocal :: (env -> env') -> SuperMonad env' err a -> SuperMonad env err a
superLocal = todo "superLocal"

superThrow :: err -> SuperMonad env err a
superThrow = todo "superThrow"

mainSuperCat :: IO ()
mainSuperCat = todo "mainSuperCat"
