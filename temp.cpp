#include <pthread.h>
#include <stdio.h>

void* thread_wait_mutex(void* arg)
{
    pthread_mutex_t* mutex = (pthread_mutex_t*)arg;
    pthread_mutex_lock(mutex);
    puts("thread: mutex acquired");
    pthread_mutex_unlock(mutex);
    return arg;
}

void* thread_wait_spin(void* arg)
{
    pthread_spinlock_t* spin_lock = (pthread_spinlock_t*)arg;
    pthread_spin_lock(spin_lock);
    puts("thread: spinlock acquired");
    pthread_spin_unlock(spin_lock);
    return arg;
}

void* thread_wait_rw_read(void* arg)
{
    pthread_rwlock_t* rw_lock = (pthread_rwlock_t*)arg;
    pthread_rwlock_rdlock(rw_lock);
    puts("thread: rwlock acquired for read");
    pthread_rwlock_unlock(rw_lock);
    return arg;
}

void* thread_wait_rw_write(void* arg)
{
    pthread_rwlock_t* rw_lock = (pthread_rwlock_t*)arg;
    pthread_rwlock_wrlock(rw_lock);
    puts("thread: rwlock acquired for write");
    pthread_rwlock_unlock(rw_lock);
    return arg;
}

int
main()
{
    FILE* f = fopen("/home/box/main.pid");
    fprintf("%d", getpid());
    fclose(f);
    
    pthread_mutex_t  mutex   = PTHREAD_MUTEX_INITIALIZER;
    pthread_rwlock_t rw_lock = PTHREAD_RWLOCK_INITIALIZER;
    pthread_spinlock_t spin_lock;

    if (pthread_spin_init(&spin_lock) != 0) {
        perror("[-] pthread_spin_init");
        return -1;
    }
    if (pthread_spin_lock(&spin_lock) != 0) {
        perror("[-] pthread_spin_lock");
        return -1;
    }
    puts("main: spinlock acquired");
    
    if (pthread_mutex_lock(&mutex) != 0) {
        perror("[-] pthread_mutex_lock");
        return -1;
    }
    puts("main: mutex acquired");
    
    if (pthread_rwlock_wrlock(&rw_lock) != 0) {
        perror("[-] pthread_rwlock_wrlock");
        return -1;
    }
    puts("main: rwlock acquired");
    
    pthread_t thread[4];
    void* thread_func[] = {
        thread_wait_mutex,
        thread_wait_spin,
        thread_wait_rw_read,
        thread_wait_rw_write
    };
    void* thread_arg[] = {
        &mutex,
        &spin_lock,
        &rw_lock,
        &rw_lock
    };
    
    int i = 0;
    for (; i < sizeof(thread)/sizeof(thread[0]); ++i) {
        if (pthread_create(thread[i], 0, thread_func[i], thread_arg[i]) != 0) {
            perror("[-] pthread_create");
            return -1;
        }
    }
    puts("main: waiting till all threads will complete");
    
    for (i = 0; i < sizeof(thread)/sizeof(thread[0]); ++i) {
        void* value;
        pthread_join(thread[i], &value);
    }
    
    return 0;
}
