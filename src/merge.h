#ifndef MERGE_H
#define MERGE_H

/* When defining a new set of rules, remember that the grid values (and the
 * therefore the arguments v1, v2) are index values and should be treated as
 * such */

/* This should return the value of a given index. 0 should default to 0 (empty) */
long merge_value(const int v1);

/* This should return the goal index value */
long merge_goal(void);

/* Return if a merge is possible between two indices */
int merge_possible(const int v1, const int v2);

/* The result of a merge of two values. If the two parameters are not mergeable,
 * then a -1 error code is the suggested return value */
int merge_result(const int v1, const int v2);

#endif
