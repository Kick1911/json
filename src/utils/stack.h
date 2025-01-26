#ifndef _UTILS_STACK_H
#define _UTILS_STACK_H

#define STACK_INIT(type, name, size) \
    type _STACK_BASE_##name[(size)]; \
    type* _STACK_HEAD_##name = _STACK_BASE_##name; \
    type* _STACK_END_##name = _STACK_BASE_##name + (size) - 1

#define STACK_PUSH(name, item) (_STACK_HEAD_##name < _STACK_END_##name) ? !!(*_STACK_HEAD_##name++ = (item)) || 1 : 0
#define STACK_POP(name, fail_value) (_STACK_BASE_##name < _STACK_HEAD_##name) ? *--_STACK_HEAD_##name : (fail_value)

#endif
