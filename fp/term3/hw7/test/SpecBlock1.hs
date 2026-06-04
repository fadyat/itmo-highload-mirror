module SpecBlock1 where

import Block1
import Data.Foldable
import Data.Function (fix)
import Data.Functor ((<&>))
import Data.Maybe
import Data.Map (Map, (!?))
import Data.Map qualified as Map
import Data.Set (Set)
import Data.Set qualified as Set
import Data.Int
import Data.List (sort)
import Shared ()
import Test.Prelude

tests :: NamedTests
tests = nameTests 1
  [ testElem'
  , testSumOrLength
  , testMap'
  , testCountMatching
  , testTreeFold
  , testMyMap
  , testMyMapMonoid
  , testMyMapFoldable
  , testNeighborTreeFoldable
  ]

testElem' :: Test
testElem' = TestList
  [ propertyToTest "Работает как elem" \(x, xs :: [Char]) ->
      elem x xs === elem' x xs
  , TestCase $ assertEqual "работает на бесконечных списках" True $ (4 :: Int) `elem'` ([1 .. 4] ++ infinite)
  ]

testSumOrLength :: Test
testSumOrLength = TestList
  [ propertyToTest "совпадает с наивной реализацией" \(input :: Either (Nat, Nat) String) ->
      naive input === sumOrLength input
  ]
  where
    naive = toEnum . \case
      Left (n, m) -> fromEnum n + fromEnum m
      Right s -> length s

instance Enum Nat where
  fromEnum = \case
    Z -> 0
    S n -> 1 + fromEnum n
  toEnum n = case compare n 0 of
    LT -> error "Not natural"
    EQ -> Z
    GT -> S (toEnum (n - 1))

deriving stock instance Generic Nat
deriving via GenericArbitrary Nat instance Arbitrary Nat

