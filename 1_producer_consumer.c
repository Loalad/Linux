#include <func.h>

typedef struct node{
    int num;
    struct node *next;
}Node;

typedef struct{
    Node *qFront;
    Node *qRear;
    int size;
}Queue;

typedef struct{
    Queue *queue;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}shareSrs;

Queue *create_queue();

void Enqueue(Queue *queue, int val);

void Dequeue(Queue *queue);

void visitQueue(Queue *queue);

void *producer(void *arg){
    shareSrs *pss = (shareSrs *)arg;

    int num = 0;
    while(1){
        pthread_mutex_lock(&pss->mutex);
        while(pss->queue->size >= 10){
            pthread_cond_wait(&pss->cond, &pss->mutex);
        }
        num = rand() % 1000;
        Enqueue(pss->queue, num);
        printf("producer, size = %2d, num   = %3d, queue =  ", pss->queue->size, num);
        visitQueue(pss->queue);
        pthread_cond_broadcast(&pss->cond);
        pthread_mutex_unlock(&pss->mutex);
        sleep(3);
    }

    pthread_exit(NULL);
}

void *consumer(void *arg){
    shareSrs *pss = (shareSrs *)arg;
    sleep(5);
    while(1){
        pthread_mutex_lock(&pss->mutex);
        while(pss->queue->size <= 0){
            pthread_cond_wait(&pss->cond, &pss->mutex);
        }
        Dequeue(pss->queue);
        printf("consumer, size = %2d, queue =  ", pss->queue->size);  
        visitQueue(pss->queue);
        pthread_cond_broadcast(&pss->cond);
        pthread_mutex_unlock(&pss->mutex);
        sleep(1);
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
    shareSrs ss;
    //初始化共享资源
    pthread_mutex_init(&ss.mutex, NULL);
    pthread_cond_init(&ss.cond, NULL);
    //创建队列
    ss.queue = create_queue();
    //向队列加商品
    int num = 0;
    for(int i = 0; i < 8; ++i){
        num = rand() % 1000;
        Enqueue(ss.queue, num);

        printf("num = %d\n", num);
    }

    visitQueue(ss.queue);

    pthread_t tid1, tid2, tid3, tid4, tid5;
    pthread_create(&tid1, NULL, producer, &ss);
    pthread_create(&tid2, NULL, producer, &ss);
    pthread_create(&tid3, NULL, producer, &ss);
    pthread_create(&tid4, NULL, consumer, &ss);
    pthread_create(&tid5, NULL, consumer, &ss);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    pthread_join(tid4, NULL);
    pthread_join(tid5, NULL);

    return 0;
}

Queue *create_queue(){
    Queue *queue = (Queue *)calloc(1, sizeof(shareSrs));
    queue->qFront = NULL;
    queue->qRear = NULL;
    queue->size = 0;
    return queue;
}

void Enqueue(Queue *queue, int num){
    Node *newnode = (Node *)calloc(1, sizeof(Node));
    newnode->num = num;
    if(queue->size == 0){
        queue->qFront = newnode;
        queue->qRear = newnode;
    }else{
        queue->qRear->next = newnode;
        queue->qRear = newnode;
    }
    ++queue->size;
}

void Dequeue(Queue *queue){
    Node *cur  = queue->qFront;
    queue->qFront = queue->qFront->next;
    --queue->size;
    free(cur);
}

void visitQueue(Queue *queue){
    Node *cur = queue->qFront;
    while(cur){
        printf("%d ", cur->num);
        cur = cur->next;
    }
    printf("\n");
}
