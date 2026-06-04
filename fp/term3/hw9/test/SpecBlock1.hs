module SpecBlock1 where

import Block1
import MyIO
import Test.Prelude

tests :: NamedTests
tests = nameTests 1
  [ testPrintFibs
  , testPoint3
  , testTranspose
  , testTree
  ]

testPrintFibs :: Test
testPrintFibs = TestList
  [ propertyToTest "printFibs" \n ->
      unsafeWorld [] (take n fibs) === execUnsafeMyIO (printFibs n) emptyWorld
  ]
  where
    fibs = 0 : 1 : zipWith (+) fibs (tail fibs)

testPoint3 :: Test
testPoint3 = TestList
  [ lawsToTest $ applicativeLaws $ Proxy @Point3
  , lawsToTest $ foldableLaws $ Proxy @Point3
  , lawsToTest $ traversableLaws $ Proxy @Point3
  , propertyToTest "traverse works" \s1 s2 s3 ->
      let expected = (s1 ++ s2 ++ s3, Point3 ('!' : s1) ('!' : s2) ('!' : s3)) in
      let actual = traverse (\s -> (s, '!' : s)) (Point3 s1 s2 s3) in
      expected === actual
  , TestCase $ assertEqual "printSum" (unsafeWorld [] [3, 4, 5, 1, 2, 3, 4, 6, 8]) $
      execUnsafeMyIO (printSum (Point3 3 4 5) (Point3 1 2 3)) emptyWorld
  , TestCase $ assertEqual "экземпляр Traversable Point3 правильно реализован"
      [ Point3 1 2 3
      , Point3 1 2 6
      , Point3 1 5 3
      , Point3 1 5 6
      , Point3 4 2 3
      , Point3 4 2 6
      , Point3 4 5 3
      , Point3 4 5 (6 :: Int)
      ]
      (sequence $ Point3 [1, 4] [2, 5] [3, 6])
  ]

deriving stock instance Generic (Point3 a)
instance Arbitrary a => Arbitrary (Point3 a) where
  arbitrary = Point3 <$> arbitrary <*> arbitrary <*> arbitrary
  shrink = genericShrink

