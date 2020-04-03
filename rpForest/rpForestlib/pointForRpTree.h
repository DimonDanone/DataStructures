#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>

namespace NSrpForest {

    class PointException {
    public:
        PointException(const std::string& error_m)
            : message(error_m)
        {}

        std::string GetError() { return message; }

    private:
        std::string message{""};
    };

    /*!
     * \brief Шаблонный класс, описывающий единицу данных в n-мерных координатах
     * \param[in] NumericType - численный тип данных координат
    */
    template<typename NumericType>
    class Point {
    public:
        Point() = default;
        
        Point(const Point<NumericType>& second) {
            coordinates = second.coordinates;
        }

        /*!
         * \brief Конструктор Point из массива NumericType
        */
        explicit Point(const std::vector<NumericType>& vec)
                : coordinates(vec)
                {}
        /*!
         * \brief Конструктор Point n-ой размерности
         * \param[in] dimension - размерность
        */
        explicit Point(int dimension) {
            if (dimension <= 0) {
                throw PointException("dimension must be more than zero");
            }
            coordinates.resize(dimension, 0);
        }

        Point& operator=(const Point& old) {
            if (this != &old) {
                coordinates = old.coordinates;
            }

            return *this;
        }

        /*!
         * \brief Получение размерности точки
        */
        size_t Dimension() const { return coordinates.size(); }

        NumericType& at(size_t pos) { return coordinates[pos]; }

        const NumericType& at(size_t pos) const { return coordinates[pos]; }

        bool operator<(const Point<NumericType> &second) const {
            if (second.Dimension() != Dimension()) {
                throw PointException("diff dimensions");
            }

            return coordinates < second.coordinates;
        }

        bool operator==(const Point<NumericType>& second) {
            if (second.Dimension() != Dimension()) {
                throw PointException("diff dimensions");
            }

            return coordinates == second.coordinates;
        }

        bool operator!=(const Point<NumericType>& second) {
            return !operator==(second);
        }

        /*!
         * \brief Записать точку в файл
         * \param[in] file - файл, в который производится запись
        */
        void WritePointTo(std::ofstream& file) const {
            int coordinates_size = coordinates.size();
            file.write(reinterpret_cast<const char*>(&coordinates_size), sizeof(coordinates_size));
            for (int i = 0; i < coordinates.size(); ++i) {
                NumericType now_cor = coordinates[i];
                file.write(reinterpret_cast<const char*>(&now_cor), sizeof(now_cor));
            }
        }

        /*!
         * \brief Чтение точки из файла
         * \param[in] file - файл, из которого производится чтение
        */
        void ReadPointFrom(std::ifstream& file) {
            coordinates.clear();

            int coordinates_size;
            file.read(reinterpret_cast<char*>(&coordinates_size), sizeof(coordinates_size));
            for (int i = 0; i < coordinates_size; ++i) {
                NumericType now_cor;
                file.read(reinterpret_cast<char*>(&now_cor), sizeof(now_cor));
                coordinates.push_back(now_cor);
            }
        }

    private:
        std::vector<NumericType> coordinates;
    };


    template <typename T>
    std::ostream& operator<<(std::ostream& out, NSrpForest::Point<T> point) {
        out << '{';
        for (int i = 0; i < point.Dimension(); ++i) {
            out << point.at(i);
            if (i != point.Dimension() - 1) {
                out << ", ";
            }
        }
        out << '}';

        return out;
    };

    /*!
     * \brief Дистанция между двумя точками
     * \param[in] first - старт (из точки)
     * \param[in] second - финиш (в точку)
    */
    template <typename NumericType>
    double Distance(const NSrpForest::Point<NumericType>& first, const NSrpForest::Point<NumericType>& second) {
        if (second.Dimension() != first.Dimension()) {
            throw PointException("cant find distance cause not equal dimensions");
        }

        NumericType sums = 0;
        for (int i = 0; i < first.Dimension(); ++i) {
            sums += (first.at(i) - second.at(i)) * (first.at(i) - second.at(i));
        }

        return std::sqrt(sums);
    }

};

#ifndef RPFOREST_POINTFORRPTREE_H
#define RPFOREST_POINTFORRPTREE_H

#endif //RPFOREST_POINTFORRPTREE_H
