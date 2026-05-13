#include <core/logger.h>
#include <core/asserts.h>

int main(void){
    RQ_FATAL("A test message: %f", 3.14f);
    RQ_ERROR("A test message: %f", 3.14f);
    RQ_WARN("A test message: %f", 3.14f);
    RQ_INFO("A test message: %f", 3.14f);
    RQ_DEBUG("A test message: %f", 3.14f);
    RQ_TRACE("A test message: %f", 3.14f);

    RQ_ASSERT(1 == 0);

    return 0;
}