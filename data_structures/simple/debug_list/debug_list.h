#ifndef LIST_H
#define LIST_H
#include <vector>
#include <string>

struct list
{
private:
    struct node
    {
        node (const std::string &key);
        node (const std::string &key, node const &other);
        node (const std::string &key, node *prev, node *next);
    //    node (const std::string &key, node const *prev, node const *next);
        node (const node& other);
        ~node();

        bool operator==(const node& b);
        std::string key;
        node *prev, *next;
    };



public:
    struct iterator
    {
        friend struct list;
        iterator (list* obj, const std::string &key);
        iterator (node& other, list* obj);
        iterator (const std::string &key, const iterator &other);
        iterator (const std::string &key, const iterator &prev, const iterator &next, list* obj);
        iterator (const iterator& other);
        ~iterator ();
        iterator operator=(iterator other);

        std::string& operator*();
        iterator& operator++();
        iterator operator++(int);
        iterator& operator--();
        iterator operator--(int);


        void swap(iterator &b);

        bool operator==(const iterator &b);
        bool operator!=(const iterator &b);
        friend void swap(iterator &a, iterator &b);
    private:
        node *cur;
        list *my_object;
        bool flag;
    };

    struct const_iterator
    {
        const_iterator (const list* obj, const std::string &key);
        const_iterator (node& other, const list* obj);
        const_iterator (const std::string &key, const const_iterator &other);
        const_iterator (const std::string &key, const const_iterator &prev, const const_iterator &next, const list* obj);
        const_iterator (const const_iterator& other);
        ~const_iterator ();
        const_iterator operator=(const_iterator other);

        const std::string& operator*();
        const_iterator& operator++();
        const_iterator operator++(int);
        const_iterator& operator--();
        const_iterator operator--(int);


        bool operator==(const const_iterator &b);
        bool operator!=(const const_iterator &b);

        void swap(const_iterator &b);


        friend bool operator==(const iterator &a, const iterator &b);
        friend bool operator!=(const iterator &a, const iterator &b);
        friend void swap(const_iterator &a, const_iterator &b);

    private:
        node *cur;
        const list* my_object;
        bool flag;
    };
    list();

    list(const list &my_list);

    ~list();

    list & operator = (const list & s);

    iterator begin();

    iterator end();

    const_iterator begin() const;

    const_iterator end() const;

    iterator erase (iterator pos);

    iterator insert(iterator pos, std::string const & s);

    void push_front(std::string const & s);

    void pop_front();

    std::string const& front() const;

    std::string front();

    void push_back(std::string const & s);

    void pop_back();

    std::string const& back() const;

    std::string back();

    bool empty() const;

    void splice(iterator pos, list & other, iterator first, iterator last);

    void swap(list &b);

    void clear();

private:

    std::vector<iterator*> v;
    mutable std::vector<const_iterator*> cv;
    iterator head, tail;
//    iterator fixed;
    size_t size;
};




#endif // LIST_H
