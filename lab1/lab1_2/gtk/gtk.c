#include<gtk/gtk.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

//编译链接加上这个  `pkg-config --cflags --libs gtk+-2.0`

#define WINDWOS_NUMBERS 3

typedef struct {
    GtkWidget *widget;
}WidgetData;

int speed=0;
int randNumebr=1;

guint timePid=0;

/**
 * @brief 
 * 
 * @param widget    组件
 * @param data      参数
 */
void hello(GtkWidget *widget, gpointer data)
{
    g_print("Hello world!\n");
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

guint modified_label(gpointer data){
    char buf[64];
    //g_print("modified label content\n");
    
    float tempNumber=((float)speed/randNumebr)*100;
    //printf("tempNumber=%f\n",tempNumber);
    if(tempNumber<101){
        if(tempNumber>=100){
            tempNumber=100;
        }
        sprintf(buf,"speed of progress = %.2f%%",tempNumber);
        speed++;
        gtk_label_set_text(GTK_LABEL(data),buf);
        while (gtk_events_pending ()) 
        {
            //printf("message circle\n");
            gtk_main_iteration ();
        }
    }else{
        gtk_timeout_remove(timePid);
    }
    return TRUE;
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

void show_windows(int argc,char **argv){
    GtkWidget *window;  //创建一个组件
    // GtkWidget *button;  //创建按钮
    // GtkWidget *testButton;  //创建按钮
    GtkWidget *label;   //创建文本呢
    GtkWidget *vbox;    //垂直容器

    gtk_init (&argc, &argv);    //初始化
    /*==========================================================*/
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);  //添加一个窗口
    // button = gtk_button_new_with_label ("Hello World"); //创建按钮
    // testButton = gtk_button_new_with_label ("test button"); //创建按钮
    label = gtk_label_new("change");

    /************组件外貌*************/
    //设置容器的边框大小
    gtk_container_set_border_width (GTK_CONTAINER (window), 50);    //设置窗口边框宽度
    gtk_window_set_resizable (GTK_WINDOW (window), TRUE); //修改窗体的伸缩属性;
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);    //窗口居中

    vbox = gtk_vbox_new(FALSE, 10);
    // gtk_container_set_border_width (GTK_CONTAINER (button), 10);    //设置窗口边框宽度
    // gtk_container_set_border_width (GTK_CONTAINER (testButton), 10);    //设置窗口边框宽度
    gtk_container_set_border_width(GTK_CONTAINER (vbox), 100);//用来设定宽度;
    
    /************信号处理*************/
    /**
     * 窗口x被点击时调用delete_event函数
     * 返回TRUE说明事件处理了，无需下一步处理，即不让delete_event信号发出
     * 返回FALSE继续正常处理，即让delete_event信号发出，关闭程序
     * */
    g_signal_connect (G_OBJECT (window), "delete_event",G_CALLBACK (delete_event), NULL);
    g_signal_connect (G_OBJECT (window), "destroy",G_CALLBACK (destroy), NULL);

    /**************组件放入盒子*****************/
    /* 把组件放入容器中。*/
    gtk_box_pack_start(GTK_BOX (vbox), label, FALSE, FALSE, 10);
    // gtk_box_pack_start(GTK_BOX (vbox), button, FALSE, FALSE, 10);
    // gtk_box_pack_start(GTK_BOX (vbox), testButton, FALSE, FALSE, 10);
    //gtk_container_add (GTK_CONTAINER (vbox), button);
    //gtk_container_add (GTK_CONTAINER (vbox), label);
    gtk_container_add(GTK_CONTAINER (window), vbox);

    /************组件显示*************/
    gtk_widget_show (label);
    // gtk_widget_show (button);
    // gtk_widget_show (testButton);
    gtk_widget_show (vbox);
    gtk_widget_show (window);   //显示窗口

    timePid = gtk_timeout_add(100, modified_label, label);

    gtk_main ();    //消息循环
    /*==========================================================*/
    return 0;
}


pid_t pids[WINDWOS_NUMBERS];

int main(int argc,char **argv){
    int i;
    pid_t pid;
    for(i=0;i<WINDWOS_NUMBERS;i++){
        pid=fork();
        if(pid==0){
            break;
        }else{
            pids[i]=pid;
        }
    }
    switch (i)
    {
    case 0:
        srand(time(NULL));
        randNumebr=abs(rand())%100+100;
        printf("1 randNumebr=%d\n",randNumebr);     
        show_windows(argc,argv);
        break;
    case 1:
        sleep(0.5);
        srand(time(NULL));
        randNumebr=abs(rand())%100+100;
        printf("2 randNumebr=%d\n",randNumebr);
        show_windows(argc,argv);
        break;
    case 2:
        sleep(1);
        srand(time(NULL));
        randNumebr=abs(rand())%100+100; 
        printf("3 randNumebr=%d\n",randNumebr);
        show_windows(argc,argv);
        break;
    default:
        wait(pids[0],NULL);
        wait(pids[1],NULL);
        wait(pids[2],NULL);
        break;
    }
    return 0;
}

