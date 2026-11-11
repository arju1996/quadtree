#pragma once


#include <functional>
#include <memory>
#include <vector>
#include <unordered_set>

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
    bool needCleanup_;
    std::vector<OBJECT_NAME*> points_;
    std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>> tl_child_, tr_child_, bl_child_, br_child_;
    QuadTreeNode<OBJECT_NAME, PointType, BoundType>* parent_;
};

enum class CleanupStrategy {FaultyList, Sweep};

template <typename OBJECT_NAME, typename PointType, typename BoundType>
class QuadTree {
public:
    using PositionGetter = std::function<PointType(const OBJECT_NAME&)>;
    std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>> BuildTree(QuadTreeNode<OBJECT_NAME, PointType, BoundType> *parent, std::vector<OBJECT_NAME*> points, PositionGetter position_getter, BoundType boundry, int capacity, int max_depth, int depth = 0);
    std::vector<OBJECT_NAME*> findNeighbors(QuadTreeNode<OBJECT_NAME, PointType, BoundType>* root, PointType point);
    std::vector<OBJECT_NAME*> findNeighbors(QuadTreeNode<OBJECT_NAME, PointType, BoundType>* root, BoundType point);
    void insert(std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& root, OBJECT_NAME* object);
    void remove(std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& root, OBJECT_NAME* object);
    void cleanup(std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& root);

private:
    PositionGetter position_getter_;
    int capacity_;
    int max_depth_;
    std::unordered_set<QuadTreeNode<OBJECT_NAME, PointType, BoundType>*> faultyList;
    CleanupStrategy cleanupstrategy_ = CleanupStrategy::FaultyList;

    // for remove
    bool exists(const std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& current, const OBJECT_NAME* object) const;
    QuadTreeNode<OBJECT_NAME, PointType, BoundType>* findnode_thisitem_existing(const std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& root, const OBJECT_NAME* object);

    // for insert
    void subdivide(std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& current);
    void initializeChild(std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& child, QuadTreeNode<OBJECT_NAME, PointType, BoundType>* parent, BoundType boundry, int depth, std::vector<OBJECT_NAME*> points = {});

    // for findNeighbors
    bool partInChild(const QuadTreeNode<OBJECT_NAME, PointType, BoundType>* current, BoundType rect) const;
    std::vector<OBJECT_NAME*> collectAllPoints(const QuadTreeNode<OBJECT_NAME, PointType, BoundType>* current) const;

    // for cleanup
    void cleanupFromTopToBottom(std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& root);
    void cleanupFromFaultyList3();
    void ownChildsPointsAndDestroyChild(QuadTreeNode<OBJECT_NAME, PointType, BoundType>* parent);
    bool canCollapse(const QuadTreeNode<OBJECT_NAME, PointType, BoundType>* parent) const;
    void setCleanupStrategy(CleanupStrategy strategy) {cleanupstrategy_ = strategy;}
    // for debug
    const std::unordered_set<QuadTreeNode<OBJECT_NAME, PointType, BoundType>*>& getFaultyList() const { return faultyList; }
};


template <typename OBJECT_NAME, typename PointType, typename BoundType>
bool QuadTree<OBJECT_NAME, PointType, BoundType>::canCollapse(
        const QuadTreeNode<OBJECT_NAME, PointType, BoundType>* parent) const {
    if (!parent || parent->isLeaf_) return false;

    const QuadTreeNode<OBJECT_NAME, PointType, BoundType>* children[4] = {
        parent->tl_child_.get(), parent->tr_child_.get(),
        parent->bl_child_.get(), parent->br_child_.get()
    };

    std::size_t total = 0;
    for (const auto* child : children) {
        // an internal child's points_ is empty, so its size says nothing
        if (!child || !child->isLeaf_) return false;
        total += child->points_.size();
    }
    return total < static_cast<std::size_t>(capacity_);
}

