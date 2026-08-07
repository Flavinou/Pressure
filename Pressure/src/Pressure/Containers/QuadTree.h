#pragma once

#include <glm/vec3.hpp>

namespace Pressure
{

	struct AABB
	{
		glm::vec3 Center;
		float HalfExtent;

		[[nodiscard]] bool Contains(const glm::vec3& point) const
		{
			return (point.x >= Center.x - HalfExtent && point.x <= Center.x + HalfExtent) &&
				   (point.y >= Center.y - HalfExtent && point.y <= Center.y + HalfExtent);
		}

		[[nodiscard]] bool Intersects(const AABB& other) const
		{
			return !(other.Center.x - other.HalfExtent > Center.x + HalfExtent ||
					 other.Center.x + other.HalfExtent < Center.x - HalfExtent ||
					 other.Center.y - other.HalfExtent > Center.y + HalfExtent ||
					 other.Center.y + other.HalfExtent < Center.y - HalfExtent);
		}
	};

	template<typename T>
	struct QuadTreeNode
	{
		static constexpr uint8_t MAX_NUMBER_CHILDREN = 4;
		static constexpr uint8_t MAX_NUMBER_ITEMS_THRESHOLD = 8;

		QuadTreeNode(glm::vec3 center, float halfExtent)
			: Center(center), HalfExtent(halfExtent) {
		}

		std::array<Scope<QuadTreeNode>, MAX_NUMBER_CHILDREN> Children{};
		std::vector<T> Items{};
		glm::vec3 Center;
		float HalfExtent;
	};

	/// Must work with either box colliders or circle colliders at the moment. 
	/// The item must have a way to get its AABB for querying.
	template<typename T>
	class QuadTree
	{
	public:
		QuadTree(const glm::vec3& center, float rootHalfExtent);
		~QuadTree() = default;

		void Clear();

		[[nodiscard]] bool Add(const T& item);
		[[nodiscard]] bool Remove(const T& item);

		void QueryRange(const AABB& range, std::vector<T>& outItems) const;

		void ForEachNode(const std::function<void(const QuadTreeNode<T>&)>& func) const
		{
			ForEachNodeRecursive(m_RootNode, func);
		}

	private:
		static constexpr uint8_t ABSOLUTE_MAX_DEPTH = 8;

		Scope<QuadTreeNode<T>> m_RootNode{nullptr};

	private:
		Scope<QuadTreeNode<T>> CreateNode(glm::vec3 center, float halfExtent)
		{
			return CreateScope<QuadTreeNode<T>>( center, halfExtent);
		}

		bool Add(const T& item, const AABB& aabb)
		{
			if (!m_RootNode)
				return false;

			return AddToNode(m_RootNode, item, aabb, 0);
		}

		bool AddToNode(const Scope<QuadTreeNode<T>>& node, const T& item, const AABB& aabb, uint8_t depth)
		{
			if (!node)
				return false;

			// If the item is not within the bounds of this node, it cannot be added
			AABB bounds = { node->Center, node->HalfExtent };
			if (!bounds.Contains(item.GetCenter()))
				return false;

			if (!node->Children[0])
			{
				// If there is space in this quad tree and if it doesn't have subdivisions, add the object here
				if (node->Items.size() < QuadTreeNode<T>::MAX_NUMBER_ITEMS_THRESHOLD || depth >= ABSOLUTE_MAX_DEPTH)
				{
					node->Items.push_back(item);
					return true;
				}

				// Otherwise threshold reached, subdivide the current node
				SubdivideNode(node);

				std::vector<T> itemsToReinsert = std::move(node->Items);
				node->Items.clear();

				for (const auto& existingItem : itemsToReinsert)
				{
					AABB existingAABB{ existingItem.GetCenter(), existingItem.GetHalfExtent() };
					bool reinserted = false;
					for (const auto& child : node->Children)
					{
						if (AddToNode(child, existingItem, existingAABB, depth + 1))
						{
							reinserted = true;
							break;
						}
					}
					if (!reinserted)
					{
						// If the existing item could not be reinserted into any child, add it
						node->Items.push_back(existingItem);
					}
				}
			}

			// Try to add the node the first child accepting it
			for (const auto& child : node->Children)
			{
				if (AddToNode(child, item, aabb, depth + 1))
					return true;
			}

			// Node cannot be inserted for some reason, should never happen
			return false;
		}

		bool RemoveFromNode(const Scope<QuadTreeNode<T>>& node, const T& item)
		{
			if (!node)
				return false;

			auto it = std::find(node->Items.begin(), node->Items.end(), item);
			if (it != node->Items.end())
			{
				node->Items.erase(it);
				return true;
			}

			for (const auto& child : node->Children)
			{
				if (RemoveFromNode(child, item))
					return true;
			}

			return false;
		}

		void SubdivideNode(const Scope<QuadTreeNode<T>>& node)
		{
			float quarterExtent = node->HalfExtent * 0.5f;
			node->Children[0] = CreateNode(node->Center + glm::vec3(-quarterExtent, -quarterExtent, 0.0f), quarterExtent);
			node->Children[1] = CreateNode(node->Center + glm::vec3(quarterExtent, -quarterExtent, 0.0f), quarterExtent);
			node->Children[2] = CreateNode(node->Center + glm::vec3(-quarterExtent, quarterExtent, 0.0f), quarterExtent);
			node->Children[3] = CreateNode(node->Center + glm::vec3(quarterExtent, quarterExtent, 0.0f), quarterExtent);
		}

		void QueryNode(const Scope<QuadTreeNode<T>>& node, const AABB& range, std::vector<T>& outItems) const
		{
			if (!node)
				return;

			for (auto& item : node->Items)
			{
				// Assuming T has a method to get its AABB, no physical precision involved here
				if (range.Contains(item.GetCenter()))
				{
					outItems.push_back(item);
				}
			}

			if (!node->Children[0])
				return;

			for (const auto& child : node->Children)
			{
				QueryNode(child, range, outItems);
			}
		}

		void ForEachNodeRecursive(const Scope<QuadTreeNode<T>>& node, const std::function<void(const QuadTreeNode<T>&)>& func) const
		{
			if (!node)
				return;

			func(*node);

			for (const auto& child : node->Children)
			{
				ForEachNodeRecursive(child, func);
			}
		}
	};

	template <typename T>
	QuadTree<T>::QuadTree(const glm::vec3& center, float rootHalfExtent)
	{
		m_RootNode = CreateNode(center, rootHalfExtent);
	}

	template <typename T>
	void QuadTree<T>::Clear()
	{
		if (m_RootNode)
		{
			m_RootNode->Items.clear();
			for (auto& child : m_RootNode->Children)
			{
				child.reset();
			}
		}
	}

	template <typename T>
	bool QuadTree<T>::Add(const T& item)
	{
		AABB aabb{ item.GetCenter(), item.GetHalfExtent() };
		return Add(item, aabb);
	}

	template <typename T>
	bool QuadTree<T>::Remove(const T& item)
	{
		if (!m_RootNode)
			return false;

		return RemoveFromNode(m_RootNode, item);
	}

	template <typename T>
	void QuadTree<T>::QueryRange(const AABB& range, std::vector<T>& outItems) const
	{
		AABB rootAABB{ m_RootNode->Center, m_RootNode->HalfExtent };
		if (!rootAABB.Intersects(range))
			return;

		QueryNode(m_RootNode, range, outItems);
	}
}
