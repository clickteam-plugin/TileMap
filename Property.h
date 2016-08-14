#pragma once

enum TMPTYPE {
    TMPT_INT,
    TMPT_FLOAT,
    TMPT_STRING,
};

struct Property {
    const TMPTYPE type;

    union {
        int i;
        float f;
        char* c;
    };

    Property(int src) : type(TMPT_INT), i(src) {}

    Property(float src) : type(TMPT_FLOAT), f(src) {}

    Property(char* src) : type(TMPT_STRING)
    {
        unsigned length = strlen(src) + 1;
        c = new char[length];
        memcpy(c, src, length);
    }

    Property(const Property& src)
        : type(src.type),
          i(src.i) // Will work for float as well, irrelevant for string
    {
        // Duplicate string
        if (type == TMPT_STRING) {
            unsigned length = strlen(src.c) + 1;
            c = new char[length];
            memcpy(c, src.c, length);
        }
    }

    ~Property()
    {
        if (type == TMPT_STRING)
            delete[] c;
    }
};