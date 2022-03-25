#include <iostream>


template<class T>
class Set {
private:
    class Node {
    public:
        Node *parent;
        Node *left;
        Node *right;
        T val;
        int diff;
        Node() {
            parent = nullptr;
            left = nullptr;
            right = nullptr;
            diff = 0;
        }
        Node(const T& x) {
            parent = nullptr;
            left = nullptr;
            right = nullptr;
            val = x;
            diff = 0;
        }
        Node(const T& x, Node * p) {
            parent = p;
            left = nullptr;
            right = nullptr;
            val = x;
            diff = 0;
        }

        Node* rotate_left() {
            if (!this->right) {
                return nullptr;
            }
            Node * right_vertex = right;
            right = right_vertex->left;
            if (right) {
                right->parent = this;
            }
            right_vertex->left = this;
            right_vertex->parent = parent;
            if (parent) {
                if (parent->left == this) {
                    parent->left = right_vertex;
                } else {
                    parent->right = right_vertex;
                }
            }
            parent = right_vertex;
            diff = diff + std::max(0, -right_vertex->diff) + 1;
            right_vertex->diff = std::max(0, diff) + 1 + right_vertex->diff;
            return right_vertex;
        }
        Node* rotate_right() {
            if (!this->left) {
                return nullptr;
            }
            Node * left_vertex = left;
            left = left_vertex->right;
            if (left) {
                left->parent = this;
            }
            left_vertex->right = this;
            left_vertex->parent = parent;
            if (parent) {
                if (parent->left == this) {
                    parent->left = left_vertex;
                } else {
                    parent->right = left_vertex;
                }
            }
            parent = left_vertex;
            diff = diff - 1 - std::max(0, left_vertex->diff);
            left_vertex->diff = left_vertex->diff - 1 - std::max(0, -diff);
            return left_vertex;
        }
        Node* big_rotate_left() {
            right->rotate_right();
            return this->rotate_left();
        }
        Node* big_rotate_right() {
            left->rotate_left();
            return this->rotate_right();
        }
    };
    Node * begin_;
    Node * end_;
    size_t sz;
    Node * root;

    void update_begin() {
        if (!root) {
            begin_ = end_;
            return;
        }
        begin_ = root;
        while (begin_->left) {
            begin_ = begin_->left;
        }
    }

    Node* go_to(const T& elem) const {
        Node* current = root;
        while (true) {
            if (current->val < elem) {
                if (current->right) {
                    current = current->right;
                } else {
                    return current;
                }
            } else if (elem < current->val) {
                if (current->left) {
                    current = current->left;
                } else {
                    return current;
                }
            } else {
                return current;
            }
        }
    }

    Node* update_to_right(Node* vertex) {
        if (!vertex->parent) {
            root = vertex->right;
            begin_ = root;
            delete root->parent;
            root->parent = nullptr;
            return nullptr;
        }
        if (vertex == vertex->parent->left) {
            vertex->parent->left = vertex->right;
            vertex = vertex->parent;
            delete vertex->left->parent;
            vertex->left->parent = vertex;
            vertex->diff--;
        } else {
            vertex->parent->right = vertex->right;
            vertex = vertex->parent;
            delete vertex->right->parent;
            vertex->right->parent = vertex;
            vertex->diff++;
        }
        return vertex;
    }
    Node* update_to_left(Node* vertex) {
        if (!vertex->parent) {
            root = vertex->left;
            delete root->parent;
            root->parent = nullptr;
            return nullptr;
        }
        if (vertex == vertex->parent->left) {
            vertex->parent->left = vertex->left;
            vertex = vertex->parent;
            delete vertex->left->parent;
            vertex->left->parent = vertex;
            vertex->diff--;
        } else {
            vertex->parent->right = vertex->left;
            vertex = vertex->parent;
            delete vertex->right->parent;
            vertex->right->parent = vertex;
            vertex->diff++;
        }
        return vertex;
    }

    Node* update_to_nothing(Node * vertex) {
        if (!vertex->parent) {
            delete root;
            root = nullptr;
            begin_ = end_;
            return nullptr;
        }
        if (vertex == vertex->parent->left) {
            vertex = vertex->parent;
            delete vertex->left;
            vertex->left = nullptr;
            vertex->diff--;
        } else {
            vertex = vertex->parent;
            delete vertex->right;
            vertex->right = nullptr;
            vertex->diff++;
        }
        return vertex;
    }

