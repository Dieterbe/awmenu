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
 * -> stdin reading :)
 *
 * Copyright (C) 2008 Christian Dywan <christian@twotoasts.de>, Dale Whittaker <dayul@users.sf.net>
 * -> layout function taken from Midori. 
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
    //printf("selected cb\n");
    printf("%s\n",g_value_get_string(&value));
    g_value_unset(&value);
    gtk_main_quit();
    return FALSE;
}  

static gboolean
activate_cb (GtkEntry *entry, gpointer user_data ) {
    (void) user_data;
    //printf("activate cb\n");
    printf("%s\n",gtk_entry_get_text(entry));
    gtk_main_quit();
    return FALSE;
}

static void
midori_location_entry_render_text_cb (GtkCellLayout* layout, GtkCellRenderer* renderer, GtkTreeModel* model, GtkTreeIter* iter, gpointer data)
{
    gchar* str;
    gchar* desc;
    GtkWidget* entry;
    gchar* search;
    gchar* start;
    gchar* ssearch;   
    gchar* temp;
    gchar** parts;  
    size_t len;
    gtk_tree_model_get (model, iter, 0, &str, -1);
    search = NULL;
    if (G_LIKELY (data)) {
        entry = gtk_entry_completion_get_entry (GTK_ENTRY_COMPLETION (data));
        search = g_utf8_strdown (gtk_entry_get_text (GTK_ENTRY (entry)), -1);
        len = 0;
    }
    if (G_LIKELY (data && str)) {
        temp = g_ascii_strdown (str, -1);
        if (start = strstr (temp, search)) {
            len = strlen (search);
            ssearch = g_malloc0 (len + 1);
            strncpy (ssearch, str + (start - temp), len);
            if (ssearch && *ssearch && (parts = g_strsplit (str, ssearch, 2))) {
                if (parts[0] && parts[1]) {
                    desc = g_markup_printf_escaped ("%s<b>%s</b>%s", parts[0], ssearch, parts[1]);
                    g_strfreev (parts);
                }
            }
            g_free (ssearch);
        }
        g_free (temp);
    }

    g_object_set (renderer, "markup", desc,
       "ellipsize-set", TRUE, "ellipsize", PANGO_ELLIPSIZE_END, NULL);

    g_free (str);
    g_free (search);  
    g_free (desc); 
}

static gboolean
awesome_match_cb (GtkEntryCompletion* completion, const gchar* key, GtkTreeIter* iter, gpointer data) {
    GtkTreeModel* model;
    gchar* str;
    gboolean match;
    gchar* temp;
    gchar * search;
    char *saveptr;
    const gchar* part;

    model = gtk_entry_completion_get_model (completion);
    gtk_tree_model_get (model, iter, 0, &str, -1);

    match = FALSE;
    if (G_LIKELY (str))
    {
        //printf("%s ######\t\t",str);
        temp = g_utf8_casefold (str, -1);
        search = g_utf8_casefold (key, -1);
        part = (char *) strtok_r (search, " ", &saveptr);
        if (part) {
            //printf("%s", part);
            match = (strstr (temp, part) != NULL);
            //printf(" %s\t", match? "Y" : "N");
            if (match) {
                while(match && (part = (char *) strtok_r (NULL, " ", &saveptr)) != NULL) {
                 // TODO: if we type 'aa aa' we should match 2 different aa strings.
                    //printf("%s", part);
                    //printf(" %s\t", match? "Y" : "N");
                          //  g_free (part);
                    match = (strstr (temp, part) != NULL);
                }
            }
        }
        g_free (temp);
        g_free (str);
        //printf("%s\n", match? "TRUE" : "FALSE");
    }
    return match;
}

int main(int argc, char **argv) {
    GtkWidget *window;
    GtkWidget *entry;
    GtkEntryCompletion *completion;
    GtkCellRenderer* renderer;

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

    renderer = gtk_cell_renderer_text_new ();
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (completion), renderer, TRUE);
    gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (completion), renderer, midori_location_entry_render_text_cb, completion, NULL);

    gtk_entry_completion_set_match_func (completion, awesome_match_cb, NULL, NULL);
    model = gtk_list_store_new(1, G_TYPE_STRING);
    readstdin();
    gtk_entry_completion_set_model(completion, GTK_TREE_MODEL(model));
    gtk_container_add (GTK_CONTAINER (window), entry);
    gtk_widget_show_all (window);
    gtk_main ();
    return 0;
}
