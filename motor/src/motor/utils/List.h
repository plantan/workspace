#ifndef __LIST_H__
#define __LIST_H__

template <typename T>
class ListNode {
public:
	explicit ListNode();
    virtual ~ListNode() {};

	T* mpNextNode;
    T* mpPrevNode;
	bool mAdded;
};

template <typename T>
inline ListNode<T>::ListNode() :
mpNextNode(0),
mpPrevNode(0),
mAdded(0) {}

template <typename T>
class List {
public:
	explicit List();
	~List() {};

	void add( T* pNode );
	void remove( T* pNode );
	void clear();

    T* mpFirstNode;
    T* mpLastNode;
	unsigned int mNumNodes;
};

template <typename T>
inline List<T>::List() :
mpFirstNode(0),
mpLastNode(0),
mNumNodes(0) {}

template <typename T>
inline void List<T>::add( T* pNode ) {
    if( pNode->mAdded ) {
        return;
    } else if( !mNumNodes ) {
       mpFirstNode = mpLastNode = pNode;
    } else {
        mpLastNode->mpNextNode = pNode;
        pNode->mpPrevNode = mpLastNode;
        mpLastNode = pNode;
    }

    pNode->mAdded = true;
    ++mNumNodes;
}

template <typename T>
inline void List<T>::remove( T* pNode ) {
    if( !mNumNodes ) {
        return;
    } else if( mNumNodes == 1 ) {
        mpFirstNode = mpLastNode = 0;
    } else if( pNode == mpLastNode ) {
        mpLastNode = mpLastNode->mpPrevNode;
        mpLastNode->mpNextNode = 0;
    } else {
        pNode->mpNextNode->mpPrevNode = pNode->mpPrevNode;
        pNode->mpPrevNode->mpNextNode = pNode->mpNextNode;
    }

    pNode->mAdded = false;
    pNode->mpNextNode = pNode->mpPrevNode = 0;
    --mNumNodes;
}

template <typename T>
inline void List<T>::clear() {
    T* pNode = mpLastNode;
    T* pTemp = 0;
    while( pNode ) {
        pTemp = pNode->mpPrevNode;
        pNode->mpNextNode = 0;
        pNode->mpPrevNode = 0;
        pNode = pTemp;
    }

    mpFirstNode = mpLastNode = 0;
}

#endif // __LIST_H__
