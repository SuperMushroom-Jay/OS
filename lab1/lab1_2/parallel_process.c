#include<gtk/gtk.h>
#include<sys/types.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<sys/ipc.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//定义缓冲区大小为1024字节
#define BUF_SIZE 1024   

//信号灯宏定义
#define SEM_SIZE 6
#define buf_s_empty 0
#define buf_s_full  1
#define buf_s_mutex 2
#define buf_t_empty 3 
#define buf_t_full  4
#define buf_t_mutex 5


pid_t pids[3];  //用来保存三个进程的pid
int shmids_buf_s;  //用来开辟两个共享缓冲区buf_s 和buf_t
int shmids_buf_t;
int shmids_buf_s_count;
int shmids_buf_t_count;
int times=0;
int copySize=BUF_SIZE;
int semID;      //信号灯集合id
union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* arry;
};
guint timePid;
  
typedef struct {
    GtkWidget *widget;
    char *filename;
    char *content;
}WidgetData;
pthread_t ph_id[3];  //线程id
GtkWidget *showlabel; 		//定义一个label;


/*************声明函数***************/
int get(const char *filename);
int copy();
int put(const char *filename);
int initSem();
int delSem();
int P(int index);
int V(int index);
int showWindows(int argc,char **argv,char *title,char* content,int mode);
guint gtk_label_modified(GtkWidget *widget);
void get_start(GtkWidget *widget, gpointer data);
void copy_start(GtkWidget *label,gpointer data);
void put_start(GtkWidget *label,gpointer data);
void get_pthread();
void copy_pthread();
void put_pthread();
void destroy( GtkWidget *widget,gpointer data );
gint delete_event( GtkWidget *widget, GdkEvent *event, gpointer data );


int main(int argc,char **argv){
    if(argc!=3){
        printf("parm != 3\n");
        exit(-1);
    }
    int i;
    //创建buf_s
    shmids_buf_s=shmget((key_t)IPC_PRIVATE,BUF_SIZE*sizeof(char),0666|IPC_CREAT);
    if(shmids_buf_s==-1){
        printf("buf_s share memory create failed\n");
        exit(-1);
    }
    //创建buf_t缓冲区
    shmids_buf_t=shmget((key_t)IPC_PRIVATE,BUF_SIZE*sizeof(char),0666|IPC_CREAT);
    if(shmids_buf_t==-1){
        printf("buf_t share memory create failed\n");
        exit(-1);
    }
    shmids_buf_t_count=shmget((key_t)IPC_PRIVATE,BUF_SIZE*sizeof(int),0666|IPC_CREAT);
    if(shmids_buf_t_count==-1){
        printf("buf_t_count share memory create failed\n");
        exit(-1);
    }
    shmids_buf_s_count=shmget((key_t)IPC_PRIVATE,BUF_SIZE*sizeof(int),0666|IPC_CREAT);
    if(shmids_buf_s_count==-1){
        printf("buf_s_count share memory create failed\n");
        exit(-1);
    }
    /**
     * @brief //创建信号灯集 
     * 
     * buf_s_empty 
     * buf_s_full
     * buf_s_mutex
     * buf_t_empty 
     * buf_t_full
     * buf_t_mutex
     */
    semID=semget((key_t)IPC_PRIVATE,SEM_SIZE,0666|IPC_CREAT);
    if(semID==-1){
        printf("sem set create failed\n");
        exit(-1);
    }
    //初始化信号灯
    if(initSem()==0){
        exit(-1);
    }
    int pid;
    for( i=0;i<3;i++){
        pid=fork(); //创建一个进程
        if(pid==0){
            break;
        }else{
            pids[i]=pid;
        }
    }
    switch (i)
    {
    case 0: //get进程
        {
            //get(argv[1]);
            showWindows(argc,argv,"get","get",0);
            if(pthread_join(ph_id[i],NULL)==0){
                printf("get pthread killed\n");
            }else{
                printf("get pthread kill failed\n");
            }
            
        }break;
    case 1: //copy进程
        {
            //copy();
            showWindows(argc,argv,"copy","copy",1);
            if(pthread_join(ph_id[i],NULL)==0){
                printf("copy pthread killed\n");
            }else{
                printf("copy pthread kill failed\n");
            }
        }break;
    case 2: //put进程
        {
            //put(\argv[2]);
            showWindows(argc,argv,"put","put",2);
            if(pthread_join(ph_id[i],NULL)==0){
                printf("put pthread killed\n");
            }else{
                printf("put pthread kill failed\n");
            }
        }break;
    default://主进程
        {
            wait(NULL);
            wait(NULL);
            wait(NULL);
            shmctl(shmids_buf_s,IPC_RMID,0);
            shmctl(shmids_buf_t,IPC_RMID,0);
            shmctl(shmids_buf_s_count,IPC_RMID,0);
            shmctl(shmids_buf_t_count,IPC_RMID,0);
            delSem();
            printf("main end\n");
        }break;
    }
    return 0;
}

