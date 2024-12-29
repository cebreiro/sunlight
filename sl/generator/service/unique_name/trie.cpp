#include "trie.h"

namespace sunlight
{
    Trie::Trie()
        : _root(std::make_unique<Node>())
    {
    }

    Trie::~Trie()
    {
    }

    bool Trie::Empty() const
    {
        return _root->children.empty();
    }

    bool Trie::Contains(const std::wstring& str) const
    {
        if (str.empty())
        {
            return false;
        }

        Node* node = _root.get();

        for (const wchar_t character : str)
        {
            const auto iter = node->children.find(character);
            if (iter == node->children.end())
            {
                return false;
            }

            node = iter->second.get();
        }

        return node->end;
    }

    bool Trie::Insert(const std::wstring& str) const
    {
        if (str.empty())
        {
            return false;
        }

        Node* node = _root.get();

        for (const wchar_t character : str)
        {
            UniquePtrNotNull<Node>& current = node->children[character];

            if (!current)
            {
                current = std::make_unique<Node>();
                current->character = character;
                current->parent = node;
            }

            node = current.get();
        }

        if (node->end)
        {
            // already exists

            return false;
        }

        node->end = true;

        return true;
    }

    bool Trie::Remove(const std::wstring& str) const
    {
        if (str.empty())
        {
            return false;
        }

        Node* last = [&]() -> Node*
            {
                Node* node = _root.get();

                for (const wchar_t character : str)
                {
                    const auto iter = node->children.find(character);
                    if (iter == node->children.end())
                    {
                        return nullptr;
                    }

                    node = iter->second.get();
                }

                return node;
            }();

        if (!last)
        {
            return false;
        }

        assert(last->end);

        Node* current = last;
        Node* parent = current->parent;

        while (parent)
        {
            const wchar_t character = current->character;

            [[maybe_unused]]
            const size_t count = parent->children.erase(character);
            assert(count > 0);

            if (parent->end || !parent->children.empty())
            {
                break;
            }

            current = parent;
            parent = parent->parent;
        }

        return true;
    }
}
