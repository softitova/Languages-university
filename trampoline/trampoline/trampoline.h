
#ifndef TRAMPOLINE_H
#define TRAMPOLINE_H

#include <xmmintrin.h>
#include <stdlib.h>
#include <sys/mman.h>

template <typename ... Args>
struct args_types;

template <>
struct args_types<>
{
    static const int INTEGER = 0;
    static const int SSE = 0;
};

template <typename First, typename ... Args>
struct args_types<First, Args ...>
{
    static const int INTEGER = args_types<Args ...>::INTEGER + 1;
    static const int SSE = args_types<Args ...>::SSE;
};

template <typename ... Args>
struct args_types<float, Args ...>
{
    static const int INTEGER = args_types<Args ...>::INTEGER;
    static const int SSE = args_types<Args ...>::SSE + 1;
};

template <typename ... Args>
struct args_types<double, Args ...>
{
    static const int INTEGER = args_types<Args ...>::INTEGER;
    static const int SSE = args_types<Args ...>::SSE + 1;
};

template <typename ... Args>
struct args_types<__m64, Args ...>
{
    static const int INTEGER = args_types<Args ...>::INTEGER;
    static const int SSE = args_types<Args ...>::SSE + 1;
};


namespace
{
    void** p = nullptr;
    const int SIZE = 123;
    const int PAGES_AMOUNT = 1;
    const int PAGE_SIZE = 4096;
    
    void alloc() {
        void* mem = mmap(nullptr, PAGE_SIZE * PAGES_AMOUNT,
                         PROT_EXEC | PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS,
                         -1, 0);
        p = (void**) mem;
        if (mem != nullptr) {
            for (auto i = 0; i < PAGE_SIZE * PAGES_AMOUNT; i += SIZE) {
                auto tmp = (char*)mem + i;
                *(void**)tmp = 0;
                if (i != 0) *(void**)(tmp - SIZE) = tmp;
            }
        }
    }

    void* malloc_ptr() {
        if (p == nullptr) {
            alloc();
            if (p == nullptr) return nullptr;
        }
        void* ans = p;
        p = (void**)*p;
        return ans;
    }

    void free_ptr(void* ptr) {
        *(void**) ptr = p;
        p = (void**) ptr;
    }
}


template <typename T>
struct trampoline
{
    template <typename F>
    trampoline(F func) {}
    trampoline(const trampoline&) = delete;
    T* get() const;
};

template <typename T, typename ... Args>
struct trampoline<T (Args ...)>
{
private:
    const char* operations[6] = {
        "\x48\x89\xfe" /*mov rsi rdi*/,
        "\x48\x89\xf2" /*mov rdx rsi*/,
        "\x48\x89\xd1" /*mov rcx rdx*/,
        "\x49\x89\xc8" /*mov r8 rcx;*/,
        "\x4d\x89\xc1" /*mov r9 r8;*/,
        "\x41\x51" /*push %%r9;*/
    };
    
