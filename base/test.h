#pragma once

#define test_run(name) test_run_(#name, &name)
internal void test_run_(char* name, void (*fn)(void));

internal void test_base(void);