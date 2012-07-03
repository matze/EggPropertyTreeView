/* Copyright (C) 2011, 2012 Matthias Vogelgesang <matthias.vogelgesang@kit.edu>
   (Karlsruhe Institute of Technology)

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by the
   Free Software Foundation; either version 2.1 of the License, or (at your
   option) any later version.

   This library is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
   details.

   You should have received a copy of the GNU Lesser General Public License along
   with this library; if not, write to the Free Software Foundation, Inc., 51
   Franklin St, Fifth Floor, Boston, MA 02110, USA */

#include <gtk/gtk.h>
#include "egg-property-tree-view.h"

static gboolean
on_delete_event (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    return FALSE;
}

int
main (int argc, char* argv[])
{
    GtkWidget *window; 
    GtkWidget *scrolled_window;
    GtkWidget *tree_view;

    gtk_init (&argc, &argv);

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    g_signal_connect (window, "delete-event", G_CALLBACK (on_delete_event), NULL);
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    tree_view = egg_property_tree_view_new (G_OBJECT (window));

    gtk_container_add (GTK_CONTAINER (scrolled_window), tree_view);
    gtk_container_add (GTK_CONTAINER (window), scrolled_window);
    gtk_widget_show_all (window);
    gtk_main ();

    return 0;
}
