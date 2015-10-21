# Octarine Optimizer

In memory of Terry Pratchett.

My entry to [this year's programming competition](http://samegame.asta-wedel.de/) at my university.

## Task

The task was to write an AI to quickly find good solutions for "same-game", i.e. a rectangular field of blocks where clicking two or more neighbouring blocks of the same color removes them, awarding score and making the remaining blocks drop down. Larger matches give quadratically more points and the program that finds the best solution in a fixed time wins the round.

## My solution

My solution is two-fold: I have multiple algorithms for choosing which piece to pick next (the step algorithms in [step/](src/step)), and multiple algorithms for choosing which of those algorithms to choose (in [algorithms.cpp](src/algorithms.cpp)).

### Step Algorithms

* **Highest Immediate Score**  
  Orders the possible moves in descending order of the score they net.
* **Lowest Immediate Score**  
  Same, but in ascending order.
* **Creating Fewest Matches** 
  The one which leaves fewer matches, which are thus hopefully larger.
* **Creating Fewest Matches, Avoiding Termination**  
  The previous algorithm has the risk of running into dead ends by resulting in no matches at all; this version penalizes those results.
* **Creating Largest Match**  
  The one which leaves the largest match on the board.
* **Creating Smallest Mean Distance To Centroid**  
  Averages the positions of each tile of each color to find the centroid of that color; then averages the distances to the centroids, preferring smaller average distance. This encourages results where blocks of the same color are close together.
* **Creating Smallest Maximum Distance To Centroid**  
  Also uses the centroids, but only scores by the maximum distance to it; this discourages outliers.

## Step-Series Algorithms

* **Single Best** 
  Best First Search (or Best Only, really): Repeatedly applies a given Step Algorithm, expanding only the preferred solution. Very quick result of acceptable quality, run once for each Step Algorithm.
* **N Best**  
  Limited Depth First Search: Repeatedly applies a given Step Algorithm, expanding only the N preferred solutions. Exponentially growing requirements, thus only used on small fields, with N=2 and using the Lowest Immediate Score and Highest Immediate Score Step Algorithms.
* **Priority Expand Heuristically**  
  Weird Breadth First / Best First Hybrid Search: Expands the N best nodes (as per total score) of the previous level of the tree using the most preferred step of each of the Step Algorithms; i.e. assuming each of the 7 Step Algorithms returns a different preferred step there will be 7\*N nodes on the next level, of which the 6\*N worst are dropped.  
  Run in parallel with N=16, N=64 and N=256; if the field is found to be sufficiently small also with N=1024 and N=8096. (That was supposed to be 8192, I suppose. No matter.)
* **Priority Expand All**  
  Variation of the above without the heuristics, instead expanding all child nodes on a tree level and keeping only the N best; found to be subpar and not actually used.
* **Randomly**
  Randomly picks its moves based on a deterministic seed (because I don't like having to get lucky); then backtracks and runs Single Best from each step. Started on each thread (with varying seeds; `0xBADF00D`, `0xC0DED00D`, `0x1337C0DE`and `0x31415927`) after the other algorithms have finished. Gets disturbingly good results.

## The Code

The code is generally fairly light on documentation, but a good place to start is [main.cpp](src/main.cpp). It uses pretty straightforward C++11 to set up the threads; the main thing of note is the use of `std::ref()` to prevent copies when forwarding arguments. (Since the [ResultManager](src/result_manager.hpp) is noncopyable (due to `std::mutex` being noncopyable) this is caught by the compiler; yay for static typing!)

The [Step Algorithms](src/step/) are also pretty straightforward, some template usage aside. (Templates are occasionally used to pass a function pointer thoughout the code - this is to improve performance, since that way the work can be done during compilation instead of at runtime. This is probably a case of premature optimization at the cost of readability though...)

The Step-Series Algorithms are in [algorithms.cpp](src/algorithms.cpp) and somewhat more complex, but the above descriptions should give you an idea of what they do.

The [Field](src/field.hpp) data structure is also of interest; it represents the whole game field using an array and provides a method of retrieving all the matches (i.e. connected blocks), which are calculated using a fairly naive flood fill. (There's potential for optimization here, I believe.) The matches are not stored in a new dynamic array, which would have to be allocated on each function call, but instead use a supplied one, which can thus be re-used. 

## Results

In the end my solution was beat only by that of the organizer, effectively netting me 1st place and thus the first pick of the book prizes. So I'm now the proud owner of "Structure and Interpretation of Computer Programs" - I've been meaning to learn Scheme for a while now, and this should be an excellent introduction.

I will check which algorithm ended up finding the winning solution in each round once the data that was used is released.