    Node* delete_node(Node* vertex) {
        if (vertex->left && vertex->right) {
            return vertex;
        }
        if (vertex->left) {
            return update_to_left(vertex);
        }
        if (vertex->right) {
            return update_to_right(vertex);
        }
        return update_to_nothing(vertex);
    }

    Node* go_parent_plus(Node* vertex) {
        if (!vertex->parent) {
            return nullptr;
        }
        if (vertex == vertex->parent->left) {
            vertex->parent->diff++;
        } else {
            vertex->parent->diff--;
        }
        return vertex->parent;
    }
    Node* go_parent_minus(Node* vertex) {
        if (!vertex->parent) {
            return nullptr;
        }
        if (vertex == vertex->parent->left) {
            vertex->parent->diff--;
        } else {
            vertex->parent->diff++;
        }
        return vertex->parent;
    }
    void update_plus(Node* vertex) {
        if (!vertex) {
            return;
        }
        if (vertex->diff == 0) {
            return;
        }
        if (vertex->diff == 1 || vertex->diff == -1) {
            update_plus(go_parent_plus(vertex));
            return;
        }
        vertex = do_rotate(vertex);
        if (vertex->diff == 0) {
            return;
        }
        update_plus(go_parent_plus(vertex));
    }
    void update_minus(Node* vertex) {
        if (!vertex) {
            return;
        }
        if (vertex->diff == 1 || vertex->diff == -1) {
            return;
        }
        if (vertex->diff == 0) {
            update_minus(go_parent_minus(vertex));
            return;
        }
        vertex = do_rotate(vertex);
        if (vertex->diff == 1 || vertex->diff == -1) {
            return;
        }
        update_minus(go_parent_minus(vertex));
    }

    Node* do_rotate(Node * vertex) {
        if (vertex->diff == -2) {
            if (!vertex->right) {
                return root;
            }
            if (vertex->right->diff < 1) {
                if (!vertex->parent) {
                    root = vertex->rotate_left();
                    return root;
                }
                return vertex->rotate_left();
            }
            if (!vertex->parent) {
                root = vertex->big_rotate_left();
                return root;
            }
            return vertex->big_rotate_left();
        }
        if (!vertex->left) {
            return root;
        }
        if (vertex->left->diff > -1) {
            if (!vertex->parent) {
                root = vertex->rotate_right();
                return root;
            }
            return vertex->rotate_right();
        }
        if (!vertex->parent) {
            root = vertex->big_rotate_right();
            return root;
        }
        return vertex->big_rotate_right();
    }
public:

    friend class Iterator;

    class iterator {
        friend class Set<T>;
    public:
        iterator() {
            st = nullptr;
            vertex = nullptr;
        }
        iterator& operator ++() {
            if (vertex->right) {
                vertex = vertex->right;
                while (vertex->left) {
                    vertex = vertex->left;
                }
                return *this;
            }
            while (vertex->parent && vertex->parent->right == vertex) {
                vertex = vertex->parent;
            }
            if (!vertex->parent) {
                vertex = st->end_;
                return *this;
            }
            vertex = vertex->parent;
            return *this;
        }
        iterator operator ++(int) {
            iterator* it = this;
            if (vertex->right) {
                vertex = vertex->right;
                while (vertex->left) {
                    vertex = vertex->left;
                }
                return *it;
            }
            while (vertex->parent && vertex->parent->right == vertex) {
                vertex = vertex->parent;
            }
            if (!vertex->parent) {
                vertex = st->end_;
                return *it;
            }
            vertex = vertex->parent;
            return *it;
        }
        iterator& operator --() {
            if (vertex == st->end_) {
                vertex = st->root;
                while (vertex->right) {
                    vertex = vertex->right;
                }
                return *this;
            }
            if (vertex->left) {
                vertex = vertex->left;
                while (vertex->right) {
                    vertex = vertex->right;
                }
                return *this;
            }
            while (vertex->parent && vertex->parent->left == vertex) {
                vertex = vertex->parent;
            }
            if (!vertex->parent) {
                vertex = st->end_;
            } else {
                vertex = vertex->parent;
            }
            return *this;
        }
        iterator operator --(int) {
            iterator* it = this;
            if (vertex == st->end_) {
                vertex = st->root;
                while (vertex->right) {
                    vertex = vertex->right;
                }
                return *it;
            }
            if (vertex->left) {
                vertex = vertex->left;
                while (vertex->right) {
                    vertex = vertex->right;
                }
                return *it;
            }
            while (vertex->parent && vertex->parent->left == vertex) {
                vertex = vertex->parent;
            }
            if (!vertex->parent) {
                vertex = st->end_;
            } else {
                vertex = vertex->parent;
            }
            return *it;
        }
        bool operator ==(const iterator & it) const {
            if (st != it.st || vertex != it.vertex) {
                return false;
            }
            return true;
        }
        bool operator != (const iterator & it) const {
            if (st == it.st && vertex == it.vertex) {
                return false;
            }
            return true;
        }
        const T& operator*() const {
            return vertex->val;
        }
        const T* operator->() {
            return &vertex->val;
        }

