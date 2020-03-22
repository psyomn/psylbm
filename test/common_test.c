#include "common.h"

#include <assert.h>

static int psylbm_strndup_test(void) {
        const unsigned char src[] = {1, 2, 3, 4};

        char* dst = NULL;
        dst = psylbm_strndup(src, sizeof(src));

        int sum = 0;
        // actual check
        for (size_t i = 0; i < sizeof(src); ++i) {
                sum |= src[i] ^ dst[i];
        }

        free(dst);

        assert(sum == 0);

        return 0;
}

int main(void) {
        return
                psylbm_strndup_test();
}
