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
    explicit Node(char c=0)
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

    public:
        ConstIterator(const TrieMap& owner, const char * prefix = "");
        ~ConstIterator();

        operator bool() const;
        ConstIterator& operator++();

        const char * key() const;
        std::shared_ptr<T> value() const;
    };


};
