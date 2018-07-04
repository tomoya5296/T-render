#pragma once

#include <atomic>
#include <functional>
#include "common.h"

enum class ParallelSchedule {
	Static = 0x01,
	Dynamic = 0x02
};
