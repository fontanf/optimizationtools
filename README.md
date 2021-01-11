# OptimizationTools

## Collections

### IndexedBinaryHeap

Scholar and efficient implementation of a binary heap.
Elements are indices between `0` and `n`.
The main advantage compared to the `std::priority_queue` is the possibility of updating the value of an element, which is required to efficiently implement Dijkstra or Kruskal algorithms.

### IndexedSet

A set implementation.
Elements are indices between `0` and `n`.

* Initialize: `O(n)`
* Get size: `O(1)`
* Check if contains an element: `O(1)`
* Add an element: `O(1)`
* Remove an element: `O(1)`
* Clear: `O(1)`
* Loop through elements inside the set: `O(|set|)`
* Loop through elements outside the set: `O(n - |set|)`

### IndexedMap

A map implementation.
Elements are indices between `0` and `n`.

* Initialize: `O(n)`
* Get size: `O(1)`
* Check if contains an element: `O(1)`
* Get the value an element: `O(1)`
* Add an element: `O(1)`
* Remove an element: `O(1)`
* Clear: `O(1)`
* Loop through elements inside the map: `O(|map|)`
* Loop through elements outside the map: `O(n - |map|)`

### DoublyIndexedMap

A map implementation.
Elements are indices between `0` and `n`.
Values are indices between `0` and `m`.

* Initialize: `O(n + m)`
* Get size: `O(1)`
* Check if contains an element: `O(1)`
* Get the value an element: `O(1)`
* Add an element: `O(1)`
* Remove an element: `O(1)`
* Get the number of elements with a certain value: `O(1)`
* Loop through elements with a certain value: `O(number of elements having this value)`
* Loop through element outside the map: `O(n - |map|)`
* Get the number of values taken by at least one element: `O(1)`
* Loop through values taken by at least one element: `O(number of values taken by at least one element)`

### SortedOnDemandArray

An array for which it is possible to request the `i`th smallest element without requiring to sort the whole array. It can be faster than a complete sorting if only a small fraction of the elements are requested.

## Utils

* Bob Floyd's algorithm to draw `k` different integers in `[0, u]`. Typically used to draw seeds for each thread of a parallel algorithm from the initial seed.
* A function to split a string according to a given separator. Useful to read `csv` files.

## Info

Various tools for optimization algorithms (logger, thread safe printer on standard output, writer for output files in `json` format, time counter and checker, solution counter, certificate writer)

