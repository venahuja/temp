#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

void* thread_wait_condvar(void* arg)
{
    pthread_cond_t* cond_var = (pthread_cond_t*)arg;
    pthread_cond_wait(cond_var);
    puts("thread: conditional variable acquired");
    return arg;
}

void* thread_wait_barrier(void* arg)
{
    pthread_barrier_t* barrier = (pthread_barrier_t*)arg;
    pthread_barrier_wait(barrier);
    puts("thread: barrier reached");
    return arg;
}

struct COND_VAR {
    pthread_mutex_t* mutex;
    pthread_cond_t*  cond_var;
};

int
main()
{
    FILE* f = fopen("/home/box/main.pid", "w");
    fprintf(f, "%d", getpid());
    fclose(f);
    
    pthread_mutex_t mutex    = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t  cond_var = PTHREAD_COND_INITIALIZER;
    pthread_barrier_t barrier;
    if (pthread_barrier_init(&barrier, 0, 2) != 0) {
        perror("[-] pthread_barrier_init");
        return -1;
    }

    struct COND_VAR cond_var_thread;
    cond_var_thread.mutex = &mutex;
    cond_var_thread.cond_var = &cond_var;
    pthread_t thread[2];
    void* thread_func[] = {
        thread_wait_condvar,
        thread_wait_barrier
    };
    void* thread_arg[] = {
        &cond_var,
        &cond_var_thread
    };
    
    int i = 0;
    for (; i < sizeof(thread)/sizeof(thread[0]); ++i) {
        if (pthread_create(&thread[i], 0, thread_func[i], thread_arg[i]) != 0) {
            perror("[-] pthread_create");
            return -1;
        }
    }
    pthread_barrier_wait(&barrier);
    puts("main: barrier reached");
    puts("main: waiting till all threads will complete");
    
    for (i = 0; i < sizeof(thread)/sizeof(thread[0]); ++i) {
        void* value;
        pthread_join(thread[i], &value);
    }
    
    return 0;
}
