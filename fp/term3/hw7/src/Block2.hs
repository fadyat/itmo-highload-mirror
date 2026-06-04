-- | Цепочки трансформаций.
-- Локальный минимум: 3б.
module Block2 where

import Data.Char qualified as Char
import Data.Function (on, (&))
import Data.Functor ((<&>))
import Data.List qualified as List
import Data.List.NonEmpty (NonEmpty (..))
import Data.List.NonEmpty qualified as NE
import Data.Map (Map, (!?))
import Data.Map qualified as Map
import Data.Ord (Down (Down), comparing)
import MetaUtils

-- 1. (1.5б)
-- Вам на вход подаётся текст в виде списка строчек. Посчитайте число вхождений каждого слова.
-- Используйте высокоуровневый интерфейс списков как язык описания трансформаций
-- (мапы, фильтры, свёртки...).
-- Не забывайте про заглавные буквы и пунктуацию (её можно рассматривать как пробелы).
-- Композируйте через (&), чтобы код читался сверху вниз, справа налево.
-- Это задание очень просто написать очень плохо. Код должен читаться как проза,
-- поработайте над этим.

wordCount :: [String] -> Map String Int
wordCount lines =
  lines
    & unwords
    & map (\c -> if Char.isAlphaNum c then Char.toLower c else ' ')
    & words
    & foldl (\m w -> Map.insertWith (+) w 1 m) Map.empty

-- 2. (2б)
-- Отформатируйте результат wordCount. Информация о каждом слове должна располагаться
-- на отдельной строчке в порядке уменьшения количества вхождений: "слово : N".
-- Если несколько слов встречаются одинаковое количество раз, выводите в лексикографическом порядке.
-- Вам передаются настройки вывода.
-- Работу своего кода вы можете протестировать (см. app/Main.hs):
--   stack run -- wc
--   text line1
--   text line2
--   Ctrl + D (конец файла)
-- Топ слов в Alice in Wonderland??
--   stack run -- wc < text/alice.txt

data WordCountShow = WordCountShow
  { -- | Количество самых популярных слов, которые нужно распечатать.
    topSize :: Int,
    -- | В рейтинг и вывод должны попадать только слова размером строго больше указанного.
    smallWordsSize :: Int
  }

stdWcShow :: WordCountShow
stdWcShow = WordCountShow {topSize = 10, smallWordsSize = 3}

wordCountShow :: WordCountShow -> Map String Int -> String
wordCountShow wc m =
  m
    & Map.toList
    & filter (\(w, _) -> length w > smallWordsSize wc)
    & List.sortBy (comparing (Down . snd) <> comparing fst)
    & take (topSize wc)
    & map (\(w, n) -> w ++ " : " ++ show n)
    & unlines

-- 3. (2б)
-- Вам на вход приходит список документов. У каждого документа есть поле docContent,
-- которое хранит содержимое документа как набор строк. Нужно вернуть структуру,
-- которая для каждого слова из каждого документа содержит информацию: в каких документах
-- это слово можно найти и с какой частотой оно в них встречается (см. пример в тестах).

data Document = Document
  { docName :: String,
    docContent :: [String]
  }
  deriving (Eq, Show)

data IndexNode = IndexNode
  { idxDocument :: Document,
    idxFrequency :: Double
  }
  deriving (Eq, Show)

type Index = Map String (NonEmpty IndexNode)

buildInvertedIndex :: [Document] -> Index
buildInvertedIndex = todo "buildInvertedIndex"

-- 4. (1б)
-- Вам на вход подается индекс и слово. Верните название документа
-- такое, что переданное слово встречается в нем с наибольшей частотой.
-- Если такого документа для заданного слова нет -- верните Nothing.
-- Проверяем!
--   stack run -- find hermione basilisk lol

findMostRelevant :: Index -> String -> Maybe String
findMostRelevant = todo "findMostRelevant"
