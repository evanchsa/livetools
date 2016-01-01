/*  Copyright (C) Stephen Evanchik <evanchsa@gmail.com>
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
#include <xmms/configfile.h>
#include <xmms/plugin.h>

#include "xmms_liveir.h"

static GtkWidget *liveir_configure_win;
GtkWidget *vbox, *frame, *devicelabel, *box, *button,
	  *device_input, *hbox, *windowlabel, *dirlabel, *dir_input, *hbox2,

	  *plugins_box, *plugins_use_cbox,
	  *plugins_list, *plugins_hbox, *scrolled_win, *plugin_menu, *item,
	  *plugins_label;

struct OutputPluginData *op_data;

static void liveir_configure_ok(GtkWidget *w, gpointer data);

void liveir_read_config(struct liveir_conf *conf);

void liveir_configure(void)
{
	int i;
	
	config_info.device = (gchar *)malloc(256 * sizeof(gchar));
	
	strncpy(config_info.device, "/dev/midi", 255);
	
    liveir_read_config(&config_info);
	
	if (liveir_configure_win)
		return;
	
	liveir_configure_win = gtk_window_new(GTK_WINDOW_DIALOG);
	
	gtk_signal_connect(GTK_OBJECT(liveir_configure_win), "destroy", GTK_SIGNAL_FUNC(gtk_widget_destroyed), &liveir_configure_win);
	gtk_window_set_title(GTK_WINDOW(liveir_configure_win), "Configure");
	gtk_window_set_policy(GTK_WINDOW(liveir_configure_win), FALSE, FALSE, FALSE);
	gtk_window_set_position(GTK_WINDOW(liveir_configure_win), GTK_WIN_POS_MOUSE);
	gtk_container_border_width(GTK_CONTAINER(liveir_configure_win), 10);

	vbox = gtk_vbox_new(FALSE, 10);
	gtk_container_add(GTK_CONTAINER(liveir_configure_win), vbox);

	windowlabel = gtk_label_new("livetools Configuration");
	gtk_box_pack_start(GTK_BOX(vbox), windowlabel, TRUE, TRUE, 0);

	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

	hbox2 = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox2, TRUE, TRUE, 0);

	devicelabel = gtk_label_new("Device:");
	gtk_box_pack_start(GTK_BOX(hbox), devicelabel, TRUE, TRUE, 0);

	device_input = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(device_input), config_info.device);
	gtk_box_pack_start(GTK_BOX(hbox), device_input, TRUE, TRUE, 0);
	gtk_widget_show(device_input);

	/*config_info.device = g_strdup(gtk_entry_get_text(GTK_ENTRY(device_input)));*/

	box = gtk_hbutton_box_new();
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(box), 5);
	gtk_box_pack_start(GTK_BOX(vbox), box, FALSE, FALSE, 0);

	button = gtk_button_new_with_label("Ok");
	gtk_signal_connect_object(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(liveir_configure_ok), NULL);
	GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
	gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);
	gtk_widget_grab_default(button);

	gtk_widget_show(button);
	gtk_widget_show(box);
	gtk_widget_show(devicelabel);
	gtk_widget_show(windowlabel);
	gtk_widget_show(vbox);
	gtk_widget_show(hbox);

	gtk_widget_show(liveir_configure_win);
}

static void liveir_configure_ok(GtkWidget *w, gpointer data)
{
	ConfigFile *cfgfile;
	gchar *filename;
	struct liveir_conf *conf = (struct liveir_conf *)data;
	
	conf->device = g_strdup(gtk_entry_get_text(GTK_ENTRY(device_input)));
	
	filename = g_strconcat(g_get_home_dir(), "/.xmms/config", NULL);
	
	cfgfile = xmms_cfg_open_file(filename);
	
	if (!cfgfile)
		  cfgfile = xmms_cfg_new();
	
	xmms_cfg_write_string(cfgfile, "livetools", "device", conf->device);
	
	xmms_cfg_write_file(cfgfile, filename);
	xmms_cfg_free(cfgfile);
	
	gtk_widget_destroy(liveir_configure_win);
}

void liveir_read_config(struct liveir_conf *conf)
{
	ConfigFile *cfgfile;
	gchar *filename;
	
	if(conf == NULL)
		return;
	
	filename = g_strconcat(g_get_home_dir(), "/.xmms/config", NULL);
	
	cfgfile = xmms_cfg_open_file(filename);
	
	if (!cfgfile)
		  cfgfile = xmms_cfg_new();
	
	if(!xmms_cfg_read_string(cfgfile, "livetools", "device", &conf->device)) {
		
		strncpy(config_info.device, "/dev/midi", 255);
	}
	
	xmms_cfg_free(cfgfile);
}
