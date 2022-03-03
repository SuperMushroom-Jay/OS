//自定义头文件
#include"graph.h"
#include"type.h"
#include"stringtool.h"
#include"sysinfo.h"
#include"cpu.h"
#include"process.h"
#include"meminfo.h"
#include"cmd.h"

//标准库头文件
#include<gtk/gtk.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include<time.h>

#define CPU_DRAW_LENGTH 200     //cpu绘图长度
#define MEM_DRAW_LENGTH 200     //mem绘图长度
#define PRO_COLUMN 8            //进程的列长度

/*声明全局变量***************************************************************************/

GtkWidget *window;                  //主窗口

boolean mutex = 1;                  //oldCpuStatInfo资源互斥锁
//有关cpu的全局变量
CPU_STAT_INFO *oldCpuStatInfo;      //保存旧的cpu数据
CPU_STAT_INFO *newCpuStatInfo;      //保存新的cpu数据
float cpuPrecent;                   //cpu利用率
// unsigned long  totalCpu;            //保存总的cpu时间
GtkWidget *cpuGraphArea;            //绘图区域
GdkPixmap *pixmapCpu = NULL;        //Pixmap绘图
gint cpuGraphArray[CPU_DRAW_LENGTH];     //保存绘图数据

//有关mem的全局变量
MEM_INFO *memInfo;                  //保存数据
float  memPrecent;                  //mem利用率
GtkWidget *memGraphArea;            //内存绘图区域
GdkPixmap *pixmapMem = NULL;        //Pixmap绘图
gint memGraphArray[MEM_DRAW_LENGTH];    //保存绘图数据

//有关进程的全局变量
GtkListStore *process_store;        //列表组件
GtkWidget *ptree_view;              //预览树
PROCESS_INFO *processArray[PID_MAX_NUMBER];    //进程数组
char pid[10];
char comm[30];
char task_stats[10];
char ppid[10];
char pgid[10];
char priority[10];
char memory[10];
char cpu[10];
GtkWidget* searchEntry;         //搜索输入
GtkWidget * searchLabel;        //搜索文本
GtkWidget * reslutLabel;        //结果文本呢 
GtkWidget * killLabel;        //结果文本呢 
GtkWidget * startProcessLabel;        //结果文本呢      c  
pid_t serachPid = 0;                //保存所搜的pid
boolean serachOK = FALSE;               //是否搜索到




/**
 * @brief 显示主界面
 * 
 * @param argc mian函数的argc
 * @param argv main函数的argv
 */
void showMainWindow(int argc,char **argv){
    // initProcessInfoArray(processArray);     //初始化进程数组
    // getAllProcessInfo(processArray);        //获取全部进程的运行状态
    
    oldCpuStatInfo =getCpuStatInfo("/proc/stat");

    /*组件定义**********************************************************/
    //GtkWidget *window;      //主窗口
    GtkWidget *vbox;        //纵向容器
    GtkWidget *notebook;    //多标签页
    printf("init window\n");
    /*初始化***********************************************************/
    gtk_init(&argc, &argv);
    /*组件创建与形状定义************************************************/
    //主窗口设置
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);               //创建窗口
    gtk_window_set_title(GTK_WINDOW(window), "System Monitor"); //设置标题
    //gtk_window_set_opacity(GTK_WINDOW(window), 0.50);           //设置透明度函数
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200); //设置窗口大小
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER); //设置窗口初始位置
    //纵向容器设置
    vbox = gtk_vbox_new(FALSE, 10);                             //创建组装盒
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 2);     //设置组装盒边距
    
    //创建多标签页
    notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos((GtkNotebook *)notebook, GTK_POS_TOP); //设置标签页为顶部位置
	gtk_notebook_set_show_border((GtkNotebook *)notebook, FALSE);   //设置多标签页的边框


    /*组件添加到对应容器************************************************/
    gtk_container_add(GTK_CONTAINER(window),vbox);              //将vbox放入window中
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0); //将notebook放入vbox
    
    
    /*添加子页面********************************************************/
    printf("showSysInfoPage\n");
    showSysInfoPage(notebook);  //显示系统信息页面
    printf("showCpuPage\n");
    showCpuPage(notebook);      //显示cpu界面
    printf("showMemPage\n");
    showMemPage(notebook);      //显示内存界面
    printf("showProcessPage\n");
    showProcessPage(notebook);  //显示进程界面
    printf("showSearchPage\n");
    showSearchPage(notebook);   //显示搜搜界面
    printf("showButtonbaar\n");
    showButtonbar(vbox);        //显示状态栏

    /*信号关联************************************************************/
    g_signal_connect (G_OBJECT (window), "delete_event",G_CALLBACK (delete_event), NULL);
    g_signal_connect (G_OBJECT (window), "destroy",G_CALLBACK (destroy), NULL);


    /*显示所有组件********************************************************/
    gtk_widget_show_all(window);
    /*信号循环************************************************************/
    gtk_main();
}

