/*================================================================================================*/
/**^M
    @file   rangewidgets.c^M
*==================================================================================================

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
   Inkina Irina               10/09/2004     ??????      Initial version

==================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/


#include <gtk/gtk.h>
gint vt=FALSE;
GtkWidget *hscale, *vscale;
static gint button_press (GtkWidget *, GdkEventButton *);

void destroy_Quit( GtkWidget *widget,gpointer data )
{
    vt=FALSE;
    g_print("Test Pass Exiting with test pass");
    gtk_main_quit();

}
void destroy_Exit( GtkWidget *widget,gpointer data )
{
    vt=TRUE;
    g_print("Test Fail Exiting with test fail");
    gtk_main_quit();
}

static GtkItemFactoryEntry menu_items[] =
{
  { "/_Quit-Pass", "<control>Q", destroy_Quit,       0, "<StockItem>", GTK_STOCK_QUIT },
  { "/_Exit-Fail", "<control>E", destroy_Exit,       0, "<StockItem>", GTK_STOCK_QUIT }
};



void cb_pos_menu_select( GtkWidget       *item,
                         GtkPositionType  pos )
{
    /* Set the value position on both scale widgets */
    gtk_scale_set_value_pos (GTK_SCALE (hscale), pos);
    gtk_scale_set_value_pos (GTK_SCALE (vscale), pos);
}

void cb_update_menu_select( GtkWidget     *item,
                            GtkUpdateType  policy )
{
    /* Set the update policy for both scale widgets */
    gtk_range_set_update_policy (GTK_RANGE (hscale), policy);
    gtk_range_set_update_policy (GTK_RANGE (vscale), policy);
}

void cb_digits_scale( GtkAdjustment *adj )
{
    /* Set the number of decimal places to which adj->value is rounded */
    gtk_scale_set_digits (GTK_SCALE (hscale), (gint) adj->value);
    gtk_scale_set_digits (GTK_SCALE (vscale), (gint) adj->value);
}

void cb_page_size( GtkAdjustment *get,
                   GtkAdjustment *set )
{
    /* Set the page size and page increment size of the sample
     * adjustment to the value specified by the "Page Size" scale */
    set->page_size = get->value;
    set->page_increment = get->value;

    /* This sets the adjustment and makes it emit the "changed" signal to 
       reconfigure all the widgets that are attached to this signal.  */
    gtk_adjustment_set_value (set, CLAMP (set->value,
					  set->lower,
					  (set->upper - set->page_size)));
}

void cb_draw_value( GtkToggleButton *button )
{
    /* Turn the value display on the scale widgets off or on depending
     *  on the state of the checkbutton */
    gtk_scale_set_draw_value (GTK_SCALE (hscale), button->active);
    gtk_scale_set_draw_value (GTK_SCALE (vscale), button->active);  
}

/* Convenience functions */

GtkWidget *make_menu_item (gchar     *name,
                           GCallback  callback,
			   gpointer   data)
{
    GtkWidget *item;
  
    item = gtk_menu_item_new_with_label (name);
    g_signal_connect (G_OBJECT (item), "activate",
	              callback, (gpointer) data);
    gtk_widget_show (item);

    return item;
}

void scale_set_default_values( GtkScale *scale )
{
    gtk_range_set_update_policy (GTK_RANGE (scale),
                                 GTK_UPDATE_CONTINUOUS);
    gtk_scale_set_digits (scale, 1);
    gtk_scale_set_value_pos (scale, GTK_POS_TOP);
    gtk_scale_set_draw_value (scale, TRUE);
}

/* makes the sample window */

