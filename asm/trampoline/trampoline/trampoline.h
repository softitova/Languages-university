#ifndef TRAMPOLINE_H
#define TRAMPOLINE_H

#include <stdlib.h>
#include <sys/mman.h>
#include <xmmintrin.h>

template <typename ... Args>
struct args_t;

template <>
struct args_t<> {
    static const int INT_PTR = 0;
    static const int SSE = 0;
};

template <typename First, typename ... Args>
struct args_t<First, Args ...> {
    static const int INT_PTR = args_t<Args ...>::INT_PTR + 1;
    static const int SSE = args_t<Args ...>::SSE;
};

template <typename ... Args>
struct args_t<double, Args ...> {
    static const int INT_PTR = args_t<Args ...>::INT_PTR;
    static const int SSE = args_t<Args ...>::SSE + 1;
};

template <typename ... Args>
struct args_t<float, Args ...> {
    static const int INT_PTR = args_t<Args ...>::INT_PTR;
    static const int SSE = args_t<Args ...>::SSE + 1;
};

template <typename ... Args>
struct args_t<__m64, Args ...> {
    static const int INT_PTR = args_t<Args ...>::INT_PTR;
    static const int SSE = args_t<Args ...>::SSE + 1;
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
        
        for (auto i = 0; i < PAGE_SIZE * PAGES_AMOUNT; i += SIZE) {
            auto с = (char*)mem + i;
            *(void**)с = 0;
            if (i != 0) *(void**)(с - SIZE) = с;
        }
        
    }

    void* get_next() {
        if (p == nullptr) {
            std::cout << "in alloc" << std::endl;
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
struct trampoline;

template<typename R, typename... Args>
void swap(trampoline<R(Args...)>& lhs, trampoline<R(Args...)>& rhs);

template <typename T, typename ... Args>
struct trampoline<T (Args ...)> {
    
private:
    
    const char* shifts[6] = {
        "\x48\x89\xfe" /*mov rsi rdi*/,"\x48\x89\xf2" /*mov rdx rsi*/,
        "\x48\x89\xd1" /*mov rcx rdx*/,"\x49\x89\xc8" /*mov r8 rcx*/,
        "\x4d\x89\xc1" /*mov r9 r8*/,"\x41\x51" /*push r9*/
    };
    
    void add(char* &p, const char* command) {
        for (const char *i = command; *i; i++) *(p++) = *i;
    }
 
public:
    
    template <typename F>
    trampoline(F func) : func_obj(new F(std::move(func))), deleter(my_deleter<F>) {
        code = get_next();
        char* pcode = (char*)code;
        
        if (args_t<Args ...>::INT_PTR < 6)
        {
            /* shift each param to the next free register */
            
            for (int i = args_t<Args ...>::INT_PTR - 1; i >= 0; i--) add(pcode, shifts[i]);
            add(pcode,"\x48\xbf");
            *(void**)pcode = func_obj;
            pcode += 8;
            add(pcode, "\x48\xb8");
            *(void**)pcode = (void*)&do_call<F>;
            pcode += 8;
            add(pcode, "\xff\xe0");
        }
        else
        {
            /*---------------- place arguments ----------------*/
            
            /* 5 INTEGER args in r[9,8], rsi, rcx, rdx, 1 will be on stack later, <= 8 SSE args in xmm[0..7]
            stack_size - amount of arguments to place on stack */
            int stack_size = 8 * (args_t<Args ...>::INT_PTR - 5 + std::max(args_t<Args ...>::SSE - 8, 0));
            
            /* save into r11 previous top of stack adress to return it after executing do_call
            move r11 [rsp] */
            add(pcode, "\x4c\x8b\x1c\x24");

            /* push 6'th argument to stack, shift other arguments, rdi is free now */
            for (int i = 5 ; i >= 0; i--) add(pcode, shifts[i]);
            
            /*--------------- shifting arguments --------------*/
            {
                /* rax on the top of the current stack
                mov rax,rsp                                                         // 48 89 E0
                rax on the bottom
                add rax, stack_size */
                add(pcode, "\x48\x89\xe0\x48\x05");
                *(int32_t*)pcode = stack_size;
                pcode += 4;
                
                /* shift rsp (+8) to reti to kill it and provide arguments shifting
                add rsp, 8  */
                add(pcode,"\x48\x81\xc4");
                *(int32_t*)pcode = 8;
                pcode += 4;
                
                /* need to shift all args by 1, to kill reti and save it in free place,
                cycle with label_1 */
                char* label_1 = pcode;
                
                /* compare rax and rsp, to understand if all arguments were shifted
                cmp rax,rsp
                if all arguments have been already shifted go to label_2
                je */
                add(pcode,"\x48\x39\xe0\x74");
                char* label_2 = pcode;
                pcode++;
//                add(pcode, "\x48");
                

                /*----------------- shifting argument -------------------*/
                {
                    /* set rsp to arg we want to shift
                    add rsp, 8    48 81 C4
                    save arg we want to shift in rdi
                    mov rdi, [rsp]   48 8B 3C 24
                    shift arg by pusing saved in rdi argument onto free place on stack
                    mov [rsp-0x8],rdi  48 89 7C 24 F8
                    cycle
                    jmp
                     */
                    add(pcode,"\x48\x81\xc4\x08");
                    pcode += 3;
                    add(pcode, "\x48\x8b\x3c\x24\x48\x89\x7c\x24\xf8\xeb");
                    *pcode = label_1 - pcode - 1;
                    pcode++;
                }
                *label_2 = pcode - label_2 - 1;
            }
            
            /*----------- working with rsp, calling function ----------*/
            {
                /* move saved in r11 return adress to rsp (onto the bottom of stack)
                mov [rsp], r11                                                    // 4C 89 1C 24
                transfer rsp to the top of stack
                sub rsp, stack_size                                               //  48 81 EC */
                add(pcode, "\x4c\x89\x1c\x24\x48\x81\xec");
                *(int32_t*)pcode = stack_size;
                pcode += 4;
                
                /* no comments here...
                mov rdi, imm 
                mov rax, imm */
                add(pcode,"\x48\xbf");
                *(void**)pcode = func_obj;
                pcode += 8;
                add(pcode, "\x48\xb8");
                *(void**)pcode = (void*)&do_call<F>;
                pcode += 8;
                
                /* calling function
                call rax                                                           // FF D0
                removing 6'th arg from stack, cause of calling conventions
                pop r9                                                             // 41 59
                save into r11 adress of current rsp plus shift on curent stack size
                have one less argument in stack cause of one arument have already been deleted from stack
                mov r11,[rsp + stack_size]                                         // 4C 8B 9C 24
                set corrrect value, previously stored in r11, to rsp
                mov [rsp],r11                                                      // 4C 89 1C 24
                return
                ret */
                add(pcode,"\xff\xd0\x41\x59\x4c\x8b\x9c\x24");
                *(int32_t*)pcode = stack_size - 8;
                pcode += 4;
                add(pcode, "\x4c\x89\x1c\x24\xc3");
            }
        }
    }
    
    trampoline(trampoline&& other) {
        func_obj = other.func_obj;
        code = other.code;
        deleter = other.deleter;
        other.func_obj = nullptr;
    }
    
    trampoline(const trampoline&) = delete;
    
    template <class TR>
    trampoline& operator=(TR&& func) {
        trampoline tmp(std::move(func));
        ::swap(*this, tmp);
        return *this;
    }
    
    T (*get() const)(Args ... args) {
        return (T(*)(Args ... args))code;
    }
    
    void swap(trampoline &other) {
        ::swap(*this, other);
    }
    
    friend void ::swap<>(trampoline& a, trampoline& b);
    
    ~trampoline() {
        if (func_obj) deleter(func_obj);
        free_ptr(code);
    }
    
private:
    
    template <typename F>
    static T do_call(void* obj, Args ...args) {
        return  (*static_cast<F*>(obj))(std::forward<Args>(args)...);
    }
    
    template <typename F>
    static void my_deleter(void* func_obj) {
        delete static_cast<F*>(func_obj);
    }
    
    
    void* func_obj;
    void* code;
    void (*deleter)(void*);
};

template<typename R, typename... Args>
void swap(trampoline<R(Args...)>& lhs, trampoline<R(Args...)>& rhs) {
    std::swap(lhs.func_obj, rhs.func_obj);
    std::swap(lhs.code, rhs.code);
    std::swap(lhs.deleter, rhs.deleter);
}

#endif



