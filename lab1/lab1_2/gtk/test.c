 
/*
 * Progress bar
 *
 * 
 */
 
#include <gtk/gtk.h>
 
 
typedef struct {
 
    GtkWidget *progressbar;
    GtkWidget *window;
    int bProgressUp;
    int nLastPct;
 
} typProgressData;
 
typProgressData *pdata = NULL;
 
/*
 * CanWindowClose
 *
 * Function that determines that if the dialog
 * window can be closed.
 */
gint CanWindowClose (GtkWidget *widget)
{
    /* --- TRUE => cannot close --- */
    /* --- FALSE => can close --- */
    return (pdata->bProgressUp);
}
 
/*
 * UpdateProgress
 *
 * Update the progress window to reflect the state
 * of the file that is being loaded.  
 *
 * pos - how much of the file has been loaded.
 * len - length of the file
 * (pos / len) = % file has been loaded.
 */
void UpdateProgress (long pos, long len)
{
    gfloat pvalue;
    int pct;
 
    /* --- Prevent divide by zero errors --- */
    if (len > 0) {
 
        /* --- Calculate the percentage --- */
        pvalue = (gfloat) pos / (gfloat) len;
    
        pct = pvalue * 100;
 
        if (pdata->nLastPct != pct) {
 
            /* --- Update the displayed value --- */
            gtk_progress_set_percentage (GTK_PROGRESS (pdata->progressbar), 
                                         pvalue);
 
            /* --- Repaint any windows - like the progress bar --- */
            while (gtk_events_pending ()) {
                gtk_main_iteration ();
            }
            pdata->nLastPct = pct;
        }
    }
}
 
 
 
/*
 * StartProgress 
 *
 * Create a window for the progress bar
 */
void StartProgress ()
{
    GtkWidget *label;
    GtkWidget *table;
    GtkWidget *window;
    GtkAdjustment *adj;
 
    pdata = g_malloc (sizeof (typProgressData));
    pdata->nLastPct = -1;
    pdata->bProgressUp = TRUE;
    /*
     * --- Create the top level window
     */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    pdata->window = window;
    
    /* --- Hook up the destroy  --- */
    gtk_signal_connect (GTK_OBJECT (window), "delete_event",GTK_SIGNAL_FUNC (CanWindowClose), pdata);
    
    gtk_container_border_width (GTK_CONTAINER (window), 10);
    
    /* --- Create a table --- */
    table = gtk_table_new (3, 2, TRUE);
    gtk_container_add (GTK_CONTAINER (window), table);
    
    /* --- Add a label to the table --- */
    label = gtk_label_new ("Loading...");
    gtk_table_attach_defaults (GTK_TABLE (table), label, 0,2,0,1);
    gtk_widget_show (label);
    
    /* --- Add the progress bar to the table --- */
    adj = (GtkAdjustment *) gtk_adjustment_new (0, 0, 400, 0, 0, 0);
    pdata->progressbar = gtk_progress_bar_new_with_adjustment (adj);
    gtk_table_attach_defaults (GTK_TABLE (table), pdata->progressbar, 0,2,1,2);

    
    /* --- Show everything --- */
    gtk_widget_show (pdata->progressbar);
    gtk_widget_show (table);
    gtk_widget_show (window);
}
 
 
/*
 * EndProgress
 *
 * Close down the progress bar.
 */
void EndProgress ()
{
    /* --- Allow it to close --- */
    pdata->bProgressUp = FALSE;
 
    /* --- Destroy the window --- */
    gtk_widget_destroy (pdata->window);
 
    /* --- Free used memory. --- */
    g_free (pdata);
 
    pdata = NULL;
}
 
 /*
 * Application code begins here.
 */
 
 
int ptimer;
int nValue;
 
/*
 * UpdateProgressTimer 
 *
 * This is the timer callback function.   It updates the 
 * progressbar and closes it when it reaches the end.
 */
UpdateProgressTimer (gpointer data)
{
 
    /* --- Increment value --- */
    nValue += 1;
 
    /* --- Update the progress bar --- */
    UpdateProgress (nValue, 100);
 
    /* --- Close if it's at the end. --- */
    if (nValue == 100) {
        EndProgress ();
        gtk_timeout_remove (ptimer);
    }
}
 
/*
 * ButtonClicked
 *
 * Called to create the progress bar and the time when
 * the button has been clicked.
 */
gint ButtonClicked (GtkWidget *widget, gpointer data)
{
    /* --- Do some initialization --- */
    nValue = 0;
    StartProgress ();
 
    /* --- Call the timer. --- */
    ptimer = gtk_timeout_add (100, UpdateProgressTimer, data);
}
 
 
/*
 * CloseAppWindow
 *
 * Close down the application 
 */
gint CloseAppWindow ()
{
    gtk_main_quit ();
    return (FALSE);
}
 
/*
 * main
 *
 * program begins here.
 */
main (int argc, char *argv[]) 
{
    GtkWidget *window;
    GtkWidget *button;
 
    gtk_init (&argc, &argv);
 
    /* --- Create main window and give it a title --- */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "Progress bar");
 
    /* --- End app when close window detected --- */
    gtk_signal_connect (GTK_OBJECT (window), "delete_event",
                        GTK_SIGNAL_FUNC (CloseAppWindow), NULL);
 
    /* --- Create a button to bring up the progress bar --- */
    button = gtk_button_new_with_label ("Progress bar");
    gtk_widget_show (button);
 
    /* --- This is where we handle it --- */
    gtk_signal_connect (GTK_OBJECT (button), "clicked", 
                        GTK_SIGNAL_FUNC (ButtonClicked), NULL);
 
    gtk_container_add (GTK_CONTAINER (window), button);
    gtk_widget_show (window);
 
    /* --- Let GTK take over --- */
    gtk_main ();
    exit (0);
}
 