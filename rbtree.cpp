#include <cassert>
#include <random>
#include <iostream>
#include <queue>
#include <utility>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <stack>
#include <functional>

class red_black_tree
{
public:

    struct tree_node
    {
        int key = 0;
        int val = 0;
        bool is_red = false;
        tree_node* left = nullptr;
        tree_node* right = nullptr;
        tree_node* parent = nullptr;
    };

    red_black_tree() : root(nullptr) {}

    [[nodiscard]] bool is_empty() const noexcept
    {
        return this->root == nullptr;
    }

    const tree_node* find(int key) const
    {
        return find_key(key);
    }
    
    const tree_node* insert(int key, int val)
    {
        if (this->root == nullptr)
        {
            this->root = new tree_node();
            this->root->key = key;
            this->root->val = val;
            return this->root;
        }

        tree_node* pos = find_insert_position(this->root, key);
        assert(pos != nullptr);
        if (pos->key == key)
        {
            pos->val = val;
            return pos;
        }
        tree_node* new_node = new tree_node();
        new_node->key = key;
        new_node->val = val;
        new_node->is_red = true;
        new_node->parent = pos;
        tree_node* ret_val = new_node;
        if (key < pos->key)
        {
            pos->left = new_node;
        }
        else
        {
            pos->right = new_node;
        }

        while (pos->is_red)
        {
            tree_node* brother = get_brother(pos);
            if (brother != nullptr && brother->is_red)
            {
                pos->is_red = brother->is_red = false;
                if (pos->parent == this->root) break;
                pos->parent->is_red = true;
                new_node = pos->parent;
                pos = pos->parent->parent;
            }
            else
            {
                if (is_left_child(pos))
                {
                    if (!is_left_child(new_node))
                    {
                        rotate_left(pos);
                        pos = new_node;
                    }
                    pos->is_red = false;
                    pos->parent->is_red = true;
                    rotate_right(pos->parent);
                }
                else
                {
                    if (is_left_child(new_node))
                    {
                        rotate_right(pos);
                        pos = new_node;
                    }
                    pos->is_red = false;
                    pos->parent->is_red = true;
                    rotate_left(pos->parent);
                }
            }
        }
        return ret_val;
    }

    void remove(int key)
    {
        tree_node* pos = find_key(key);
        if (pos == nullptr) return;

        while (!(pos->left == nullptr && pos->right == nullptr))
        {
            if (pos->left == nullptr)
            {
                pos->key = pos->right->key;
                pos->val = pos->right->val;
                pos = pos->right;
            }
            else if (pos->right == nullptr)
            {
                pos->key = pos->left->key;
                pos->val = pos->left->val;
                pos = pos->left;
            }
            else
            {
                // Find successor

                tree_node* tmp = pos->right;
                while (tmp->left != nullptr)
                {
                    tmp = tmp->left;
                }
                pos->key = tmp->key;
                pos->val = tmp->val;
                pos = tmp;
            }
        }

        if (pos == this->root)
        {
            delete pos;
            this->root = nullptr;
            return;
        }

        tree_node* brother = get_brother(pos);
        bool is_left = false;
        if (is_left_child(pos))
        {
            pos->parent->left = nullptr;
            is_left = true;
        }
        else pos->parent->right = nullptr;
        if (pos->is_red)
        {
            delete pos;
            return;
        }
        delete pos;
        assert(brother != nullptr);

        while (true)
        {
            if (is_left)
            {
                if (brother->is_red)
                {
                    brother->is_red = false;
                    brother->parent->is_red = true;
                    rotate_left(brother->parent);
                    brother = brother->left->right;
                }

                if (brother->right != nullptr && brother->right->is_red)
                {
                to_be_deleted_is_left_and_brother_is_black_and_right_child_is_red:
                    brother->right->is_red = false;
                    brother->is_red = brother->parent->is_red;
                    brother->parent->is_red = false;
                    rotate_left(brother->parent);
                }
                else if (brother->left != nullptr && brother->left->is_red)
                {
                    brother->is_red = true;
                    brother->left->is_red = false;
                    rotate_right(brother);
                    brother = brother->parent;
                    goto to_be_deleted_is_left_and_brother_is_black_and_right_child_is_red;
                }
                else
                {
                    brother->is_red = true;
                    if (brother->parent->is_red)
                    {
                        brother->parent->is_red = false;
                    }
                    else if (brother->parent != this->root)
                    {
                        is_left = is_left_child(brother->parent);
                        brother = get_brother(brother->parent);
                        continue;
                    }
                }
                
            }
            else
            {
                if (brother->is_red)
                {
                    brother->is_red = false;
                    brother->parent->is_red = true;
                    rotate_right(brother->parent);
                    brother = brother->right->left;
                }

                if (brother->left != nullptr && brother->left->is_red)
                {
                to_be_deleted_is_right_and_brother_is_black_and_left_child_is_red:
                    brother->left->is_red = false;
                    brother->is_red = brother->parent->is_red;
                    brother->parent->is_red = false;
                    rotate_right(brother->parent);
                }
                else if (brother->right != nullptr && brother->right->is_red)
                {
                    brother->is_red = true;
                    brother->right->is_red = false;
                    rotate_left(brother);
                    brother = brother->parent;
                    goto to_be_deleted_is_right_and_brother_is_black_and_left_child_is_red;
                }
                else
                {
                    brother->is_red = true;
                    if (brother->parent->is_red)
                    {
                        brother->parent->is_red = false;
                    }
                    else if (brother->parent != this->root)
                    {
                        is_left = is_left_child(brother->parent);
                        brother = get_brother(brother->parent);
                        continue;
                    }
                }
            }
            break;
        }
    }

