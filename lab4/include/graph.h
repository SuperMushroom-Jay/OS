#ifndef __GRAPH_H__
#define __GRAPH_H__

#include"type.h"
#include<gtk/gtk.h>
#include<stdio.h>
#include<sys/types.h>

#define SYS_INFO_FONT_SIZE 30       //系统信息字体大小

/*结构体定义*************************************************/

/*函数定义*************************************************/

void showMainWindow(int argc,char **argv);

void showSysInfoPage(GtkWidget *notebook);

void showLabel(GtkWidget *vbox,const char *content);

void createMenu(GtkWidget *vbox);

void setLabelFontSize(GtkWidget *label,gint size);

gint delete_event( GtkWidget *widget, GdkEvent *event, gpointer data );

void destroy( GtkWidget *widget,gpointer data );

//有关cpu的界面函数

void showCpuPage(GtkWidget *notebook);

void showCpuInfo(GtkWidget *vbox);

void showCpuGraph(GtkWidget *vbox);

void showCpuUse(GtkWidget *vbox);

gboolean cpu_configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data);

gboolean cpu_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data);

gint updateCpuPrecent(gpointer data);

gboolean drawCpuGraph();

//有关内存的界面函数

void showMemPage(GtkWidget *notebook);

void showMemInfo(GtkWidget *vbox);

void showMemGraph(GtkWidget *vbox);

gboolean mem_configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data);

gboolean mem_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data);

boolean drawMemGraph();

gint updateMem(gpointer data);

//有关进程的界面函数

void showProcessPage(GtkWidget *notebook);

void getProcessList(GtkListStore *store);

gint updateProcessList(gpointer data);

void showSearchPage(GtkWidget *notebook);

void dealSearch(GtkWidget *widget, gpointer data );

void dealKill(GtkWidget *widget, gpointer data );

void dealStartProcess(GtkWidget *widget, gpointer data );

//状态栏函数

void showButtonbar(GtkWidget *vbox);

gint updateButtonbar(gpointer data);

#endif