/**
 * @brief  ：简介，简单介绍函数作用
 * 
 * @param  ：介绍函数参数
 * @return：函数返回类型说明 
 * @exception NSException 可能抛出的异常.
 * @author zhangsan：  作者
 * @date 2011-07-27 22:30:00 ：时间
 * @version 1.0 ：版本
 * @property ：属性介绍
 */



/**
 * @brief 从文件中复制消息进缓冲区buf_s中
 * 
 * @param filename //要复制的文件名
 * @return int 
 */
int get(const char *filename)
{
    int *ret=shmat(shmids_buf_s_count,NULL,0);  //获取共享缓冲区的指针  
    char *buf=shmat(shmids_buf_s,NULL,0);     
    int f_source=open(filename,O_RDONLY);   //只读模式打开
    if(f_source==-1){
        printf("\"%s\" open failed\n",filename);
        return 0;
    }
    // timePid=gtk_timeout_add(1000, (GSourceFunc)gtk_label_modified, label);
    // printf("get pid = %d\n",timePid);
    while (1)
    {
        //sleep(1);
        P(buf_s_empty); //申请buf_s空间
        P(buf_s_mutex); //buf_s互斥信号灯
        *ret=read(f_source,buf,BUF_SIZE);
        copySize=*ret;
        times++;
        ///printf("get = %d  count=%d\n",*ret,times++);
        if(*ret==0){
            break;
        }
        V(buf_s_mutex);
        V(buf_s_full);
    }
    close(f_source);
    printf("get end\n");
    return  0;
}

/**
 * @brief 从buf_s缓冲区复制到buf_t缓冲区
 * 
 * @return int 
 */
int copy(){
    int *ret_s=shmat(shmids_buf_s_count,NULL,0);  //获取共享缓冲区的指针  
    char *buf_s=shmat(shmids_buf_s,NULL,0);   
    int *ret_t=shmat(shmids_buf_t_count,NULL,0);  //获取共享缓冲区的指针  
    char *buf_t=shmat(shmids_buf_t,NULL,0); 
    char tempBuf[BUF_SIZE]; 
    int size;
    int ok=1;
    // timePid=gtk_timeout_add(1000, (GSourceFunc)gtk_label_modified, label);
    // printf("get pid = %d\n",timePid);
    while (ok)
    {
        //sleep(1);
        P(buf_s_full); 
        P(buf_s_mutex);
        //将buf_s的内容复制到tempBuf中
        memcpy(tempBuf,buf_s,BUF_SIZE);
        times++;
        //printf("copy ret_s= %d  count=%d\n",*ret_s,times++);
        copySize=*ret_s;
        size=*ret_s;
        if(*ret_s<BUF_SIZE){
            ok=0;
        }
        V(buf_s_mutex);
        V(buf_s_empty);

        P(buf_t_empty);
        P(buf_t_mutex);
        //将tempBuf中的内容
        memcpy(buf_t,tempBuf,BUF_SIZE);
        *ret_t=size;
        //printf("copy ret_t= %d\n",*ret_t);
        V(buf_t_mutex);
        V(buf_t_full);
    }
    printf("copy end\n");
    return 0;
}

/**
 * @brief 从buf_t缓冲区复制到目标文件中
 * 
 * @param filename //目标文件名
 * @return int 
 */
int put(const char *filename)
{
    int size;
    int *ret=shmat(shmids_buf_t_count,NULL,0);  //获取共享缓冲区的指针  
    char *buf=shmat(shmids_buf_t,NULL,0);     
    int f_target=open(filename,O_WRONLY|O_CREAT,0777);   //只读模式打开
    if(f_target==-1){
        printf("\"%s\" open failed\n",filename);
        return 0;
    }
    int ok=1;
    // timePid=gtk_timeout_add(1000, (GSourceFunc)gtk_label_modified, label);
    // printf("get pid = %d\n",timePid);
    while (ok)
    {
        //sleep(1);
        P(buf_t_full); //申请buf_s空间
        P(buf_t_mutex); //buf_s互斥信号灯
        size=write(f_target,buf,*ret);
        copySize=*ret;
        times++;
        //printf("put = %d  count=%d\n",*ret,times++);
        if(size<BUF_SIZE){
            ok=0;
        }
        V(buf_t_mutex);
        V(buf_t_empty);
    }
    close(f_target);
    printf("put end\n");
    return  0;
}

