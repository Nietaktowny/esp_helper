#include "unity.h"
#include "log.h"
#include "time.h"
#include "tcp_client.h"
#include "error.h"

SET_LOG_LEVEL(LOG_LEVEL_INFO);

/*HELPER FUNCTIONS*/
int get_random_value (int range) {
    time_t t;
    srand((unsigned) time(&t));
    return (rand() % range);
}


/*TESTS*/


int main(void)
{
    UNITY_BEGIN();
    return UNITY_END();
}