module Control.Examples where

import Control.Monad
import Control.SimpleMonad
import Data.IORef

fac :: Integer -> IO Integer
fac n = do
  acc <- newIORef 1
  forM_ [1..n] $ \i -> modifyIORef' acc (* i)
  readIORef acc

stateComputation :: (s -> s) -> (s -> a) -> (s -> (a, s))
--stateComputation f g s = let s' = f s in (g s, s')
stateComputation f g = runState do
  s <- get   -- get    ::              s -> (s , s)
  modify f   -- modify :: (s -> s) -> (s -> ((), s))
  pure (g s) -- pure   :: a ->        (s -> (a , s))
