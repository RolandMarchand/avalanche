#pragma once

#include "src/value.h"

struct value value_negate(struct value val);
struct value value_logical_not(struct value val);
struct value value_add(struct value val1, struct value val2);
struct value value_substract(struct value val1, struct value val2);
struct value value_multiply(struct value val1, struct value val2);
struct value value_divide(struct value val1, struct value val2);
struct value value_modulo(struct value val1, struct value val2);
struct value value_greater(struct value val1, struct value val2);
struct value value_greater_or_equal(struct value val1, struct value val2);
struct value value_less(struct value val1, struct value val2);
struct value value_less_or_equal(struct value val1, struct value val2);
struct value value_equal(struct value val1, struct value val2);
struct value value_not_equal(struct value val1, struct value val2);
