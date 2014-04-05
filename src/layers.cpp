/** @file */
#include "clientmanager.h"

/**
 * Raises a client to the layer above it.
 * @param window The client to raise
 */
void LayerManager::raise_layer(Window window)
{
    if (!m_clients->is_client(window))
        return;

    Layer current = m_layers[window];
    if (current < MAX_LAYER)
    {
        m_layers[window]++;
        relayer_clients();
    }
}

/**
 * Lowers a client to the layer below it.
 * @param window The client to lower
 */
void LayerManager::lower_layer(Window window)
{
    if (!m_clients->is_client(window))
        return;

    Layer current = m_layers[window];
    if (current > MIN_LAYER)
    {
        m_layers[window]--;
        relayer_clients();
    }
}

/**
 * Sets the layer of a client.
 * @param window The window of a client
 * @param layer The layer to set the client to
 */
void LayerManager::set_layer(Window window, Layer layer)
{
    if (!m_clients->is_client(window))
        return;

    m_layers[window] = layer;
    relayer_clients();
}

/**
 * Removes a client from the layer list.
 * @param window The client window to remove.
 */
void LayerManager::delete_layer(Window window)
{
    m_layers.erase(window);
}

/**
 * Relayers all clients which are currently CS_VISIBLE or CS_ACTIVE. Puts all
 * CS_ICON clients, or the placeholder windows for any CS_MOVING or CS_RESIZING
 * clients on the very top.
 */
void LayerManager::relayer_clients()
{
    // First, only layer the 'normal' windows which are not icons or placeholders.
    std::vector<Window> clients;
    for (std::map<Window,ClientState>::iterator client_iter = 
                m_clients->clients_begin();
            client_iter != m_clients->clients_end();
            client_iter++)
    {
        if (client_iter->second == CS_VISIBLE || client_iter->second == CS_ACTIVE)
            clients.push_back(client_iter->first);
    }

    MappedVectorSorter<Window,Layer> sorter(m_layers, true);
    std::sort(clients.begin(), clients.end(), sorter);
    XRestackWindows(m_shared.display, &clients[0], clients.size());
}
