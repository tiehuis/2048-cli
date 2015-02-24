#include "merge.h"

#define MERGE_GOAL (int)((sizeof(merge_values)/sizeof(merge_values[0]))-1)

const long merge_values[] = {
    0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144,
    233, 377, 610, 987, 1597
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
    return v1 == v2 - 1 || v2 == v1 - 1 ||
        ((v1 == 1 || v1 == 2) && (v2 == 1 || v2 == 2));
}

inline int merge_result(const int v1, const int v2)
{
    int max = v1 > v2 ? v1 : v2;
    return merge_possible(v1, v2) ? max + 1 : -1;
}
