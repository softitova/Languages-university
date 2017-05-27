#include <bits/stdc++.h>
#include "big_integer.h"
#include <iostream>



using namespace std;
unsigned long long p = 1 << 30;
const unsigned long long mod = p * 4;

void my_resize(big_integer &a)
{
    if(a.mass.empty())
        a.mass.push_back(0);
    while(a.mass.size() > 1 && a.mass.back() == 0)
        a.mass.pop_back();
    if(a.mass.size() == 1 && a.mass.back() == 0)
        a.sign = 0;
}

big_integer::big_integer() : mass(1, 0), sign(0) {}

big_integer::big_integer(big_integer const& new_big) : mass(new_big.mass), sign(new_big.sign) {}

big_integer::big_integer(unsigned int new_int) : mass(1, new_int), sign(0) {}

big_integer::big_integer(unsigned long long new_int) :  sign(0)
{
    mass.push_back(new_int % mod);
    if (new_int> mod)
    {  
        mass.push_back(new_int / mod);
    }
}

big_integer::big_integer(int new_int)
{
    mass.resize(1);
    if (new_int < 0) {
        sign = 1;
        mass[0] = new_int * -1ll;
    } else {
        sign = 0;
        mass[0] = new_int;
    }
    
}

big_integer::big_integer(std::string const& line)
{
    big_integer TEN(10);
    for (int i = (line[0] == '-') ? 1 : 0 ; i < (int) line.size(); i++)
    {
        *this *= TEN;
        *this += line[i] - '0';
    }
    this->sign = ( line[0] == '-' ) ? 1  :  0;
}

big_integer::~big_integer() {}

int abs_compare(opt_vector const& a, opt_vector const& b) {
    
    if (a.size() != b.size())
    {
        return (a.size() < b.size() ?  -1 : 1);
    }
    for (int i = (int) a.size() - 1; i >= 0; --i)
    {
        if (a[i] < b[i])
            return -1;
        if (a[i] > b[i])
            return 1;
    }
    return 0;
}

int compare(big_integer const& a, big_integer const& b) {
    
    if (a.sign != b.sign)
    {
        if(a.mass.size()==1 && b.mass.size()==1 && a.mass.back()==0 && b.mass.back()==0 )
        {
            return 0;
        }
        return b.sign - a.sign;
    }
    else
    {
        return (abs_compare(a.mass, b.mass) * ( (a.sign==1) ? -1 : 1));
    }
}

big_integer& big_integer::operator=(big_integer const& a)
{
    this->mass = a.mass;
    this->sign = a.sign;
    return *this;
}

bool operator==(big_integer const& a, big_integer const& b)
{
    return compare(a,b) == 0;
}

bool operator!=(big_integer const& a, big_integer const& b)
{
    return compare(a,b) != 0;
}

bool operator>(big_integer const& a, big_integer const& b)
{
    return compare(a,b) == 1;
}

bool operator<(big_integer const& a, big_integer const& b)
{
    return compare(a,b) == -1;
}

bool operator<=(big_integer const& a, big_integer const& b)
{
    return compare(a,b) != 1;
}

bool operator>=(big_integer const& a, big_integer const& b)
{
    return compare(a,b) != -1;
}

big_integer operator-(big_integer a, big_integer const& b)
{
    return a -= b;
}

big_integer operator+(big_integer a, big_integer const& b)
{
    return a += b;
}

void multiply(opt_vector &res, const opt_vector &a, uint64_t b)
{
    res.resize(a.size());
    uint64_t x = 0;
    for (size_t i = 0; i < a.size(); i++)
    {
        x += (uint64_t) a[i] * b;
        res[i] = (x & ((1ll >> 32) - 1));
        x >>= 32;
    }
    if (x)
    {
        res.push_back(x);
    }
}

unsigned int binary (const big_integer &dop, const big_integer &second) {
    unsigned long long l = 0;
    unsigned long long r = mod;
    if (abs_compare(second.mass, dop.mass) > 0)
    {
        return 0;
    }
    opt_vector temp;
    while (l < r -1) {
        unsigned m = (l + r) >> 1;

        multiply(temp, second.mass, m);
        if (abs_compare(temp, dop.mass) > 0)
        {
            r = m;
        }
        else
        {
            l = m;
        }
    }
    return l;
}

void div_long_short (big_integer &first, int second) {
    int carry = 0;
    for (int i = (int)first.mass.size() - 1; i >= 0; --i)
    {
        long long cur = first.mass[i] + carry * 1ll *  mod;
        first.mass[i] = int (cur / second);
        carry = int (cur % second);
    }
    my_resize(first);
}

big_integer operator%(big_integer a, big_integer const& b)
{
    return a %= b;
}

big_integer& big_integer::operator+=(big_integer const& a)
{
    return *this -= -a;
}