/**
 * @brief 显示系统信息界面
 * @param notebook 标签页
 */
void showSysInfoPage(GtkWidget *notebook){
    SYS_INFO *sysinfo;      //系统信息
    sysinfo=getSysInfo();   //获取系统信息
    /*组件定义及外貌设置**********************************************************/
    GtkWidget *vbox;        //纵向容器
    vbox = gtk_vbox_new(TRUE, 10);  //创建纵向容器
    gtk_container_border_width(GTK_CONTAINER(vbox), 5);    //设置边距
    //将vbox添加到notebook容器中
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, gtk_label_new("System infomation"));
    /*显示系统信息**********************************************************/
    char buf[128];
    sprintf(buf,"os type    : %s",sysinfo->osType);
    showLabel(vbox,buf);
    sprintf(buf,"host name  : %s",sysinfo->hostName);
    showLabel(vbox,buf);
    sprintf(buf,"boot time  : %s",sysinfo->bootTime);
    showLabel(vbox,buf);
    sprintf(buf,"os release : %s",sysinfo->osRelease);
    showLabel(vbox,buf);
    // showLabel(vbox,sysinfo->hostName);
    // showLabel(vbox,sysinfo->bootTime);
    // showLabel(vbox,sysinfo->osRelease);
    /*已经显示完毕，销毁指针申请的空间****************************************/
    destorySysInfo(&sysinfo);
}

/**
 * @brief 将标签添加进vbox里
 * 
 * @param vbox GtkWidget* 组件
 * @param content 显示内容
 */
void showLabel(GtkWidget *vbox,const char *content){
    GtkWidget *label;
    label = gtk_label_new(content);                  //创建label
    //setLabelFontSize(label,20);
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0.9); //小部件对齐
    gtk_container_add(GTK_CONTAINER(vbox), label);   //将label放入vbox中
}

/**
 * @brief 设置label字体大小
 * 
 * @param label 
 * @param size 
 */
