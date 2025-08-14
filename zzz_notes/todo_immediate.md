main task:
- [] make quadtree able to test moving items without recreation
not yet completed: ie its an big task;

done:
- [x] BugFix(neighbors empty) : boid isnt behaving like boids;
fix: move removing boid from qtree from before to after ApplyRules2,
cause it was searching for beighbors in an empty qtree
- [x] BugFix(strict collapse) : add 6 points to make some subdivision, then remove 2 from that, so technically now it should/can collapse, but its not happening;
fix: add strictCollapseWhenUnderCapacity flag

pending:

- [] [PerfImp: remove] : immediate or delayed or both cleanup
- [] [PerfImp: remove] : add parent to each node; 
- [] [PerfImp: remove] : add a vector of faulty nodes;
- [] : im not using the flag needCleanup_ that i set;


===
===
===
temp :
assume delayed then whats pending ?
delayed cleaning:
options:
1. full recreation n log n
2. cleaning from top to bottom visiting all items; log n
3. using memory to keep track of all faulty/removed nodes and only update that and its childrens
and an additional parent pointer in all the nodes; k
==
lets try 3 now:




