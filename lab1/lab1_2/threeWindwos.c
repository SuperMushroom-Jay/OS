#include <gtk/gtk.h>
#include <sys/types.h>
#include <unistd.h>

void destroy_progress( GtkWidget *widget)
{
    gtk_main_quit ();
}
void show(int argc,char *argv[],char *title )
{ 
    char id_char[50];
    GtkWidget *window;		//定义一个窗口;
    GtkWidget *vbox;  		//定义一个组装盒;
    GtkWidget *button;		//定义一个按钮;
    GtkWidget *label; 		//定义一个label;
    
    gtk_init (&argc, &argv); 	//初始化工具包并且获取命令行参数;
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL); 	  //创建一个新的窗口;
    gtk_window_set_resizable (GTK_WINDOW (window), TRUE); //修改窗体的伸缩属性;
    
    //设定窗口的位置;
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    
    //监听窗口的destroy事件;
    g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy_progress), NULL);
    
    gtk_window_set_title (GTK_WINDOW (window), title);//用来设定或更改窗口的标题;
    gtk_container_set_border_width (GTK_CONTAINER (window), 20);//用来设定宽度;

    //使用gtk_vbox_new函数建立纵向组装盒;
    //为了显示构件，必须将构件放入组装盒中,并将组装盒放在窗口内;
    vbox = gtk_vbox_new (FALSE, 10);
    gtk_container_set_border_width (GTK_CONTAINER (vbox), 100);//用来设定宽度;
    gtk_container_add (GTK_CONTAINER (window), vbox);
    gtk_widget_show (vbox);
   
   
    //使用gtk_box_pack_start函数将构件放到组装盒中;
    sprintf (id_char, "%s ,My ID:%d", title,getpid ());
    label = gtk_label_new (id_char);
    gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 10);
    gtk_widget_show (label);
    
    sprintf (id_char, "父进程ID:%d", getppid ());
    label = gtk_label_new (id_char); 
    gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 10);
    gtk_widget_show (label);

    button = gtk_button_new_with_label ("close");
    
    //信号登记函数,监听按钮的clicked事件．当窗口clicked时， gtk_widget_destroy 就会被调用.
    //而 gtk_widget_destroy 函数又调用 gtk_main_quit() 结束程序运行.
    g_signal_connect_swapped (G_OBJECT (button), "clicked", G_CALLBACK (gtk_widget_destroy), window);
    gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 10);
    GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
    gtk_widget_grab_default (button);
    //函数显示窗口中的组件
    gtk_widget_show (button);
    gtk_widget_show (window);
    //准备将窗口和所有的组件显示在屏幕上,这个函数必须在GTK程序的最后调用.
    gtk_main ();
}

int main(int argc,char * argv[])
{
   pid_t pid_1,pid_2;
   char id_char[50];
   if((pid_1=fork())==0)
   {
   	printf("Child 1# is running......\n");
   	show(argc,argv,"This is Child 1#");
   	sprintf (id_char, "123123");
   	
   	
   }
   else
   {
   	if((pid_2=fork())==0)
   	{
   		printf("Child 2# is running......\n");
   		show(argc,argv,"This is Child 2#");
   		
   	}
   	else
   	{
   		printf("Parent # is running......\n");
   		//Unique showing style for parent process
   		
   		char id_char[50];
   		char * title="This is Parent Process!";
    		GtkWidget *window;		//定义一个窗口;
    		GtkWidget *vbox;  		//定义一个组装盒;
    		GtkWidget *button;		//定义一个按钮;
    		GtkWidget *label; 		//定义一个label;
  			gtk_init (&argc, &argv); 	//初始化工具包并且获取命令行参数;
    		window = gtk_window_new (GTK_WINDOW_TOPLEVEL); 	  //创建一个新的窗口;
    		gtk_window_set_resizable (GTK_WINDOW (window), TRUE); //修改窗体的伸缩属性;
    		//设定窗口的位置;
    		gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    		//监听窗口的destroy事件;
    		g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy_progress), NULL);
    		gtk_window_set_title (GTK_WINDOW (window), title);//用来设定或更改窗口的标题;
    		gtk_container_set_border_width (GTK_CONTAINER (window), 20);//用来设定宽度;
    		
    		//使用gtk_vbox_new函数建立纵向组装盒;
    		//为了显示构件，必须将构件放入组装盒中,并将组装盒放在窗口内;
    		vbox = gtk_vbox_new (FALSE, 10);
    		gtk_container_set_border_width (GTK_CONTAINER (vbox), 100);//用来设定宽度;
    		gtk_container_add (GTK_CONTAINER (window), vbox);
    		gtk_widget_show (vbox);
   
   			//使用gtk_box_pack_start函数将构件放到组装盒中;
    		sprintf (id_char, "%s ,My ID:%d", title,getpid ());
    		label = gtk_label_new (id_char);
    		gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 10);
    		gtk_widget_show (label);
    
    		sprintf (id_char, "Child 1# 's ID:%d", pid_1);
    		label = gtk_label_new (id_char);
   			gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 10);
    		gtk_widget_show (label);
    		
    		sprintf (id_char, "Child 2# 's ID:%d", pid_2);
    		label = gtk_label_new (id_char);
   			gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 10);
    		gtk_widget_show (label);
    		
    		button = gtk_button_new_with_label ("close");
    
    		//信号登记函数,监听按钮的clicked事件．当窗口clicked时， gtk_widget_destroy 就会被调用.
    		//而 gtk_widget_destroy 函数又调用 gtk_main_quit() 结束程序运行.
    		g_signal_connect_swapped (G_OBJECT (button), "clicked", G_CALLBACK (gtk_widget_destroy), window);
    		gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 10);
    		GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
    		gtk_widget_grab_default (button);
    		//函数显示窗口中的组件
    		gtk_widget_show (button);
    		gtk_widget_show (window);
    		//准备将窗口和所有的组件显示在屏幕上,这个函数必须在GTK程序的最后调用.
    		gtk_main ();	
   	}
   }
   return 0;
}

