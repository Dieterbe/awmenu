/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 * Authors/copyright:
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version. See LICENSE file
 * 
 * Copyright (C) 2008 chantra chantra <chantra_AT_debuntu_-DOT-_org>
 * -> original entrycompletion code http://coding.debuntu.org/c-gtk-text-completion-gtkentry-gtkentrycompletion
 *
 * Dmenu writers.  See LICENCE-dmenu
 *
 * Dieter Plaetinck <dieter@plaetinck.be>
 * -> enhancements to form awmenu
 */


#include <stdio.h>
#include <gtk/gtk.h>
#include <string.h>

#define PACKAGE "Awmenu"
#define VERSION "0.1"

GtkListStore *model;
GtkTreeIter iter;

void
readstdin(void) {
    char buf[1024];
    unsigned int len = 0;
    while(fgets(buf, sizeof buf, stdin)) {
        len = strlen(buf);
        if (buf[len - 1] == '\n')
            buf[len - 1] = 0;
        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model, &iter, 0, buf, -1);
        //printf("appended %s\n",buf);
    }
}

static void
on_destroy(GtkWidget *widget, gpointer data) {
    (void) widget;
    (void) data;
    gtk_main_quit();
}

static gboolean
selected_cb (GtkEntryCompletion *widget, GtkTreeModel* model, GtkTreeIter *iter, gpointer  user_data) {  
    (void)  widget;
    (void) user_data;
    GValue value = {0, };
    gtk_tree_model_get_value(model, iter, 0, &value);
    printf("selected cb\n");
    printf("%s\n",g_value_get_string(&value));
    g_value_unset(&value);
    gtk_main_quit();
    return FALSE;
}  

static gboolean
activate_cb (GtkEntry *entry, gpointer user_data ) {
    (void) user_data;
    printf("activate cb\n");
    printf("%s\n",gtk_entry_get_text(entry));
    gtk_main_quit();
    return FALSE;
}

static gboolean
awesome_match_cb (GtkEntryCompletion* completion, const gchar* key, GtkTreeIter* iter, gpointer data) {
    GtkTreeModel* model;
    gchar* str;
    gboolean match;
    gchar* temp;
    char *saveptr;
    const gchar* part;

    model = gtk_entry_completion_get_model (completion);
    gtk_tree_model_get (model, iter, 0, &str, -1);
    printf("got %s\t",str);

// TODO:
//    part = (char *) strtok_r (str, " ", &saveptr);
//    part = (char *) strtok_r (NULL, " ", &saveptr);

    match = FALSE;
    if (G_LIKELY (str))
    {
        temp = g_utf8_casefold (str, -1);
        match = (strstr (temp, key) != NULL);
        printf("match: %s\n", match? "yes" : "no");
        g_free (temp);
        g_free (str);

    }
    return match;
}

int main(int argc, char **argv) {
    GtkWidget *window;
    GtkWidget *entry;
    GtkEntryCompletion *completion;

    gtk_init (&argc, &argv);

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width (GTK_CONTAINER (window), 20);
    gtk_window_set_title (GTK_WINDOW (window), PACKAGE " " VERSION);
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 50);

    g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (on_destroy), NULL);

    entry = gtk_entry_new ();
    completion = gtk_entry_completion_new();
    gtk_entry_completion_set_text_column(completion, 0);
    gtk_entry_set_completion(GTK_ENTRY(entry), completion);
    g_signal_connect(G_OBJECT (completion), "match-selected", G_CALLBACK (selected_cb), NULL);
    g_signal_connect(G_OBJECT (entry)     , "activate",       G_CALLBACK (activate_cb), NULL);
    gtk_entry_completion_set_match_func (completion, awesome_match_cb, NULL, NULL);
    model = gtk_list_store_new(1, G_TYPE_STRING);
    readstdin();
    gtk_entry_completion_set_model(completion, GTK_TREE_MODEL(model));
    gtk_container_add (GTK_CONTAINER (window), entry);
    gtk_widget_show_all (window);
    gtk_main ();
    return 0;
}
