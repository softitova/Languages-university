#include<iostream>
#include<string.h>
#include<vector>
#include <emmintrin.h>
#include <stdio.h>
#include <tmmintrin.h>

__m128i tmp_reg;
//size_t ans = 0;

void print128_num_by_words(__m128i var)
{
    uint16_t *val = (uint16_t*) &var;
    printf("Value in words: %i %i %i %i %i %i %i %i \n",
           val[0], val[1], val[2], val[3], val[4], val[5],
           val[6], val[7]);
}

void print128_num_by_bytes(__m128i var)
{
    char *val = (char *) &var;
    printf("Value in bytes: ");
    for(size_t i = 0; i < 16; i++)
    {
        printf("%d ", (int)*(val + i));
    }
    printf("\n");
}

uint32_t count_words_slowly(std::string const& str)
{
    uint32_t ret = 0;
    
    bool prev_is_space = true;
    for(size_t i = 0; i < str.size(); i++) {
        if(str[i] != ' ' && prev_is_space) {
            ret++;
            prev_is_space = false;
        }
        else if(str[i] == ' ') {
            prev_is_space = true;
        }
    }
    
    return ret;
}

void get_ans(__m128i store) {
    
//    __m128i result_of_abs, abs_low, tt;
//    uint32_t high, low;
//    
//    __asm__("psadbw\t" "%3, %0\n"
//            "movd\t"   "%0, %2\n"
//            "movhlps\t" "%0, %0\n"
//            "movd\t" "%0, %1\n"
//            :"=x" (tt), "=r"(high), "=r"(low), "=x"(abs_low)
//            :"0"(_mm_set_epi32(0, 0, 0, 0)), "3"(store)
//            :"0");
//    
//    ans += high + low;
//    store = _mm_set_epi32(0, 0, 0, 0);
    
}

int count(std::string const& str) {
    size_t ans = 0;
    char const *s = str.c_str();
    size_t sz = str.size();
    
    __m128i space_reg =
    _mm_set_epi8(32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32);
    __m128i cmp_result, cmp_result_sh;
    __m128i store = _mm_set_epi32(0, 0, 0, 0);
    
    size_t cur_pos = 0;
    bool is_whitespace = false;
    
    
    if(sz < 64) {
        return count_words_slowly(str);
    }
    
    
    while((size_t) (s + cur_pos) % 16 != 0 && cur_pos < sz) {
        char cur_symbol = *(s + cur_pos);
        if(is_whitespace && cur_symbol != ' ') {
            ans++;
        }
        is_whitespace = (cur_symbol == ' ');
        cur_pos++;
    }
    
    if ((is_whitespace && *(s + cur_pos) != ' ' && cur_pos != 0)
        || (cur_pos == 0 && *s != ' ')) ans++;
    if(cur_pos != 0 && *s != ' ') ans++;
    
    size_t size = sz - (sz - cur_pos) % 16;
    
    __asm__("movdqa\t" "(%2), %1\n"
            "pcmpeqb\t" "%1, %0"
            :"=x"(cmp_result_sh), "=x"(tmp_reg)
            :"r"(s + cur_pos), "0"(space_reg));
    
    for (size_t i = cur_pos; i < size - 16 ; i += 16) {
        
        cmp_result = cmp_result_sh;
        uint32_t moskva;
        
        __m128i tmp04, tmp05, tmp06, tmp07;
        __asm__("movdqa\t" "(%7), %3\n"
                "pcmpeqb\t" "%3, %0\n"
                "movdqa\t" "%0, %6\n"
                "palignr\t" "$1, %4, %6\n"
                "pandn\t" "%4, %6\n"
                "psubsb\t" "%6, %5\n"
                "paddusb\t" "%5, %1\n"
                "pmovmskb\t" "%1, %2"
                :"=x"(cmp_result_sh), "=x"(store), "=r"(moskva), "=x"(tmp04), "=x"(tmp05), "=x"(tmp06), "=x"(tmp07)
                :"r"(s + i + 16), "0"(space_reg), "4"(cmp_result), "5"(_mm_set_epi32(0, 0, 0, 0)), "1"(store));
        
        if (moskva != 0) {
            __m128i result_of_abs, abs_low, tt;
            uint32_t high, low;
            
            __asm__("psadbw\t" "%3, %0\n"
                    "movd\t"   "%0, %2\n"
                    "movhlps\t" "%0, %0\n"
                    "movd\t" "%0, %1\n"
                    :"=x" (tt), "=r"(high), "=r"(low), "=x"(abs_low)
                    :"0"(_mm_set_epi32(0, 0, 0, 0)), "3"(store)
                    :"0");
            
            ans += high + low;
            store = _mm_set_epi32(0, 0, 0, 0);
        }
        
    }
    
    __m128i result_of_abs, abs_low, tt;
    uint32_t high, low;
    
    __asm__("psadbw\t" "%3, %0\n"
            "movd\t"   "%0, %2\n"
            "movhlps\t" "%0, %0\n"
            "movd\t" "%0, %1\n"
            :"=x" (tt), "=r"(high), "=r"(low), "=x"(abs_low)
            :"0"(_mm_set_epi32(0, 0, 0, 0)), "3"(store)
            :"0");
    
    ans += high + low;
    store = _mm_set_epi32(0, 0, 0, 0);
    
    
//    get_ans(store);
  
    cur_pos = size - 16;
    
    if(*(s + cur_pos- 1) == ' ' && *(s + cur_pos) != ' ')  ans--;
    
    is_whitespace = *(s + size - 17) == ' ';
    for (int i = (size - 16) ; i < sz ; i++)  {
        if (*(s + i) != ' ' && is_whitespace) ans++;
        is_whitespace = *(s + i) == ' ';
    }
    
    return ans;
}

int main()
{
    std::string str_strange = "        pfc e  nmwqc yrqnd  wzlz d  qlnvmk  w g   iv  ic qa sertyuiopoiuytretyuiop[oiuytryuiopoiuytryuiop[]poiytretyuiop[ptryuiop[poiuytuiop[oiuytuiopiuytuioiuyghjkopiuyujiokijughbo  d e a v                                                  ";
    //char * s = (char *)"a  bcd fxxxxxxxxxxxxxxxxxxxx  ghi wwwwwwwwwwwaaaaaaaaaaaaaaaax a";
    std::cout <<count(str_strange) << " ans ";
    return 0;
}
