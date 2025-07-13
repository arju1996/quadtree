#pragma once


#include <functional>
#include <memory>
#include <vector>

template <typename OBJECT_NAME, typename PointType, typename BoundType>
class QuadTree;

template <typename OBJECT_NAME, typename PointType, typename BoundType>
class QuadTreeNode {
public:
    using PositionGetter = std::function<PointType(const OBJECT_NAME&)>;
    QuadTreeNode() = default;
    QuadTreeNode(std::vector<OBJECT_NAME*> data, PositionGetter position_getter, BoundType boundry, int capacity, int depth) = delete; // adding just for fun
    ~QuadTreeNode() = default;
    // getters
    const BoundType& getBoundry() const { return boundry_; }
    const bool getisleaf() const { return isLeaf_; }
    const std::vector<OBJECT_NAME*>& getPoints() const { return points_; }
    const std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& getTLChild() const { return tl_child_; }
    const std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& getTRChild() const { return tr_child_; }
    const std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& getBLChild() const { return bl_child_; }
    const std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& getBRChild() const { return br_child_; }
    friend class QuadTree<OBJECT_NAME, PointType, BoundType>;
private:
    BoundType boundry_;
    int depth_;
    bool isLeaf_;
    std::vector<OBJECT_NAME*> points_;
    std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>> tl_child_, tr_child_, bl_child_, br_child_;
};

template <typename OBJECT_NAME, typename PointType, typename BoundType>
class QuadTree {
public:
    using PositionGetter = std::function<PointType(const OBJECT_NAME&)>;
    std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>> BuildTree(std::vector<OBJECT_NAME*> points, PositionGetter position_getter, BoundType boundry, int capacity, int max_depth, int depth = 0);
    std::vector<OBJECT_NAME*> findNeighbors(QuadTreeNode<OBJECT_NAME, PointType, BoundType>* root, PointType point);
    void insert(std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& root, OBJECT_NAME* object);
    void subdivide(std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& current);
    void initializeChild(std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& child, BoundType boundry, int depth, std::vector<OBJECT_NAME*> points = {});

private:
    PositionGetter position_getter_;
    int capacity_;
    int max_depth_;
};



//implementations
template <typename OBJECT_NAME, typename PointType, typename BoundType>
void QuadTree<OBJECT_NAME, PointType, BoundType>::initializeChild(
            std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& child,
            BoundType boundry,
            int depth,
            std::vector<OBJECT_NAME*> points) {
    child->boundry_ = boundry;
    child->depth_ = depth;
    child->points_ = points;
    child->isLeaf_ = true;
}

template <typename OBJECT_NAME, typename PointType, typename BoundType>
void QuadTree<OBJECT_NAME, PointType, BoundType>::subdivide(std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& current) {
    
    if (!current) throw std::runtime_error("node is null");
    if (!current->getisleaf()) {
        return ;
    }
    if (current->depth_ + 1 > max_depth_) {
        return ;
    }

    BoundType tl, tr, bl, br;
    PointType mid = (current->boundry_.tl + current->boundry_.br) / 2.0f;
    tl.tl = current->boundry_.tl;
    tl.br = mid;
    tr.tl = {mid.x, current->boundry_.tl.y};
    tr.br = {current->boundry_.br.x, mid.y};
    bl.tl = {current->boundry_.tl.x, mid.y};
    bl.br = {mid.x, current->boundry_.br.y};
    br.tl = mid;
    br.br = current->boundry_.br;

    std::vector<OBJECT_NAME*> points_tl, points_tr, points_bl, points_br; 
    for (auto obj: current->points_) {
        const PointType& item = position_getter_(*obj);
        // is a point within which of the crossing?
        if(item.x < mid.x) {
            if(item.y < mid.y) {
                points_tl.push_back(obj);
            } else {
                points_bl.push_back(obj);
            }
        } else {
            if(item.y < mid.y) {
                points_tr.push_back(obj);
            } else {
                points_br.push_back(obj);
            }
        }
    }

    std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>> childs[4];
    childs[0] = std::make_unique<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>();
    childs[1] = std::make_unique<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>();
    childs[2] = std::make_unique<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>();
    childs[3] = std::make_unique<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>();

    initializeChild(childs[0], tl, current->depth_ + 1, points_tl);
    initializeChild(childs[1], tr, current->depth_ + 1, points_tr);
    initializeChild(childs[2], bl, current->depth_ + 1, points_bl);
    initializeChild(childs[3], br, current->depth_ + 1, points_br);

    current->tl_child_ = std::move(childs[0]);
    current->tr_child_ = std::move(childs[1]);
    current->bl_child_ = std::move(childs[2]);
    current->br_child_ = std::move(childs[3]);
    current->isLeaf_ = false;

    // should subdivide further?
    if(points_tl.size() >= capacity_ && true) {
        subdivide(current->tl_child_);
    }
    // does this child have enough points to warrant subdivision
    if(points_tr.size() >= capacity_) {
        subdivide(current->tr_child_);
    }
    if(points_bl.size() >= capacity_) {
        subdivide(current->bl_child_);
    }
    if(points_br.size() >= capacity_) {
        subdivide(current->br_child_);
    }
}
template <typename OBJECT_NAME, typename PointType, typename BoundType>
void QuadTree<OBJECT_NAME, PointType, BoundType>::insert(std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& root, OBJECT_NAME* object) {
    
    if (!object) throw std::runtime_error("object is null");
    if (!root) throw std::runtime_error("root is null");
    if (!position_getter_) throw std::runtime_error("position functor is null");


    const PointType& item = position_getter_(*object);
    
    BoundType boundry = root->getBoundry();
    PointType mid = (boundry.tl + boundry.br) / 2.0f;

    // make this bit neater    
    if(root->getisleaf()) {
        // can I add one more without exceeding capacity
        if(root->points_.size() < capacity_) {
            root->points_.push_back(object);
        } else {
            // split it if level isnt too much;
            if(root->depth_ < max_depth_) {
                subdivide(root);
            } else {
                // can final leaf nodes exceed capacity?
                // root->points_.push_back(object);
                throw std::runtime_error("cannot insert, size full");
            }
        }
    }

    if(!root->getisleaf()) {
        if(item.x < mid.x) {
            if(item.y < mid.y) {
                insert(root->tl_child_, object);
            } else {
                insert(root->bl_child_, object);
            }
        } else {
            if(item.y < mid.y) {
                insert(root->tr_child_, object);
            } else {
                insert(root->br_child_, object);
            }
        }
    }
}

