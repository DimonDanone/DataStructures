#pragma onces
#include "pointForRpTree.h"
#include "rpTreeNode.h"

#include <queue>

namespace NSrpForest {

    /*!
     * \brief Шаблон класса rpTree, NumericType - тип данных Point
     * \param[in] NumericType hui
    * */
    template <typename NumericType>
    class RpTree {
    public:
        RpTree() = default;
        RpTree(const RpTree& old) {
            delete start;
            start = new RpTreeNode(*old.start);
            Ns = old.Ns;
        }

        ~RpTree() { delete start; }

        /*!
         * \brief Создание RpTree на основе выборки (U)
        */
        explicit RpTree(const std::set<Point<NumericType>>& U, int min_W_size)
            : Ns(min_W_size)
        {
            start = new RpTreeNode(U, min_W_size);
        }

        std::set<Point<NumericType>> FindKnn(const Point<NumericType>& point) {
            return start->TreeDownhill(point);
        }

        void WriteTreeTo(std::ofstream& file) const {
            int Ns_copy = Ns;
            file.write(reinterpret_cast<const char*>(&Ns_copy), sizeof(Ns_copy));
            start->WriteNodeTo(file);
        }

        void ReadTreeFrom(std::ifstream& file) {
            file.read(reinterpret_cast<char*>(&Ns), sizeof(Ns));
            delete start;
            start = new RpTreeNode<NumericType>;
            start->ReadNodeFrom(file);
        }

    private:
        int Ns{1};
        RpTreeNode<NumericType>* start{nullptr};
    };

};

#ifndef RPFOREST_RPTREE_H
#define RPFOREST_RPTREE_H

#endif //RPFOREST_RPTREE_H
