

#ifndef MY_DEC_H
#define MY_DEC_H

struct dec
{
public:
    struct iterator
    {
        iterator ();
        iterator (const dec::iterator& other);
        friend struct dec;
        ~iterator ();
        iterator operator=(iterator other);
        std::string& operator*();
        iterator& operator++();
        iterator operator++(int);
        iterator& operator--();
        iterator operator--(int);
        iterator& operator+=(const int b);
        friend iterator operator+(iterator, const int);
        iterator& operator-=(const int b);
        friend iterator operator-(iterator, const int);
        friend int operator-(iterator, iterator);
        void swap(iterator &b);
        bool operator==(const iterator &b);
        bool operator!=(const iterator &b);
        bool operator>(iterator);
        bool operator<(iterator);
        bool operator>=(iterator);
        bool operator<=(iterator);
        //friend void swap(iterator &a, iterator &b);
    private:
        iterator (int index, dec&);
        std::string *a;
        std::string *head;
        size_t *_capacity;
        size_t pos;
    };
//public:
    struct const_iterator
    {
        const_iterator ();
        const_iterator (const dec::const_iterator& other);
        friend struct dec;

        ~const_iterator ();
        const_iterator operator=(const_iterator other);
        const std::string& operator*();
        const_iterator& operator++();
        const_iterator operator++(int);
        const_iterator& operator--();
        const_iterator operator--(int);
        const_iterator& operator+=(const int b);
        friend const_iterator operator+(const_iterator, const int);
        const_iterator& operator-=(const int b);
        friend const_iterator operator-(const_iterator, const int);
        friend int operator-(const_iterator, const_iterator);
        void swap(const_iterator &b);
        bool operator==(const const_iterator &b);
        bool operator!=(const const_iterator &b);
        bool operator>(const_iterator);
        bool operator<(const_iterator);
        bool operator>=(const_iterator);
        bool operator<=(const_iterator);
        //friend void swap(iterator &a, iterator &b);
    private:
        const_iterator (int index, const dec&);
        std::string *a;
        std::string *head;
        size_t const *_capacity;
        size_t pos;
    };

    dec();
    dec(size_t);
    dec(dec const &);

    ~dec();

    dec& operator=(dec);

    std::string& back();

    std::string const& front() const;

    std::string const& back() const;

    std::string& front();

    bool empty() const;

    size_t size() const;

    std::string const& operator[](size_t) const;

    std::string& operator[](size_t);

    iterator begin();

    iterator end();

    const_iterator begin() const;

    const_iterator end() const;

    iterator erase(iterator);

    iterator erase(iterator first, iterator last);

    iterator insert(iterator, std::string const&);

    iterator insert(iterator position, size_t, std::string const & x);


    void push_back(std::string const&);

    void pop_back();

    void push_front(std::string const&);

    void pop_front();

    //void erase(std::string*, std::string*);

    void clear();

private:

    size_t capacity() const;

    friend void emplace_capacity(dec &x);

    friend void swap(dec &lhs, dec &rhs);

    dec(size_t cpct, size_t sz, int  head, int tail);
    friend int main();
    size_t _capacity;
    size_t _size;
    std::string *a;
    std::string *head;
 };



    #endif // MY_DEC_H

