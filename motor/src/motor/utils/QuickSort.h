#ifndef __QUICKSORT_H__
#define __QUICKSORT_H__

#include "../core/Defines.h"

class QuickSort {
public:
	template <typename T>
		static T* sort( T* nodes, const int startIndex, const int endIndex );
	template <typename T>
		static int partition( T* nodes, int left, int right );
};

template <typename T>
T* QuickSort::sort( T* nodes, const int startIndex, const int endIndex ) {
	if( startIndex < endIndex ) {
		int splitPoint = partition( nodes, startIndex, endIndex );
		sort( nodes, startIndex, splitPoint - 1 );
		sort( nodes, splitPoint + 1, endIndex );
	}

	return nodes;
}

template <typename T>
int QuickSort::partition( T* nodes, int left, int right ) {
	float pivot = nodes[left]->mQuickSortValue;
	T temp = nullptr;

	while( left < right ) {
		while( nodes[right]->mQuickSortValue > pivot ) {
			--right;
		}
		while( nodes[left]->mQuickSortValue < pivot ) {
			++left;
		}

		if( nodes[left]->mQuickSortValue == nodes[right]->mQuickSortValue )
			--right;
		else if( left < right ) {
			temp = nodes[right];
			nodes[right] = nodes[left];
			nodes[left] = temp;
		}
	}

	return left;
}

#endif // __QUICKSORT_H__