void setLabelFontSize(GtkWidget *label,gint size){
	PangoFontDescription *font;  
	font = pango_font_description_from_string("Sans");          //"Sans"字体名   
	pango_font_description_set_size(font, size*PANGO_SCALE);//设置字体大小   
 
	//设置label的字体
	gtk_widget_modify_font(GTK_WIDGET(label), font);
	pango_font_description_free(font);
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
/**
 * @brief 
 * 
 * @param widget 
 * @param data 
 */
void destroy( GtkWidget *widget,gpointer data ){
    printf("pid:%d quit\n",getpid());
    gtk_main_quit ();
}

/**
 * @brief 显示cpu页面
 * 
 * @param notebook 
 */
void showCpuPage(GtkWidget *notebook){
    /*组件定义*********************************************************/
    GtkWidget *table;
    GtkWidget *cpuInfoVbox;
    GtkWidget *cpuGraphVbox;
    GtkWidget *cpuUseVbox;
    GtkWidget *cpuInfoFrame;
    GtkWidget *cpuGraphFrame;
    GtkWidget *cpuUseFrame;
    // GtkWidget *cpuGraphArea;
    /*组件创建及外貌设置************************************************/
    table = gtk_table_new(10, 10, TRUE);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), table, gtk_label_new("CPU")); 
    //frame布局
    cpuInfoFrame = gtk_frame_new("cpu infomation");
    cpuGraphFrame = gtk_frame_new("cpu use history");
    cpuUseFrame = gtk_frame_new("cpu usage");

    gtk_table_attach_defaults(GTK_TABLE(table), cpuInfoFrame, 0, 5, 0, 10);  //数字含义左右上下
    gtk_table_attach_defaults(GTK_TABLE(table), cpuGraphFrame, 5, 10, 0, 10);
    // gtk_table_attach_defaults(GTK_TABLE(table), cpuUseFrame, 0, 10, 6, 10);
    //区域cpuInfo
    cpuInfoVbox = gtk_vbox_new(TRUE, 5);
    gtk_container_border_width(GTK_CONTAINER(cpuInfoVbox), 5);
    gtk_container_add(GTK_CONTAINER(cpuInfoFrame), cpuInfoVbox);
    //区域cpuGraph
    cpuGraphVbox = gtk_vbox_new(TRUE, 0);
    gtk_container_border_width(GTK_CONTAINER(cpuGraphVbox), 20);
    gtk_container_add(GTK_CONTAINER(cpuGraphFrame), cpuGraphVbox);
    // //区域cpuUse
    // cpuUseVbox = gtk_vbox_new(TRUE, 5);
    // gtk_container_border_width(GTK_CONTAINER(cpuUseVbox), 5);
    // gtk_container_add(GTK_CONTAINER(cpuUseFrame), cpuUseVbox);
    /*组件显示*******************************************************/
    showCpuInfo(cpuInfoVbox);
    showCpuGraph(cpuGraphVbox);
    // showCpuUse(cpuUseVbox);
    
    /*信号关联*******************************************************/
    g_signal_connect(cpuGraphArea, "expose_event",G_CALLBACK(cpu_expose_event), NULL);
    g_signal_connect(cpuGraphArea, "configure_event",G_CALLBACK(cpu_configure_event), NULL);
}

/**
 * @brief 显示cpu基本信息
 * 
 * @param vbox 
 */
void showCpuInfo(GtkWidget *vbox){
    CPU_INFO *info;
    info=getCpuInfo("/proc/cpuinfo");
    showLabel(vbox,info->modelName);
    showLabel(vbox,info->cacheSize);
    showLabel(vbox,info->cpuMHz);
    showLabel(vbox,info->vendorID);
    destoryCpuInfo(&info);
}

/**
 * @brief 显示Cpu历史使用图像
 * 
 * @param vbox 
 */
void showCpuGraph(GtkWidget *vbox){
    cpuGraphArea = gtk_drawing_area_new();  //cpuGraphArea是全局组件变量，创建绘图区
    gtk_widget_set_app_paintable(cpuGraphArea, TRUE);   //设置可以画图
    gtk_drawing_area_size(GTK_DRAWING_AREA(cpuGraphArea), 400, 300);    //设置画布大小
    gtk_container_add(GTK_CONTAINER(vbox),cpuGraphArea);    //将画布添加到画图容器中
    gtk_widget_show(cpuGraphArea);   //感觉可以省略
    int i;
    //初始化存储数组
    for(i=0;i<CPU_DRAW_LENGTH;i++){
        cpuGraphArray[i] = CPU_DRAW_LENGTH;
    }
    //printf("time out fun\n");
    g_timeout_add(1000,updateCpuPrecent,NULL);    
}


/**
 * @brief 属性改变事件产生的信号,创建窗口时调用该函数，创建一块新的PIXMAP
 * 
 * @param widget 
 * @param event 
 * @param data 
 * @return gboolean 
 */
gboolean cpu_configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data){
    if(pixmapCpu){
        g_object_unref(pixmapCpu); //减少一次引用次数
    }
    //printf("cpu_configure_event\n");
    pixmapCpu = gdk_pixmap_new(widget->window, widget->allocation.width, 
                            widget->allocation.height, -1);
    gdk_draw_rectangle(pixmapCpu,widget->style->white_gc,TRUE,0, 0,
                        widget->allocation.width,widget->allocation.height);
    return TRUE;
}