    void clear() noexcept
    {
        clear_subtree(this->root);
        this->root = nullptr;
    }

    virtual ~red_black_tree()
    {
        clear();
    }

protected:
    const tree_node* get_root() const { return root; }

private:

    tree_node* root;

    void clear_subtree(tree_node* root) noexcept
    {
        if (root == nullptr) return;
        clear_subtree(root->left);
        clear_subtree(root->right);
        delete root;
    }

    tree_node* find_key(int key) const
    {
        return find_key_at_root(this->root, key);
    }

    void rotate_left(tree_node* node)
    {
        assert(node->right != nullptr);

        if (node->parent != nullptr)
        {
            if (node->parent->left == node)
            {
                node->parent->left = node->right;
            }
            else
            {
                node->parent->right = node->right;
            }
        }
        else
        {
            this->root = node->right;
        }

        node->right->parent = node->parent;
        node->parent = node->right;
        tree_node* tmp = node->right->left;
        node->right->left = node;
        if (tmp != nullptr) tmp->parent = node;
        node->right = tmp;
    }

    void rotate_right(tree_node* node)
    {
        assert(node->left != nullptr);

        if (node->parent != nullptr)
        {
            if (node->parent->right == node)
            {
                node->parent->right = node->left;
            }
            else
            {
                node->parent->left = node->left;
            }
        }
        else
        {
            this->root = node->left;
        }

        node->left->parent = node->parent;
        node->parent = node->left;
        tree_node* tmp = node->left->right;
        node->left->right = node;
        if (tmp != nullptr) tmp->parent = node;
        node->left = tmp;
    }

    static bool is_left_child(tree_node* node)
    {
        assert(node != nullptr);
        assert(node->parent != nullptr);
        return node->parent->left == node;
    }

    static tree_node* get_brother(tree_node* node)
    {
        assert(node != nullptr);
        assert(node->parent != nullptr);
        return is_left_child(node) ? node->parent->right : node->parent->left;
    }

    static tree_node* find_key_at_root(tree_node* root, int key)
    {
        if (root == nullptr) return nullptr;

        while (root->key != key)
        {
            if (key < root->key) root = root->left;
            else root = root->right;
            if (root == nullptr) return nullptr;
        }
        return root;
    }

    static tree_node* find_insert_position(tree_node* root, int key)
    {
        assert(root != nullptr);

        while (true)
        {
            if (key == root->key) return root;
            if (key < root->key)
            {
                if (root->left == nullptr) return root;
                root = root->left;
            }
            else
            {
                if (root->right == nullptr) return root;
                root = root->right;
            }
        }
        assert(0);
    }

    friend void print_stage(const red_black_tree& tree);

};

class red_black_tree_wrapper : public red_black_tree
{
private:
    using tree_node = red_black_tree::tree_node;

public:
    const tree_node* insert(int key, int val)
    {
        m_store[key] = val;
        return red_black_tree::insert(key, val);
    }

    void remove(int key)
    {
        m_store.erase(key);
        return red_black_tree::remove(key);
    }

    void check()
    {
        // Check legal

        const tree_node* root = get_root();
        const tree_node* tmp = root;
        int black_num = 0;
        int size = 0;
        int founded_black_num = -1;
        std::unordered_map<int, int> tree_stored;
        
        std::function<void(const tree_node*, bool)> count_num = [&](const tree_node* node, bool is_parent_red) -> void
        {
            if (node == nullptr)
            {
                if (founded_black_num == -1) founded_black_num = black_num;
                else if (founded_black_num != black_num)
                    throw std::logic_error("Not all paths between root to leaves have the same number of black nodes!");
                return;
            }

            if (is_parent_red && node->is_red)
            {
                throw std::logic_error("Two red nodes cannot be next to each other!");
            }

            ++size;
            tree_stored[node->key] = node->val;
            if (!node->is_red) ++black_num;
            count_num(node->left, node->is_red);
            count_num(node->right, node->is_red);
            if (!node->is_red) --black_num;
        };

        count_num(root, false);

        // std::cout << "Black num: " << black_num << std::endl;

        // Check data

        if (size != m_store.size())
        {
            throw std::logic_error("The total number of the nodes is not true!");
        }

        for (auto [key, val] : tree_stored)
        {
            if (auto itr = m_store.find(key); itr == m_store.end())
            {
                throw std::logic_error("Data fault: unexpected key!");
            }
            else if (itr->second != val)
            {
                throw std::logic_error("Data fault: val is not true!");
            }
        }

        for (auto [key, val] : m_store)
        {
            if (auto itr = tree_stored.find(key); itr == tree_stored.end())
            {
                throw std::logic_error("Data fault: key lost!");
            }
            else if (itr->second != val)
            {
                throw std::logic_error("Data fault: val is not true!");
            }
        }

        // std::cout << "Finished!" << std::endl;

    }

private:

