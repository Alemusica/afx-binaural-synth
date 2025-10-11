
#pragma once
#include <cstdio>
#define AFX_LOG(fmt, ...) std::fprintf(stderr, "[AFX] " fmt "\n", ##__VA_ARGS__)