big_integer& big_integer::operator-=(big_integer const &b)
{
    size_t asize = mass.size();
    size_t bsize = b.mass.size();
    int64_t carry = 0, ncarry;
    size_t sz = std::max(asize, bsize);
    mass.resize(sz);
    if (sign != b.sign)
    {
        
        for (size_t cur = 0; cur < sz; cur++)
        {
            carry = carry + (cur < asize ? mass[cur] : 0) + (cur < bsize ? b.mass[cur] : 0);
            mass[cur] = carry % mod;
            carry /= mod;
        }
        if (carry != 0)
        {
            mass.push_back(carry);
        }
    }
    else
    {
        bool flag = (sign == 1) == (*this < b);
        
        for (size_t it = 0; it < sz; it++)
        {
            int64_t buf = (int64_t)mass[it] - (it < bsize ? b.mass[it] : 0);
            carry = carry + (flag ?  buf: -buf);
            ncarry = 0;
            if (carry < 0)
            {
                carry += mod;
                ncarry--;
            }
            mass[it] = carry;
            carry = ncarry;
        }
        if (!flag)
        {
            sign ^= 1;
        }
    }
    my_resize(*this);
    return *this;
}

big_integer& big_integer::operator*=(big_integer const& b)
{
    if (b.mass.size() == 1 || mass.size() == 1)
    {
        uint64_t q = b.mass.back();
        if (mass.size() == 1)
        {
            q = mass.back();
            mass = b.mass;
        }
        uint64_t x = 0;
        for (size_t i = 0; i < mass.size(); i++)
        {
            x += mass[i] * q;
            mass[i] = x % mod;
            x /= mod;
        }
        if (x)
        {
            mass.push_back(x);
        }
    }
    else
    {
        std::vector<uint64_t> result(mass.size() + b.mass.size() + 1, 0);
        for (size_t i = 0; i < mass.size(); i++)
            for (size_t j = 0; j < b.mass.size(); j++)
            {
                uint64_t cur = result[i + j] + (uint64_t)mass[i] * b.mass[j];
                result[i + j] = cur % mod;
                result[i + j + 1] += cur / mod;
            }
        mass.resize(result.size());
        for (size_t cur = 0; cur + 1 < result.size(); cur++)
        {
            result[cur + 1] += result[cur] / mod;
            mass[cur] = result[cur] % mod;
        }
    }
    sign ^= b.sign;
    my_resize(*this);
    return *this;
    
}

big_integer operator*(big_integer a, big_integer const& b)
{
    return a *= b;
}

big_integer& big_integer::operator/=(big_integer const& b)
{
    
    int this_sign = sign;
    sign = 0;
    if (abs_compare(mass, b.mass) < 0)
    {
        mass.resize(1);
        mass[0] = 0;
    }
    else if (b.mass.size() == 1)
    {
        div_long_short(*this, b.mass[0]);
    }
    else
    {
        static big_integer dop;
        dop.mass.clear();
        opt_vector result;
        while (mass.size() != 0)
        {
            dop.mass.push_front(mass.back());
            mass.pop_back();
            unsigned l = binary(dop, b);
            if (result.size() != 1 || l != 0)
            {
                result.push_back(l);
            }
            if (l != 0)
            {
                dop -= l * b * (b.sign ? -1 : 1);
            }
            while (!dop.mass.empty() && dop.mass.back() == 0)
            {
                dop.mass.pop_back();
            }
        }
        result.all_reverse();
        swap(mass, result);
    }
    sign = this_sign ^ b.sign;
    my_resize(*this);
    return *this;
}
big_integer operator/(big_integer a, big_integer const& b)
{
    return a /= b;
}

big_integer& big_integer::operator%=(big_integer const& b)
{
    return *this = *this - (*this / b) * b;
}
big_integer& big_integer::operator--()

{
    *this -= 1;
    return *this;
}

big_integer big_integer::operator--(int)
{
    big_integer t = *this;
    --(*this);
    return t;
}

big_integer& big_integer::operator++()
{
    *this += 1;
    return *this;
}

big_integer big_integer::operator++(int)
{
    big_integer t = *this;
    ++(*this);
    return t;
}

big_integer double_code(big_integer a)
{
    if (a.sign == 0)
    {
        return a;
    }
    for (int i = 0; i < (int) a.mass.size(); i++)
    {
        a.mass[i] = ~a.mass[i];
    }
    return a;
}

big_integer operator&(big_integer a, big_integer const& b)
{
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const& b)
{
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const& b)
{
    return a ^= b;
}

big_integer operator<<(big_integer a , int b)
{
    return a <<= b;
}

big_integer operator>>(big_integer a , int b)
{
    return a >>= b;
}


