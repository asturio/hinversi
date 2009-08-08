#include <stdio.h>
#include <gtk/gtk.h>

// static void hello (GtkWidget* widget, gpointer data) {
//     g_print("Hallo Welt!\n");
// }

static gboolean delete_event(GtkWidget* widget, GdkEvent* event, gpointer data) {
    g_print("delete_event occured\n");
    return FALSE;
}

static void click(GtkWidget *widget, gpointer data) {
    g_print("Button %s clicked\n", (char *)data);
}

static void destroy(GtkWidget *widget, gpointer data) {
    g_print("destroy occured\n");
    gtk_main_quit();
}

int main (int argc, char** argv) {
    GtkWidget *window = NULL;
    GtkWidget *cell = NULL;
    GtkWidget *table = NULL;
    char *pfield = NULL;
    char field_name[8][8][3]; 
    char token[64] = "";
    
    gint line = 0;
    gint column = 0;

    gtk_init(&argc, &argv);

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK (delete_event), NULL);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK (destroy), NULL);
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);

    table = gtk_table_new(9, 9, TRUE);
    gtk_container_add(GTK_CONTAINER(window), table);

    for (line = 0; line < 9; line++) {
        for (column = 0; column < 9; column++) {
            if (column == 0) {
                if (line == 0) {
                    continue;
                }
                snprintf(token, sizeof(token), "%c", line - 1 + 'A');
                cell = gtk_label_new(token);
            } else {
                if (line == 0) {
                    snprintf(token, sizeof(token), "%d", column);
                    cell = gtk_label_new(token);
                } else {
                    pfield = field_name[line - 1][column - 1];
                    snprintf(pfield, 3, "%c%d", line + 'a' - 1, column);
                    cell = gtk_button_new_with_label(pfield);
                    g_signal_connect(G_OBJECT(cell), "clicked",
                                     G_CALLBACK(click), (gpointer) pfield);
                }
            }
            gtk_table_attach_defaults(GTK_TABLE(table), cell, 
                                      column, column + 1, line, line + 1);
            gtk_widget_show(cell);
        }
    }
    gtk_widget_show(table);


    

    
    /* GtkWidget *button; */
    /* button = gtk_button_new_with_label ("Hello World"); */
    /* g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (hello), NULL);
    g_signal_connect_swapped (G_OBJECT (button), "clicked", G_CALLBACK
	    (gtk_widget_destroy), G_OBJECT (window));*/
    /* gtk_container_add (GTK_CONTAINER (window), button); */

    /* gtk_widget_show(button); */
    gtk_widget_show(window);

    gtk_main();

    g_print("Program finished\n");

    return 0;
    
}
