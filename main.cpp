#include "dlfcn.h"
#include "plthook.h"
#include <iostream>

static int UseDLMOpen() {
    auto bar_handle = dlmopen(LM_ID_NEWLM, "./libbar.so", RTLD_NOW | RTLD_LOCAL);

    if (bar_handle == nullptr) {
        std::cerr << dlerror() << std::endl;
        return 1;
    }

    Lmid_t lm{};
    if (dlinfo(bar_handle, RTLD_DI_LMID, &lm) != 0) {
        std::cerr << dlerror() << std::endl;
        return 2;
    }

    auto foo_handle = dlmopen(lm, "./libfoo.so", RTLD_NOW | RTLD_LOCAL);
    if (foo_handle == nullptr) {
        std::cerr << dlerror() << std::endl;
        return 3;
    }
    void *foo = dlsym(foo_handle, "Foo");
    if (foo == nullptr) {
        std::cerr << dlerror() << std::endl;
        return 4;
    }

    std::cout << "Foo = " << " " << reinterpret_cast<int (*)()>(foo)() << std::endl;

    plthook_t *plt;
    if (plthook_open_by_handle(&plt, foo_handle) != 0) {
        std::cerr << dlerror() << std::endl;
        return 5;
    }

    {
        void *old{};
        if (plthook_replace(plt, "Bar", reinterpret_cast<void *>( +[]() -> int {
            return 10;
        }), &old) != 0) {
            std::cerr << dlerror() << std::endl;
            return 6;
        }
    }

    std::cout << "Foo(AfterHook) = " << " " << reinterpret_cast<int (*)()>(foo)() << std::endl;

    return 0;
}

static int UseDLOpen() {
    auto bar_handle = dlopen("./libbar.so", RTLD_NOW | RTLD_LOCAL);

    if (bar_handle == nullptr) {
        std::cerr << dlerror() << std::endl;
        return 1;
    }

    Lmid_t lm{};
    if (dlinfo(bar_handle, RTLD_DI_LMID, &lm) != 0) {
        std::cerr << dlerror() << std::endl;
        return 2;
    }

    auto foo_handle = dlopen("./libfoo.so", RTLD_NOW | RTLD_LOCAL);
    if (foo_handle == nullptr) {
        std::cerr << dlerror() << std::endl;
        return 3;
    }
    void *foo = dlsym(foo_handle, "Foo");
    if (foo == nullptr) {
        std::cerr << dlerror() << std::endl;
        return 4;
    }

    std::cout << reinterpret_cast<int (*)()>(foo)() << std::endl;
    return 0;
}

int main() {
    int rc = UseDLMOpen();
    if (rc != 0) {
        return rc;
    }

    rc = UseDLOpen();
    if (rc == 0) {
        std::cerr << "dlopen should be error" << std::endl;
        return -1;
    }
}