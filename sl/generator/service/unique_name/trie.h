#pragma once

namespace sunlight
{
    class Trie
    {
        struct Node
        {
            wchar_t character = {};
            bool end = false;

            Node* parent = nullptr;
            std::unordered_map<wchar_t, UniquePtrNotNull<Node>> children;
        };

    public:
        Trie(const Trie& other) = delete;
        Trie& operator=(const Trie& other) = delete;

        Trie(Trie&& other) noexcept = default;
        Trie& operator=(Trie&& other) noexcept = default;

    public:
        Trie();
        ~Trie();

        bool Empty() const;

        bool Contains(const std::wstring& str) const;
        bool Insert(const std::wstring& str) const;
        bool Remove(const std::wstring& str) const;

    private:
        UniquePtrNotNull<Node> _root;
    };
}
