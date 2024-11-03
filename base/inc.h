#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <sys/mman.h>
#include <pthread.h>
#include <unistd.h>

#include "../vendor/simde/arm/neon.h"

#include "base.h"
#include "memory.h"
#include "arena.h"
#include "string.h"
#include "math.h"
#include "channel.h"
#include "hasharray.h"
#include "xml.h"

#if TEST
#include "test.h"
#endif