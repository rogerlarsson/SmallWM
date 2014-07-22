/** @file */
#ifndef __SMALLWM_CLIENT_MODEL__
#define __SMALLWM_CLIENT_MODEL__

#include "changes.h"
#include "desktop-type.h"
#include "unique-multimap.h"

#include <algorithm>
#include <utility>
#include <vector>

enum InitialState
{
    IS_VISIBLE,
    IS_HIDDEN,
};

/**
 * This defines the data model used for the client.
 *
 * This is intended to be totally divorced from Xlib, and is meant to do
 * transformations of data relating to clients and validations of that data.
 *
 * Note that the type of the client doesn't matter, so the user is allowed to
 * fill that in via a template.
 */
class ClientModel
{
private:
    typedef Desktop const *desktop_ptr;
    typedef UserDesktop const *user_desktop_ptr;
    typedef Change const * change_ptr;

public:
    desktop_ptr ALL_DESKTOPS;
    desktop_ptr ICON_DESKTOP;
    desktop_ptr MOVING_DESKTOP;
    desktop_ptr RESIZING_DESKTOP;
    std::vector<user_desktop_ptr> USER_DESKTOPS;

    typedef std::vector<change_ptr>::iterator change_iter;
    typedef UniqueMultimap<desktop_ptr,Window>::member_iter client_iter;

    /**
     * Initializes all of the categories in the maps
     */
    ClientModel(unsigned long long max_desktops) :
        m_max_desktops(max_desktops),
        m_focused(None),
        // Initialize all the desktops
        ALL_DESKTOPS(new AllDesktops()), 
        ICON_DESKTOP(new IconDesktop()),
        MOVING_DESKTOP(new MovingDesktop()), 
        RESIZING_DESKTOP(new ResizingDesktop())
    {
        m_desktops.add_category(ALL_DESKTOPS);
        m_desktops.add_category(ICON_DESKTOP);
        m_desktops.add_category(MOVING_DESKTOP);
        m_desktops.add_category(RESIZING_DESKTOP);

        for (unsigned long long desktop = 0; desktop < max_desktops;
                desktop++)
        {
            USER_DESKTOPS.push_back(new UserDesktop(desktop));
            m_desktops.add_category(USER_DESKTOPS[desktop]);
        }

        for (Layer layer = MIN_LAYER; layer <= MAX_LAYER; layer++)
            m_layers.add_category(layer);

        m_current_desktop = USER_DESKTOPS[0];
    }

    ~ClientModel()
    {
        flush_changes();
    }

    void flush_changes();
    change_iter changes_begin();
    change_iter changes_end();

    bool is_client(Window);
    bool is_visible(Window);

    void get_clients_of(desktop_ptr, std::vector<Window>&);
    void get_visible_clients(std::vector<Window>&);
    void get_visible_in_layer_order(std::vector<Window>&);
    
    void add_client(Window, InitialState, Dimension2D, Dimension2D);
    void remove_client(Window);

    void change_location(Window, Dimension, Dimension);
    void change_size(Window, Dimension, Dimension);

    void focus(Window);
    void unfocus();
    void unfocus_if_focused(Window);

    desktop_ptr find_desktop(Window);
    Layer find_layer(Window);

    void up_layer(Window);
    void down_layer(Window);
    void set_layer(Window, Layer);

    void toggle_stick(Window);
    void client_next_desktop(Window);
    void client_prev_desktop(Window);
    void next_desktop();
    void prev_desktop();

    void iconify(Window);
    void deiconify(Window);

    void start_moving(Window);
    void stop_moving(Window, Dimension2D);
    void start_resizing(Window);
    void stop_resizing(Window, Dimension2D);

protected:
    void push_change(change_ptr);
    void move_to_desktop(Window, desktop_ptr, bool);

private:
    /// A list of the changes made to the client data
    std::vector<change_ptr> m_changes;
    /// The maximum number of user-visible desktops
    unsigned long long m_max_desktops;

    /// A mapping between clients and their desktops
    UniqueMultimap<desktop_ptr, Window, 
        PointerLess<const Desktop>> m_desktops;
    /// A mapping between clients and the layers they inhabit
    UniqueMultimap<Layer, Window> m_layers;
    /// A mapping between clients and their locations
    std::map<Window, Dimension2D> m_location;
    /// A mapping between clients and their sizes
    std::map<Window, Dimension2D> m_size;

    /// The currently visible desktop
    user_desktop_ptr m_current_desktop;
    /// The currently focused client
    Window m_focused;
};

#endif