void create_range_controls( void )
{
    GtkWidget *window;
    GtkWidget *box1, *box2, *box3;
    GtkWidget *button;
    GtkWidget *scrollbar;
    GtkWidget *separator;
    GtkWidget *opt, *menu, *item;
    GtkWidget *label;
    GtkWidget *scale;
    GtkObject *adj1, *adj2;
    GtkWidget *menu_p;
    GtkItemFactory *item_factory;

    /* Standard window-creating stuff */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (window),240,320);
    g_signal_connect (G_OBJECT (window), "destroy",
                      G_CALLBACK (gtk_main_quit),
                      &window);
    gtk_window_set_title (GTK_WINDOW (window), "range controls");
    gtk_container_set_border_width (GTK_CONTAINER (window), 0);

   /* Same as before but don't bother with the accelerators */
   item_factory = gtk_item_factory_new (GTK_TYPE_MENU, "<main>",
                                        NULL);
   gtk_item_factory_create_items (item_factory,G_N_ELEMENTS(menu_items), menu_items, NULL);
   menu_p = gtk_item_factory_get_widget(item_factory, "<main>");
   gtk_widget_show (menu_p);
   g_signal_connect_swapped (G_OBJECT (window), "button_press_event",G_CALLBACK (button_press),
                              G_OBJECT (menu_p));
   gtk_widget_set_events (window, gtk_widget_get_events (window)| GDK_BUTTON_PRESS_MASK);

    box1 = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (window), box1);
    gtk_window_set_policy(GTK_WINDOW (window),TRUE,TRUE,TRUE);

    box2 = gtk_hbox_new (FALSE, 10);
    gtk_container_set_border_width (GTK_CONTAINER (box2), 10);
    gtk_box_pack_start (GTK_BOX (box1), box2, TRUE, TRUE, 0);

    /* value, lower, upper, step_increment, page_increment, page_size */
    /* Note that the page_size value only makes a difference for
     * scrollbar widgets, and the highest value you'll get is actually
     * (upper - page_size). */
    adj1 = gtk_adjustment_new (0.0, 0.0, 101.0, 0.1, 1.0, 1.0);
  
    vscale = gtk_vscale_new (GTK_ADJUSTMENT (adj1));
    scale_set_default_values (GTK_SCALE (vscale));
    gtk_box_pack_start (GTK_BOX (box2), vscale, TRUE, TRUE, 0);

    box3 = gtk_vbox_new (FALSE, 5);
    gtk_box_pack_start (GTK_BOX (box2), box3, TRUE, TRUE, 0);

    /* Reuse the same adjustment */
    hscale = gtk_hscale_new (GTK_ADJUSTMENT (adj1));
    gtk_widget_set_size_request (GTK_WIDGET (hscale), 50, -1);
    scale_set_default_values (GTK_SCALE (hscale));
    gtk_box_pack_start (GTK_BOX (box3), hscale, TRUE, TRUE, 0);

    /* Reuse the same adjustment again */
    scrollbar = gtk_hscrollbar_new (GTK_ADJUSTMENT (adj1));
    /* Notice how this causes the scales to always be updated
     * continuously when the scrollbar is moved */
    gtk_range_set_update_policy (GTK_RANGE (scrollbar), 
                                 GTK_UPDATE_CONTINUOUS);
    gtk_box_pack_start (GTK_BOX (box3), scrollbar, TRUE, TRUE, 0);

    box2 = gtk_hbox_new (FALSE, 10);
    gtk_container_set_border_width (GTK_CONTAINER (box2), 10);
    gtk_box_pack_start (GTK_BOX (box1), box2, TRUE, TRUE, 0);

    /* A checkbutton to control whether the value is displayed or not */
    button = gtk_check_button_new_with_label("Display value on scale widgets");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
    g_signal_connect (G_OBJECT (button), "toggled",
                      G_CALLBACK (cb_draw_value), NULL);
    gtk_box_pack_start (GTK_BOX (box2), button, TRUE, TRUE, 0);
  
    box2 = gtk_hbox_new (FALSE, 10);
    gtk_container_set_border_width (GTK_CONTAINER (box2), 10);

    /* An option menu to change the position of the value */
    label = gtk_label_new ("Scale Value Position:");
    gtk_box_pack_start (GTK_BOX (box2), label, FALSE, FALSE, 0);
  
    opt = gtk_option_menu_new ();
    menu = gtk_menu_new ();

    item = make_menu_item ("Top",
                           G_CALLBACK (cb_pos_menu_select),
                           GINT_TO_POINTER (GTK_POS_TOP));
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
  
    item = make_menu_item ("Bottom", G_CALLBACK (cb_pos_menu_select), 
                           GINT_TO_POINTER (GTK_POS_BOTTOM));
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
  
    item = make_menu_item ("Left", G_CALLBACK (cb_pos_menu_select),
                           GINT_TO_POINTER (GTK_POS_LEFT));
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
  
    item = make_menu_item ("Right", G_CALLBACK (cb_pos_menu_select),
                           GINT_TO_POINTER (GTK_POS_RIGHT));
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
  
    gtk_option_menu_set_menu (GTK_OPTION_MENU (opt), menu);
    gtk_box_pack_start (GTK_BOX (box2), opt, TRUE, TRUE, 0);

    gtk_box_pack_start (GTK_BOX (box1), box2, TRUE, TRUE, 0);

    box2 = gtk_hbox_new (FALSE, 5);
    gtk_container_set_border_width (GTK_CONTAINER (box2), 5);
  
    /* An HScale widget for adjusting the number of digits on the
     * sample scales. */
    label = gtk_label_new ("Scale Digits:");
    gtk_box_pack_start (GTK_BOX (box2), label, FALSE, FALSE, 0);

    adj2 = gtk_adjustment_new (1.0, 0.0, 5.0, 1.0, 1.0, 0.0);
    g_signal_connect (G_OBJECT (adj2), "value_changed",
                      G_CALLBACK (cb_digits_scale), NULL);
    scale = gtk_hscale_new (GTK_ADJUSTMENT (adj2));
    gtk_scale_set_digits (GTK_SCALE (scale), 0);
    gtk_box_pack_start (GTK_BOX (box2), scale, TRUE, TRUE, 0);

    gtk_box_pack_start (GTK_BOX (box1), box2, TRUE, TRUE, 0);
  
    box2 = gtk_hbox_new (FALSE, 10);
    gtk_container_set_border_width (GTK_CONTAINER (box2), 10);
  
    /* And, one last HScale widget for adjusting the page size of the
     * scrollbar. */
    label = gtk_label_new ("Scrollbar Page Size:");
    gtk_box_pack_start (GTK_BOX (box2), label, FALSE, FALSE, 0);
    gtk_widget_show (label);

    adj2 = gtk_adjustment_new (1.0, 1.0, 101.0, 1.0, 1.0, 0.0);
    g_signal_connect (G_OBJECT (adj2), "value_changed",
                      G_CALLBACK (cb_page_size), (gpointer) adj1);
    scale = gtk_hscale_new (GTK_ADJUSTMENT (adj2));
    gtk_scale_set_digits (GTK_SCALE (scale), 0);
    gtk_box_pack_start (GTK_BOX (box2), scale, TRUE, TRUE, 0);

    gtk_box_pack_start (GTK_BOX (box1), box2, TRUE, TRUE, 0);

    separator = gtk_hseparator_new ();
    gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 0);

    box2 = gtk_vbox_new (FALSE, 10);
    gtk_container_set_border_width (GTK_CONTAINER (box2), 10);
    gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, TRUE, 0);

    gtk_widget_show_all (window);
}

static gint button_press( GtkWidget *widget,GdkEventButton *event )
{
if (event->button==3){
        gtk_menu_popup (GTK_MENU (widget), NULL, NULL, NULL, NULL,
                        event->button, event->time);
        /* Tell calling code that we have handled this event; the buck
         * stops here. */
        return TRUE;
    }

    /* Tell calling code that we have not handled this event; pass it on. */
    return FALSE;
}

int rangewidgets_main( int   argc,
          char *argv[] )
{
    gtk_init (&argc, &argv);

    create_range_controls ();

    gtk_main ();

    return (vt);
}

