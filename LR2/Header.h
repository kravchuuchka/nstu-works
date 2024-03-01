#pragma once
#define EXPORT extern "C" __declspec (dllexport)
EXPORT int MyFunction(char*, char*);
#pragma once
