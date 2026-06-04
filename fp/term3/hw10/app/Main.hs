module Main where

import Block2
import Control.Monad (guard)
import Data.List qualified as List
import Data.Maybe
import Data.Traversable (for)
import MyIO
import Text.Read

main :: IO ()
main = do
  putStrLn "Input `mass x y z` per line, finish input with empty line!"
  lines <- getInput
  let iniWorld = world $ fromMaybe (error parseMsg) (parseInts lines)
  let (finalWorld, ()) = runUnsafeMyIO centerMain iniWorld
  putStrLn $ "Center of mass in " <> formatOutput (worldOutput finalWorld)
  where
    parseMsg = "Expected 4 ints per line!"
    formatOutput coords = "(" <> List.intercalate ", " (show <$> coords) <> ")"

getInput :: IO [String]
getInput = do
  line <- getLine
  if null line then
    pure []
  else
    (line :) <$> getInput

parseInts :: [String] -> Maybe [Int]
parseInts ss = do
  guard $ not (null ss)
  intss <- for ss \s -> do
    let ws = words s
    guard $ length ws == 4
    mapM readMaybe ws
  pure $ concat intss
