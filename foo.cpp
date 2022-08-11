extern "C" int Bar();
extern "C" int Foo() {
    return Bar();
}