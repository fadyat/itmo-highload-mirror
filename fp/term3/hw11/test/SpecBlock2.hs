module SpecBlock2 where

import Block2
import Control.SimpleMonad
import Data.Functor ((<&>))
import Data.Set (Set)
import Data.Set qualified as Set
import Data.IORef
import Generic.Random
import System.Environment
import Test.Prelude

tests :: NamedTests
tests = nameTests 2
  [ testFreeVarsReader
  , testFreeVarsWriter
  , testFibIO
  , testCat
  , testSuperMonad
  ]

testFreeVarsReader :: Test
testFreeVarsReader = TestList
  [ propertyToTest "freeVarsReader" $ validateFreeVars freeVarsReader
  ]

testFreeVarsWriter :: Test
testFreeVarsWriter = TestList
  [ propertyToTest "freeVarsWriter" $ validateFreeVars freeVarsWriter
  ]

deriving stock instance Generic (Lam a)
instance Arbitrary a => Arbitrary (Lam a) where
  arbitrary = genericArbitraryU

validateFreeVars :: (Lam (Small Int) -> Set (Small Int)) -> Property
validateFreeVars f = property $ \lam -> f lam === f' (pure False) lam mempty
  where
    f' :: Ord b => (b -> Bool) -> Lam b -> Set b -> Set b
    f' b (Var a) = if b a then id else (<>) (Set.singleton a)
    f' b (App m n) = f' b m . f' b n
    f' b (Lam a m) = f' (\x -> x == a || b x) m

testFibIO :: Test
testFibIO = TestList
  [ TestCase $ assertEqual "whileM_ завершается на первом шаге, когда надо, "
      (Just ())
      (whileM_ (Just False) Nothing)
  , TestCase $ assertEqual "whileM_ выполняет код в цикле, "
      1
      (execState (whileM_ (get <&> (<= 0)) (modify (+ 1))) (-3))
  , TestCase $ do
      ref <- newIORef 5
      decrementIORef ref
      v' <- readIORef ref
      assertEqual "decrementIORef работает" 4 v'
  , TestCase $ do
      fibs <- mapM fibIO [0 .. 15]
      let trueFibs = [0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610]
      assertEqual "fib считает числа Фибоначчи" trueFibs fibs
  ]

testCat :: Test
testCat = TestList
  [ TestLabel "ordinary execution" $ TestCase do
      putStrLn "    Cat output:"
      withArgs ["Setup.hs"] mainCat
  , TestLabel "with line numbers" $ TestCase do
      putStrLn "    Cat output:"
      withArgs ["Setup.hs", "-n"] mainCat
  ]

testSuperMonad :: Test
testSuperMonad = TestList
  [ TestLabel "ordinary execution" $ TestCase do
      putStrLn "    Super cat output:"
      withArgs ["Setup.hs"] mainSuperCat
  , TestLabel "with line numbers" $ TestCase do
      putStrLn "    Super cat output:"
      withArgs ["Setup.hs", "-n"] mainSuperCat
  ]
