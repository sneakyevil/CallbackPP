# CallbackPP
CallbackPP is a header only C++ library that implements simple callback struct that can be declared in your project and then you can add callbacks from other threads and then simply callback them from another thread without needing to handle everything by yourself.

## Quick Example
```cpp
#include "CallbackPP.hpp"

bool __fastcall ExampleCallback(CExampleClass* p_Class, int* p_Integer)
{
    if (p_Class->m_Stop)
    {
        printf("Stopping callback: %d\n", *p_Integer);
        delete p_Integer;
        return false;
    }

    printf("Called callback: %d\n", *p_Integer);
    return true;
}

// Declared callback
CallbackPP_t g_Callback;

void Thread()
{
     for (int i = 0; 2 > i; ++i)
        g_Callback.Run();

    g_ExampleClass.m_Stop = true;
    g_Callback.Run();
}

// Somewhere in your project...
for (int i = 0; 5 > i; ++i)
    g_Callback.Add(ExampleCallback, &g_ExampleClass, new int(i));
```

### Output:
```
Called callback: 4
Called callback: 3
Called callback: 2
Called callback: 1
Called callback: 0
Called callback: 4
Called callback: 3
Called callback: 2
Called callback: 1
Called callback: 0
Stopping callback: 4
Stopping callback: 3
Stopping callback: 2
Stopping callback: 1
Stopping callback: 0
```
