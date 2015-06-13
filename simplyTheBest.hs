import Control.Monad.State
import Control.Monad.Trans.Maybe
import System.IO
import Control.Exception
import Data.List

type Field = [ [ Integer ] ]

data GameState = GameState
    {   field :: Field
    ,   score :: Integer
    ,   width :: Int
    ,   height :: Int
    }

type Move = ( Int, Int )
type Moves = [ Move ]

main :: IO ()
main = do
    field <- ( readLn :: IO Field )
    let initialState = GameState{
        field = field,
        score = 0,
        width = case field of
            [] -> 0
            ( x : xs ) -> length x,
        height = length field
    }
    runMaybeT $ do
        -- initial move is the best so far
        ( initialScore, moves ) <- processLine initialState
        lift $ print moves
        runStateT ( filterBest initialState ) initialScore
    return ()

filterBest :: GameState -> StateT Integer ( MaybeT IO ) ()
filterBest initialState = do
    ( score, moves ) <- lift $ processLine initialState
    best <- get
    when ( score > best ) $ do
        put score
        lift $ lift $ print moves
    filterBest initialState

-- read lines until you get a valid one
-- return Just it and its score
-- or Nothing if at EOF
processLine :: GameState -> MaybeT IO ( Integer, Moves )
processLine initialState = do
    eof <- lift isEOF
    guard $ not eof
    moves <- lift ( readLn :: IO Moves )
    case evalState ( runMaybeT $ runGame moves ) initialState of
        Just score -> do
            lift $ hPutStrLn stderr $ "new solution " ++ show moves ++ " has score " ++ show score
            return ( score, moves )
        Nothing -> do
            lift $ hPutStrLn stderr $ "illegal solution: " ++ show moves
            processLine initialState

addScore :: Integer -> State GameState ()
addScore increase = do
    modify $ \state -> state { score = score state + increase }
    return ()

applyPenalty :: State GameState ()
applyPenalty = do
    state <- get
    let penalty = sum $ map ( ( ^ 2 ) .  pred . toInteger . length ) $ group $ sort $ filter ( /= 0 ) $ concat $ field state
    put state { score = score state - penalty }
    return ()

doMove :: Move -> MaybeT ( State GameState ) ()
doMove ( x, y ) = do
    state <- get
    -- no result in case of invalid move
    guard ( x < width state && y < height state )
    -- TODO
    lift $ addScore 1
    return ()

runGame :: Moves -> MaybeT ( State GameState ) Integer
runGame [] = do
    -- last move? calculate penalty and return score
    lift applyPenalty
    state <- lift $ get
    return $ score state
runGame ( move : moves ) = do
    doMove move
    runGame moves