/**
 * @brief 初始化信号灯
 * 
 * @return int 
 */
int initSem(){
	union semun arg0;
    union semun arg1;
    union semun arg2;
    union semun arg3;
    union semun arg4;
    union semun arg5;
    arg0.val=1;
    arg1.val=0;
    arg2.val=1;
    arg3.val=1;
    arg4.val=0;
    arg5.val=1;
    if( semctl(semID,buf_s_empty,SETVAL,arg0)==-1||
        semctl(semID,buf_s_full,SETVAL,arg0)==-1||
        semctl(semID,buf_s_mutex,SETVAL,arg0)==-1||
        semctl(semID,buf_t_empty,SETVAL,arg0)==-1||
        semctl(semID,buf_t_full,SETVAL,arg0)==-1||
        semctl(semID,buf_t_mutex,SETVAL,arg0)==-1 )
    {
        printf("init sem set failed\n");
        return 0;
    }
    return 1;
}
/**
 * @brief 删除信号灯
 * 
 * @return int 
 */
int delSem(){
    if (semctl(semID, 0, IPC_RMID) == -1) {
        printf("del segment failed\n");
        return 0;
    }
    return 1;
}
/**
 * @brief P操作，申请资源
 * 
 * @param index 信号灯索引
 * @return int 
 */
int P(int index){
	struct sembuf arg;
    arg.sem_num = index;
    arg.sem_op = -1;
    arg.sem_flg = 0;
    if (semop(semID, &arg, 1) == -1) {
        printf("P failed %d\n", index);
        return 0;
    }
    return 1;
}
/**
 * @brief V操作，释放资源
 * 
 * @param index 
 * @return int 
 */
int V(int index){
	struct sembuf arg;
    arg.sem_num = index;
    arg.sem_op = 1;
    arg.sem_flg = 0;
    if (semop(semID, &arg, 1) == -1) {
        printf("V failed %d\n", index);
        return 0;
    }
    return 1;
}

/**
 * @brief 显示对应进程的窗口
 * 
 * @param title    窗口标题
 * @param content  内容
 * @param count    拷贝次数
 * @param size     每次拷贝文件的字节
 * @return int 
 */
int showWindows(int argc,char **argv,char *title,char* content,int mode){
    //定义gtk全局组件
    GtkWidget *window;      //定义主窗口
    GtkWidget *vbox;  		//定义一个组装盒;
    GtkWidget *startButton;		//定义开始按钮;
    GtkWidget *label; 		//定义一个label;
    
    WidgetData *data = (WidgetData*)malloc(sizeof(WidgetData));
    data->widget=label;

    gtk_init(&argc,&argv);
    
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);//参数表示创建主窗口
    /*****窗口设计******/
    gtk_window_set_title(GTK_WINDOW(window),title);
    gtk_window_set_resizable (GTK_WINDOW (window), TRUE); //修改窗体的伸缩属性;
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);    //窗口居中
    gtk_container_set_border_width(GTK_CONTAINER (window), 100);//用来设定宽度;
    //使用gtk_vbox_new函数建立纵向组装盒;
    //为了显示构件，必须将构件放入组装盒中,并将组装盒放在窗口内;
    vbox = gtk_vbox_new(FALSE, 10);
    gtk_container_set_border_width(GTK_CONTAINER (vbox), 100);//用来设定宽度;
    gtk_container_add(GTK_CONTAINER (window), vbox);
    //使用gtk_box_pack_start函数将构件放到组装盒中;
    label = gtk_label_new("NULL");
    showlabel=label;
    gtk_box_pack_start(GTK_BOX (vbox), label, FALSE, FALSE, 10);
    
    startButton=gtk_button_new_with_label("start");
    gtk_box_pack_start(GTK_BOX (vbox), startButton, FALSE, FALSE, 10);
    
    /*****关联函数******/
    //g_signal_connect(G_OBJECT(window),"destory",G_CALLBACK(gtk_main_quit),NULL);//空间和消息函数创建关联
    //timePid=gtk_timeout_add(1000, (GSourceFunc)gtk_label_modified, label);
    
    if(mode==0){
        data->filename=argv[1];
        printf("====================================get\n");
        pthread_create(&ph_id[mode],NULL,(void *)get_pthread,NULL);
        g_signal_connect(G_OBJECT(startButton),"clicked",GTK_SIGNAL_FUNC(get_start),data);//空间和消息函数创建关联
    }else if(mode==1){
        printf("====================================copy\n");
        pthread_create(&ph_id[mode],NULL,(void *)copy_pthread,NULL);
        // timePid=gtk_timeout_add(1000, (GSourceFunc)gtk_label_modified, label);
        // g_print("======================****** timePid=%d     +++++++++++\n",timePid);
        g_signal_connect(G_OBJECT(startButton),"clicked",GTK_SIGNAL_FUNC(copy_start),data);//空间和消息函数创建关联
    }else if(mode==2){
        data->filename=argv[2];
        printf("====================================put\n");
        pthread_create(&ph_id[mode],NULL,(void *)put_pthread,NULL);
        g_signal_connect(G_OBJECT(startButton),"clicked",GTK_SIGNAL_FUNC(put_start),data);//空间和消息函数创建关联
    }

    g_signal_connect (G_OBJECT (window), "delete_event",G_CALLBACK (delete_event), NULL);
    g_signal_connect (G_OBJECT (window), "destroy",G_CALLBACK (destroy), NULL);
    //g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(gtk_label_modified),content);//空间和消息函数创建关联

    /*****窗口或组件显示******/
    gtk_widget_show (label);
    gtk_widget_show(startButton);
    gtk_widget_show (vbox);
    gtk_widget_show(window);
    
    printf("!!!!!!!!!!!!!!!!!! ==> ph_id[%d]=%d\n",mode,ph_id[mode]);

    gtk_main();

    printf("end windows\n");
    free(data);
    return 0;
}

