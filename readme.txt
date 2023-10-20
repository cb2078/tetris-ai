Tweaks to the implementation to make it more similar to the nes game

Write a search function to find all the placements given a shape.  BFS should
be sufficient for now.  Depends on a hash table for remembering visited
positions and a queue for possible positions.

Evaluation with neural network?  The inputs should be each cell in the board
and the drop speed.  The drop speed is important on lower levels as it can
afford to be more aggressive.

Rienforcement learning CTM style.

Maybe optmizations if this ends up being slow.
