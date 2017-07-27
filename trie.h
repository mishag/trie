#include <map>
#include <memory>
#include <cassert>
#include <iostream>

template <typename T>
class Node;

template <typename T>
class TrieMap;

// ===========
// class Node
// ===========

template <typename T>
class Node {

friend class TrieMap<T>;

private:
    typedef std::map<char, Node*> Children;
    typedef std::shared_ptr<T>    TPtr;

    char      d_char;
    Children  d_childrenMap;
    Node     *d_parent;
    TPtr      d_value;
    bool      d_terminal;

    // Not Implemented
    Node(const Node&);
    Node& operator=(const Node&);

public:
    explicit Node(char c = 0)
        : d_char(c)
        , d_childrenMap()
        , d_parent(nullptr)
        , d_value()
        , d_terminal(false)
    {
    }

    ~Node() {
        d_parent = nullptr;
        d_value.reset();
        auto it = d_childrenMap.begin();
        auto ite = d_childrenMap.end();

        for (; it != ite; ++it) {
            delete it->second;
        }
    }

    char& chr() { return d_char; }
    const char& chr() const { return d_char; }

    const TPtr value() const  { return d_value; }
    void setValue(const TPtr& val) { d_value = val; }

    void setParent(Node * parent) { d_parent = parent; }

    Node * parent()  { return d_parent; }
    const Node * parent() const { return d_parent; }

    bool& terminal() { return d_terminal; }
    const bool& terminal() const { return d_terminal; }

    const Children& children() const { return d_childrenMap; }
    Children& children() { return d_childrenMap; }
};

template <typename T>
std::ostream& operator<<(std::ostream& o, const Node<T>& node)
{
    o << " [ char: " << node.chr() << " terminal: " << node.terminal()
      << " parent: " << node.parent() << " value: [ ";

    if (node.value()) {
        o << *(node.value());
    }

    o << " ] ] ";

    return o;
}


// =============
// class TrieMap
// =============

template <typename T>
class TrieMap {

    friend class ConstIterator;

private:
    Node<T>   *d_root;
    size_t     d_size;

    Node<T> * advanceToNode(const char * key) const
    {
        Node<T> * currentNodePtr = d_root;

        for (size_t i = 0; key[i]; ++i) {
            const auto& children = currentNodePtr->children();
            auto childit = children.find(key[i]);

            if (childit == children.end()) {
                return nullptr;
            }

            currentNodePtr = childit->second;
        }

        assert(currentNodePtr);

        return currentNodePtr;
    }

public:
    TrieMap()
        : d_root(new Node<T>())
        , d_size(0)
    {
    }

    std::shared_ptr<T> find(const char * key) const
    {
        Node<T> * node = advanceToNode(key);

        if (!node) {
            return std::shared_ptr<T>();
        }

        return node->terminal() ? node->value() : std::shared_ptr<T>();
    }

    size_t count(const char * key) const
    {
        Node<T> * node =  advanceToNode(key);

        if (!node) {
            return 0;
        }

        return node->terminal() ? 1 : 0;
    }

    std::shared_ptr<T> insert(const char * key,
                              const std::shared_ptr<T>& value,
                              bool replaceExisting = false)
    {
        Node<T> * currentNodePtr = d_root;

        for (size_t i = 0; key[i]; ++i) {
            auto& children = currentNodePtr->children();
            auto childit = children.find(key[i]);

            if (childit == children.end()) {
                Node<T> * newNode = new Node<T>(key[i]);
                newNode->setParent(currentNodePtr);
                children[key[i]] = newNode;
                currentNodePtr = newNode;
                continue;
            }

            currentNodePtr = childit->second;
        }

        assert(currentNodePtr);

        if (currentNodePtr->terminal() && !replaceExisting) {
            return currentNodePtr->value();
        }

        currentNodePtr->terminal() = true;
        currentNodePtr->setValue(value);

        ++d_size;

        return currentNodePtr->value();
    }

    ~TrieMap()
    {
        d_size = 0;
        delete d_root;
    }

    size_t size() const { return d_size; }


    // ==========
    // Iterators
    // ==========

    class ConstIterator {
    private:
        Node<T>       *d_currentNode;
        std::string    d_currentKey;

        ConstIterator& nextNode()
        {
            // Pre: d_currentNode is pointing to a valid node
            // Post: d_currentNode advanced to the next valid node or
            // becomes null in case iteration is done

            // std::cout << "Calling nextNode on " << *d_currentNode << "\n";

            if (d_currentNode->chr() == 0) {   // root node
                if (d_currentNode->children().empty()) {
                    d_currentNode = nullptr;
                    d_currentKey.clear();
                    return *this;
                }

                auto it = d_currentNode->children().begin();
                d_currentNode = it->second;
                d_currentKey += it->first;
                return *this;
            }

            // non root node

            if (!d_currentNode->children().empty()) {
                auto it = d_currentNode->children().begin();
                d_currentNode = it->second;
                d_currentKey += it->first;
                return *this;
            }

            // Non root node but children are empty:
            // Move up the tree until we have a next available child or until we
            // reach root node with no more children available for traversing.

            for (;;) {
                char curChar = d_currentNode->chr();
                Node<T> * parent = d_currentNode->parent();
                auto curCharIt = parent->children().find(curChar);
                auto nextCharIt = ++curCharIt;

                bool nextCharItValid = nextCharIt != parent->children().end();

                if (!parent->chr() && !nextCharItValid) {
                    
                    // root node but no more children.
                    d_currentNode = nullptr;
                    d_currentKey.clear();
                    return *this;
                }

                if (!nextCharItValid) {
                    d_currentNode = parent;
                    assert(d_currentNode->chr() != 0);
                    d_currentKey.pop_back();
                    continue;
                }

                d_currentNode = nextCharIt->second;
                d_currentKey.pop_back();
                d_currentKey += nextCharIt->first;
                return *this;
            }

            // Should not reach here
            return *this;
        }

    public:
        ConstIterator(const TrieMap& owner, const char * prefix = "")
            : d_currentNode(owner.d_root)
            , d_currentKey(prefix)
        {
            assert (prefix != nullptr);

            if (prefix[0] != '\0') {
                d_currentNode = owner.advanceToNode(prefix);
            }

            if (!d_currentNode) {
                d_currentKey.clear();
                return;
            }

            if (!d_currentNode->terminal()) {
                this->operator++();
            }
        }

        ~ConstIterator() {}

        ConstIterator(const ConstIterator& other)
            : d_currentNode(other.d_currentNode)
            , d_currentKey(other)
        { }

        ConstIterator& operator=(const ConstIterator& other)
        {
            if (this == &other) {
                return *this;
            }

            d_currentNode = other.d_currentNode;
            d_currentKey = other.d_currentKey;
        }

        operator bool() const
        {
            return d_currentNode != nullptr;
        }

        ConstIterator& operator++()
        {
            do {
                this->nextNode();
            } while (d_currentNode && !d_currentNode->terminal());

            return *this;
        }

        const char * key() const { return d_currentKey.c_str(); }
        std::shared_ptr<T> value() const { return d_currentNode->value(); }
    };


};