testMap' :: Test
testMap' = TestList
  [ propertyToTest "аналогично map" \(Fun _ f :: Fun Int Int, xs :: [Int]) ->
      map f xs === map' f xs
  , propertyToTest "работает на бесконечных списках" \(Fun _ f :: Fun Int Int, Positive n) ->
      take n (map f [1 .. n]) === take n (map' f $ [1 .. n] ++ infinite)
  ]

testCountMatching :: Test
testCountMatching = TestList
  [ propertyToTest "аналогично `length . filter p`" \(Fun _ p, xs :: [Int]) ->
      length (filter p xs) === countMatching p xs
  ]

testTreeFold :: Test
testTreeFold = TestList
  [ lawsToTest $ foldableLaws $ Proxy @(TestTreeFoldable Tree)
  , TestCase $ assertEqual "order is right" [2, 5, 3] $ toList (wrap tree)
  ]
  where
    wrap = wrap2 @(TestTreeFoldable Tree)
    tree = Node (2 :: Int)
      [ Node 5 [Leaf]
      , Node 3 []
      , Leaf
      ]

testMyMap :: Test
testMyMap = TestList
  [ propertyToTest "empty map is empty" \(k :: Tiny) ->
      (Nothing :: Maybe Char) === myEmpty `myLookup` k
  , propertyToTest "insert works" \(k, v, m :: MyMap Tiny Char) ->
      Just v === myInsert k v m `myLookup` k
  , propertyToTest "myInsertMany works" \(kvs, m :: MyMap Tiny Char) ->
      let expected = Map.fromList kvs in
      let m' = myInsertMany m kvs in
      map ((expected !?) . fst) kvs === map ((m' `myLookup`) . fst) kvs
  , propertyToTest "myDelete works" \(k, m :: MyMap Tiny Char) ->
      Nothing === myDelete k m `myLookup` k
  , propertyToTest "myDeleteMany works" \(ks, m :: MyMap Tiny Char) ->
      let m' = myDeleteMany m ks in
      (Nothing <$ ks) === map (m' `myLookup`) ks
  , propertyToTest "MyMap contains its elements" \(keys :: Set Char, values :: [Int]) ->
      let content = zip (Set.toList keys) values in
      let expected = Map.fromList content in
      let actual = myInsertMany myEmpty content in
      forAll (oneof [elements (('\0', 4) : content), arbitrary]) \(k, _) ->
        expected !? k === actual `myLookup` k
  , propertyToTest "myDeleteMany works" \(keys :: Set Char, values :: [Int], toRemove :: [Char]) ->
      let content = zip (Set.toList keys) values in
      let expected = foldr Map.delete (Map.fromList content) toRemove in
      let actual = myInsertMany myEmpty content `myDeleteMany` toRemove in
      forAll (oneof [elements (('\0', 0) : content), arbitrary]) \(k, _) ->
        expected !? k === actual `myLookup` k
  ]

testMyMapMonoid :: Test
testMyMapMonoid = TestList
  [ lawsToTest $ semigroupLaws $ Proxy @(TestMyMapMonoid (MyMap Tiny Int))
  , lawsToTest $ monoidLaws $ Proxy @(TestMyMapMonoid (MyMap Tiny Int))
  , propertyToTest "union works properly" \(m1 :: Map Tiny Int, m2 :: Map Tiny Int) ->
      let expected = m1 <> m2 in
      let actual = wrap (mapToMyMap m1) <> wrap (mapToMyMap m2) in
      wrap expected === fmap myMapToMap actual
  ]
  where
    wrap = wrap1 @TestMyMapMonoid
    mapToMyMap = myInsertMany myEmpty . Map.toList
    myMapToMap m = Map.fromList $ catMaybes $ zipWith (curry sequence) domain (myLookup m <$> domain)

instance (Enum k, Bounded k, Eq v) => Eq (MyMap k v) where
  MyMap m1 == MyMap m2 = map m1 domain == map m2 domain

instance Show (MyMap k v) where
  show _ = "<MyMap>"

instance (Function k, CoArbitrary k, Arbitrary v) => Arbitrary (MyMap k v) where
  arbitrary = arbitrary <&> \(Fun _ f) -> MyMap f

data Tiny = One | Two | Three | Four | Five
  deriving stock (Eq, Show, Ord, Enum, Bounded, Generic)
  deriving Arbitrary via GenericArbitrary Tiny

instance Function Tiny
instance CoArbitrary Tiny

testMyMapFoldable :: Test
testMyMapFoldable = TestList
  [ lawsToTest $ foldableLaws $ Proxy @(TestMyMapFoldable (MyMap Int8))
  , propertyToTest "MyMap foldable works" \(m :: Map Int8 Int) ->
      let expected = sort $ foldMap (:[]) m in
      let actual = sort $ foldMap (:[]) $ wrap $ myInsertMany myEmpty (Map.toList m) in
      expected === actual
  ]
  where
    wrap = wrap2 @(TestMyMapFoldable (MyMap Int8))

testNeighborTreeFoldable :: Test
testNeighborTreeFoldable = TestList
   [ TestCase $ assertEqual "пример из условия" [1 .. 10] (foldMap (: []) $ wrap testTree)
   , lawsToTest $ foldableLaws $ Proxy @(TestNeighborTreeFoldable NeighborTree)
   ]
  where
    wrap = wrap2 @(TestNeighborTreeFoldable NeighborTree)
    testTree =
      NNode
        1
        (NNode 4 NLeaf NLeaf)
        (NNode
          2
          (NNode
            5
            (NNode 8 NLeaf NLeaf)
            (NNode 6 (NNode 9 (NNode 10 NLeaf NLeaf) NLeaf) NLeaf)
          )
          (NNode 3 (NNode 7 NLeaf NLeaf) NLeaf)
        )

instance Arbitrary a => Arbitrary (NeighborTree a) where
  arbitrary = sized $ fix \rec -> \case
    0 -> pure NLeaf
    n -> oneof [pure NLeaf, NNode <$> arbitrary <*> rec (n - 1) <*> rec (n - 1)]
  shrink = genericShrink
