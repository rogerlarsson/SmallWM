#include "clientmodel-events.h"

/**
 * Handles all the currently queued change events, returning when the
 * ClientModel change list is exhausted.
 */
void ClientModelEvents::handle_queued_changes()
{
    for (ClientModel::change_iter change_iter = model.changes_begin();
            change_iter != model.changes_end();
            change_iter++)
    {
        m_change = *change_iter;

        if (m_change->is_layer_change())
            handle_layer_change();
        else if (m_change->is_focus_change())
            handle_focus_change();
        else if (m_change->is_client_desktop_change())
            handle_client_desktop_change();
        else if (m_change->is_current_desktop_change())
            handle_current_desktop_change();
        else if (m_change->is_location_change())
            handle_location_change();
        else if (m_change->is_size_change())
            handle_size_change();
    }
}