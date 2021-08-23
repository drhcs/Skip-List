#ifndef SKIP_LIST
#define SKIP_LIST

#include <random>
#include <stack>

template <class Key, class Val>
struct Node {

    Key key_;
    Val val_;
    Node * right_;
    Node * down_;
    bool infinite_;

    explicit Node(bool infinite) {
        key_ = NULL;
        val_ = NULL;
        right_ = nullptr;
        down_ = nullptr;
        infinite_ = infinite;
    }

    Node(Key key, Val val, Node * right, Node * down) {
        key_ = key;
        val_ = val;
        right_ = right;
        down_ = down;
        infinite_ = false;
    }

    Node(Key key, Val val, Node * right, Node * down, bool infinite) {
        key_ = key;
        val_ = val;
        right_ = right;
        down_ = down;
        infinite_ = infinite;
    }

};

template <class Key, class Val>
class SkipList {

    Node<Key, Val> * head_;
    int length_;
    int height_;
    int node_count_;

    std::default_random_engine generator_;
    std::bernoulli_distribution distribution_;
    float probability_;

    void DeleteList(Node<Key, Val> * head) {
        if (head == nullptr)
        {
            return;
        }
        DeleteList(head->right_);
        delete head;
    }

    void DeleteNodes(Node<Key, Val> * head) {

        if (head == nullptr)
        {
            return;
        }

        DeleteNodes(head->down_);
        DeleteList(head);
    }

    bool FlipCoin() {
        return distribution_(generator_);
    }

public:

    SkipList() {
        head_ = new Node<Key, Val>(NULL, NULL, new Node<Key, Val>(true), nullptr, true);
        head_->down_ = new Node<Key, Val>(NULL, NULL, new Node<Key, Val>(true), nullptr, true);
        length_ = 0;
        height_ = 0;
        node_count_ = 0;
        probability_ = 0.5;
        distribution_ = std::bernoulli_distribution(probability_);
    }

    ~SkipList() {
        Clear();
    }

    Node<Key, Val> * Find(Key key) {
        Node<Key, Val> * itr = head_;
        while (itr->down_ != nullptr)
        {
            itr = itr->down_;
            while (!itr->right_->infinite_ && key >= itr->right_->key_)
            {
                itr = itr->right_;
                if (itr->key_ == key)
                {
                    return itr;
                }
            }
        }
        return nullptr;
    }

    void Insert(Key key, Val val) {
        std::stack<Node<Key, Val> *> prev_level;
        Node<Key, Val> * itr = head_;

        while (itr->down_ != nullptr)
        {
            itr = itr->down_;
            prev_level.push(itr);
            while (!itr->right_->infinite_ && key >= itr->right_->key_)
            {
                itr = itr->right_;
                if (itr->key_ == key)
                {
                    return;
                }
            }
        }

        Node<Key, Val> * new_nodes_right = itr->right_;
        itr->right_ = new Node<Key, Val>(key, val, new_nodes_right, nullptr);
        node_count_++;

        Node<Key, Val> * level_below = itr->right_;

        prev_level.pop();
        while (FlipCoin())
        {
            if (!prev_level.empty())
            {
                Node<Key, Val> * curr_level = prev_level.top();
                prev_level.pop();

                new_nodes_right = curr_level->right_;
                curr_level->right_ = new Node<Key, Val>(key, val, new_nodes_right, level_below);

                level_below = curr_level->right_;
            }
            else
            {
                new_nodes_right = head_->right_;
                head_->right_ = new Node<Key, Val>(key, val, new_nodes_right, level_below);

                level_below = head_->right_;

                auto new_head = new Node<Key, Val>(NULL, NULL, new Node<Key, Val>(true), head_, true);
                head_ = new_head;

                height_++;
            }
            node_count_++;
        }
        length_++;
    }

    void Remove(Key key) {
        std::stack<Node<Key, Val> *> prev_level;
        Node<Key, Val> * itr = head_;

        while (itr->down_ != nullptr)
        {
            itr = itr->down_;
            while (!itr->right_->infinite_ && key > itr->right_->key_)
            {
                itr = itr->right_;
                if (itr->key_ == key)
                {
                    return;
                }
            }

            if (!itr->right_->infinite_ && key == itr->right_->key_)
            {
                prev_level.push(itr);
            }
        }

        while (!prev_level.empty())
        {
            Node<Key, Val> * curr_level = prev_level.top();
            prev_level.pop();

            Node<Key, Val> * to_delete = curr_level->right_;
            curr_level->right_ = curr_level->right_->right_;

            to_delete->right_ = nullptr;
            to_delete->down_ = nullptr;
            delete to_delete;

            node_count_--;
        }
        length_--;
    }

    void Clear() {
        DeleteNodes(head_);
        head_ = nullptr;
    }

    bool Empty() {
        return length_ == 0;
    }

    int Length() {
        return length_;
    }

    int Height() {
        return height_;
    }

    int NodeCount() {
        return node_count_;
    }

};

#endif
