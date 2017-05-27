//#ifndef "MY_LRUCACHE"
//#define "MY_LRUCACHE"
#include <string>

//template <typename key_type, mapped_type>
struct lru_cache
{
    typedef int key_type;
    typedef std::string mapped_type;
    typedef std::pair<key_type, mapped_type> value_type;
    
    struct iterator;
    
    struct node
    {
        friend iterator;
        friend lru_cache;
        key_type key;
        mapped_type mapped;
        
        node* left;
        node* right;
        node* parent;
        
        node* next;
        node* prev;
        
    private:
        node();
        node(value_type);
        node(const node&);
        ~node();
        
        node* next_tree_node();
        node* prev_tree_node();
        node* find(key_type);
        void insert(node*);
        void delete_node();
        void erase_tree();
        void erase();
        friend void swap(node&, node&);
    };
    
    
    struct iterator {
        friend lru_cache;
        friend node;
        
        iterator();
        iterator(const iterator&);
        iterator& operator++();
        iterator& operator--();
        iterator operator++(int);
        iterator operator--(int);
        value_type operator*() const;
        bool operator==(iterator) const;
        bool operator!=(iterator) const;
        ~iterator();
        
  //  private:
        iterator(node*);
        void next();
        node* cur;
    };
    
    lru_cache(size_t capacity);
    
    ~lru_cache();
    
    iterator find(key_type);
    
    std::pair<iterator, bool> insert(value_type);
    
    void erase(iterator);
    
    iterator begin() const;
    
    iterator end() const;
//private:
    node* f_root;
    iterator iterator_begin, iterator_end;
    iterator list_begin;
    size_t capacity;
    size_t size;
};






//#endif "MY_LRUCASHE"