/**
 * @brief 实现区域的重绘。每次当drawing_area的窗口的某块区域
 * 被挡住或者最小化后再重现都会自动调用该函数，对失效区域重画
 * 
 * @param widget 
 * @param event 
 * @param data 
 * @return gboolean 
 */
gboolean cpu_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data){
   //printf("cpu_expose_event\n");
    gdk_draw_drawable(widget->window,
                      widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
                      pixmapCpu,
                      event->area.x, event->area.y,
                      event->area.x, event->area.y,
                      event->area.width, event->area.height);
    return TRUE;
}

/**
 * @brief 更新cpu频率信息
 * 
 * @param data 
 * @return gint 
 */
gint updateCpuPrecent(gpointer data){
    // if(mutex==0){
    //     return TRUE;
    // }
    // mutex=0;                            //上锁
    newCpuStatInfo = getCpuStatInfo("/proc/stat");
    cpuPrecent = getCpuPrecent(newCpuStatInfo,oldCpuStatInfo);
    //updateAllProcessInfo(processArray,newCpuStatInfo,oldCpuStatInfo);
    //getProcessList(process_store); 

    destoryCpuStatInfo(&oldCpuStatInfo);
    oldCpuStatInfo = newCpuStatInfo;
    drawCpuGraph();                     //画图
    //mutex==1;                           //解锁
    return TRUE;
}

/**
 * @brief 画CPU使用率图
 * 
 * @return gboolean
 */
gboolean drawCpuGraph(){
    int width;
    int height;
    float stepWidth;
    float stepHeight;
    if(pixmapCpu==NULL){ 
        return FALSE;
    }
    //设置风格
    GdkGC *gc = gdk_gc_new(GDK_DRAWABLE(pixmapCpu));
    gdk_draw_rectangle(GDK_DRAWABLE(pixmapCpu), window->style->dark_gc[4], TRUE, 0, 0,
                       cpuGraphArea->allocation.width,
                       cpuGraphArea->allocation.height);
    width = cpuGraphArea->allocation.width;
    height = cpuGraphArea->allocation.height;
    int i;
    cpuGraphArray[CPU_DRAW_LENGTH-1]=CPU_DRAW_LENGTH-cpuPrecent*CPU_DRAW_LENGTH/100 -1 ;
    for(i=0;i<CPU_DRAW_LENGTH-1;i++){
        cpuGraphArray[i]=cpuGraphArray[i+1];
    }
    //更新数据
    //printf("cpuGraphArray[CPU_DRAW_LENGTH-1] = %d\n",cpuGraphArray[CPU_DRAW_LENGTH-1]);
    //获取比例数据
    stepWidth=(float)width/CPU_DRAW_LENGTH;
    stepHeight=(float)height/CPU_DRAW_LENGTH;
    // printf("stepWidth = %.2f\n",stepWidth);
    // printf("stepHeight = %.2f\n",stepHeight);
    GdkColor color;         //绘图颜色
    //设置绘图颜色
    gdk_color_parse("#995599", &color);
    //设置前景色
    gdk_gc_set_rgb_fg_color(gc, &color);
    //函数点连线

    for(i=CPU_DRAW_LENGTH-1;i>=1;i--){
        gdk_draw_line(pixmapCpu, gc, i * stepWidth, cpuGraphArray[i] * stepHeight,
                      (i - 1) * stepWidth, cpuGraphArray[i - 1] * stepHeight);
    }
    gtk_widget_queue_draw(cpuGraphArea);    //触发信号，重绘cpu图像
    return TRUE;
}


/**
 * @brief 显示cpu使用信息
 * 
 * @param vbox 
 */
void showCpuUse(GtkWidget *vbox){

}


/**
 * @brief 显示内存页面
 * 
 * @param notebook 
 */
