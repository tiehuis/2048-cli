#include "merge.h"

#define MERGE_GOAL (int)((sizeof(merge_values)/sizeof(merge_values[0]))-1)

const long merge_values[] = {
    0, 2, 4, 8, 16, 32, 64, 128, 256, 512,
    1024, 2048
};

inline long merge_value(const int v1)
{
    return v1 <= MERGE_GOAL ? merge_values[v1] : -1;
}

inline long merge_goal(void)
{
    return MERGE_GOAL;
}

inline int merge_possible(const int v1, const int v2)
{
    return v1 == v2;
}

inline int merge_result(const int v1, const int v2)
{
    return merge_possible(v1, v2) ? v1 + 1 : -1;
}
