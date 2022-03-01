#include <iostream>

using namespace std;

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
            if (!this->right) return nullptr;
            Node * b = right;
            right = b->left;
            if (right)
                right->parent = this;
            b->left = this;
            b->parent = parent;
            if (parent) {
                if (parent->left == this) parent->left = b;
                else parent->right = b;
            }
            parent = b;
            diff = diff + max(0, -b->diff) + 1;
            b->diff = max(0, diff) + 1 + b->diff;
            return b;
        }
        Node* rotate_right() {
            if (!this->left) return nullptr;
            Node * b = left;
            left = b->right;
            if (left)
                left->parent = this;
            b->right = this;
            b->parent = parent;
            if (parent) {
                if (parent->left == this) parent->left = b;
                else parent->right = b;
            }
            parent = b;
            diff = diff - 1 - max(0, b->diff);
            b->diff = b->diff - 1 - max(0, -diff);
            return b;
        }
        Node* big_rotate_left() {
            right->rotate_right();
            return this->rotate_left();
        }
        Node* big_rotate_right() {
            left->rotate_left();
            return this->rotate_right();
        }
        void print() const{
            if (!this) return;
            left->print();
            if (!parent) {
                cout << "(" << val << ", " << "p = none, diff = " << diff <<  ");   ";
            } else {
                cout << "(" << val << ", " << "p =" << parent->val << ", diff = " << diff << ");   ";
            }
            right->print();
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
        Node* v = root;
        while (true) {
            if (v->val < elem) {
                if (v->right) v = v->right;
                else return v;
            } else if (elem < v->val) {
                if (v->left) v = v->left;
                else return v;
            } else return v;
        }
    }

    Node* update_to_right(Node* v) {
        if (!v->parent) {
            root = v->right;
            begin_ = root;
            delete root->parent;
            root->parent = nullptr;
            return nullptr;
        }
        if (v == v->parent->left) {
            v->parent->left = v->right;
            v = v->parent;
            delete v->left->parent;
            v->left->parent = v;
            v->diff--;
        } else {
            v->parent->right = v->right;
            v = v->parent;
            delete v->right->parent;
            v->right->parent = v;
            v->diff++;
        }
        return v;
    }
    Node* update_to_left(Node* v) {
        if (!v->parent) {
            root = v->left;
            delete root->parent;
            root->parent = nullptr;
            return nullptr;
        }
        if (v == v->parent->left) {
            v->parent->left = v->left;
            v = v->parent;
            delete v->left->parent;
            v->left->parent = v;
            v->diff--;
        } else {
            v->parent->right = v->left;
            v = v->parent;
            delete v->right->parent;
            v->right->parent = v;
            v->diff++;
        }
        return v;
    }

    Node* update_to_nothing(Node * v) {
        if (!v->parent) {
            delete root;
            root = nullptr;
            begin_ = end_;
            return nullptr;
        }
        if (v == v->parent->left) {
            v = v->parent;
            delete v->left;
            v->left = nullptr;
            v->diff--;
        } else {
            v = v->parent;
            delete v->right;
            v->right = nullptr;
            v->diff++;
        }
        return v;
    }

    Node* delete_node(Node* v) {
        if (v->left && v->right) {
            return v;
        }
        if (v->left) {
            return update_to_left(v);
        }
        if (v->right) {
            return update_to_right(v);
        }
        return update_to_nothing(v);
    }

    Node* go_parent_plus(Node* v) {
        if (!v->parent) {
            return nullptr;
        }
        if (v == v->parent->left) {
            v->parent->diff++;
        } else {
            v->parent->diff--;
        }
        return v->parent;
    }
    Node* go_parent_minus(Node* v) {
        if (!v->parent) {
            return nullptr;
        }
        if (v == v->parent->left) {
            v->parent->diff--;
        } else {
            v->parent->diff++;
        }
        return v->parent;
    }
    void update_plus(Node* v) {
        if (!v) return;
        if (v->diff == 0) return;
        if (v->diff == 1 || v->diff == -1) {
            update_plus(go_parent_plus(v));
            return;
        }
        v = do_rotate(v);
        if (v->diff == 0) return;
        update_plus(go_parent_plus(v));
    }
    void update_minus(Node* v) {
        if (!v) return;
        if (v->diff == 1 || v->diff == -1) return;
        if (v->diff == 0) {
            update_minus(go_parent_minus(v));
            return;
        }
        v = do_rotate(v);
        if (v->diff == 1 || v->diff == -1) return;
        update_minus(go_parent_minus(v));
    }

    Node* do_rotate(Node * v) {
        if (v->diff == -2) {
            if (!v->right) return root; // cant
            if (v->right->diff < 1) {
                if (!v->parent) {
                    root = v->rotate_left();
                    return root;
                }
                return v->rotate_left();
            }
            if (!v->parent) {
                root = v->big_rotate_left();
                return root;
            }
            return v->big_rotate_left();
        }
        if (!v->left) return root;
        if (v->left->diff > -1) {
            if (!v->parent) {
                root = v->rotate_right();
                return root;
            }
            return v->rotate_right();
        }
        if (!v->parent) {
            root = v->big_rotate_right();
            return root;
        }
        return v->big_rotate_right();
    }
