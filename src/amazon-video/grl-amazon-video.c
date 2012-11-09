/*
 * Copyright (C) 2012 Toyatech, Inc.
 *
 * Author: Dusty Little <dlittle@toyatech.net>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <grilo.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "grl-amazon-video.h"
#include "gamazon-video.h"

#define GRL_AMAZON_VIDEO_SOURCE_GET_PRIVATE(object)                           \
  (G_TYPE_INSTANCE_GET_PRIVATE((object),                                      \
                               GRL_AMAZON_VIDEO_SOURCE_TYPE,                  \
                               GrlAmazonVideoSourcePrivate))

/* --------- Logging --------- */

#define GRL_LOG_DOMAIN_DEFAULT amazon_video_log_domain
GRL_LOG_DOMAIN_STATIC(amazon_video_log_domain);

/* --- Plugin information --- */

#define PLUGIN_ID   AMAZON_VIDEO_PLUGIN_ID

#define SOURCE_ID   "grl-amazon-video"
#define SOURCE_NAME "Amazon Video"
#define SOURCE_DESC "A source for browsing and searching Amazon Prime Instant Videos"

typedef struct {
  GrlSourceSearchSpec *ss;
  gint offset;
  gint page;
} SearchData;

struct _GrlAmazonVideoSourcePrivate {
  GAmazonVideo *amazon_video;
};

static GrlAmazonVideoSource *grl_amazon_video_source_new (void);

gboolean grl_amazon_video_plugin_init (GrlRegistry *registry,
                                       GrlPlugin *plugin,
                                       GList *configs);

static const GList *grl_amazon_video_source_supported_keys(GrlSource *source);

static void grl_amazon_video_source_resolve (GrlSource *source,
                                             GrlSourceResolveSpec *rs);

static void grl_amazon_video_source_search (GrlSource *source,
                                            GrlSourceSearchSpec *ss);

/* =================== Amazon Video Plugin =================== */

gboolean
grl_amazon_video_plugin_init (GrlRegistry *registry,
                              GrlPlugin *plugin,
                              GList *configs)
{
  gchar *email;
  gchar *password;
  GrlConfig *config;
  gint config_count;
  gboolean init_result = FALSE;
  GrlAmazonVideoSource *source;

  GRL_LOG_DOMAIN_INIT (amazon_video_log_domain, "amazon-video");

  GRL_DEBUG ("amazon_video_plugin_init");

#if !GLIB_CHECK_VERSION(2,32,0)
  if (!g_thread_supported ()) {
    g_thread_init (NULL);
  }
#endif

  if (!configs) {
    GRL_INFO ("Configuation not provided! Plugin not loaded");
    return FALSE;
  }

  config_count = g_list_length (config);
  if (config_count > 1) {
    GRL_INFO ("Provided %d configs, but will only use one", config_count);
  }

  config = GRL_CONFIG (configs->data);

  email = grl_config_get_string(config, GRL_AMAZON_VIDEO_EMAIL_KEY);
  password = grl_config_get_string(config, GRL_AMAZON_VIDEO_PASSWORD_KEY);

  if (!email || !password) {
    GRL_INFO ("Required email or password configuration not provided."
              " Plugin not loaded");
    goto go_out;
  }

  source = grl_amazon_video_source_new ();
  source->priv->amazon_video = g_amazon_video_new (email, password);

  grl_registry_register_source (registry,
                                plugin,
                                GRL_SOURCE (source),
                                NULL);

  init_result = TRUE;

 go_out:

  if (email != NULL)
    g_free (email);
  if (password != NULL)
    g_free (password);

  return init_result;
}

GRL_PLUGIN_REGISTER (grl_amazon_video_plugin_init,
                     NULL,
                     PLUGIN_ID);

/* =================== Amazon Video GObject =================== */

static GrlAmazonSource *
grl_amazon_video_source_new (void)
{
  GRL_DEBUG ("grl_amazon_video_source_new");

  return g_object_new (GRL_AMAZON_VIDEO_SOURCE_TYPE,
                       "source-id", SOURCE_ID,
                       "source-name", SOURCE_NAME,
                       "source-desc", SOURCE_DESC,
                       "supported-media", GRL_MEDIA_TYPE_VIDEO,
                       NULL);
}

static void
grl_amazon_video_source_class_init (GrlVimeoSourceClass * klass)
{
  GrlSourceClass *source_class = GRL_SOURCE_CLASS (klass);

  source_class->supported_keys = grl_amazon_video_source_supported_keys;
  source_class->resolve = grl_amazon_video_source_resolve;
  source_class->search = grl_amazon_video_source_search;

  g_type_class_add_private (klass, sizeof (GrlAmazonVideoSourcePrivate));
}

static void
grl_amazon_video_source_init (GrlAmazonSource *source)
{
  source->priv = GRL_AMAZON_VIDEO_SOURCE_GET_PRIVATE (source);
}

G_DEFINE_TYPE (GrlAmazonVideoSource, grl_amazon_video_source, GRL_TYPE_SOURCE);

/* =================== Utilities =================== */

/* =================== API Implementation =================== */

static const GList *
grl_amazon_video_source_supported_keys (GrlSource *source)
{
  static GList *keys = NULL;
  if (!keys) {
    keys = grl_metadata_key_list_new (GRL_METADATA_KEY_ID,
        GRL_METADATA_KEY_TITLE,
        GRL_METADATA_KEY_DESCRIPTION,
        GRL_METADATA_KEY_PERFORMER,
        GRL_METADATA_KEY_DIRECTOR,
        GRL_METADATA_KEY_CREATION_DATE,
        GRL_METADATA_KEY_PUBLICATION_DATE,
        GRL_METADATA_KEY_RATING,
        GRL_
