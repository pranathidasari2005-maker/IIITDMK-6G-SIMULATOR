#ifndef NS3_SIXG_NODE_H
#define NS3_SIXG_NODE_H

#include "ns3/6g-resource.h"

#include "ns3/object.h"
#include "ns3/vector.h"

#include <string>
#include <vector>

namespace ns3
{

/**
 * \ingroup network-nodes
 *
 * Base abstraction for a 6G-LENA network node.
 */
class SixGNode : public Object
{
  public:
    /**
     * \brief Get the TypeId of this class.
     */
    static TypeId GetTypeId();

    SixGNode();
    ~SixGNode() override;

    /**
     * \brief Initialize the 6G node.
     */
    virtual void Initialize();

    /**
     * \brief Check whether the node has been initialized.
     */
    bool IsInitialized() const;

    /**
     * \brief Set the unique identifier of this 6G node.
     */
    void SetNodeId(const std::string& nodeId);

    /**
     * \brief Get the unique identifier of this 6G node.
     */
    std::string GetNodeId() const;

    /**
     * \brief Set the current simulated position of this node.
     */
    void SetPosition(const Vector& position);

    /**
     * \brief Get the current simulated position of this node.
     */
    Vector GetPosition() const;

    /**
     * \brief Set whether this node is currently active.
     */
    void SetActive(bool active);

    /**
     * \brief Check whether the node is currently active.
     */
    bool IsActive() const;

    /**
     * \brief Add a resource to this node.
     */
    bool AddResource(Ptr<SixGResource> resource);

    /**
     * \brief Get a resource by type.
     */
    Ptr<SixGResource> GetResource(const std::string& type) const;

    /**
     * \brief Check whether a resource of the given type exists.
     */
    bool HasResource(const std::string& type) const;

    /**
     * \brief Remove a resource by type.
     */
    bool RemoveResource(const std::string& type);

    /**
     * \brief Get all resources owned by this node.
     */
    const std::vector<Ptr<SixGResource>>& GetResources() const;

  private:
    bool m_initialized;
    std::string m_nodeId;
    Vector m_position;
    bool m_active;
    std::vector<Ptr<SixGResource>> m_resources;
};

} // namespace ns3

#endif // NS3_SIXG_NODE_H
