module Shared where

import Block1
import Data.Function (fix)
import Test.Prelude

deriving stock instance Generic (Tree a)
instance Arbitrary a => Arbitrary (Tree a) where
  arbitrary = sized $ fix \rec -> \case
    0 -> pure Leaf
    n -> oneof
      [ pure Leaf
      , choose (0, 3) >>= \len -> Node <$> arbitrary <*> vectorOf len (rec $ n - 1)
      ]
  shrink = genericShrink
