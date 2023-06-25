# CallbackPP
CallbackPP is a header only C++ library that implements simple callback struct that can be declared in your project and then you can add callbacks from other threads and then simply callback them from another thread without needing to handle everything by yourself.

## Quick Example
```cpp
#include "CallbackPP.hpp"

bool __fastcall ExampleCallback(int* m_GenericArg, int* m_Arg)
{
    if (!m_GenericArg)
    {
        printf("Deleting Callback: %d\n", *m_Arg);
        delete m_Arg;
        return false;
    }

    printf("GenericArg: %d | m_Arg: %d\n", *m_GenericArg, *m_Arg);
    return true;
}

// Declared callback
CallbackPP_t g_Callback;

void Thread()
{
    for (int i = 0; 3 > i; ++i)
    {
        static int m_GenericValue;
        m_GenericValue = i;

        g_Callback.Run(&m_GenericValue);
    }

    g_Callback.Run();
}

// Somewhere in your project...
for (int i = 0; 4 > i; ++i)
    g_Callback.Add(ExampleCallback, new int(i));
```

### Output:
```
GenericArg: 0 | m_Arg: 0
GenericArg: 0 | m_Arg: 1
GenericArg: 0 | m_Arg: 2
GenericArg: 0 | m_Arg: 3
GenericArg: 1 | m_Arg: 0
GenericArg: 1 | m_Arg: 1
GenericArg: 1 | m_Arg: 2
GenericArg: 1 | m_Arg: 3
GenericArg: 2 | m_Arg: 0
GenericArg: 2 | m_Arg: 1
GenericArg: 2 | m_Arg: 2
GenericArg: 2 | m_Arg: 3
Deleting Callback: 0
Deleting Callback: 1
Deleting Callback: 2
Deleting Callback: 3
```
