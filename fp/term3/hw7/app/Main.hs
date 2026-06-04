{-# OPTIONS_GHC -Wno-orphans #-}

module Main where

import Block2
import Control.DeepSeq
import Control.Exception (evaluate)
import Data.Foldable (for_)
import Data.Text qualified as Text
import Data.Traversable (for)
import GHC.Generics
import NeatInterpolation
import System.Environment
import System.FilePath
import System.IO

main :: IO ()
main = getArgs >>= \case
  ["wc"] -> mainWc
  "find" : words -> mainFind words
  args -> error $ "Unknown sequence of CLI args: " <> show args

mainWc :: IO ()
mainWc = do
  putStrLn aliceIntro
  putStrLn "Top used words!"
  text <- lines <$> getContents
  putStrLn $ wordCountShow stdWcShow{ topSize = 30 } $ wordCount text

aliceIntro :: String
aliceIntro = Text.unpack [text|-
           .'\   /`.
         .'.-.`-'.-.`.
    ..._:   .-. .-.   :_...
  .'    '-.(o ) (o ).-'    `.
 :  _    _ _`~(_)~`_ _    _  :
:  /:   ' .-=_   _=-. `   ;\  :
:   :|-.._  '     `  _..-|:   :
 :   `:| |`:-:-.-:-:'| |:'   :
  `.   `.| | | | | | |.'   .'
    `.   `-:_| | |_:-'   .'
      `-._   ````    _.-'
          ``-------''
  |]

mainFind :: [String] -> IO ()
mainFind words = do
  putStrLn harryIntro
  putStrLn "Welcome to the Harry-Search!"
  docs <- for harryBooks \name -> getDoc "text" name ".txt"
  putStrLn "Building inverted index..." *> flush
  index <- evaluate $ buildInvertedIndex docs
  putStrLn "Searching... " *> flush
  for_ words \word ->
    putStrLn $ word <> " - " <>
      case index `findMostRelevant` word of
        Nothing -> "not found :<"
        Just name -> "most used in " <> "\"" <> name <> "\""
  where
    flush = hFlush stdout

deriving stock instance Generic Document
deriving stock instance Generic IndexNode
instance NFData Document
instance NFData IndexNode

harryIntro :: String
harryIntro = Text.unpack [text|-
        ___                             | '  \
   ___  \ /  ___         ,'\_           | .-. \        /|
   \ /  | |,'__ \  ,'\_  |   \          | | | |      ,' |_   /|
 _ | |  | |\/  \ \ |   \ | |\_|    _    | |_| |   _ '-. .-',' |_   _
// | |  | |____| | | |\_|| |__    //    |     | ,'_`. | | '-. .-',' `. ,'\_
\\_| |_,' .-, _  | | |   | |\ \  //    .| |\_/ | / \ || |   | | / |\  \|   \
 `-. .-'| |/ / | | | |   | | \ \//     |  |    | | | || |   | | | |_\ || |\_|
   | |  | || \_| | | |   /_\  \ /      | |`    | | | || |   | | | .---'| |
   | |  | |\___,_\ /_\ _      //       | |     | \_/ || |   | | | |  /\| |
   /_\  | |           //_____//       .||`  _   `._,' | |   | | \ `-' /| |
        /_\           `------'        \ |  /-\   _     `.\  | |  `._,' /_\
                                       \|        |           `.\
  |]

harryBooks :: [FilePath]
harryBooks =
  [ "Harry Potter and the Chamber of Secrets"
  , "Harry Potter and the Deathly Hallows"
  , "Harry Potter and the Goblet of Fire"
  , "Harry Potter and The Half-Blood Prince"
  , "Harry Potter and the Order of the Phoenix"
  , "Harry Potter and the Prisoner of Azkaban"
  , "Harry Potter and the Sorcerer's Stone"
  ]

getDoc :: FilePath -> FilePath -> FilePath -> IO Document
getDoc path fileName ext = do
  text <- lines <$> readFile (path </> fileName <> ext)
  pure Document { docName = fileName, docContent = text }
