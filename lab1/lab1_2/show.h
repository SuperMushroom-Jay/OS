#ifndef __SHOW_H__
#define __SHOW_H__
#include<gtk/gtk.h>
#include<stdio.h>

#define BUF_SIZE 64

typedef struct{
    GtkWidget *widget;      //组件
    guint t_pid;            //定时器id
    char mode;              //计算方式
    char *title;            //窗口标题
}DATA;

void showWindow(int argc,char **argv,gpointer data);
gint delete_event( GtkWidget *widget, GdkEvent *event, gpointer data );
void destroy( GtkWidget *widget,gpointer data );
guint modified_label(gpointer data);
void start_event(GtkWidget *widget,gpointer data);
void pause_event(GtkWidget *widget,gpointer data);

#endif