/*写个label回调函数来解决文字动态显示问题*/
guint gtk_label_modified(GtkWidget *widget){
     char buf[128];
     if(copySize==BUF_SIZE){
        sprintf(buf, "count=%d , size=%d",times,copySize);
        printf("-------------continue label modified ==>buf: %s  timePid=%d ph_id=%d\n",buf,timePid);
        gtk_label_set_text(GTK_LABEL(widget),buf);
        while (gtk_events_pending ()) 
        {
            gtk_main_iteration ();
        }
    }else{
        printf("***************end label modified\n");
        sprintf(buf, "count=%d , size=%d",times,copySize);
        gtk_label_set_text(GTK_LABEL(widget),buf);
        while (gtk_events_pending ()) 
        {
            gtk_main_iteration ();
        }
        gtk_timeout_remove(timePid);
    }
    return TRUE;
}

void get_start(GtkWidget *widget, gpointer data){
     WidgetData * wd=(WidgetData *)data;
     printf("get start\n");
     printf("get from %s\n",wd->filename);
     get(wd->filename);
 }
void copy_start(GtkWidget *label,gpointer data){
     printf("copy start\n");
     copy();
 }
void put_start(GtkWidget *label,gpointer data){
     WidgetData * wd=(WidgetData *)data;
     printf("put start\n");
     printf("put to %s\n",wd->filename);
     put(wd->filename);
 }

void get_pthread(){
    timePid=gtk_timeout_add(1000, (GSourceFunc)gtk_label_modified, showlabel);
    g_print("======================******get timePid=%d \n",timePid);
}

void copy_pthread(){
    timePid=gtk_timeout_add(1000, (GSourceFunc)gtk_label_modified, showlabel);
    g_print("======================******copy timePid=%d \n",timePid);
}

void put_pthread(){
    timePid=gtk_timeout_add(1000, (GSourceFunc)gtk_label_modified, showlabel);
    g_print("======================******put timePid=%d \n",timePid);
}
/**
 * @brief 通过调用 gtk_main_quit() 来退出程序。这个函数告诉 GTK 当控制权返回给它时
 * 就从 gtk_main 退出。
 * 
 * @param widget 窗口组件
 * @param data   参数
 */
void destroy( GtkWidget *widget,gpointer data )
{
    g_print("quit\n");
    gtk_main_quit ();
}
/**
 * @brief 调用这个函数结束窗口
 * 
 * @param widget 组件
 * @param event 事件
 * @param data 参数
 * @return gint 返回值
 */
gint delete_event( GtkWidget *widget, GdkEvent *event, gpointer data )
{
    /* 如果你的 "delete_event" 信号处理函数返回 FALSE，GTK 会发出 "destroy" 信号。
    * 返回 TRUE，你不希望关闭窗口。
    * 当你想弹出“你确定要退出吗?”对话框时它很有用。*/
    g_print ("delete event occurred\n");
    /* 改 TRUE 为 FALSE 程序会关闭。*/
    return FALSE;
}