/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2008 chantra.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 * 
 * Authors: chantra <chantra_AT_debuntu_-DOT-_org>
 */

/** compile with
 * gcc -Wall -g entry_completion.c -o entry_completion `pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0`
 */

#include <stdio.h>

#include <gtk/gtk.h>

#define PACKAGE "ContactLookUp"
#define VERSION "0.1.0"

typedef struct contact {
  char *name;
  char *nickname;
  char *email;
} contact;

typedef enum {
  CONTACT_NAME = 0,
  CONTACT_NICK,
  CONTACT_EMAIL
} ContactCol;

contact contact_list[] = {
  { "Foobar", "foo", "foobar@example.com" },
  { "Super Framework", "frame", "frame@work.com" },
  { "Gtk Framework", "gtk", "gtk@framework.com" },
  { "Non sense", "bla", "nowhere@myplace.net" },
  { "GtkEntryCompletion", "completion", "fancybox@mail.com" },
  { "Gtk rules", "fantastic", "gtk@gtk.org" },
  { "some more words", "fillingTheGap", "another@email.com" },
  { "new line", "somemore nick", "again@mail.com"},
  { NULL, NULL, NULL}
};

/*
 * Terminate the main loop.
 */
static void
on_destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}

static gboolean
on_match_select(GtkEntryCompletion *widget,
  GtkTreeModel       *model,
  GtkTreeIter        *iter,
  gpointer            user_data)
{  
  GValue value = {0, };
  gtk_tree_model_get_value(model, iter, CONTACT_EMAIL, &value);
  fprintf(stdout, "You have selected %s\n", g_value_get_string(&value));
  g_value_unset(&value);
  return FALSE;
}    

int main(int argc, char **argv)
{
    contact *l;

    GtkWidget *window;
    GtkWidget *textentry;
    GtkEntryCompletion *completion;
    GtkListStore *model;
    GtkTreeIter iter; 
    gtk_init (&argc, &argv);

    /* create the main, top level, window */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width (GTK_CONTAINER (window), 20);
    gtk_window_set_title (GTK_WINDOW (window), PACKAGE " " VERSION);
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 50);

    /* Connect the destroy event of the window */
    g_signal_connect (G_OBJECT (window), "destroy",
                      G_CALLBACK (on_destroy), l);

    textentry = gtk_entry_new ();
    /* set up completion */
    completion = gtk_entry_completion_new();
    /* set completion on the first column, the
    * 2 others won't be used for now */
    gtk_entry_completion_set_text_column(completion, CONTACT_NAME);
    gtk_entry_set_completion(GTK_ENTRY(textentry), completion);
    g_signal_connect(G_OBJECT (completion), "match-selected",
                G_CALLBACK (on_match_select), NULL); 
    /* Create the ListStore, add "contacts" and 
     * set it as the model of the entrycompletion */
    model = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    for(l=contact_list; l && l->name; l++) {
      gtk_list_store_append(model, &iter);
      gtk_list_store_set(model, &iter, CONTACT_NAME, l->name, 
              CONTACT_NICK, l->nickname, 
              CONTACT_EMAIL, l->email, -1); 
    }
    gtk_entry_completion_set_model(completion, GTK_TREE_MODEL(model));

    /* and insert our entry into the main window  */
    gtk_container_add (GTK_CONTAINER (window), textentry);
    /* make sure that everything is visible */
    gtk_widget_show_all (window);
    /* start the main loop */
    gtk_main ();
    return 0;
}