void showMemPage(GtkWidget *notebook){
    /*组件定义****************************************************************/
    GtkWidget *table;
    GtkWidget *memInfoVbox;
    GtkWidget *memGraphVbox;
    GtkWidget *memInfoFrame;
    GtkWidget *memGraphFrame;

    /*组件创建及外貌设置****************************************************************/
    table = gtk_table_new(10, 10, TRUE);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), table, gtk_label_new("Memory"));
    
    memInfoFrame = gtk_frame_new("memory information");
    memGraphFrame = gtk_frame_new("memory history");
    gtk_table_attach_defaults(GTK_TABLE(table), memGraphFrame, 5, 10, 0, 10);
    gtk_table_attach_defaults(GTK_TABLE(table), memInfoFrame, 0, 5, 0, 10);
    
    memInfoVbox = gtk_vbox_new(TRUE, 5);
    gtk_container_border_width(GTK_CONTAINER(memInfoVbox), 2);
    gtk_container_add(GTK_CONTAINER(memInfoFrame), memInfoVbox);

    memGraphVbox = gtk_vbox_new(TRUE, 0);
    gtk_container_border_width(GTK_CONTAINER(memGraphVbox), 20);
    gtk_container_add(GTK_CONTAINER(memGraphFrame), memGraphVbox);

    /*显示信息****************************************************************/
    memInfo=getMemInfo("/proc/meminfo");    //获取第一次的内存详情数据
    showMemInfo(memInfoVbox);
    showMemGraph(memGraphVbox);

    /*信号关联****************************************************************/
    g_signal_connect(memGraphArea, "expose_event",G_CALLBACK(mem_expose_event), NULL);
    g_signal_connect(memGraphArea, "configure_event",G_CALLBACK(mem_configure_event), NULL);

}

/**
 * @brief 
 * 
 * @param vbox 
 */
void showMemInfo(GtkWidget *vbox){
    char buf[48];
    sprintf(buf,"MemTotal    : %ld KB", memInfo->MemTotal);
    showLabel(vbox,buf);
    sprintf(buf,"MemAvailable: %ld KB", memInfo->MemAvailable);
    showLabel(vbox,buf);
    sprintf(buf,"MemFree     : %ld KB", memInfo->MemFree);
    showLabel(vbox,buf);
    sprintf(buf,"Cached      : %ld KB", memInfo->Cached);
    showLabel(vbox,buf);
    sprintf(buf,"Buffers     : %ld KB", memInfo->Buffers);
    showLabel(vbox,buf);
}


/**
 * @brief 属性改变事件产生的信号,创建窗口时调用该函数，创建一块新的PIXMAP
 * 
 * @param widget 
 * @param event 
 * @param data 
 * @return gboolean 
 */
gboolean mem_configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data){
    if(pixmapMem){
        g_object_unref(pixmapMem); //减少一次引用次数
    }
   // printf("mem_configure_event\n");
    pixmapMem = gdk_pixmap_new(widget->window, widget->allocation.width, 
                            widget->allocation.height, -1);
    gdk_draw_rectangle(pixmapMem,widget->style->white_gc,TRUE,0, 0,
                        widget->allocation.width,widget->allocation.height);
    return TRUE;
}

/**
 * @brief 实现区域的重绘。每次当drawing_area的窗口的某块区域
 * 被挡住或者最小化后再重现都会自动调用该函数，对失效区域重画
 * 
 * @param widget 
 * @param event 
 * @param data 
 * @return gboolean 
 */
gboolean mem_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data){
    //printf("mem_expose_event\n");
    gdk_draw_drawable(widget->window,
                      widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
                      pixmapMem,
                      event->area.x, event->area.y,
                      event->area.x, event->area.y,
                      event->area.width, event->area.height);
    return TRUE;
}


/**
 * @brief 
 * 
 * @param vbox 
 */
void showMemGraph(GtkWidget *vbox){
    memGraphArea = gtk_drawing_area_new();  //cpuGraphArea是全局组件变量，创建绘图区
    gtk_widget_set_app_paintable(memGraphArea, TRUE);   //设置可以画图
    gtk_drawing_area_size(GTK_DRAWING_AREA(memGraphArea), 400, 300);    //设置画布大小
    gtk_container_add(GTK_CONTAINER(vbox),memGraphArea);    //将画布添加到画图容器中
    gtk_widget_show(memGraphArea);   //感觉可以省略
    int i;
    //初始化存储数组
    for(i=0;i<MEM_DRAW_LENGTH;i++){
        memGraphArray[i] = MEM_DRAW_LENGTH;
    }
    //printf("time out fun\n");
    g_timeout_add(1000,updateMem,NULL);  
}

