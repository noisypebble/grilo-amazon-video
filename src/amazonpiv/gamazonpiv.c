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

#include <libsoup/soup.h>
#include "gamazon.h"

#define G_AMAZON_GET_PRIVATE(object) \
  (G_TYPE_INSTANCE_GET_PRIVATE((object), \
                               G_AMAZON_TYPE, \
                               GAmazonPrivate))

#define PLUGIN_USER_AGENT               "Grilo Amazon Plugin"

#define AMAZON_LOGIN_URL                "http://www.amazon.com/gp/flex/sign-out.html"

typedef struct {
  GAmazon *amazon;
  GAmazonVideoSearchCb search_cb;
  gpointer user_data;
} GAmazonVideoSearchData;

typedef struct {
  GAmazon *amazon;
  gint video_id;
  GAmazonURLCb callback;
  gpointer user_data;
} GAmazonVideoURLData;

struct _GAmazonPrivate {
  SoupSession *async_session;
};

enum InfoType {SIMPLE, EXTENDED};

typedef struct {
  enum InfoType type;
  gchar *name;
} VideoInfo;

static void g_amazon_finalize (GObject *object);
static gchar * encode_uri (const gchar *uri);

G_DEFINE_TYPE (GAmazon, g_amazon, G_TYPE_OBJECT);

static void
g_amazon_class_init (GAmazonClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = g_amazon_finalize;

  g_type_class_add_private (klass, sizeof (GAmazonPrivate));
}

static void
g_amazon_finalize (GObject *object)
{
  GAmazon *amazon = G_AMAZON (object);
  g_object_unref (amazon->priv->async_session);

  G_OBJECT_CLASS (g_amazon_parent_class)->finalize (object);
}

GAmazon *
g_amazon_new ()
{
  GAmazon *amazon = g_object_new (G_AMAZON_TYPE, NULL);

  return amazon;
}

/* ----------------------- PRIVATE API ------------------------ */

static void
login (GAmazon *amazon) 
{
  amazon->priv->async_session = soup_session_async_new_with_options (
      SOUP_SESSION_ADD_FEATURE_BY_TYPE, SOUP_TYPE_CONTENT_DECODER,
      SOUP_SESSION_ADD_FEATURE_BY_TYPE, SOUP_TYPE_COOKIE_JAR,
      SOUP_SESSION_USER_AGENT, 
}
