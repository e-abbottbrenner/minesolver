# minesolver
A solver for minesweeper that for a given board, computes the probability of each unknown cell being a mine. You can also just play minesweeper with it.

# Installation
Only 64 bit is supported

1. Extract zip somewhere
0. Go to the bin folder in the extracted zip contents.
0. If you don't have the MSVC 2022 redist installed, run the included vc_redist.x64 to install it.
0. Run Minesolver. If it doesn't work and you skipped step 3, try step 3.

# Dependencies for building the code
Qt6 - Easiest way to get this is the open source installer, QtCreator is the best way to build the application as well.

boost multiprecision - cmake gets it

gtest - cmake gets it

# How fast is it?
The standard "expert" board (30x16 with 99 mines) is evaluated in a fraction of a second. It can fully solve(or hit a mine and lose) such a board in under a second.

For harder boards it will take longer to evaluate them.

# How much memory does it use?
Again not much for normal minesweeper boards that can be solved by people. But it can eat many gigabytes if you feed it a harder board (memory used is not guaranteed to be polynomial in the input).

# How it works
The probability of each unknown cell is determined by counting all the ways that cell could be a mine vs all the ways it could be clear.

This requires exploring the set of all valid minefields - which is exponential in size. A series of clever tricks and optimizations are used to substantially reduce the size of the space that needs to be explored. Minesweeper is NP-Complete though so there are still structures in the field that can lead to uncontrollable exponential growth(and if you look at the reduction from circuit satisfaction to minesweeper you'll see those exact structures getting built).

The core strategy of the algorithm is to construct the space of all valid minefields as a DAG, and then count valid paths in the DAG.

### The DAG

A single node in this DAG (in the code this is called a ChoiceNode) consists of coordinates for the unknown cell whose state is being chosen and a complete state of the revealed minefield. In the state, cells can either be counts of adjacent mines(count cells), unknown, or visited.

The DAG is broken down into columns (in the code these are called ChoiceColumn). Each column contains all of the choice nodes for a corresponding coordinate. When a board is difficult to solve, it is because there is exponential growth in the size of these columns. Every optimization is intended to fight this.

There are two connections forward out of each choice node. One is for when the unknown cell is a mine, the other for when it's clear. They connect to a node for another coordinate with the state updated to match the choice. This update marks the coordinate of the chosen cell as visited and updates the counts according to whether it is a mine or clear. The connection is not formed if the update would violate the constraints of the count cells(meaning an adjacent cell's count goes below zero or would be stalled above zero).

There is a final column appended to the end of the DAG. This is so all the choices made in the last cell can lead into a single node. The only valid state of this node is that all count cells are zero and all unknown cells are visited. No choices are made out of this final node.

When the full DAG is built, a valid path through it represents a valid state of a minefield. A path is valid if it has a certain exact cost (mine edges add one to the cost, clear edges add zero). If there are 40 mines and a path from beginning to end costs 40, that is a valid path. It will assign a value to every coordinate in the board (by virtue of pathing through each column) and use the appropriate number of mines.

Because whether a cell is a mine or not is determined by the connection - that state information is in the edges of the graph and does not need to be in the nodes. This prevents mine counts from contributing to the number of states in each column (a contribution that would be exponential in mine count).

Because the count cells are updated in each state transition, the states can just store whether a previous unknown cell has been visited or not rather than whether it's a mine or clear. This prevents the previously visited cells from contributing to the number of nodes in each column (a contribution that would be exponential in the number of unknown cells).

There is just one remaining aspect that can contribute exponentially to the number of nodes in a column: The number of count cells that can have more than one value in the column's nodes' states. The number of nodes in the column will be exponential in the number of these count cells.


### Count the paths
For a given node in the DAG, there can be a certain number of mines used before it and a certain number of mines used after it, the sum of those mine counts must be the total number of mines in the unknown cells.

So for each possible combination of mines before and after: Count valid paths back to the start. Multiply this with the valid paths forward that treat the unknown cell as a mine or clear for the total ways the cell could be a mine or clear.

To get the total for a column, sum the ways each node could be a mine or not over all possible combinations. The total in the column is how many ways that column's unknown cell could be a mine or not for the current state of the board. Probability is trivial to calculate from here.

This just leaves how to count the paths to the start or end using a certain number of mines. This is a relatively straightforward dynamic programming problem:
* You start with the final node. There is one valid path there that uses zero mines and no valid paths that use more than zero mines.
* You then walk through each previous node. At each node _node_, the number of valid paths using _n_ mines (_paths(node,n)_) is the sum over all forward edges _edge_ of _paths(edge.node, n - edge.cost)_ 
* These values are computed for all possible values of mine count and stored. The same computation is also applied in the reverse direction

Since _paths(node, n)_ is known forward and backward for each of the paths, it is easy to get path counts required to compute the probabilities.

# Optimizations Used
### Choosing a good ordering for the columns
Every count cell adjacent to the path of column choices is influenced by it. For a count cell, once every adjacent unknown cell is in the path, there is only one possible count - zero. For count cells that are not influenced by the path there is also only one possible count - the original count of that cell.

So a good ordering is one that minimizes the number of count cells that are currently influenced and not fully "closed" by the path. These "fringe" cells that can have more than one value contribute exponentially to the runtime.

The ordering used by the solver currently is to just follow rows or columns. While not guaranteed to be minimal, this does put an upper bound on the number of fringe cells based on the lesser value of the field's width/height. This seems to work well enough in practice.

### Eliminating unknown cells with known states
Many unknown cells have a probability of 0 or 1. These never change with more information. This is often the case in formations that cause the fringe to get out of control.

So another optimization in this program is to simply remember if past calculations for a cell have already been proven to be either a mine or clear. If such proof exists, then their status is pre-applied to the starting state of the board.

It is also possible to find many of these by simply checking for count cells that have exactly as many adjacent unknown cells as their counts. All of these must be mines. Additionally, if a count cell is zero, none of the adjacent unknown cells can be mines.

### Using math to calculate possibilities in the "open ocean" part of the board
Many unknown cells have no adjacent count cells. These cells can have the number of ways they could be a mine or clear calculated with the choose operator because you have a certain number of them and you're choosing a certain number of mines to distribute among them.

### Use floating point instead of unlimited precision ints
This has no practical impact on the results and is much much faster.

# Limitations
If you make the field too big, the solver will get slow. If the field gets really big, it might have too many states for floating point arithmetic to function.

# Testing and confirmation
The code contains a test to verify that the computed probabilities are correct: 
* Many random inputs are generated for the solver.
* The solver runs on each input
* The test samples every unknown cell on the board of the input and records the number of mines and clear cells into a bucket(nearest multiple of 5%) corresponding to the probability reported by the solver.
* The sampled rate of being a mine for each bucket is compared to the average solver probability. The test passes if in each bucket, the difference between the average solver probability and the sampled rate is within the margin of error with 99.99% certainty.

The test is tuned to generate hundreds to thousands of samples in every bucket. It consistently passes.
