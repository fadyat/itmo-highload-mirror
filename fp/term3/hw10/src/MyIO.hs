-- | В реализации в этом файле можно не вчитываться.
module MyIO
  ( MyIO
  , runUnsafeMyIO
  , evalUnsafeMyIO
  , execUnsafeMyIO
  , isEof
  , readInt
  , readIntMaybe
  , writeInt
  , emptyWorld
  , world
  , unsafeWorld
  , worldOutput
  ) where

-- | Вычислительный контекст, который эмулирует работу с внешним миром.
-- Вчитываться в реализации не обязательно. Достаточно посмотреть, что этот
-- тип выставляет интерфейс аппликативного функтора и имеются некоторые
-- функции для работы с ним.
newtype MyIO a = MyIO
  { runUnsafeMyIO :: World -> (World, a)
  -- ^ Запрещено использовать для реализаций, нужна только для тестов и GHCi!
  }

-- | Запрещено использовать для реализаций, нужна только для тестов и GHCi!
evalUnsafeMyIO :: MyIO a -> World -> a
evalUnsafeMyIO = fmap snd . runUnsafeMyIO

-- | Запрещено использовать для реализаций, нужна только для тестов и GHCi!
execUnsafeMyIO :: MyIO a -> World -> World
execUnsafeMyIO = fmap fst . runUnsafeMyIO

instance Functor MyIO where
  fmap f (MyIO io) = MyIO $ fmap f <$> io

instance Applicative MyIO where
  pure x = MyIO (, x) -- \w -> (w, x)
  MyIO fs <*> MyIO xs = MyIO $ \w ->
    let (w', f) = fs w in
    let (w'', x) = xs w' in
    (w'', f x)

instance Monad MyIO where
  MyIO m >>= k = MyIO \w -> let (w', x) = m w in runUnsafeMyIO (k x) w'

-- | Определяет, закончился ли входной поток.
isEof :: MyIO Bool
isEof = MyIO \w -> (w, null $ worldInput w)

-- | Считывает число с "консоли".
readInt :: MyIO Int
readInt = MyIO \case
  World { worldInput = [] } -> error "The world is so empty and cold"
  w@World { worldInput = x:xs } -> let w' = w { worldInput = xs } in (w', x)

-- | Всё понятно из типа.
readIntMaybe :: MyIO (Maybe Int)
readIntMaybe = MyIO \case
  w@World { worldInput = [] } -> (w, Nothing)
  w@World { worldInput = x:xs } -> let w' = w { worldInput = xs } in (w', Just x)

-- | Записывает число в "консоль"
writeInt :: Int -> MyIO ()
writeInt x = MyIO \w@World {..} -> let w' = w { worldOutput = worldOutput ++ [x] } in (w', ())

-- | Мир -- это магическая деталь реализации, используйте только для тестирования!
data World = World { worldInput :: [Int], worldOutput :: [Int] }
  deriving stock (Eq, Show)

emptyWorld :: World
emptyWorld = World { worldInput = [], worldOutput = [] }

-- | Конструирует мир, в котором пользователь на консоль напечатал числа из переданного списка.
world :: [Int] -> World
world worldInput = World { worldOutput = [], .. }

-- | Только для тестирования.
unsafeWorld :: [Int] -> [Int] -> World
unsafeWorld = World