/**
 * @brief 
 * 
 * @return boolean 
 */
boolean drawMemGraph(){
    int width;
    int height;
    float stepWidth;
    float stepHeight;
    if(pixmapMem==NULL){ 
        return FALSE;
    }
    //设置风格
    GdkGC *gc = gdk_gc_new(GDK_DRAWABLE(pixmapMem));
    gdk_draw_rectangle(GDK_DRAWABLE(pixmapMem), window->style->dark_gc[4], TRUE, 0, 0,
                       memGraphArea->allocation.width,
                       memGraphArea->allocation.height);
    width = memGraphArea->allocation.width;
    height = memGraphArea->allocation.height;
    int i;
    memGraphArray[MEM_DRAW_LENGTH-1]=MEM_DRAW_LENGTH-memPrecent*MEM_DRAW_LENGTH/100 -1 ;
    for(i=0;i<MEM_DRAW_LENGTH-1;i++){
        memGraphArray[i]=memGraphArray[i+1];
    }
    //更新数据
    //printf("cpuGraphArray[CPU_DRAW_LENGTH-1] = %d\n",cpuGraphArray[CPU_DRAW_LENGTH-1]);
    //获取比例数据
    stepWidth=(float)width/MEM_DRAW_LENGTH;
    stepHeight=(float)height/MEM_DRAW_LENGTH;
    // printf("stepWidth = %.2f\n",stepWidth);
    // printf("stepHeight = %.2f\n",stepHeight);
    GdkColor color;         //绘图颜色
    //设置绘图颜色
    gdk_color_parse("#995599", &color);
    //设置前景色
    gdk_gc_set_rgb_fg_color(gc, &color);
    //函数点连线

    for(i=CPU_DRAW_LENGTH-1;i>=1;i--){
        gdk_draw_line(pixmapMem, gc, i * stepWidth, memGraphArray[i] * stepHeight,
                      (i - 1) * stepWidth, memGraphArray[i - 1] * stepHeight);
    }
    gtk_widget_queue_draw(memGraphArea);    //触发信号，重绘cpu图像
    return TRUE;
}

/**
 * @brief 
 * 
 * @param data 
 * @return gint 
 */
gint updateMem(gpointer data){
    MEM_INFO *info=getMemInfo("/proc/meminfo");
    memPrecent=getMemPrecent(info);
    destoryMemInfo(&info);
    drawMemGraph();
    return TRUE;
}

/**
 * @brief 
 * 
 * @param notebook 
 */
void showProcessPage(GtkWidget *notebook){
    /*组件定义**********************************************************/
    GtkWidget *vbox;
    GtkWidget *scrolled_window;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    const char *columnName[PRO_COLUMN] = {"pid", "comm", "stat", "ppid", "pgid", "priority","memory","cpu"};
    const int columnwidth[PRO_COLUMN] = {100, 300,100 ,150, 100, 150, 100,100};
     // 创建盒装容器并添加
    vbox = gtk_vbox_new(FALSE, 3);
    gtk_container_border_width(GTK_CONTAINER(vbox), 5);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, gtk_label_new("process"));
    scrolled_window = gtk_scrolled_window_new(NULL, NULL); //添加滚动窗口控件
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_widget_show(scrolled_window);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    //添加list控件
    process_store = gtk_list_store_new(PRO_COLUMN,
                                       G_TYPE_STRING,
                                       G_TYPE_STRING,
                                       G_TYPE_STRING,
                                       G_TYPE_STRING,
                                       G_TYPE_STRING,
                                       G_TYPE_STRING,
                                       G_TYPE_STRING,
                                       G_TYPE_STRING);
    //添加tree view控件,显示进程信息树
    ptree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(process_store));
    g_object_unref(G_OBJECT(process_store));                       //减少引用次数
    gtk_container_add(GTK_CONTAINER(scrolled_window), ptree_view); //Tree in scrolled_window
    // 悬浮菜单
    gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(ptree_view), FALSE);
    // 右键弹出菜单连接
    //g_signal_connect(G_OBJECT(ptree_view), "button-release-event", G_CALLBACK(PopMenu), NULL);
    // 显示列                                   
    int i;
    for (i = 0; i < PRO_COLUMN; i++)
    {
        renderer = gtk_cell_renderer_text_new(); //添加一个cell_renderer_text用于显示文字
        column = gtk_tree_view_column_new_with_attributes(columnName[i],renderer, "text", i, NULL); //新建一列
        gtk_tree_view_append_column(GTK_TREE_VIEW(ptree_view), column);               //将该列加到树中
        // 改变宽度
        gtk_tree_view_column_set_min_width(column, columnwidth[i]);
    }
    //initProcessInfoArray(processArray);
    // printf("list\n");
    getProcessList(process_store);
    // printf("list end\n");
    g_timeout_add(5000, updateProcessList, NULL);
}

