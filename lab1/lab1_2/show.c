#include"show.h"
#include<gtk/gtk.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

/**
 * @brief 显示计算的窗口
 * 
 * @param argc main函数的第一个参数
 * @param argv main函数的第二个参数
 * @param data DATA 结构体
 */
void showWindow(int argc,char **argv, gpointer data){
    GtkWidget *window;      //创建一个窗口组件
    GtkWidget *startButton;  //创建开始按钮
    GtkWidget *pauseButton;  //创建暂停按钮
    GtkWidget *label;       //创建文本呢
    GtkWidget *vbox;        //垂直容器

    DATA *p=(DATA*)data;

    gtk_init (&argc, &argv);    //初始化
    /*==========================================================*/
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);  //添加一个窗口
    startButton = gtk_button_new_with_label ("start"); //创建按钮
    pauseButton = gtk_button_new_with_label ("pause"); //创建按钮
    label = gtk_label_new("wait to start");

    p->widget=label;

    /************组件外貌*************/
    //设置窗口标题
    gtk_window_set_title(window,p->title);

    //设置容器的边框大小
    gtk_container_set_border_width (GTK_CONTAINER (window), 50);    //设置窗口边框宽度
    gtk_window_set_resizable (GTK_WINDOW (window), TRUE); //修改窗体的伸缩属性;
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);    //窗口居中

    vbox = gtk_vbox_new(FALSE, 10);
    gtk_container_set_border_width (GTK_CONTAINER (startButton), 10);    //设置窗口边框宽度
    gtk_container_set_border_width (GTK_CONTAINER (pauseButton), 10);    //设置窗口边框宽度
    gtk_container_set_border_width(GTK_CONTAINER (vbox), 100);//用来设定宽度;
    
    /************信号处理*************/
    /**
     * 窗口x被点击时调用delete_event函数
     * 返回TRUE说明事件处理了，无需下一步处理，即不让delete_event信号发出
     * 返回FALSE继续正常处理，即让delete_event信号发出，关闭程序
     * */
    g_signal_connect (G_OBJECT (window), "delete_event",G_CALLBACK (delete_event), NULL);
    g_signal_connect (G_OBJECT (window), "destroy",G_CALLBACK (destroy), NULL);
    g_signal_connect (G_OBJECT (startButton), "clicked",G_CALLBACK (start_event),p);
    g_signal_connect (G_OBJECT (pauseButton), "clicked",G_CALLBACK (pause_event),p);

    /**************组件放入盒子*****************/
    /* 把组件放入容器中。*/
    gtk_box_pack_start(GTK_BOX (vbox), label, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX (vbox), startButton, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX (vbox), pauseButton, FALSE, FALSE, 10);
    gtk_container_add (GTK_CONTAINER (vbox), startButton);
    gtk_container_add (GTK_CONTAINER (vbox), pauseButton);
    gtk_container_add (GTK_CONTAINER (vbox), label);
    gtk_container_add(GTK_CONTAINER (window), vbox);

    /************组件显示*************/
    gtk_widget_show (label);
    gtk_widget_show (startButton);
    gtk_widget_show (pauseButton);
    gtk_widget_show (vbox);
    gtk_widget_show (window);   //显示窗口

    gtk_main ();    //消息循环
    /*==========================================================*/
}

/**
 * @brief 如果你的 "delete_event" 信号处理函数返回 FALSE，GTK 会发出 "destroy" 信号
 *        返回 TRUE，你不希望关闭窗口
 * 
 * @param widget 
 * @param event 
 * @param data 
 * @return gint 
 */
gint delete_event( GtkWidget *widget, GdkEvent *event, gpointer data ){
    return FALSE;
}
void destroy( GtkWidget *widget,gpointer data ){
    printf("pid:%d quit\n",getpid());
    gtk_main_quit ();
}

/**
 * @brief 修改标签的信息
 * 
 * @param data 这是label标签和定时器id的结构体
 * @return guint 返回TRUE定时器会一直循环
 */
guint modified_label(gpointer data){
    DATA *p=(DATA*)data;
    char buf[BUF_SIZE];
    srand(time(NULL));      //获取随机数种子
    int a=rand()%1000;
    //sleep(0.2);               //睡眠0.2秒
    int b=rand()%1000;
    int c;
    switch (p->mode)
    {
    case '+':
        c=a+b;
        break;
    case '-':
        c=a-b;
        break;
    case 'x':
        c=a*b;
        break;
    default:
        break;
    }
    sprintf(buf,"%d %c %d = %d",a,p->mode,b,c);
    gtk_label_set_text(GTK_LABEL(p->widget),buf);
    while (gtk_events_pending ()) 
    {
        gtk_main_iteration ();
    }
    printf("pid: %d label modified | t_pid = %d\n",getpid(),p->t_pid);
}

/**
 * @brief 开始按钮的回调函数
 * 
 * @param widget 按钮组件
 * @param data DATA结构指针
 */
void start_event(GtkWidget *widget,gpointer data){
    DATA *p=(DATA*)data;
    if(p->t_pid==0){
        printf("pid: %d start comptute | t_pid = %d\n",getpid(),p->t_pid);
        p->t_pid=gtk_timeout_add(1000,modified_label,data);
    }
    //printf("start t_pid=%u\n",p->t_pid);
}

/**
 * @brief 暂停按钮的回调函数
 * 
 * @param widget 按钮组件
 * @param data DATA结构指针
 */
void pause_event(GtkWidget *widget,gpointer data){
    DATA *p=(DATA*)data;
    printf("pid: %d pause comptute | t_pid = %d\n",getpid(),p->t_pid);
    gtk_timeout_remove(p->t_pid);
    p->t_pid=0;
    gtk_label_set_text(GTK_LABEL(p->widget),"pause");
    while (gtk_events_pending ()) 
    {
        gtk_main_iteration ();
    }
}