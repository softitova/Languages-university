#include <ctime>
#include <iostream>
#include <cstdint>
#include <emmintrin.h>

#define N 1024 * 1024 * 1024 //2 ^ 30

//using naive memcpy impl: copying byte by byte
void *memcpy_for(void *dst, const void *src, size_t sz) {
    for (size_t i = 0; i < sz; i++) {
        *((char *) (dst) + i) = *((const char *) (src) + i);
    }
    return dst;
}

//copying by 8 bytes (on 64-bit system) using regular register
void *memcpy_asm(void *dst, const void *src, size_t sz) {
    for (size_t i = 0; i < sz; i += 8) {
        size_t reg;

        __asm__("mov\t" "(%1), %0 \n"
        	"mov\t" "%0,(%2)"
        	:
        	:"r"(reg), "r"((const char *) src + i), "r"((char *) dst + i));

    }
    return dst;
}

//copying by 16 bytes using %xmm 128-bit register
void *memcpy_asm_xmm(void *dst, const void *src, size_t sz) {
    for (size_t i = 0; i < sz; i += 16) {
        __m128i reg;

        __asm__("mov\t" "(%1), %0"
        	:"=x"(reg)
       		:"r"((const char *) src + i), "0"(reg));
        __asm__("mov\t" "%1,(%0)"
        	:
        	:"r"((char *) dst + i), "x" (reg));
    }
    return dst;

}

//copying directly without using cache, the fastest known impl of memcpy
void *memcpy_asm_xmm_dqa(char *dst, const char *src, size_t sz) {
    
    ssize_t cur_pos = 0;

    
    while((size_t) (dst + cur_pos) % 16 != 0 && cur_pos < sz) {
        char cur_symbol = *(src + cur_pos);
        *(dst + cur_pos) = cur_symbol;
        cur_pos++;
    }
    
    ssize_t aligned_sz = (sz - cur_pos) % 16;
    
    for (size_t i = cur_pos; i < sz - aligned_sz; i += 16) {
        __m128i reg;

        __asm__("movdqu\t" "(%1), %0"
        	:"=x"(reg)
        	:"r"((const char *) src + i), "0"(reg));
        __asm__("movntdq\t" "%1,(%0)"
        	:
       		:"r"((char *) dst + i), "x" (reg));
    }
    
    
    for (int i = (sz - aligned_sz) ; i < sz ; i++)  {
        *(dst + i) = *(src + i);
    }
    _mm_sfence();
    
    return dst;
}

int main()
{

    char *src = new char[N];
    char *dst = new char[N];

    std::clock_t begin = std::clock();
    memcpy_for(dst, src, N);
    std::cout << std::clock() - begin << std::endl;

    begin = std::clock();
    memcpy_asm(dst, src, N);
    std::cout << std::clock() - begin << std::endl;

    begin = std::clock();
    memcpy_asm_xmm(dst, src, N);
    std::cout << std::clock() - begin << std::endl;

    begin = std::clock();
    memcpy_asm_xmm_dqa(dst, src, N);
    std::cout << std::clock() - begin << std::endl;

    return 0;
}