/**
 * @brief Get the Process List object
 * 
 * @param store 
 */
void getProcessList(GtkListStore *store){
    // gtk_list_store_clear(process_store);    //清除列表
    //printf("list in\n");
    PROCESS_INFO node;
    PROCESS_INFO *p;
    DIR *dir;
    struct dirent *entry;
    char dir_buf[64];
    GtkTreeIter iter;
    dir = opendir("/proc");
    while ((entry = readdir(dir)) != NULL){
         if ((entry->d_name[0] >= '0') && (entry->d_name[0] <= '9')){
            getProcessInfo_new(atoi(entry->d_name),&node);
            p=&node;
            //printf("i=%d\n",i);
            //printf("get i=%d    pid=%d\n",i,p->pid);
            gtk_list_store_append(store, &iter); //增加到列表
            //printf("add %d iter\n",i);
            sprintf(pid,"%d",p->pid);
            sprintf(comm,"%s",p->comm);
            sprintf(task_stats,"%s",p->task_state);
            //printf("c stat %s\n",p->task_state);
            sprintf(ppid,"%d",p->ppid);
            sprintf(pgid,"%d",p->pgid);
            sprintf(priority,"%d",p->priority);
            sprintf(memory,"%.2f",p->memery);
            sprintf(cpu,"%.2f",p->cpu);
            gtk_list_store_set(store,&iter,
                               0, pid,
                               1, comm,
                               2, task_stats,
                               3, ppid,
                               4, pgid,
                               5, priority,
                               6, memory,
                               7, cpu,-1);
         }

     }
}

/**
 * @brief 
 * 
 * @param data 
 * @return gint 
 */
gint updateProcessList(gpointer data){
    //printf("list update\n");
    gtk_list_store_clear(process_store);    //清除列表
    getProcessList(process_store);
    return TRUE;
}

/**
 * @brief 显示搜索界面
 * 
 * @param notebook 
 */
void showSearchPage(GtkWidget *notebook){
    GtkWidget *vbox = gtk_vbox_new(TRUE, 5);
    GtkWidget *serachButton=gtk_button_new_with_label("search");
    GtkWidget *killButton = gtk_button_new_with_label("kill");
    GtkWidget *startProcessButton = gtk_button_new_with_label("start process");
    searchLabel = gtk_label_new("please input pid or comm to search | name to start process");
    killLabel = gtk_label_new("kill null");
    searchEntry = gtk_entry_new(); 
    gtk_box_pack_start(GTK_BOX(vbox),searchLabel,FALSE,FALSE,5);  
    gtk_box_pack_start(GTK_BOX(vbox),searchEntry,FALSE,FALSE,5); 
    gtk_box_pack_start(GTK_BOX(vbox),serachButton,FALSE,FALSE,5);
    reslutLabel = gtk_label_new("show answer");
    gtk_box_pack_start(GTK_BOX(vbox),reslutLabel,FALSE,FALSE,5);  
    gtk_box_pack_start(GTK_BOX(vbox),killButton,FALSE,FALSE,5);
    gtk_box_pack_start(GTK_BOX(vbox),killLabel,FALSE,FALSE,5);
    gtk_box_pack_start(GTK_BOX(vbox),startProcessButton,FALSE,FALSE,5);
    startProcessLabel= gtk_label_new("start process null");
    gtk_box_pack_start(GTK_BOX(vbox),startProcessLabel,FALSE,FALSE,5);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, gtk_label_new("search"));
    
    //信号关联函数
    g_signal_connect(G_OBJECT(serachButton),"clicked",G_CALLBACK(dealSearch),NULL);
    g_signal_connect(G_OBJECT(killButton),"clicked",G_CALLBACK(dealKill),NULL);
    g_signal_connect(G_OBJECT(startProcessButton),"clicked",G_CALLBACK(dealStartProcess),NULL);

}

