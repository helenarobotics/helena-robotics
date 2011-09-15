//
// FiniteFloatTest.c
//
#include "..\lib\MainPrelude.h"

void DoFiniteTest(float fl)
    {
    FLOAT_LONG pair;
    BOOL fFinite;
    IsFinite(fFinite, pair, fl);
    TRACE(("%s: %f", (fFinite ? "true" : "false"), fl ));
    }

task main()
    {
    TRACE(("---"));
    DoFiniteTest(1.0 / 0.0);
    DoFiniteTest(-1.0 / 0.0);
    DoFiniteTest(sqrt(-1.0));
    DoFiniteTest(-sqrt(-1.0));

    DoFiniteTest(0.0);
    DoFiniteTest(1.0);
    DoFiniteTest(-1.0);
    DoFiniteTest(PI);
    DoFiniteTest(0.0 / 0.0);
    DoFiniteTest(1.0 / 1000000000000000.0);
    DoFiniteTest(3400000000.0);

    FLOAT_LONG inp;
    inp.b0 = 0xc0;
    inp.b1 = 0;
    inp.b2 = 0;
    inp.b3 = 0;
    DoFiniteTest(inp.f);    // -2

    inp.b0 = 0x3f;
    inp.b1 = 0x80;
    DoFiniteTest(inp.f);    // 1

    inp.b0 = 0x80;
    inp.b1 = 0;
    DoFiniteTest(inp.f);    // -0

    inp.b0 = 0x7f;
    inp.b1 = 0x7f;
    inp.b2 = 0xff;
    inp.b3 = 0xff;
    DoFiniteTest(inp.f);    // 3.4e38 (max single precision)

    float fl = 1.0;
    int i = 0;
    while (i < 50)
        {
        DoFiniteTest(fl);
        fl *= -2.0;
        i++;
        }
    }
