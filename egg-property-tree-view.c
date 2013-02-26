/* Copyright (C) 2011, 2012 Matthias Vogelgesang <matthias.vogelgesang@kit.edu>
   (Karlsruhe Institute of Technology)

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by the
   Free Software Foundation; either version 2.1 of the License, or (at your
   option) any later version.

   This library is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
   details.

   You should have received a copy of the GNU Lesser General Public License along
   with this library; if not, write to the Free Software Foundation, Inc., 51
   Franklin St, Fifth Floor, Boston, MA 02110, USA */


#include "egg-property-tree-view.h"
#include "egg-property-cell-renderer.h"

G_DEFINE_TYPE (EggPropertyTreeView, egg_property_tree_view, GTK_TYPE_TREE_VIEW)

#define EGG_PROPERTY_TREE_VIEW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), EGG_TYPE_PROPERTY_TREE_VIEW, EggPropertyTreeViewPrivate))

struct _EggPropertyTreeViewPrivate
{
    GtkListStore *list_store;
    GObject *object;
};

enum {
    PROP_0,
    PROP_OBJECT,
    N_PROPERTIES
};

enum {
    COLUMN_PROP_NAME,
    COLUMN_PROP_ROW,
    COLUMN_PROP_ADJUSTMENT,
    N_COLUMNS
};

static GParamSpec *properties[N_PROPERTIES] = { NULL, };

static void
populate_model_with_properties (GtkListStore *model,
                                GObject *object)
{
    GParamSpec **pspecs;
    GObjectClass *oclass;
    guint n_properties;
    GtkTreeIter iter;

    oclass = G_OBJECT_GET_CLASS (object);
    pspecs = g_object_class_list_properties (oclass, &n_properties);

    for (guint i = 0; i < n_properties; i++) {
        if (pspecs[i]->flags & G_PARAM_READABLE) {
            GtkObject *adjustment;

            adjustment = gtk_adjustment_new (5, 0, 1000, 1, 10, 0);

            gtk_list_store_append (model, &iter);
            gtk_list_store_set (model, &iter,
                    COLUMN_PROP_NAME, pspecs[i]->name,
                    COLUMN_PROP_ROW, FALSE,
                    COLUMN_PROP_ADJUSTMENT, adjustment,
                    -1);
        }
    }

    g_free (pspecs);
}

static void
append_property_columns (EggPropertyTreeView *view,
                         GObject *object)
{
    GtkTreeView *tree_view;
    GtkTreeViewColumn *prop_column, *value_column;
    GtkCellRenderer *prop_renderer, *value_renderer;

    tree_view = GTK_TREE_VIEW (view);
    prop_renderer = gtk_cell_renderer_text_new ();
    value_renderer = egg_property_cell_renderer_new (object, view->priv->list_store);

    prop_column = gtk_tree_view_column_new_with_attributes ("Property", prop_renderer,
            "text", COLUMN_PROP_NAME,
            NULL);

    value_column = gtk_tree_view_column_new_with_attributes ("Value", value_renderer,
            "prop-name", COLUMN_PROP_NAME,
            NULL);

    gtk_tree_view_append_column (tree_view, prop_column);
    gtk_tree_view_append_column (tree_view, value_column);
}

GtkWidget *
egg_property_tree_view_new (GObject *object)
{
    EggPropertyTreeView *view;

    view = EGG_PROPERTY_TREE_VIEW (g_object_new (EGG_TYPE_PROPERTY_TREE_VIEW, NULL));

    if (object)
        egg_property_tree_view_set_object (view, object);

    return GTK_WIDGET (view);
}

static void
remove_columns (GtkTreeView *view)
{
    GList *columns;

    columns = gtk_tree_view_get_columns (view);

    for (GList *it = g_list_first (columns); it != NULL; it = g_list_next (it)) {
        GtkTreeViewColumn *column;

        column = GTK_TREE_VIEW_COLUMN (it->data);
        gtk_tree_view_remove_column (view, column);
    }
}

void
egg_property_tree_view_set_object (EggPropertyTreeView *view,
                                   GObject *object)
{
    EggPropertyTreeViewPrivate *priv;

    priv = EGG_PROPERTY_TREE_VIEW_GET_PRIVATE (view);

    if (priv->object == object)
        return;

    if (priv->object != NULL) {
        g_object_unref (priv->object);
        priv->object = NULL;
    }

    priv->object = object;
    g_object_ref (object);

    remove_columns (GTK_TREE_VIEW (view));
    gtk_list_store_clear (priv->list_store);

    populate_model_with_properties (priv->list_store, object);
    gtk_tree_view_set_model (GTK_TREE_VIEW (view),
                             GTK_TREE_MODEL (priv->list_store));
    append_property_columns (view, object);
}

static void
egg_property_tree_view_set_property (GObject        *object,
                                     guint           property_id,
                                     const GValue   *value,
                                     GParamSpec     *pspec)
{
    switch (property_id) {
        case PROP_OBJECT:
            egg_property_tree_view_set_object (EGG_PROPERTY_TREE_VIEW (object),
                                               g_value_get_object (value));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            return;
    }
}

static void
egg_property_tree_view_dispose (GObject *object)
{
    EggPropertyTreeViewPrivate *priv;

    priv = EGG_PROPERTY_TREE_VIEW_GET_PRIVATE (object);

    if (priv->list_store) {
        g_object_unref (priv->list_store);
        priv->list_store = NULL;
    }

    if (priv->object) {
        g_object_unref (priv->object);
        priv->object = NULL;
    }

    G_OBJECT_CLASS (egg_property_tree_view_parent_class)->dispose (object);
}

static void
egg_property_tree_view_class_init (EggPropertyTreeViewClass *klass)
{
    GObjectClass *oclass;

    oclass = G_OBJECT_CLASS (klass);
    oclass->set_property = egg_property_tree_view_set_property;
    oclass->dispose = egg_property_tree_view_dispose;

    properties[PROP_OBJECT] =
        g_param_spec_object ("object",
                             "GObject to observe",
                             "GObject to observe",
                             G_TYPE_OBJECT,
                             G_PARAM_WRITABLE);

    g_object_class_install_property (oclass, PROP_OBJECT, properties[PROP_OBJECT]);

    g_type_class_add_private (klass, sizeof (EggPropertyTreeViewPrivate));
}

static void
egg_property_tree_view_init (EggPropertyTreeView *tree_view)
{
    EggPropertyTreeViewPrivate *priv = EGG_PROPERTY_TREE_VIEW_GET_PRIVATE (tree_view);

    tree_view->priv = priv = EGG_PROPERTY_TREE_VIEW_GET_PRIVATE (tree_view);
    priv->list_store = gtk_list_store_new (N_COLUMNS, G_TYPE_STRING, G_TYPE_BOOLEAN, GTK_TYPE_ADJUSTMENT);
    priv->object = NULL;
}