/**
 * @brief 
 * 
 * @param data 
 * @return gint 
 */
void dealSearch(GtkWidget *widget ,gpointer data){
    const gchar * content = gtk_entry_get_text(GTK_ENTRY(searchEntry)); 
    printf("entry: %s\n",content);
    int pid = atoi(content);
    printf("pid= %d\n",pid);
    PROCESS_INFO *node=getProcessInfo(pid);
    char buf[200];
    if(node!=NULL){
        sprintf(buf,"pid:%d comm:%s stat:%s mem:%.2f cpu:%.2f",node->pid,node->comm,node->task_state,node->memery,node->cpu);
        gtk_label_set_text(GTK_LABEL(reslutLabel),buf);
        serachPid=pid;
        serachOK=TRUE;
    }else{
        gtk_label_set_text(GTK_LABEL(reslutLabel),"serach null");
        serachOK=FALSE;
    }
    gtk_entry_set_text(GTK_ENTRY(searchEntry),"");
}

/**
 * @brief 处理杀死
 * 
 * @param widget 
 * @param data 
 */
void dealKill(GtkWidget *widget, gpointer data ){
    char buf[64];
    if(serachOK==TRUE){
        sprintf(buf,"kill -9 %d",serachPid);
        system(buf);
        sprintf(buf,"kill pid %d successs",pid);
        gtk_label_set_text(GTK_LABEL(killLabel),buf);
        serachOK=FALSE;
    }else{
        sprintf(buf,"please search process pid and kill it");
        gtk_label_set_text(GTK_LABEL(killLabel),buf);
    }
}

/**
 * @brief 
 * 
 * @param widget 
 * @param data 
 */
void dealStartProcess(GtkWidget *widget, gpointer data ){
    const gchar * content = gtk_entry_get_text(GTK_ENTRY(searchEntry)); 
    printf("entry: %s\n",content);
    char buf[200];
    int ok = system(content);
    if(ok==0){
        sprintf(buf,"%s start success",content);
        gtk_label_set_text(GTK_LABEL(startProcessLabel),buf);
    }else{
        sprintf(buf,"%s start failed",content);
        gtk_label_set_text(GTK_LABEL(startProcessLabel),buf);
    }
    gtk_entry_set_text(GTK_ENTRY(searchEntry),"");
}


/**
 * @brief 显示状态栏
 * 
 */
void showButtonbar(GtkWidget *vbox){
    GtkWidget *label;

    label = gtk_label_new("");
    gtk_box_pack_end(GTK_BOX(vbox), label, FALSE, FALSE, 1);
    //gtk_misc_set_alignment(GTK_MISC(label), 0, 0.9); //小部件对齐
    // 延时1000ms 循环调用函数
    g_timeout_add(1000, updateButtonbar, (gpointer)label);
}

gint updateButtonbar(gpointer data){
    float cpu = cpuPrecent;
    float mem = memPrecent;
    SYS_INFO *sysinfo;      //系统信息
    sysinfo=getSysInfo();   //获取系统信息
    time_t loaclTime;
    time(&loaclTime);
    char buf[128];
    sprintf(buf,"cpu:%.2f %% mem:%.2f %% time:%s uptime:%s",cpu,mem,ctime(&loaclTime),sysinfo->upTime);
    gtk_label_set_text(GTK_LABEL(data),buf);
    destorySysInfo(&sysinfo);
}
