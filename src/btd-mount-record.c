/*
 * Copyright (C) Matthias Klumpp <matthias@tenstral.net>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

/**
 * SECTION:btd-mount-record
 * @short_description: Helper class to store state about a mounted Btrfs filesystem.
 *
 * Store state about a Btrfs mountpoint for later use.
 */

#include "config.h"
#include "btd-mount-record.h"

#include "btd-utils.h"

typedef struct {
    gchar *mountpoint;
    GKeyFile *state;
} BtdMountRecordPrivate;

enum {
    PROP_0,
    PROP_MOUNTPOINT,
    N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {
    NULL,
};

G_DEFINE_TYPE_WITH_PRIVATE (BtdMountRecord, btd_mount_record, G_TYPE_OBJECT)
#define GET_PRIVATE(o) (btd_mount_record_get_instance_private (o))

static void
btd_mount_record_init (BtdMountRecord *self)
{
    BtdMountRecordPrivate *priv = GET_PRIVATE (self);

    priv->state = g_key_file_new ();
}

static void
btd_mount_record_finalize (GObject *object)
{
    BtdMountRecord *bmount = BTD_MOUNT_RECORD (object);
    BtdMountRecordPrivate *priv = GET_PRIVATE (bmount);

    g_free (priv->mountpoint);
    g_object_unref (priv->state);

    G_OBJECT_CLASS (btd_mount_record_parent_class)->finalize (object);
}

static void
btd_mount_record_set_property (GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec)
{
    BtdMountRecord *self = BTD_MOUNT_RECORD (object);
    BtdMountRecordPrivate *priv = GET_PRIVATE (self);

    switch (property_id) {
    case PROP_MOUNTPOINT:
        g_free (priv->mountpoint);
        priv->mountpoint = g_value_dup_string (value);
        if (btd_is_empty (priv->mountpoint))
            g_critical ("Mountpoint for record file is empty!");
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
btd_mount_record_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    BtdMountRecord *self = BTD_MOUNT_RECORD (object);
    BtdMountRecordPrivate *priv = GET_PRIVATE (self);

    switch (property_id) {
    case PROP_MOUNTPOINT:
        g_value_set_string (value, priv->mountpoint);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
btd_mount_record_class_init (BtdMountRecordClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = btd_mount_record_finalize;
    object_class->set_property = btd_mount_record_set_property;
    object_class->get_property = btd_mount_record_get_property;

    obj_properties[PROP_MOUNTPOINT] = g_param_spec_string ("mountpoint",
                                                           "Mountpoint",
                                                           "Mountpoint property",
                                                           NULL,
                                                           G_PARAM_READWRITE |
                                                               G_PARAM_CONSTRUCT_ONLY);

    g_object_class_install_properties (object_class, N_PROPERTIES, obj_properties);
}

/**
 * btd_mount_record_new:
 *
 * Creates a new #BtdMountRecord.
 *
 * Returns: (transfer full): a #BtdMountRecord
 */
BtdMountRecord *
btd_mount_record_new (const gchar *mountpoint)
{
    BtdMountRecord *self;
    self = g_object_new (BTD_TYPE_MOUNT_RECORD, "mountpoint", mountpoint, NULL);
    return BTD_MOUNT_RECORD (self);
}

/**
 * btd_mount_record_load:
 * @self: An instance of #BtdMountRecord.
 * @error: A #GError
 *
 * Load mount record.
 *
 * Returns: %TRUE on success.
 */
gboolean
btd_mount_record_load (BtdMountRecord *self, GError **error)
{
    BtdMountRecordPrivate *priv = GET_PRIVATE (self);
    g_autofree gchar *state_path = NULL;
    g_autofree gchar *state_filename = NULL;
    g_autofree gchar *btrfsd_state_dir = NULL;

    state_filename = btd_path_to_filename (priv->mountpoint);
    btrfsd_state_dir = btd_get_state_dir ();
    state_path = g_strconcat (btrfsd_state_dir, "/", state_filename, ".state", NULL);

    if (g_file_test (state_path, G_FILE_TEST_EXISTS)) {
        if (!g_key_file_load_from_file (priv->state, state_path, G_KEY_FILE_NONE, error))
            return FALSE;
    }

    return TRUE;
}

/**
 * btd_mount_record_get_mountpoint:
 * @self: An instance of #BtdMountRecord.
 *
 * Returns: The mountpoint path.
 */
const gchar *
btd_mount_record_get_mountpoint (BtdMountRecord *self)
{
    BtdMountRecordPrivate *priv = GET_PRIVATE (self);
    return priv->mountpoint;
}

/**
 * btd_mount_record_set_mountpoint:
 * @self: An instance of #BtdMountRecord.
 * @mount_path: The new mountpoint path.
 *
 * Set a new mountpoint path.
 */
void
btd_mount_record_set_mountpoint (BtdMountRecord *self, const gchar *mount_path)
{
    BtdMountRecordPrivate *priv = GET_PRIVATE (self);
    g_free (priv->mountpoint);
    priv->mountpoint = g_strdup (mount_path);
}