    private:
        iterator(const Set* st_, const Node* vertex_){
            st = st_;
            if (!vertex_) {
                vertex = st_->end_;
            } else {
                vertex = vertex_;
            }
        }
        const Node* vertex;
        const Set* st;
    };

    Set() {
        sz = 0;
        begin_ = new Node();
        end_ = begin_;
        root = nullptr;
    }

    template<typename it>
    Set(it first, it last) {
        sz = 0;
        begin_ = new Node();
        end_ = begin_;
        root = nullptr;
        for (it i = first; i != last; i++) {
            insert(*i);
        }
    }

    Set(std::initializer_list<T> elems) {
        sz = 0;
        begin_ = new Node();
        end_ = begin_;
        root = nullptr;
        for (auto elem : elems) {
            insert(elem);
        }
    }

    void del(Node* vertex) {
        if (!vertex) {
            return;
        }
        del(vertex->left);
        del(vertex->right);
        delete vertex;
    }
    ~Set() {
        del(root);
        delete end_;
    }
    Node* copy(Node* vertex) {
        if (!vertex) {
            return nullptr;
        }
        Node* vertex_ = new Node(vertex->val);
        vertex_->diff = vertex->diff;
        vertex_->left = copy(vertex->left);
        vertex_->right = copy(vertex->right);
        if (vertex_->left) {
            vertex_->left->parent = vertex_;
        }
        if (vertex_->right) {
            vertex_->right->parent = vertex_;
        }
        return vertex_;
    }
    Set(const Set& st) {
        root = copy(st.root);
        end_ = new Node();
        update_begin();
        sz = st.sz;
    }
    Set& operator=(const Set& st) {
        Node *vertex = copy(st.root);
        del(root);
        root = vertex;
        update_begin();
        sz = st.sz;
        return *this;
    }
    size_t size() const {
        return sz;
    }
    bool empty() const {
        return sz == 0;
    }
    void insert(const T& elem) {
        if (!root) {
            root = new Node(elem);
            begin_ = root;
            sz++;
            return;
        }
        Node *vertex = go_to(elem);
        if (vertex->val < elem) {
            vertex->right = new Node(elem, vertex);
            vertex = vertex->right;
        } else if (elem < vertex->val) {
            vertex->left = new Node(elem, vertex);
            vertex = vertex->left;
        } else {
            return;
        }
        sz++;
        update_plus(go_parent_plus(vertex));
        update_begin();
    }

    void erase(const T& elem) {
        if (!root) {
            return;
        }
        Node *vertex = go_to(elem);
        if (vertex->val < elem || elem < vertex->val) {
            return;
        }
        sz--;
        if (!vertex->right || !vertex->left) {
            vertex = delete_node(vertex);
        } else {
            Node* ch = vertex->right;
            while (ch->left) {
                ch = ch->left;
            }
            vertex->val = ch->val;
            vertex = delete_node(ch);
        }
        update_minus(vertex);
        update_begin();
    }


    iterator begin() const{
        return iterator(this, begin_);
    }

    iterator end() const {
        return iterator(this, end_);
    }

    iterator find(const T& elem) const{
        if (!root) {
            return end();
        }
        Node* vertex = go_to(elem);
        if (vertex && !(vertex->val < elem) && !(elem < vertex->val)) {
            return iterator(this, vertex);
        }
        return end();
    }

    iterator lower_bound(const T& elem) const{
        Node* vertex = end_;
        Node * current = root;
        while (current) {
            if (current->val < elem) {
                current = current->right;
            } else {
                vertex = current;
                current = current->left;
            }
        }
        return iterator(this, vertex);
    }
};
