inline float __getFloat(LPRDATA rdPtr)
{
    int foo = CNC_GetFloatParameter(rdPtr);
    return *(float *)&foo;
}
#define intParam() CNC_GetIntParameter(rdPtr)
#define strParam() (const char *)CNC_GetStringParameter(rdPtr)
#define fltParam() __getFloat(rdPtr)
#define objParam() CNC_GetParameter(rdPtr)
#define anyParam() CNC_GetParameter(rdPtr)
