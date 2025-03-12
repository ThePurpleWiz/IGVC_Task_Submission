# IGVC_Task_Submission
Q1) Solution in pathfinding.c

Solved using modified A*. Set nodes to include the entire state (position, speed and heading) instead of just position and search the entire phase space for solutions. To form a complete loop, the program makes checkpoints at the 4 corners and pathfinds to the checkpoints at turning speed.

Possible improvements:
1. Improve heuristic to penalise high speed near goal
2. Improve memory and time efficiency by giving partial map to each segment
3. Refactor and clean the main a* function
4. Make null value fail-safes
