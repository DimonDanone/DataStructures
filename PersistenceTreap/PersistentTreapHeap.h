#include "treap.h"
#include <vector>

class PTHeap {
public:
   PTHeap() = default;

   PTHeap(PersistentTreap* first) {
      heap.push_back(first);
   }

   void InsertToTreap(int index, int val) {
      heap.push_back(Insert(heap.back(), index, val));
   }

   void RemoveFromTreap(int index) {
      heap.push_back(Remove(heap.back(), index));
   }

   long long GetSumFromTreap(int l, int r) {
      return GetSum(heap.back(), l, r);
   }

   std::ostream& PrintTreapSegment(std::ostream& out, int l, int r) {
      PrintSegment(out, heap.back(), l, r);

      return out;
   }

   long long GetSize() {
       return getSize(heap.back());
   }

   void CancelOperations(int count) {
      for (int i = 0; i < count; ++i) {
         heap.back()->Del();
         heap.pop_back();
      }
   }

private:
   std::vector<PersistentTreap*> heap;

};

#ifndef TREAP_PERSISTENTTREAPHEAP_H
#define TREAP_PERSISTENTTREAPHEAP_H

#endif //TREAP_PERSISTENTTREAPHEAP_H
