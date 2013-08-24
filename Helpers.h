#pragma once

// Divide x by d, round towards -infinity, always
inline int floordiv(int x, int d)
{
	// Various solutions from the Internet didn't work. This one makes sense and the efficiency is negligible
	if (x < 0)
	{
		int unaligned = x;
		x = 0;

		while (x > unaligned)
			x -= d;
	}

	return x / d;
}

// Modulo that loops negative numbers as well
inline int signmod(int x, int room)
{
	while (x < 0)
		x += room;

	return x %= room;
}