    std::unordered_map<int, int> m_store;
};

void print_stage(const red_black_tree& tree)
{
    if (tree.root == nullptr)
    {
        endl(std::cout);
        return;
    }

    using tree_node = red_black_tree::tree_node;

    std::queue<std::pair<tree_node*, int>> q;
    q.emplace(tree.root, 0);
    int last_stage = 0;
    while (!q.empty())
    {
        auto [tmp, stage] = q.front();
        q.pop();
        if (stage != last_stage)
        {
            std::cout.put('\n');
            last_stage = stage;
        }
        if (tmp == nullptr) std::cout << "(null)";
        else
        {
            std::cout << "(" << tmp->key << ", " << tmp->val << ", " << (tmp->is_red ? "r" : "b") << ") ";
            q.emplace(tmp->left, stage + 1);
            q.emplace(tmp->right, stage + 1);
        }
    }
    endl(std::cout);
}

int main()
{
    red_black_tree_wrapper tree;
    try
    {
        // 30 36 83 22

        int keys[] = { 670, 310, 494, 405, 584, 873, 247, 365, 63, 184, 418, 935, 462, 606, 949, 800, 96, 18, 475, 852, 96, 737, 491, 242, 196, 526, 87, 469, 671, 917, 904, 434, 344, 747, 368, 313, 266, 10, 371, 961, 926, 951, 722, 765, 56, 431, 670, 558, 371, 619, 36, 341, 821, 943, 281, 411, 274, 848, 619, 961, 993, 225, 604, 204, 679, 807, 486, 77, 276, 416, 115, 73, 453, 852, 836, 926, 91, 833, 885, 333, 330, 753, 972, 25, 128, 404, 12, 940, 481, 31, 153, 429, 918, 489, 527, 808, 988, 891, 651, 61 };

        
        std::mt19937_64 mt(std::random_device{}());
        std::uniform_int_distribution<> d(0, 100000);
        for (int i = 0; i < 20000; ++i)
        {
            int key = d(mt), val = d(mt);
            // key = keys[i];
            // std::cout << "pushed: (" << key << ", " << val << ")\n";
            tree.insert(key, val);
            // print_stage(tree);
            if (i % 100 == 0) tree.check();
        }

        // print_stage(tree);

        int dkeys[] = { 959, 912, 40, 29, 498, 557, 222, 673, 451, 805, 354, 341, 938, 375, 517, 28, 997, 716, 186, 842, 242, 852, 498, 69, 887, 398, 246, 963, 406, 2, 255, 413, 218, 799, 130, 881, 598, 549, 775, 100, 644, 640, 589, 93, 200, 666, 698, 583, 597, 578, 584, 353, 133, 984, 944, 553, 743, 756, 985, 273, 327, 296, 727, 390, 334, 231, 402, 10, 464, 225, 190, 312, 609, 327, 652, 321, 940, 170, 963, 881, 173, 493, 916, 282, 624, 545, 288, 533, 195, 127, 594, 558, 343, 632, 844, 126, 342, 332, 383, 572, 677, 287, 466, 768, 595, 348, 718, 310, 139, 872, 731, 897, 9, 324, 959, 275, 272, 545, 857, 94, 826, 612, 259, 963, 574, 280, 394, 687, 19, 58, 373, 762, 185, 54, 907, 149, 895, 580, 124, 928, 771, 98, 511, 856, 665, 107, 12 };

        for (int i = 0; i < 50000; ++i)
        {
            int key = d(mt);
            // key = dkeys[i];
            // std::cout << "deleted: (" << key << ")\n";
            tree.remove(key);
            // if (i >= sizeof(dkeys) / sizeof(decltype(*dkeys)) - 2) print_stage(tree);
            // print_stage(tree);
            if (i % 100 == 0) tree.check();
        }
        // print_stage(tree);


        tree.check();

        std::cout << "Finished!" << std::endl;
    }

    catch (std::exception& e)
    {
        std::cout << "==========================" << std::endl;
        // print_stage(tree);
        std::cout << "Error: " << e.what() << std::endl;
    }

    return 0;
}