//implementations
template <typename OBJECT_NAME, typename PointType, typename BoundType>
void QuadTree<OBJECT_NAME, PointType, BoundType>::initializeChild(
            std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& child,
            QuadTreeNode<OBJECT_NAME, PointType, BoundType>* parent,
            BoundType boundry,
            int depth,
            std::vector<OBJECT_NAME*> points) {
    child->boundry_ = boundry;
    child->depth_ = depth;
    child->points_ = points;
    child->isLeaf_ = true;
    child->needCleanup_ = false;
    child->parent_ = parent;
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

    // can non-leaf node store data?
    std::vector<OBJECT_NAME*> buffer = std::move(current->points_);
    std::vector<OBJECT_NAME*> points_tl, points_tr, points_bl, points_br; 
    for (auto obj: buffer) {
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

    initializeChild(childs[0], current.get(), tl, current->depth_ + 1, points_tl);
    initializeChild(childs[1], current.get(), tr, current->depth_ + 1, points_tr);
    initializeChild(childs[2], current.get(), bl, current->depth_ + 1, points_bl);
    initializeChild(childs[3], current.get(), br, current->depth_ + 1, points_br);

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
                root->points_.push_back(object);
                // throw std::runtime_error("cannot insert, size full");
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
bool QuadTree<OBJECT_NAME, PointType, BoundType>::exists(const std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& current, const OBJECT_NAME* object) const {
    
    if (!object) throw std::runtime_error("object is null");
    if (!current) throw std::runtime_error("current is null");
    if (!position_getter_) throw std::runtime_error("position functor is null");
    // i could seperate the gaurds before the recursion

    const PointType& item = position_getter_(*object);
    
    BoundType boundry = current->getBoundry();
    PointType mid = (boundry.tl + boundry.br) / 2.0f;

    if(current->getisleaf()) {
        auto found = std::find(current->points_.begin(), current->points_.end(), object);
        if (found != current->points_.end())
            return true;
        else
            return false;
    } else {
        mid = (current->boundry_.tl + current->boundry_.br) / 2.0f;
        if(item.x < mid.x) {
            if(item.y < mid.y) {
                return exists(current->tl_child_, object);
            } else {
                return exists(current->bl_child_, object);
            }
        } else {
            if(item.y < mid.y) {
                return exists(current->tr_child_, object);
            } else {
                return exists(current->br_child_, object);
            }
        }
    }
}

template <typename OBJECT_NAME, typename PointType, typename BoundType>
QuadTreeNode<OBJECT_NAME, PointType, BoundType>* QuadTree<OBJECT_NAME, PointType, BoundType>::findnode_thisitem_existing(const std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& current, const OBJECT_NAME* object) {
    
    if (!object) throw std::runtime_error("object is null");
    if (!current) throw std::runtime_error("current is null");
    if (!position_getter_) throw std::runtime_error("position functor is null");
    // i could seperate the gaurds before the recursion

    const PointType& item = position_getter_(*object);
    
    BoundType boundry = current->getBoundry();
    PointType mid = (boundry.tl + boundry.br) / 2.0f;

    if(current->getisleaf()) {
        auto found = std::find(current->points_.begin(), current->points_.end(), object);
        if (found != current->points_.end())
            return current.get();
        else
            return nullptr;
    } else {
        mid = (current->boundry_.tl + current->boundry_.br) / 2.0f;
        if(item.x < mid.x) {
            if(item.y < mid.y) {
                return findnode_thisitem_existing(current->tl_child_, object);
            } else {
                return findnode_thisitem_existing(current->bl_child_, object);
            }
        } else {
            if(item.y < mid.y) {
                return findnode_thisitem_existing(current->tr_child_, object);
            } else {
                return findnode_thisitem_existing(current->br_child_, object);
            }
        }
    }
}

template <typename OBJECT_NAME, typename PointType, typename BoundType>
void QuadTree<OBJECT_NAME, PointType, BoundType>::remove(std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& root, OBJECT_NAME* object) {
    
    if (!object) throw std::runtime_error("object is null");
    if (!root) throw std::runtime_error("root is null");
    if (!position_getter_) throw std::runtime_error("position functor is null");
    // i could seperate the gaurds before the recursion

    // const PointType& item = position_getter_(*object);
    
    BoundType boundry = root->getBoundry();
    PointType mid = (boundry.tl + boundry.br) / 2.0f;

    if(!exists(root, object)) {
        return;
    }

    // oh man, should i make this doubly linked now?
    auto* node = findnode_thisitem_existing(root, object);
    if(!node)
        return;

    // std::cout << "Found node: " << node << std::endl;
    // std::cout << "Node parent: " << node->parent_ << std::endl;
    
    auto loc = std::find(node->points_.begin(), node->points_.end(), object);
    if(loc != node->points_.end()) {
        node->points_.erase(loc);
        // std::cout<<"removed from quadtree "<<"\n";
    }

    // mark the node for cleanup (ie collapse parents nodes if possible)
    node->needCleanup_ = true;
    // insert this node to a vector that later should be visited to cleanup;    
    faultyList.insert(node);
}

template <typename OBJECT_NAME, typename PointType, typename BoundType>
void QuadTree<OBJECT_NAME, PointType, BoundType>::cleanup(std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& node) {

    switch(cleanupstrategy_) {
        case CleanupStrategy::FaultyList : 
            if(faultyList.empty())
                return;
            cleanupFromFaultyList3();
            break;
        case CleanupStrategy::Sweep :
            cleanupFromTopToBottom(node);
            break;         
    }
}

template <typename OBJECT_NAME, typename PointType, typename BoundType>
void QuadTree<OBJECT_NAME, PointType, BoundType>::ownChildsPointsAndDestroyChild(
    QuadTreeNode<OBJECT_NAME, PointType, BoundType>* parent)
{
    if (!parent) return;

    std::vector<std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>> children;

    if (parent->tl_child_) children.push_back(std::move(parent->tl_child_));
    if (parent->tr_child_) children.push_back(std::move(parent->tr_child_));
    if (parent->bl_child_) children.push_back(std::move(parent->bl_child_));
    if (parent->br_child_) children.push_back(std::move(parent->br_child_));

    for (auto &child : children) {
        if (child) {
            parent->points_.insert(
                parent->points_.end(),
                std::make_move_iterator(child->points_.begin()),
                std::make_move_iterator(child->points_.end())
            );
        }
    }

    for (auto &child : children) {
        if (child) {
            faultyList.erase(child.get());
        }
    }

    // childs will get destroyed (because they are unique_ptrs)

    parent->isLeaf_ = true;
}




template <typename OBJECT_NAME, typename PointType, typename BoundType>
void QuadTree<OBJECT_NAME, PointType, BoundType>::cleanupFromFaultyList3() {
    while (!faultyList.empty()) {
        auto it = faultyList.begin();
        auto* leaf = *it;
        faultyList.erase(it);

        auto* parent = leaf->parent_;
        while (parent && canCollapse(parent)) {
            auto* grandparent = parent->parent_;
            ownChildsPointsAndDestroyChild(parent);
            parent = grandparent;   // looping
        }
    }
}

template <typename OBJECT_NAME, typename PointType, typename BoundType>
void QuadTree<OBJECT_NAME, PointType, BoundType>::cleanupFromTopToBottom(std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>& node) {
    // std::cout<<"cleanup from quadtree "<<"\n";
    bool strictCollapseWhenUnderCapacity = true;

    if(!node || node->getisleaf())
        return;
    
    cleanupFromTopToBottom(node->tl_child_);
    cleanupFromTopToBottom(node->tr_child_);
    cleanupFromTopToBottom(node->bl_child_);
    cleanupFromTopToBottom(node->br_child_);

    const auto &tl = node->getTLChild();
    const auto &tr = node->getTRChild();
    const auto &bl = node->getBLChild();
    const auto &br = node->getBRChild();

    bool allChildrensAreLeaf = 
        tl && tl->getisleaf() &&
        tr && tr->getisleaf() &&
        bl && bl->getisleaf() &&
        br && br->getisleaf();

    if (!allChildrensAreLeaf)
        return;
    // all childs must exists

    bool allLeafsAreEmpty =
        tl->points_.empty() &&
        tr->points_.empty() &&
        bl->points_.empty() &&
        br->points_.empty();

    int total_points = static_cast<int>(tl->points_.size() + tr->points_.size() + bl->points_.size() + br->points_.size());
    
    bool canCollapseEmpty = allLeafsAreEmpty && !strictCollapseWhenUnderCapacity;
    bool canCollpaseWithPoints = (total_points < capacity_) && strictCollapseWhenUnderCapacity;

    if (canCollapseEmpty || canCollpaseWithPoints) {
        if(canCollpaseWithPoints) {
            // move points to parent and collapse;
            node->points_.insert(node->points_.end(), tl->points_.begin(), tl->points_.end());
            node->points_.insert(node->points_.end(), tr->points_.begin(), tr->points_.end());
            node->points_.insert(node->points_.end(), bl->points_.begin(), bl->points_.end());
            node->points_.insert(node->points_.end(), br->points_.begin(), br->points_.end());
        }
        // and childs should have no data now;
        node->tl_child_.reset();
        node->tr_child_.reset();
        node->bl_child_.reset();
        node->br_child_.reset();
        node->isLeaf_ = true;
    }
}

template <typename OBJECT_NAME, typename PointType, typename BoundType>
std::unique_ptr<QuadTreeNode<OBJECT_NAME, PointType, BoundType>> QuadTree<OBJECT_NAME, PointType, BoundType>::BuildTree(QuadTreeNode<OBJECT_NAME, PointType, BoundType>* parent,
        std::vector<OBJECT_NAME*> points,
        PositionGetter position_getter,
        BoundType boundry,
        int capacity,
        int max_depth,
        int depth
        ) {
    
    position_getter_ = position_getter;
    max_depth_ = max_depth;
    capacity_ = capacity;
    // step1: split to 4;
    // step2: find and assign points to 4 of the child nodes;
    // step3: call BuildTree recursively on the 4 child nodes;
    // step4: return the current node which will be the root of the tree;


    //edge case
    auto current = std::make_unique<QuadTreeNode<OBJECT_NAME, PointType, BoundType>>();

    current->parent_ = parent;
    if(points.size() <= capacity || depth >= max_depth) {
        current->boundry_ = boundry;
        current->depth_ = depth;
        current->isLeaf_ = true;
        current->needCleanup_ = false;
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
    current->tl_child_ = BuildTree(current.get(), points_tl, position_getter, tl, capacity, max_depth, depth + 1);
    current->tr_child_ = BuildTree(current.get(), points_tr, position_getter, tr, capacity, max_depth, depth + 1);
    current->bl_child_ = BuildTree(current.get(), points_bl, position_getter, bl, capacity, max_depth, depth + 1);
    current->br_child_ = BuildTree(current.get(), points_br, position_getter, br, capacity, max_depth, depth + 1);
    current->boundry_ = boundry;
    current->depth_ = depth;
    current->isLeaf_ = false;
    current->needCleanup_ = false;
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

template <typename OBJECT_NAME, typename PointType, typename BoundType>
bool QuadTree<OBJECT_NAME, PointType, BoundType>::partInChild(const QuadTreeNode<OBJECT_NAME, PointType, BoundType>* current, BoundType rect) const {
    return current && current->boundry_.intersects(rect);
}

template <typename OBJECT_NAME, typename PointType, typename BoundType>
std::vector<OBJECT_NAME*> QuadTree<OBJECT_NAME, PointType, BoundType>::collectAllPoints(const QuadTreeNode<OBJECT_NAME, PointType, BoundType>* current) const {
    if (current == nullptr) return {};
    std::vector<OBJECT_NAME*> result;
    if(current->isLeaf_) {
        result.insert(result.end(), current->points_.begin(), current->points_.end());
    } else {
        auto tl_points = collectAllPoints(current->tl_child_.get());
        auto tr_points = collectAllPoints(current->tr_child_.get());
        auto bl_points = collectAllPoints(current->bl_child_.get());
        auto br_points = collectAllPoints(current->br_child_.get());
        result.insert(result.end(), tl_points.begin(), tl_points.end());
        result.insert(result.end(), tr_points.begin(), tr_points.end());
        result.insert(result.end(), bl_points.begin(), bl_points.end());
        result.insert(result.end(), br_points.begin(), br_points.end());
    }
    return result;
}

template <typename OBJECT_NAME, typename PointType, typename BoundType>
std::vector<OBJECT_NAME*> QuadTree<OBJECT_NAME, PointType, BoundType>::findNeighbors(QuadTreeNode<OBJECT_NAME, PointType, BoundType>* current, BoundType rect) {

//    std::cout<<"rect = "<<rect<<"\n";

    if(current == nullptr)
        return {};

    if(rect.wholyContains(current->boundry_)) {
        return collectAllPoints(current);
    }


    if(current->isLeaf_) {
        std::vector<OBJECT_NAME*> res;
        if (rect.intersects(current->boundry_)) {
            for(auto &point: current->points_) {
                if(rect.contains(position_getter_(*point)))
                    res.push_back(point);
            }
        }
        return res;
    }  else {

        // search only in childs that has a part of this rect in it;
        
        // i think i can search specifically, here, i mean rather than checking for all childs,
        // i can decide to check on some i guess?

        // or like more educated guesses to ignore one child, based on rect and result of another child;

        std::vector<OBJECT_NAME*> total;
        auto collectFromChild = [&](auto child){
            if(partInChild(child, rect)) {
                auto childResult = findNeighbors(child, rect);
                total.insert(total.end(), childResult.begin(), childResult.end());
            }
        };

        collectFromChild(current->tl_child_.get());
        collectFromChild(current->tr_child_.get());
        collectFromChild(current->bl_child_.get());
        collectFromChild(current->br_child_.get());

        return total;
    }
}