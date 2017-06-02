{-# LANGUAGE Strict, StrictData #-}
import Data.Array (listArray, (!))
import Data.Ix (range)
import Control.Monad (mapM_)

data Op = Ins Int Char | Sub Int Char | Del Int
instance Show Op where
    show (Ins x c) = "INS " ++ show x ++ (' ' : c : [])
    show (Sub x c) = "SUB " ++ show x ++ (' ' : c : [])
    show (Del x)   = "DEL " ++ show x

dist x y = d' ! (lx, ly) where
    (lx, ly) = (length x, length y)
    x' = listArray (0, lx - 1) x
    y' = listArray (0, ly - 1) y
    d (0, 0) = (0, [])
    d (i, 0) = log (d' ! (i - 1, 0)) $ Del (i - 1)
    d (0, j) = log (d' ! (0, j - 1)) $ Ins 0       (y' ! (j - 1))
    d (i, j)
        | x' ! (i - 1) == y' ! (j - 1) = d' ! (i - 1, j - 1)
        | otherwise = minimumFst [ log (d' ! (i - 1, j - 1)) $ Sub (i - 1) (y' ! (j - 1))
                                 , log (d' ! (i - 1, j))     $ Del (i - 1)
                                 , log (d' ! (i, j - 1))     $ Ins i       (y' ! (j - 1))
                                 ]
    minimumFst (x@(a, _) : y@(b, _) : rem)
        | a <= b = minimumFst $ x : rem
        | otherwise = minimumFst $ y : rem
    minimumFst (x : []) = x
    log (r, o) a = (r + 1, a : o)
    d' = listArray bounds $ map d $ range bounds
    bounds = ((0, 0), (lx, ly))

main = do
    x <- getLine
    y <- getLine
    let (res, op) = dist x y
    print res
    mapM_ print . reverse $ op
