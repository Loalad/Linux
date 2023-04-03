#include <func.h>

typedef struct{
    int flag;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}shareSrs;

void *thread1(void *arg){
    shareSrs * pss = (shareSrs *)arg;
    sleep(3);
    pthread_mutex_lock(&pss->mutex);
    if(pss->flag != 1){
        pthread_cond_wait(&pss->cond, &pss->mutex);
    }
    printf("B\n");
    pss->flag = 2;
    pthread_cond_signal(&pss->cond);
    pthread_mutex_unlock(&pss->mutex);
    pthread_exit(NULL);
}

int main(){
   shareSrs ss;
   //初始化共享资源
    ss.flag = 0;
    pthread_mutex_init(&ss.mutex, NULL);
    pthread_cond_init(&ss.cond, NULL);

    pthread_t tid;
    pthread_create(&tid, NULL, thread1, &ss);

    sleep(2);

    printf("A\n");
    pthread_mutex_lock(&ss.mutex);
    ss.flag = 1;
    pthread_cond_signal(&ss.cond);
    pthread_mutex_unlock(&ss.mutex);

    pthread_mutex_lock(&ss.mutex);
    if(ss.flag != 2){
        pthread_cond_wait(&ss.cond, &ss.mutex);
    }
    printf("C\n");
    pthread_mutex_unlock(&ss.mutex);

    pthread_join(tid, NULL);
    return 0;
}
