#pragma once

// Divide x by d, round towards -infinity, always
template <class T> inline int floordiv(T x, T d)
{
    // Various solutions from the Internet didn't work. This one makes sense and
    // the efficiency is negligible
    if (x < 0) {
        T unaligned = x;
        x = 0;

        while (x > unaligned)
            x -= d;
    }

    return int(x / d);
}

// Modulo that loops negative numbers as well
inline int signmod(int x, int room)
{
    while (x < 0)
        x += room;

    return x %= room;
}

// Wraps a pair of two numbers representing a low and a high boundary whose
// difference must be retained
// Returns true if there was no split. Otherwise, split contains the offset from
// a where the split must be performed
inline int signmodPair(int & a, int & b, unsigned * split, int room)
{
    // Wrapping right/bottom
    if (a >= room) {
        int old = a;
        a %= room;
        b = b - old + a;
    }

    // Wrapping left/top
    while (a < 0) {
        a += room;
        b += room;
    }

    // Crossing the line... need to split
    if (split && b >= room) {
        *split = room - a;
        return false;
    }

    return true;
}