template <typename OBJECT_NAME, typename PointType, typename BoundType>
std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>> QuadTree<OBJECT_NAME, PointType, BoundType>::BuildTree(std::vector<OBJECT_NAME*> points,
        PositionGetter position_getter,
        BoundType boundry,
        int capacity,
        int max_depth,
        int depth
        ) {
    
    position_getter_ = position_getter;
    max_depth_ = max_depth;
    // step1: split to 4;
    // step2: find and assign points to 4 of the child nodes;
    // step3: call BuildTree recursively on the 4 child nodes;
    // step4: return the current node which will be the root of the tree;


    //edge case
    auto current = std::make_unique<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>();

    if(points.size() <= capacity || depth >= max_depth) {
        current->boundry_ = boundry;
        current->depth_ = depth;
        current->isLeaf_ = true;
        current->points_ = points;
        current->tl_child_ = nullptr;
        current->tr_child_ = nullptr;
        current->bl_child_ = nullptr;
        current->br_child_ = nullptr;

        return current;
    }


    BoundType tl, tr, bl, br;

    PointType mid = (boundry.tl + boundry.br) / 2.0f;
    tl.tl = boundry.tl;
    tl.br = mid;

    tr.tl = {mid.x, boundry.tl.y};
    tr.br = {boundry.br.x, mid.y};

    bl.tl = {boundry.tl.x, mid.y};
    bl.br = {mid.x, boundry.br.y};

    br.tl = mid;
    br.br = boundry.br;

    std::vector<OBJECT_NAME*> points_tl, points_tr, points_bl, points_br; 
    for (auto obj: points) {
        const PointType& item = position_getter(*obj);
        // is a point within which of the crossing?
        if(item.x < mid.x) {
            if(item.y < mid.y) {
                points_tl.push_back(obj);
            } else {
                points_bl.push_back(obj);
            }
        } else {
            if(item.y < mid.y) {
                points_tr.push_back(obj);
            } else {
                points_br.push_back(obj);
            }
        }
    }
    
    // first find the data that will come in tl_child;
    current->tl_child_ = BuildTree(points_tl, position_getter, tl, capacity, max_depth, depth + 1);
    current->tr_child_ = BuildTree(points_tr, position_getter, tr, capacity, max_depth, depth + 1);
    current->bl_child_ = BuildTree(points_bl, position_getter, bl, capacity, max_depth, depth + 1);
    current->br_child_ = BuildTree(points_br, position_getter, br, capacity, max_depth, depth + 1);
    current->boundry_ = boundry;
    current->depth_ = depth;
    current->isLeaf_ = false;
    return current;
}

template <typename OBJECT_NAME, typename PointType, typename BoundType>
std::vector<OBJECT_NAME*> QuadTree<OBJECT_NAME, PointType, BoundType>::findNeighbors(QuadTreeNode<OBJECT_NAME, PointType, BoundType>* root, PointType point) {
    // need function to check if a point is inside a box/boundry
    if (root == nullptr) {
        return {};
    }
    if(!root->boundry_.contains(point)) {
        return {};
    } else {
        if(root->isLeaf_) {
            return root->points_;
        } else {
            PointType mid = (root->boundry_.tl + root->boundry_.br) / 2.0f;
            if(point.x < mid.x) {
                if(point.y < mid.y) {
                    return findNeighbors(root->tl_child_.get(), point);
                } else {
                    return findNeighbors(root->bl_child_.get(), point);
                }
            } else {
                if(point.y < mid.y) {
                    return findNeighbors(root->tr_child_.get(), point);
                } else {
                    return findNeighbors(root->br_child_.get(), point);
                }
            }
        }
    }
}

