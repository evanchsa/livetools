/*  AST Remote control plugin by Ryan Veety <ryan@ryanspc.com>
 *  
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#include <gtk/gtk.h>

static GtkWidget *liveir_about_win;

void liveir_about(void)
{
	GtkWidget *vbox, *frame, *label, *box, *button, *textbox;

	if (liveir_about_win)
		return;
	liveir_about_win = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_signal_connect(GTK_OBJECT(liveir_about_win), "destroy", GTK_SIGNAL_FUNC(gtk_widget_destroyed), &liveir_about_win);
	gtk_window_set_title(GTK_WINDOW(liveir_about_win), "About");
	gtk_window_set_policy(GTK_WINDOW(liveir_about_win), FALSE, FALSE, FALSE);
	gtk_window_set_position(GTK_WINDOW(liveir_about_win), GTK_WIN_POS_MOUSE);
	gtk_container_border_width(GTK_CONTAINER(liveir_about_win), 10);

	vbox = gtk_vbox_new(FALSE, 10);
	gtk_container_add(GTK_CONTAINER(liveir_about_win), vbox);

	frame = gtk_frame_new("Creative Live!Drive IR plugin 1.0");
	gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);
	textbox = gtk_vbox_new(FALSE, 10);	
	gtk_container_border_width(GTK_CONTAINER(textbox), 10);
	gtk_container_add(GTK_CONTAINER(frame), textbox);

	label = gtk_label_new("Stephen Evanchik <evanchsa@gmail.com>\n");

	gtk_box_pack_start_defaults(GTK_BOX(textbox), label);

	box = gtk_hbutton_box_new();
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(box), 5);
	gtk_box_pack_start(GTK_BOX(vbox), box, FALSE, FALSE, 0);

	button = gtk_button_new_with_label("Ok");
	gtk_signal_connect_object(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(liveir_about_win));
	GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
	gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);
	gtk_widget_grab_default(button);

	gtk_widget_show(button);
	gtk_widget_show(box);
	gtk_widget_show(frame);
	gtk_widget_show(textbox);
	gtk_widget_show(label);
	gtk_widget_show(vbox);
	gtk_widget_show(liveir_about_win);
}
