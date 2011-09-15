//
// StompBug.c
//
#define TRACE(x)  writeDebugStreamLine x

typedef struct
    {
    long field0;
    long field1;
    } FOO;

int globalDummy;
FOO foo;

typedef union
    {
    void*   pv;
    long    l;
    } UNION;

UNION u;

long Field(long& l)
    {
    u.pv = l;
    return u.l;
    }

task main()
    {
    TRACE(("------"));

    foo.field0 = 77;
    foo.field1 = 88;

    long lvar;

    u.pv = &foo;
    lvar = u.l;
    TRACE(("0x%08x", lvar));
    long ibFoo = lvar;

    lvar = Field(foo.field1);
    TRACE(("0x%08x", lvar));

    long dib = lvar - ibFoo;
    TRACE(("%d", dib));

    lvar = foo.field0;
    TRACE(("%d", lvar));
    }
