#pragma once

#include <future>
#include <mutex>
#include "rpTree.h"


namespace NSrpForest {

    class RpForestExperssion {
    public:
        RpForestExperssion(const std::string& error_m)
                : message(error_m)
        {}

        std::string GetError() { return message; }

    private:
        std::string message{""};
    };

    template<typename NumericType>
    class RpForest {
    public:
        RpForest() = default;

        /*!
         * \brief Конструктор леса на основе тренировочной выборки
         * \param[in] train - тренировочная выборка
         * \param[in] how_much - кол-во деревьев в лесу 
        */
        RpForest(const std::set<Point<NumericType>>& train, int how_much)
                : U(train)
                , how_much_trees_in_forest(how_much)
        {
            int leaf_size = train.size() * 0.01 > 2 ? train.size() * 0.01 : 2;
            for (int i = 0; i < how_much_trees_in_forest; ++i) {
                forest.push_back(RpTree(U, leaf_size));
            }
        }

        /*!
         * \brief Конструктор леса на основк тренировочной выборки (многопоточный)
         * \param[in] train - тренировочная выборка
         * \param[in] how_much - кол-во деревьев в лесу
         * \param[in] thread_count - кол-во потоков
        */
        RpForest(const std::set<Point<NumericType>> &train, int how_much, int thread_count);


        /*!
         * \brief Поиск Knn для точки
         * param[in] point_q - точка, для которой ищется knn
         * param[in] k - кол-во соседей
        */
        std::vector<Point<NumericType>> KnnForPoint(const Point<NumericType>& point_q, int k) {
            std::set<Point<NumericType>> all_knn;

            for (auto& tree : forest) {
                auto knn_for_tree = tree.FindKnn(point_q);
                for (const auto& k_point : knn_for_tree) {
                    all_knn.insert(k_point);
                }
            }

            std::vector<Point<NumericType>> res(all_knn.size());
            int now_pos = 0;
            for (auto& p : all_knn) {
                res[now_pos] = p;
                now_pos++;
            }

            std::sort(res.begin(), res.end(),
                      [point_q](const Point<NumericType>& first, const Point<NumericType> &second) {
                          return Distance(first, point_q) < Distance(second, point_q);
                      });

            if (res.size() > k) {
                res.resize(k);
            }

            return res;
        }

        /*!
         * \brief Запись леса в файл
         * \param[in] file - файл, в который ведется запись
        */
        void WriteForestTo(std::ofstream& file) const {
            int Usize = U.size();
            file.write(reinterpret_cast<const char*>(&Usize), sizeof(Usize));
            for (const auto& point : U) {
                point.WritePointTo(file);
            }

            int hmtif = how_much_trees_in_forest;
            file.write(reinterpret_cast<const char*>(&hmtif), sizeof(how_much_trees_in_forest));
            for (int i = 0; i < how_much_trees_in_forest; ++i) {
                forest[i].WriteTreeTo(file);
            }
        }

        /*!
         * \brief Чтение леса из файла
         * \param[in] file - файл, из которого ведется чтение
        */
        void ReadForestFrom(std::ifstream& file) {
            U.clear();
            forest.clear();

            int Usize;
            file.read(reinterpret_cast<char*>(&Usize), sizeof(Usize));
            for (int i = 0; i < Usize; ++i) {
                Point<NumericType> point;
                point.ReadPointFrom(file);
                U.insert(point);
            }

            file.read(reinterpret_cast<char*>(&how_much_trees_in_forest), sizeof(how_much_trees_in_forest));
            for (int i = 0; i < how_much_trees_in_forest; ++i) {
                RpTree<NumericType> tree;
                tree.ReadTreeFrom(file);
                forest.push_back(tree);
            }
        }

    private:
        std::set<Point<NumericType>> U;
        int how_much_trees_in_forest{1};
        std::vector<RpTree<NumericType>> forest;
        std::mutex m_;

        static void MakeTrees(RpForest<NumericType>* now_forest, const std::set<Point<NumericType>> &train, int trees_count);

    };

    template <typename NumericType>
    void RpForest<NumericType>::MakeTrees(RpForest<NumericType>* now_forest, const std::set<Point<NumericType>>& train, int trees_count) {
        int leaf_size = train.size() * 0.01 > 2 ? train.size() * 0.01 : 2;
        for (int i = 0; i < trees_count; ++i) {
            auto new_tree = RpTree<NumericType>(train, leaf_size);
            std::lock_guard<std::mutex> locker(now_forest->m_);
            now_forest->forest.push_back(new_tree);
        }
    }

    template <typename NumericType>
    RpForest<NumericType>::RpForest(const std::set<Point<NumericType>>& train, int how_much, int thread_count)
        : U(train)
        , how_much_trees_in_forest(how_much)
    {
        if (thread_count <= 0) {
            throw RpForestExperssion("min count of threads is 1!!");
        }

        int size_for_one_thread = how_much_trees_in_forest / thread_count;
        while (size_for_one_thread == 0) {
            thread_count--;
            size_for_one_thread = how_much_trees_in_forest / thread_count;
        }
        forest.reserve(how_much_trees_in_forest);

        std::vector<std::future<void>> async_trees;
        for (int i = 0; i < thread_count; ++i) {
            async_trees.push_back(std::async(MakeTrees, this, std::ref(train), size_for_one_thread));
        }
    }

};

#ifndef RPFOREST_RPFOREST_H
#define RPFOREST_RPFOREST_H

#endif //RPFOREST_RPFOREST_H
