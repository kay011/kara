#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

pthread_mutex_t mutex;
pthread_cond_t cond;
int good = 0;
void *customer(void *argv){
    while(1){
        pthread_mutex_lock(&mutex);
        if(good == 0){
            pthread_cond_wait(&cond, &mutex);
        }
        good--;
        printf("consumer : %d\n", good);
        pthread_mutex_unlock(&mutex);
        sleep(rand()%3);
    }
    return NULL;
}

void *producer(void *argv){
    while(1){
        pthread_mutex_lock(&mutex);//获取互斥锁进行生产
		good++;
		printf("producer:%d\n",good);
		pthread_mutex_unlock(&mutex);//生产完成释放该锁
		if(good == 1)
		pthread_cond_signal(&cond);//产品==1的时候释放阻塞在产品为0的线程，通知其开始消费
		sleep(rand()%3);

    }
}
void main()
{
	pthread_t pt1,pt2;
	pthread_mutex_init(&mutex,NULL);//初始化互斥锁
	pthread_cond_init(&cond,NULL);//初始化条件变量
    pthread_create(&pt1,NULL,customer,NULL);//创建消费者线程
	pthread_create(&pt2,NULL,producer,NULL);//创建生产者线程
     
	pthread_join(pt1,NULL);//阻塞等待结束
	pthread_join(pt2,NULL);
     
	pthread_mutex_destroy(&mutex);//销毁对应的锁
	pthread_cond_destroy(&cond);//销毁条件变量
}
    
    
}