    void add(char* &p, const char* command) {
        for (const char *i = command; *i; i++) {
            *(p++) = *i;
        }
    }
 
public:
    template <typename F>
    trampoline(F func) : func_obj(new F(std::move(func))), deleter(my_deleter<F>)
    {
        code = malloc_ptr();
        char* pcode = (char*)code;
        
        if (args_types<Args ...>::INTEGER < 6)
        {
            /* shift each param for the next free register */
            for (int i = args_types<Args ...>::INTEGER - 1; i >= 0; i--) add(pcode, operations[i]);
            /*  move executing function ptr to the first register */
            add(pcode,"\x48\xbf");
            *(void**)pcode = func_obj;
            pcode += 8;
            /* set ret function ptr */
            add(pcode, "\x48\xb8");
            *(void**)pcode = (void*)&do_call<F>;
            pcode += 8;
            /* jump to ret function */
            add(pcode, "\xff\xe0");
        }
        else
        {
            /*---------------- place arguments ----------------*/
            
            // save into r11 previous top of stack adress to return it after executing do_call\
            move r11 [rsp]
            add(pcode, "\x4c\x8b\x1c\x24");

            // push 6'th argument to stack, shift other arguments\
            rdi is free now
            for (int i = 5 ; i >= 0; i--) add(pcode, operations[i]);
            
            // 5 INTEGER args in r[9,8], rsi, rdi, rdx, 1 on stack, 8 SSE args in xmm[0..7]\
            stack_size - amount of arguments to place on stack
            int stack_size = 8 * (args_types<Args ...>::INTEGER - 6 + std::max(args_types<Args ...>::SSE - 8, 0)) + 8;
            
            
            /*--------------- shifting arguments --------------*/
            
            // rax on the top of current stack\
            mov rax,rsp
            add(pcode, "\x48\x89\xe0");
            
            // set rax onto the zero argument on stack\
            add rax, stack_size + 8
            add(pcode,"\x48\x05");
            *(int32_t*)pcode = stack_size;
            pcode += 4;
            
            // shift rsp to free place to provide shifting of args on stack using rsp
            add(pcode,"\x48\x81\xc4");
            *(int32_t*)pcode = 8;
            pcode += 4;
            
            //need to shift all args by 1, to free place for return function adress\
            do shifting in cycle using label_1
            char* label_1 = pcode;
            
            // compare rax and rsp, to understand if all arguments were shifted\
            cmp rax,rsp
            add(pcode,"\x48\x39\xe0");
            // if all arguments have been already shifted go to label_2\
            je
            add(pcode, "\x74");
            char* label_2 = pcode;
            pcode++;
            add(pcode, "\x48");
                      

            /*----------------- shifting argument -------------------*/
            {
                // rsp point to free place, set it to arg we want to shift\
                add rsp,
                add(pcode,"\x48\x81\xc4\x08");
                pcode += 3;
                // save arg we want to shift in rdi, it was free before this\
                mov rdi,[rsp]
                add(pcode, "\x48\x8b\x3c\x24");
             
                // shift arg by pusing saved in rdi argument onto free place on stack\
                mov [rsp-0x8],rdi
                add(pcode, "\x48\x89\x7c\x24\xf8");
             
                // jupm cause need to shift all args in cycle\
                jmp
                add(pcode, "\xeb");
                *pcode = label_1 - pcode - 1;
                pcode++;
            }
            *label_2 = pcode - label_2 - 1;
            
            /*----------- working with rsp, calling function ----------*/
            
            //move saved in r11 return adress onto the free place\
            mov [rsp],r11
            add(pcode, "\x4c\x89\x1c\x24");
          
            //update rsp\
            sub rsp,...
            add(pcode,"\x48\x81\xec");
            *(int32_t*)pcode = stack_size;
            pcode += 4;
            
            /*  move executing function ptr to the first register */
            add(pcode,"\x48\xbf");
            *(void**)pcode = func_obj;
            pcode += 8;
            
            /* set ret function ptr */
            add(pcode, "\x48\xb8");
            *(void**)pcode = (void*)&do_call<F>;
            pcode += 8;
            
            // calling function, in rax\
            call rax
            add(pcode,"\xff\xd0");

            // removing 6'th arg from stack
            add(pcode,"\x41\x59");
         
            /* updating rsp */
            
                //saving into r11 adress of current rsp plus shift on curent size of stack\
                have one less argument in stack cause of one arument have already been deleted from stack\
                mov r11,[rsp+const]
                add(pcode,"\x4c\x8b\x9c\x24");
                *(int32_t*)pcode = stack_size - 8;
                pcode += 4;

                //set corrrect value previously stored in r11 to rsp\
                mov [rsp],r11
                add(pcode, "\x4c\x89\x1c\x24");
            
            
            /* FINALLY RETURN */
            add(pcode,"\xc3");
        }
    
    }
    
    template <typename F>
    static T do_call(void* obj, Args ...args) {
        return (*(F*)obj)(args...);
    }
    
    T (*get() const)(Args ... args)
    {
        return (T(*)(Args ... args))code;
    }
    
    ~trampoline()
    {
        if (func_obj)
        {
            deleter(func_obj);
        }
        free_ptr(code);
    }
    
private:
    void* func_obj;
    void* code;
    void (*deleter)(void*);
    
    template <typename F>
    static void my_deleter(void* func_obj)
    {
        delete static_cast<F*>(func_obj);
    }
};
#endif



