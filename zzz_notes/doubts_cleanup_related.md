1.
https://claude.ai/share/6cae0779-ed10-4a82-86f5-c102ad8277e0
std::function might not be the right thing to do to get the point;

tell me if this is a good usecase of std::function or not.
so im writing a quadtree template class.

template <typename OBJECT_NAME, typename PointType, typename BoundType>
class QuadTree {
public:
    using PositionGetter = std::function<PointType(const OBJECT_NAME&)>;
}

and i read the PositionGetter position_getter from its constructor.
object_name curresponds to datatype of which this quadtree is made.
like it can be boid, circle, trees_from_nature, bus, anything.
PointType says if the node of the quadtree a point or a rectagle(to represent a bus)
BoundType ya as the name suggests its just boundary right now im making boundary only as rect. but later why not a cube or something.
so problem is i want to access the position from the object_name,
for that only im using this positiongetter.
does my idea look good or should i use something else or any better ideas?