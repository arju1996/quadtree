*. create object pool and take the stuffs from it, to reduce dynamic memory allocations

- [] make quadtree able to test moving items without recreation
not yet completed: ie its an big task;

- [] : im not using the flag needCleanup_ that i set;

=uniform initialization?=?

*. PositionGetter understand why, or improve if possible
*. distribute quadtree in 1.headeronly, 2.static, 3.shared
*. linux windows cmake
*. quadtreerenderer class
*. ideally i should move random number generator to the math header

==
random:
*.parellelize things that can be, say like searching neighbors;


===
*.
why QuadTree<OBJECT_NAME, PointType, BoundType>::remove should call both
exists and
findnode_thisitem_existing

*.
technically almost all the root in
QuadTree should actually be named as current or node

*.
cleanupFromFaultyList3
doesn't need node as argument
===
===
===
main tasks:
create dynamic quad tree