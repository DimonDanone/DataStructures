#include <iostream>
#include <algorithm>
#include <cmath>
#include <memory>
#include <random>
#include <sstream>

struct PersistentTreap {

   PersistentTreap() = default;

   PersistentTreap(PersistentTreap* cur) {
      if (!cur) {
         return;
      }
      left = cur->left;
      right = cur->right;
      sum = cur->sum;
      val = cur->val;
      size = cur->size;
      add = cur->add;
   }

   PersistentTreap(int x) {
      sum = x;
      val = x;
      size = 1;
   }

   void Del() {
      link--;
      if (link <= 0) {
         if (left != nullptr)
            left->Del();
         if (right != nullptr)
            right->Del();
         delete this;
      }
   }

   int link = 0;
   int size = 0;
   long long sum = 0, val = 0, add = 0;

   PersistentTreap *left = nullptr;
   PersistentTreap* right = nullptr;

};

int getSize(PersistentTreap* root) {
   if (!root) {
      return 0;
   }

   return root->size;
}

long long getSum(PersistentTreap* root) {
   if (!root) {
      return 0;
   }

   return root->sum;
}

void update(PersistentTreap* root) {
   if (!root) {
      return;
   }

   root->val += root->add;

   root->size = 1 + getSize(root->left) + getSize(root->right);
   root->sum = root->val + getSum(root->left) + getSum(root->right);

   if (root->left != nullptr) {
      root->left->add += root->add;
      root->sum += root->left->size * root->left->add;
   }
   if (root->right != nullptr) {
      root->right->add += root->add;
      root->sum += root->right->size * root->right->add;
   }
}


void addLink(PersistentTreap* root) {
   if (!root) {
      return;
   }

   root->link++;
}

void DelNode(PersistentTreap* root) {
   if (!root) {
      return;
   }

   root->Del();
}

void Split(PersistentTreap* root, PersistentTreap* &L, PersistentTreap* &R, int size) {
   if (!root) {
      L = R = nullptr;
      return;
   }

   PersistentTreap* cur = new PersistentTreap(root);
   if (getSize(cur->left) + 1 <= size) {
      Split(cur->right, cur->right, R, size - getSize(cur->left) - 1);
      addLink(cur->left);
      addLink(cur->right);
      L = cur;
   } else {
      Split(cur->left, L, cur->left, size);
      addLink(cur->left);
      addLink(cur->right);
      R = cur;
   }
   update(L);
   update(R);
}

PersistentTreap* Merge(PersistentTreap* L, PersistentTreap* R) {
   PersistentTreap* ptrNode = nullptr;
   if (!L || !R) {
      ptrNode = new PersistentTreap((!L ? R : L));
      addLink(ptrNode->left);
      addLink(ptrNode->right);
      return ptrNode;
   }

   int l = getSize(L),  r = getSize(R), rang = rand() % (l + r);
   if (rang > r) {
      ptrNode = new PersistentTreap(L);
      ptrNode->right = Merge(ptrNode->right, R);
      addLink(ptrNode->left);
      addLink(ptrNode->right);
      update(ptrNode);
      return ptrNode;
   } else {
      ptrNode = new PersistentTreap(R);
      ptrNode->left = Merge(L, ptrNode->left);
      addLink(ptrNode->left);
      addLink(ptrNode->right);
      update(ptrNode);
      return ptrNode;
   }
}

PersistentTreap* Insert(PersistentTreap* root, int index, int x) {

   PersistentTreap* L;
   PersistentTreap* R;
   Split(root, L, R, index);
   PersistentTreap* new_ = new PersistentTreap(x);

   PersistentTreap* res = Merge(Merge(L, new_), R);
   if (L) {
      L->Del();
   }
   if (R) {
      R->Del();
   }

   return res;
}

PersistentTreap* Remove(PersistentTreap* root, int index) {

   PersistentTreap* L;
   PersistentTreap* R;
   Split(root, L, R, index + 1);

   PersistentTreap *new_L, *new_R;
   Split(L, new_L, new_R, index);

   PersistentTreap* res = Merge(new_L, R);
   if (L) {
      L->Del();
   }
   if (R) {
      R->Del();
   }
   if (new_L) {
      new_L->Del();
   }
   if (new_R) {
      new_R->Del();
   }

   return res;
}

long long GetSum(PersistentTreap* root, int l, int r) {
   update(root);

   PersistentTreap* L;
   PersistentTreap* R;
   Split(root, L, R, r + 1);

   PersistentTreap *new_L, *new_R;
   Split(L, new_L, new_R, l);

   long long res = new_R->sum;

   if (L) {
      L->Del();
   }
   if (R) {
      R->Del();
   }
   if (new_L) {
      new_L->Del();
   }
   if (new_R) {
      new_R->Del();
   }

   return res;
}

void Search(PersistentTreap* now_root, std::ostream& out) {
   if (now_root->left != nullptr) {
      Search(now_root->left, out);
   }

   out << now_root->val << " ";

   if (now_root->right != nullptr) {
      Search(now_root->right, out);
   }
}

std::ostream& PrintSegment(std::ostream& out, PersistentTreap* root, int l, int r) {
   PersistentTreap* L;
   PersistentTreap* R;
   Split(root, L, R, r + 1);

   PersistentTreap *new_L, *new_R;
   Split(L, new_L, new_R, l);

   Search(new_R, out);

   if (L) {
      L->Del();
   }
   if (R) {
      R->Del();
   }
   if (new_L) {
      new_L->Del();
   }
   if (new_R) {
      new_R->Del();
   }

   return out;
}