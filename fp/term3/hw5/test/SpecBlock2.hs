module SpecBlock2 where

import Block2
import Control.Exception
import Data.Digits qualified as Digits
import Test.QuickCheck
import Test.Prelude

tests :: NamedTests
tests = nameTests 2
  [ testLexLess
  , testDigits
  , testCountFibs
  , testApplyMaybeList
  , testWithWindow
  ]

testLexLess :: Test
testLexLess = TestList
  [ propertyToTest "totality (Либо A < B, либо B < A, либо A == B)" \(s1 :: String, s2) ->
      s1 === s2 .||. lexLess s1 s2 .||. lexLess s2 s1
  , propertyToTest "irreflexivity (Неправда, что A < A)" \(s :: String) ->
      not (lexLess s s)
  , propertyToTest "antisymmetry (Если A < B, то неправда, что B < A)" \(s1 :: String, s2) ->
      s1 === s2 .||. lexLess s1 s2 =/= lexLess s2 s1
  , propertyToTest "rule1 (Правило №1 из условия должно выполняться)" \(s1, c :: Char, s2) ->
      lexLess s1 (s1 ++ c : s2)
  , propertyToTest "rule2 (Правило №2 из условия должно выполняться)" \(s :: String, s1, s2) ->
      lexLess (s ++ s1) (s ++ s2) === lexLess s1 s2
  ]

testDigits :: Test
testDigits = TestList
  [ propertyToTest "on negative" \(Negative n) -> ioProperty $ assertThrows @ErrorCall $ digits n
  , propertyToTest "digits works" \(NonNegative n) -> expected n === digits n
  ]
  where
    expected n = case Digits.digits 10 n of
      [] -> [0]
      ds -> ds

testCountFibs :: Test
testCountFibs = TestList
  [ TestCase $ assertEqual "Одно и то же число Фибоначчи несколько раз" 4 (countFibs [1, 1, 1, 1])
  , TestCase $ assertEqual "Много разных чисел Фибоначчи" 5 (countFibs [0, 1, 1, 2, 3])
  , TestCase $ assertEqual "Разные числа" 3 (countFibs [0, 1, 4, 6, 7, 8, 9, 11])
  , TestCase $ assertEqual "Пустой список" 0 (countFibs [])
  , TestCase $ assertEqual "Ни одного числа Фибоначчи" 0 (countFibs [4, 9])
  ]

testApplyMaybeList :: Test
testApplyMaybeList = TestList
  [ TestCase $ assertEqual "Пустой список" (Just []) (f (Just . show . (* 2) . (+ 1)) [])
  , TestCase $ assertEqual "Just не меняет список" (Just [1, 2, 3]) (f Just [1, 2, 3])
  , TestCase $ assertEqual "Nothing убирает список" (Nothing :: Maybe [Bool]) (f (const Nothing) [1, 2, 3])
  , TestCase $ assertEqual "Nothing убирает *весь* список" Nothing (f (\x -> if even x then Just x else Nothing) [1, 2, 3])
  , TestCase $ assertEqual "Список не убирается, если всё Just" (Just [2, 4, 6]) (f (\x -> if even x then Just x else Nothing) [2, 4, 6])
  , TestCase $ assertEqual "Сложная функция" (Just ["6", "10", "14"]) (f (Just . show . (* 2) . (+ 1)) [2, 4, 6])
  ]
  where
    f = applyMaybeList

testWithWindow :: Test
testWithWindow = TestList
  [ TestCase $ assertEqual "Единичное окно" [[1], [2], [3]] ([1, 2, 3] `withWindow` 1)
  , TestCase $ assertEqual "Окно размера 2" [[1, 2], [2, 3]] ([1, 2, 3] `withWindow` 2)
  , TestCase $ assertEqual "Окно, равное списку" [[1, 2, 3]] ([1, 2, 3] `withWindow` 3)
  , TestCase $ assertEqual "Окно размера 3" [[1, 2, 3], [2, 3, 4], [3, 4, 5]] ([1, 2, 3, 4, 5] `withWindow` 3)
  , TestCase $ assertEqual "Окно крупнее списка" [] ([1] `withWindow` 2)
  , TestCase $ assertEqual "Не последовательные числа в списке" [[4, 3], [3, 7], [7, 2]] ([4, 3, 7, 2] `withWindow` 2)
  , propertyToTest "Весь список в окнах" \(NonNegative len, Positive windowSize) -> do
      source <- len `vectorOf` (arbitrary :: Gen Int)
      let windows = source `withWindow` windowSize
      let collected = map head windows ++ tail (last windows)
      pure $ len >= windowSize ==> collected === source
  , propertyToTest "invalid window size" \(xs :: [Int], NonPositive windowSize) ->
      ioProperty $ assertThrows @ErrorCall $ xs `withWindow` windowSize
  ]
