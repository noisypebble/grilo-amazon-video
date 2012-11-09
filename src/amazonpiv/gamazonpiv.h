/*
 * Copyright (C) 2012 Dusty Little
 *
 * Contact: Dusty Little <dlittle@toyatech.net>
 *
 * Authors: Dusty Little <dlittle@toyatech.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */
#ifndef _G_AMAZON_H_
#define _G_AMAZON_H_

#include <glib-object.h>

#define G_AMAZON_TYPE \
  (g_amazon_get_type ())

#define G_AMAZON(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                               G_AMAZON_TYPE, \
                               GAmazon))

#define G_AMAZON_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass), \
                           G_AMAZON_TYPE \
                           GAmazonClass))

#define G_AMAZON_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                              G_AMAZON_TYPE, \
                              GAmazonClass))

//define feed elements

typedef struct _GAmazon GAmazon;
typedef struct _GAmazonPrivate GAmazonPrivate;

struct _GAmazon {

  GObject parent;

  /*< private >*/
  GAmazonPrivate *priv;
};

typedef struct _GAmazonClass GAmazonClass;

struct _GAmazonClass {

  GObjectClass parent_class;

};

typedef void (*GAmazonVideoSearchCb) (GAmazon *amazon,
    GList *videolist,
    gpointer user_data);

typedef void (*GAmazonURLCb) (gchar *url, gpointer user_data);

GType g_amazon_get_type (void);

GAmazon *g_amazon_new (const gchar *api_key, const gchar *auth_secret);

void g_amazon_video_get_play_url(GAmazon *amazon,
    gint id,
    GAmazonURLCb callback,
    gpointer user_data);

void g_amazon_set_per_page (GAmazon *amazon, gint per_page);

void g_amazon_videos_search (GAmazon *amazon,
    const gchar *text,
    gint page,
    GAmazonVideoSearchCb callback,
    gpointer user_data);

#endif /* _G_AMAZON_H_ */
