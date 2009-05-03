/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 * Authors/copyright:
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 * 
 * Copyright (C) 2008 chantra chantra <chantra_AT_debuntu_-DOT-_org>
 * -> original code http://coding.debuntu.org/c-gtk-text-completion-gtkentry-gtkentrycompletion
 *
 * Dieter Plaetinck <dieter@plaetinck.be>
 * -> enhancements to form awmenu
 */



#include <stdio.h>
#include <gtk/gtk.h>

#define PACKAGE "Awmenu"
#define VERSION "0.1"

typedef struct item {
  char *string;
} item;

item item_list[] = {
  { "Foobar"},
  { "Super Framework"},
  { "Gtk Framework"},
  { "Non sense"},
  { "GtkEntryCompletion"},
  { "Gtk rules" },
  { "some more words"},
  { NULL}
};

static void
on_destroy(GtkWidget *widget, gpointer data) {
    (void) widget;
    (void) data;
    gtk_main_quit();
}

static gboolean
selected_cb (GtkEntryCompletion *widget, GtkTreeModel* model, GtkTreeIter *iter, gpointer  user_data) {  
    GValue value = {0, };
    gtk_tree_model_get_value(model, iter, 1, &value);
    printf("selected cb\n");
    printf("%s\n",g_value_get_string(&value));
    g_value_unset(&value);
    gtk_main_quit();
    return FALSE;
}  

static gboolean
activate_cb (GtkEntry *entry, gpointer user_data ) {
    printf("activate cb\n");
    printf("%s\n",gtk_entry_get_text(entry));
    gtk_main_quit();
    return FALSE;
}

int main(int argc, char **argv) {
    item *l;
    GtkWidget *window;
    GtkWidget *entry;
    GtkEntryCompletion *completion;
    GtkListStore *model;
    GtkTreeIter iter;

    gtk_init (&argc, &argv);

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width (GTK_CONTAINER (window), 20);
    gtk_window_set_title (GTK_WINDOW (window), PACKAGE " " VERSION);
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 50);

    g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (on_destroy), l);

    entry = gtk_entry_new ();
    completion = gtk_entry_completion_new();
    /* set completion on the first column, the
    * 2 others won't be used for now */
    gtk_entry_completion_set_text_column(completion, 1);
    gtk_entry_set_completion(GTK_ENTRY(entry), completion);
    g_signal_connect(G_OBJECT (completion), "match-selected", G_CALLBACK (selected_cb), NULL);
    g_signal_connect(G_OBJECT (entry)     , "activate",      G_CALLBACK (activate_cb), NULL);
    model = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    for(l=item_list; l && l->string; l++) {
      gtk_list_store_append(model, &iter);
      gtk_list_store_set(model, &iter, 1, l->string, -1);
    }
    gtk_entry_completion_set_model(completion, GTK_TREE_MODEL(model));
    gtk_container_add (GTK_CONTAINER (window), entry);
    gtk_widget_show_all (window);
    gtk_main ();
    return 0;
}