testTranspose :: Test
testTranspose = TestList
  [ TestCase $ assertEqual "матрица транспонируется"
       [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
       (transpose [[1, 4, 7], [2, 5, 8], [3, 6, 9]])
  , TestCase $ assertEqual "одноэлементная матрица транспонируется" [[1]] (transpose [[1]])
  , TestCase $ assertEqual "пустая матрица транспонируется" ([] :: [[Int]]) (transpose [])
  , TestCase $ assertEqual "экземпляр Traversable Point3 правильно реализован"
      [ Point3 1 2 3
      , Point3 1 2 6
      , Point3 1 5 3
      , Point3 1 5 6
      , Point3 4 2 3
      , Point3 4 2 6
      , Point3 4 5 3
      , Point3 4 5 (6 :: Int)
      ]
      (sequence $ Point3 [1, 4] [2, 5] [3, 6])
  ]

testTree :: Test
testTree = TestList
  [ lawsToTest $ foldableLaws $ Proxy @PreorderTraversal
  , lawsToTest $ traversableLaws $ Proxy @PreorderTraversal
  , lawsToTest $ foldableLaws $ Proxy @InorderTraversal
  , lawsToTest $ traversableLaws $ Proxy @InorderTraversal
  , lawsToTest $ foldableLaws $ Proxy @PostorderTraversal
  , lawsToTest $ traversableLaws $ Proxy @PostorderTraversal
  , TestCase $ assertEqual "Preorder-обход правильный" huh1
      (sequence $ PreorderTraversal tree1)
  , TestCase $ assertEqual "Postorder-обход правильный" huh2
      (sequence $ PostorderTraversal tree1)
  , TestCase $ assertEqual "Inorder-обход правильный" huh3
      (sequence $ InorderTraversal tree1)
  , TestCase $ assertEqual "Postorder-обход правильный" "125364"
      (fst $ sequence $ PostorderTraversal $ (\x -> (show x, x)) <$> tree2)
  , TestCase $ assertEqual "Preorder-обход правильный" "416325"
      (fst $ sequence $ PreorderTraversal $ (\x -> (show x, x)) <$> tree2)
  , TestCase $ assertEqual "Inorder-обход правильный" "146235"
      (fst $ sequence $ InorderTraversal $ (\x -> (show x, x)) <$> tree2)
  , TestCase $ assertEqual "Postorder-обход правильный" (PostorderTraversal tree2)
      (snd $ sequence $ PostorderTraversal $ (\x -> (show x, x)) <$> tree2)
  , TestCase $ assertEqual "Preorder-обход правильный" (PreorderTraversal tree2)
      (snd $ sequence $ PreorderTraversal $ (\x -> (show x, x)) <$> tree2)
  , TestCase $ assertEqual "Inorder-обход правильный, " (InorderTraversal tree2)
      (snd $ sequence $ InorderTraversal $ (\x -> (show x, x)) <$> tree2)
  ]
  where
    tree1 = Node1 (Node1 Leaf1 [1, 2, 3] Leaf1) [4, 5] (Node1 Leaf1 [6 :: Int .. 9] Leaf1)
    tree2 = Node1 (Node1 Leaf1 1 Leaf1) 4 (Node1 Leaf1 6 (Node1 (Node1 Leaf1 2 Leaf1) 3 (Node1 Leaf1 5 Leaf1)))
    huh1 =
      [ PreorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 4 (Node1 Leaf1 6 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 4 (Node1 Leaf1 7 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 4 (Node1 Leaf1 8 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 4 (Node1 Leaf1 9 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 4 (Node1 Leaf1 6 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 4 (Node1 Leaf1 7 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 4 (Node1 Leaf1 8 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 4 (Node1 Leaf1 9 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 4 (Node1 Leaf1 6 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 4 (Node1 Leaf1 7 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 4 (Node1 Leaf1 8 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 4 (Node1 Leaf1 9 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 5 (Node1 Leaf1 6 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 5 (Node1 Leaf1 7 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 5 (Node1 Leaf1 8 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 5 (Node1 Leaf1 9 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 5 (Node1 Leaf1 6 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 5 (Node1 Leaf1 7 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 5 (Node1 Leaf1 8 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 5 (Node1 Leaf1 9 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 5 (Node1 Leaf1 6 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 5 (Node1 Leaf1 7 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 5 (Node1 Leaf1 8 Leaf1))
      , PreorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 5 (Node1 Leaf1 9 Leaf1))
      ]
    huh2 =
      [ PostorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 4 (Node1 Leaf1 6 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 5 (Node1 Leaf1 6 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 4 (Node1 Leaf1 7 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 5 (Node1 Leaf1 7 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 4 (Node1 Leaf1 8 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 5 (Node1 Leaf1 8 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 4 (Node1 Leaf1 9 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 5 (Node1 Leaf1 9 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 4 (Node1 Leaf1 6 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 5 (Node1 Leaf1 6 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 4 (Node1 Leaf1 7 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 5 (Node1 Leaf1 7 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 4 (Node1 Leaf1 8 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 5 (Node1 Leaf1 8 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 4 (Node1 Leaf1 9 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 5 (Node1 Leaf1 9 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 4 (Node1 Leaf1 6 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 5 (Node1 Leaf1 6 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 4 (Node1 Leaf1 7 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 5 (Node1 Leaf1 7 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 4 (Node1 Leaf1 8 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 5 (Node1 Leaf1 8 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 4 (Node1 Leaf1 9 Leaf1))
      , PostorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 5 (Node1 Leaf1 9 Leaf1))
      ]
    huh3 =
      [ InorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 4 (Node1 Leaf1 6 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 4 (Node1 Leaf1 7 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 4 (Node1 Leaf1 8 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 4 (Node1 Leaf1 9 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 5 (Node1 Leaf1 6 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 5 (Node1 Leaf1 7 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 5 (Node1 Leaf1 8 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 1 Leaf1) 5 (Node1 Leaf1 9 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 4 (Node1 Leaf1 6 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 4 (Node1 Leaf1 7 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 4 (Node1 Leaf1 8 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 4 (Node1 Leaf1 9 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 5 (Node1 Leaf1 6 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 5 (Node1 Leaf1 7 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 5 (Node1 Leaf1 8 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 2 Leaf1) 5 (Node1 Leaf1 9 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 4 (Node1 Leaf1 6 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 4 (Node1 Leaf1 7 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 4 (Node1 Leaf1 8 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 4 (Node1 Leaf1 9 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 5 (Node1 Leaf1 6 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 5 (Node1 Leaf1 7 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 5 (Node1 Leaf1 8 Leaf1))
      , InorderTraversal (Node1 (Node1 Leaf1 3 Leaf1) 5 (Node1 Leaf1 9 Leaf1))
      ]

deriving newtype instance Arbitrary a => Arbitrary (PreorderTraversal a)
deriving newtype instance Arbitrary a => Arbitrary (PostorderTraversal a)
deriving newtype instance Arbitrary a => Arbitrary (InorderTraversal a)
deriving via GenericArbitrary (Tree1 a) instance Arbitrary a => Arbitrary (Tree1 a)
