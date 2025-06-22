// gcc `pkg-config --cflags gtk+-3.0` -o gtk_scale gtk_scale.c `pkg-config --libs gtk+-3.0`

#include <gtk/gtk.h>
#include <stdio.h>

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    window = gtk_application_window_new(app); // creates GtkApplicationWindow, returns a GtkWidget*
    gtk_window_set_title(GTK_WINDOW (window), "Window"); // macro, checks types and does function style cast
    gtk_window_set_default_size(GTK_WINDOW (window), 200, 200);
    gtk_widget_show_all(window);
    
    GdkWindow* gdk_window = gtk_widget_get_window(window);
    GdkDisplay* gdk_display = gdk_display_get_default();

    // prints sadly always '1' on my system
    GdkMonitor* gdk_monitor = gdk_display_get_monitor_at_window(gdk_display, gdk_window);
    int scale = gdk_monitor_get_scale_factor(gdk_monitor);
    printf("scale is %d\n", scale);

    // seems to work fine
    int n = gdk_display_get_n_monitors(gdk_display);
    for (int i = 0; i < n; ++i) {
        GdkMonitor* monitor = gdk_display_get_monitor(gdk_display, i);
        int scale = gdk_monitor_get_scale_factor(monitor);
        printf("monitor %d, scale %d\n", i, scale);
    }
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run(G_APPLICATION (app), argc, argv);
    g_object_unref(app); // free memory of GtkApplication

    return status;
}