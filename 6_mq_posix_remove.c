#include <stdio.h>
#include <mqueue.h>

#define POSIX_MQ_NAME   "/posixmq"

int main() {
    if (mq_unlink(POSIX_MQ_NAME) == -1) {
        perror("mq_unlink");
        return 1;
    }

    printf("Message queue %s deleted successfully.\n", POSIX_MQ_NAME);
    return 0;
}
