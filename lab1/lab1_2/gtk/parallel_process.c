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


pid_t pids[3];       //用来保存三个进程的pid
int shmids_buf_s;    //用来开辟两个共享缓冲区buf_s 和buf_t
int shmids_buf_t;
int semID;      //信号灯集合id
union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* arry;
};
typedef struct {
    GtkWidget *widget;  //组件
    char *filename;     //文件名
    char *title;        //窗口标题
    int mode;           //类型
    int t_pid;        //定时器
    int filesize;       //文件大小
    int count;          //次数
    int copySize;       //复制一次的大小
}DATA;

typedef struct {
    char buf[1024];
    int count;
}FileData;

/*************声明函数***************/
int get(const char *filename,gpointer data);
int copy(gpointer data);
int put(const char *filename,gpointer data);
int initSem();
int delSem();
int P(int index);
int V(int index);
void showWindows(int argc,char **argv,gpointer data);
guint gtk_label_modified(gpointer data);
void destroy( GtkWidget *widget,gpointer data );
gint delete_event( GtkWidget *widget, GdkEvent *event, gpointer data );
guint get_start(gpointer data);
guint copy_start(gpointer data);
guint put_start(gpointer data);

int main(int argc,char **argv){
    if(argc!=3){
        printf("parm != 3\n");
        exit(-1);
    }
    DATA data;
    struct stat statbuf;
    int ret = stat(argv[1],&statbuf);
    if(ret==-1){
        exit(-1);
    }
    data.filesize=statbuf.st_size;  //返回文件大小
    printf("file size:%d\n",data.filesize);
    int i;
    //创建buf_s
    shmids_buf_s=shmget((key_t)IPC_PRIVATE,BUF_SIZE*sizeof(FileData),0666|IPC_CREAT);
    if(shmids_buf_s==-1){
        printf("buf_s share memory create failed\n");
        exit(-1);
    }
    //创建buf_t缓冲区
    shmids_buf_t=shmget((key_t)IPC_PRIVATE,BUF_SIZE*sizeof(FileData),0666|IPC_CREAT);
    if(shmids_buf_t==-1){
        printf("buf_t share memory create failed\n");
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
    pthread_t pth_id;
    data.mode=i;
    switch (i)
    {
    case 0: //get进程
        {
            data.filename=argv[1];
            printf("get filename %s\n",data.filename);
            char buf[]="get";
            data.title=buf;
            showWindows(argc,argv,&data);
            printf("get end\n");
        }break;
    case 1: //copy进程
        {
            char buf[]="copy";
            data.title=buf;
            showWindows(argc,argv,&data);
            printf("copy end\n");
        }break;
    case 2: //put进程
        {
            char buf[]="put";
            data.title=buf;
            data.filename=argv[2];
            printf("put filename %s\n",data.filename);
            showWindows(argc,argv,&data);
            printf("put end\n");
        }break;
    default://主进程
        {
            wait(NULL);
            wait(NULL);
            wait(NULL);
            shmctl(shmids_buf_s,IPC_RMID,0);
            shmctl(shmids_buf_t,IPC_RMID,0);
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
int get(const char *filename,gpointer data)
{
    DATA *p=(DATA*)data;
    FileData *filedata=shmat(shmids_buf_s,NULL,0);      //获取共享缓冲区的指针         
    int f_source=open(filename,O_RDONLY);           //只读模式打开
    if(f_source==-1){
        printf("get \"%s\" open failed\n",filename);
        return 0;
    }
    int ok=1;
    while (ok)
    {
        
        P(buf_s_empty); //申请buf_s空间
        P(buf_s_mutex); //buf_s互斥信号灯
        filedata->count=read(f_source,filedata->buf,BUF_SIZE);
        p->count++;
        p->copySize=filedata->count;
        sleep(1);
        printf("get modified\n");
        gtk_label_modified(data);

        if(filedata->count<BUF_SIZE){
            ok=0;
        }
        V(buf_s_mutex);
        V(buf_s_full);
    }
    close(f_source);
    gtk_label_set_text(GTK_LABEL(p->widget),"get ok");
    while (gtk_events_pending ()) 
    {
        gtk_main_iteration ();
    } 
    return  0;
}

/**
 * @brief 从buf_s缓冲区复制到buf_t缓冲区
 * 
 * @return int 
 */
int copy(gpointer data){
    DATA *p=(DATA*)data;
    FileData *filedata_s=shmat(shmids_buf_s,NULL,0);  //获取共享缓冲区的指针
    FileData *filedata_t=shmat(shmids_buf_t,NULL,0);  //获取共享缓冲区的指针
    char tempBuf[BUF_SIZE]; 
    int size;
    int ok=1;
    // timePid=gtk_timeout_add(1000, (GSourceFunc)gtk_label_modified, label);
    // printf("get pid = %d\n",timePid);
    while (ok)
    {
        
        P(buf_s_full); 
        P(buf_s_mutex);
        //将buf_s的内容复制到tempBuf中
        memcpy(tempBuf,filedata_s->buf,filedata_s->count);
        p->count++;
        p->copySize=filedata_s->count;
        size=filedata_s->count;
        sleep(1);
        printf("copy modified\n");
        gtk_label_modified(data);
        
        if(filedata_s->count<BUF_SIZE){
            ok=0;
        }
        V(buf_s_mutex);
        V(buf_s_empty);

        P(buf_t_empty);
        P(buf_t_mutex);
        memcpy(filedata_t->buf,tempBuf,BUF_SIZE);
        filedata_t->count=size;
        V(buf_t_mutex);
        V(buf_t_full);
    }
    gtk_label_set_text(GTK_LABEL(p->widget),"copy ok");
    while (gtk_events_pending ()) 
    {
        gtk_main_iteration ();
    } 
    return 0;
}

/**
 * @brief 从buf_t缓冲区复制到目标文件中
 * 
 * @param filename //目标文件名
 * @return int 
 */
int put(const char *filename,gpointer data)
{
    DATA *p=(DATA*)data;
    FileData *filedata=shmat(shmids_buf_t,NULL,0);     
    int f_target=open(filename,O_WRONLY|O_CREAT,0777);   //只读模式打开
    int size; 
    if(f_target==-1){
        printf("put \"%s\" open failed\n",filename);
        return 0;
    }
    int ok=1;
    while (ok)
    {
        P(buf_t_full); //申请buf_s空间
        P(buf_t_mutex); //buf_s互斥信号灯
        size=write(f_target,filedata->buf,filedata->count);
        p->count++;
        p->copySize=size;
        sleep(1);
        printf("put modified\n");
        gtk_label_modified(data);
        if(size<BUF_SIZE){
            ok=0;
        }
        V(buf_t_mutex);
        V(buf_t_empty);
    }
    close(f_target);
    gtk_label_set_text(GTK_LABEL(p->widget),"put ok");
    while (gtk_events_pending ()) 
    {
        gtk_main_iteration ();
    } 
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
        semctl(semID,buf_s_full,SETVAL,arg1)==-1||
        semctl(semID,buf_s_mutex,SETVAL,arg2)==-1||
        semctl(semID,buf_t_empty,SETVAL,arg3)==-1||
        semctl(semID,buf_t_full,SETVAL,arg4)==-1||
        semctl(semID,buf_t_mutex,SETVAL,arg5)==-1 )
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
void showWindows(int argc,char **argv,gpointer data){
    //定义gtk全局组件
    GtkWidget *window;      //定义主窗口
    GtkWidget *vbox;  		//定义一个组装盒;
    //GtkWidget *startButton;		//定义开始按钮;
    GtkWidget *label; 		//定义一个label;
    
    DATA *p=(DATA *)data;
    p->count=0;
    p->copySize=0;

    gtk_init(&argc,&argv);
    
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);//参数表示创建主窗口

    /*****窗口设计******/
    gtk_window_set_title(GTK_WINDOW(window),p->title);
    gtk_window_set_resizable (GTK_WINDOW (window), TRUE); //修改窗体的伸缩属性;
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);    //窗口居中
    gtk_container_set_border_width(GTK_CONTAINER (window), 100);//用来设定宽度;
    vbox = gtk_vbox_new(FALSE, 10);
    gtk_container_set_border_width(GTK_CONTAINER (vbox), 100);//用来设定宽度;
    gtk_container_add(GTK_CONTAINER (window), vbox);
    //使用gtk_box_pack_start函数将构件放到组装盒中;
    label = gtk_label_new("wait data coming");
    p->widget = label;
    gtk_box_pack_start(GTK_BOX (vbox), label, FALSE, FALSE, 10);
    
    //startButton = gtk_button_new_with_label("start");
    //gtk_box_pack_start(GTK_BOX (vbox), startButton, FALSE, FALSE, 10);
    
    /*****关联函数******/

    g_signal_connect (G_OBJECT (window), "delete_event",G_CALLBACK (delete_event), NULL);
    g_signal_connect (G_OBJECT (window), "destroy",G_CALLBACK (destroy), NULL);

    /*****窗口或组件显示******/
    gtk_widget_show (label);
    //gtk_widget_show(startButton);
    gtk_widget_show (vbox);
    gtk_widget_show(window);

    if(p->mode==0){
        gtk_timeout_add(1000,(GtkFunction)get_start,data);
    }else if(p->mode==1){
        gtk_timeout_add(1000,(GtkFunction)copy_start,data);
    }else if(p->mode==2){
        gtk_timeout_add(1000,(GtkFunction)put_start,data);
    }

    gtk_main();
}

/*写个label回调函数来解决文字动态显示问题*/
guint gtk_label_modified(gpointer data){
    DATA *p=(DATA *)data;
    char buf[128];
    float num=((float)(p->count*BUF_SIZE)/p->filesize)*100;
    if(num>100){
        num=100;
    }
    sprintf(buf,"次数：%d | 字节：%d | 进度：%.2lf %%",p->count,p->copySize,num);
    gtk_label_set_text(GTK_LABEL(p->widget),buf);
    while (gtk_events_pending ()) 
    {
        gtk_main_iteration ();
    } 
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

guint get_start(gpointer data){
    DATA *p=(DATA *)data;
    get(p->filename,data);
    return FALSE;
}

guint copy_start(gpointer data){
    copy(data);
    return FALSE;
}

guint put_start(gpointer data){
    DATA *p=(DATA *)data;
    put(p->filename , data);
    return FALSE;
}