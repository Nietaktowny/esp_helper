#include "unity.h"
#include "arena.h"
#include "log.h"


void test_if_arena_inits(void) {
    //given
    arena_t* test_arena = NULL;

    //when
    test_arena = arena_init(1, 1, "test_arena");

    //then
    TEST_ASSERT_NOT_NULL_MESSAGE(test_arena, "arena should not be null after init");
}

void test_if_arena_allocs(void) {
    //given
    arena_t* arena = NULL;
    uint64_t* data = NULL;

    //when
    arena = arena_init(sizeof(data), 1, "test_arena");
    data = arena_alloc(arena);

    //then
    TEST_ASSERT_NOT_NULL_MESSAGE(data, "returned node should not be null");
}
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_if_arena_inits);
    RUN_TEST(test_if_arena_allocs);
    return UNITY_END();
}