big_integer& make_binary_op (big_integer &a, big_integer const &b, int operation)
{
    big_integer  second = double_code(b);
    a = double_code(a);
    second.sign == 1 ? --second : second;
    if (a.mass.size() > second.mass.size())
    {
        std::swap(a, second);
    }
    for (int i = 0; i < (int) second.mass.size(); i++)
    {
        if (i >= (int)a.mass.size())
        {
            a.mass.push_back(a.sign == 1 ? std::numeric_limits<unsigned>::max() : 0);
        }
        switch (operation)
        {
            case 1 :
                a.mass[i] &= second.mass[i];
                a.sign = a.sign & b.sign;
                break;
            case 2 :
                a.mass[i] ^= second.mass[i];
                a.sign = a.sign ^ b.sign;
                break;
            case 3 :
                a.mass[i] |= second.mass[i];
                a.sign = a.sign | b.sign;
                break;
        }
        
    }
    if (a.sign)
    {
        a.mass.push_back(std::numeric_limits<unsigned>::max());
        a = double_code(a + 1);
    }
    my_resize(a);
    return a;
}

big_integer& big_integer::operator&=(big_integer const& b)
{
    return make_binary_op(*this, b, 1);
}

big_integer& big_integer::operator^=(big_integer const& b)
{
    return make_binary_op(*this, b, 2);
}

big_integer& big_integer::operator|=(big_integer const& b)
{
    return make_binary_op(*this, b, 3);
}

big_integer& big_integer::operator<<=(int b)
{
    if (b < 0) {
        *this >>= (-b);
        return *this;
    }
    int count = b/32;
    int ost = b%32;
    unsigned int last  = 0;
    if (ost)
    {
        for (int i = 0; i < (int) mass.size(); ++i)
        {
            unsigned long long now = (unsigned long long) mass[i] << ost;
            mass[i] = (unsigned int) now % mod + last;
            if (now >= mod)
            {
                now >>= 32;
                last = now;
            }
            else
            {
                break;
            }
        }
        if (last)
        {
            mass.push_back(last);
        }
    }
    int k = (int)mass.size();
    mass.resize(k + count);
    for (int i = 0; i < (int) count; i++)
    {
        mass[i + k] = mass[i];
    }
    for (int i = 0; i < count; i++) {
        mass[i]= 0;
    }
    if (mass.back() == 0 && mass.size() == 1)
    {
        sign=0;
    }
    return *this;
}

big_integer& big_integer::operator>>=(int b)
{
    if (b < 0)
    {
        *this <<= (-b);
        return *this;
    }
    if (*this < 0)
    {
        *this =double_code(*this);
        --*this;
    }
    int count = b/32;
    int ost = b%32;
    unsigned long long last  = 0;
    int k = (int)mass.size();
    if (k - count  < 0)
    {
        *this =  big_integer();
        return *this;
    } if ( count > 0)
    {
        for (int i = count; i < (int) mass.size(); i++)
        {
            
            mass[i - count] = mass[i];
            mass[i] = 0;
        }
    }
    if (ost)
    {
        unsigned int M = 1 << ost;
        for (int i = (int) mass.size() - count - 1; i >= 0; --i)
        {
            unsigned long long f = (unsigned long long) mass[i] + (last << 32);
            last = f % M;
            mass[i] = f >> ost;
        }
        if (sign == 1 ){
            mass[(int) mass.size() - count - 1] += (M - 1) * (1 << (32 - ost));
        }
    }
    *this = sign == 1 ? double_code(*this + 1) :  *this;
    my_resize(*this);
    return *this;
}

big_integer big_integer::operator+() const
{
    return *this;
}

big_integer big_integer::operator-() const
{
    big_integer a(*this);
    a.sign ^= 1;
    return a;
}

big_integer big_integer::operator~() const
{
    big_integer a(*this + 1);
    a.sign ^= 1;
    return a;
}

std::string to_string(big_integer const& a)
{
    big_integer aa = a, nul(0);
	int module = 1000000;
    std::string s = "";
    while(aa != nul)
    {
        big_integer M = aa % module;
        int m = M.mass[0];
        std::string ss = "";
        while (m > 0)
        {
            ss += (char) (m % 10 + 48);
            m /= 10;
        }
        aa /= big_integer(module);
        if (aa != nul)
        {
            for (int i = ss.length(); i < 6; i++)
            {
                ss += "0";
            }
        }
        s += ss;
    }
    reverse(s.begin(), s.end());
    

    if (a.sign == 1)
    {
        if (s.empty())
        {
            return "0";
        }
        s = "-" + s;
    }
    return s.empty() ? "0" : s;
}

std::istream& operator>>(std::istream& s, big_integer& a)
{
    std::string str;
    s >> str;
    a = big_integer(str);
    return s;
}

std::ostream& operator<<(std::ostream& s, big_integer const& a)
{
    s << to_string(a);
    return s;
}
// int main()
// {
//    // big_integer a(1);
//  big_integer a("1000000000000000000000000000000000000000000000000000000000000");
//  big_integer b(                                                     "10000000");
//  big_integer c("100000000000000000000000000000000000000000000000000000");
//     // a *= b; 
//  assert(a / b == c);

//     return 0;
// }




