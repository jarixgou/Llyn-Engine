#ifndef ASSERT__H
#define ASSERT__H

#define STATIC_ASSERT(expr) \
	static_assert(expr, "static assert failed: ", #expr);

#endif