public:

    friend class Iterator;

    class iterator {
        friend class Set<T>;
    public:
        iterator() {st = nullptr; v = nullptr;}
        iterator& operator ++() {
            if (v->right) {
                v = v->right;
                while (v->left) v = v->left;
                return *this;
            }
            while (v->parent && v->parent->right == v) {
                v = v->parent;
            }
            if (!v->parent) {
                v = st->end_;
                return *this;
            }
            v = v->parent;
            return *this;
        }
        iterator operator ++(int) {
            iterator* it = this;
            if (v->right) {
                v = v->right;
                while (v->left) v = v->left;
                return *it;
            }
            while (v->parent && v->parent->right == v) {
                v = v->parent;
            }
            if (!v->parent) {
                v = st->end_;
                return *it;
            }
            v = v->parent;
            return *it;
        }
        iterator& operator --() {
            if (v == st->end_) {
                v = st->root;
                while (v->right) v = v->right;
                return *this;
            }
            if (v->left) {
                v = v->left;
                while (v->right) v = v->right;
                return *this;
            }
            while (v->parent && v->parent->left == v) {
                v = v->parent;
            }
            if (!v->parent) {
                v = st->end_;
            } else {
                v = v->parent;
            }
            return *this;
        }
        iterator operator --(int) {
            iterator* it = this;
            if (v == st->end_) {
                v = st->root;
                while (v->right) v = v->right;
                return *it;
            }
            if (v->left) {
                v = v->left;
                while (v->right) v = v->right;
                return *it;
            }
            while (v->parent && v->parent->left == v) {
                v = v->parent;
            }
            if (!v->parent) {
                v = st->end_;
            } else {
                v = v->parent;
            }
            return *it;
        }
        bool operator ==(const iterator & i) const {
            if (st != i.st || v != i.v) {
                return false;
            }
            return true;
        }
        bool operator != (const iterator & i) const {
            if (st == i.st && v == i.v) {
                return false;
            }
            return true;
        }
        const T& operator*() const {
            return v->val;
        }
        const T* operator->() {
            return &v->val;
        }

    private:
        iterator(const Set* st_, const Node* v_){
            st = st_;
            if (!v_) v = st_->end_;
            else v = v_;
        }
        const Node* v;
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

    void del(Node* v) {
        if (!v) return;
        del(v->left);
        del(v->right);
        delete v;
    }
    ~Set() {
        del(root);
        delete end_;
    }
    Node* copy(Node* v) {
        if (!v) return nullptr;
        Node* v_ = new Node(v->val);
        v_->diff = v->diff;
        v_->left = copy(v->left);
        v_->right = copy(v->right);
        if (v_->left) {
            v_->left->parent = v_;
        }
        if (v_->right) {
            v_->right->parent = v_;
        }
        return v_;
    }
    Set(const Set& st) {
        root = copy(st.root);
        end_ = new Node();
        update_begin();
        sz = st.sz;
    }
    Set& operator=(const Set& st) {
        // del(root);
        Node *v = copy(st.root);
        del(root);
        root = v;
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
    void print() const {
        cout << sz << endl;
        root->print();
        cout << endl;
    }

    void insert(const T& elem) {
        if (!root) {
            root = new Node(elem);
            begin_ = root;
            sz++;
            return;
        }
        Node *v = go_to(elem);
        if (v->val < elem) {
            v->right = new Node(elem, v);
            v = v->right;
        } else if (elem < v->val) {
            v->left = new Node(elem, v);
            v = v->left;
        } else return;
        sz++;
        update_plus(go_parent_plus(v));
        update_begin();
    }

    void erase(const T& elem) {
        if (!root) return;
        Node *v = go_to(elem);
        if (v->val < elem || elem < v->val) return;
        sz--;
        if (!v->right || !v->left) {
            v = delete_node(v);
        } else {
            Node* ch = v->right;
            while (ch->left) {
                ch = ch->left;
            }
            v->val = ch->val;
            v = delete_node(ch);
        }
        update_minus(v);
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
        Node* v = go_to(elem);
        if (v && !(v->val < elem) && !(elem < v->val)) {
            return iterator(this, v);
        }
        return end();
    }

    iterator lower_bound(const T& elem) const{
        Node* v = end_;
        Node * curr = root;
        while (curr) {
            if (curr->val < elem) {
                curr = curr->right;
            } else {
                v = curr;
                curr = curr->left;
            }
        }
        return iterator(this, v);
    }
};