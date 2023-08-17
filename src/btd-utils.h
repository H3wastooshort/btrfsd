/*
 * Copyright (C) Matthias Klumpp <matthias@tenstral.net>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * btd_str_equal0:
 * Returns TRUE if strings are equal, ignoring NULL strings.
 * This is a convenience wrapper around g_strcmp0
 */
#define btd_str_equal0(str1, str2) (g_strcmp0 ((gchar *) str1, (gchar *) str2) == 0)

gboolean btd_is_empty (const gchar *str);
gchar   *btd_strstripnl (gchar *string);

gchar   *btd_get_state_dir (void);

gulong   btd_parse_duration_string (const gchar *str);

gchar   *btd_render_template (const gchar *template, const gchar *key1, ...) G_GNUC_NULL_TERMINATED;

gchar   *btd_path_to_filename (const gchar *path);

G_END_DECLS
