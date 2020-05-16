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

    template<typename NumericType>
    class Point {
    public:
        Point() = default;
        Point(const Point<NumericType>& second) {
            coordinates = second.coordinates;
        }

        explicit Point(const std::vector<NumericType>& vec)
                : coordinates(vec)
                {}

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

        void WritePointTo(std::ofstream& file) const {
            int coordinates_size = coordinates.size();
            file.write(reinterpret_cast<const char*>(&coordinates_size), sizeof(coordinates_size));
            for (int i = 0; i < coordinates.size(); ++i) {
                NumericType now_cor = coordinates[i];
                file.write(reinterpret_cast<const char*>(&now_cor), sizeof(now_cor));
            }
        }

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

    template <typename NumericType>
    long long Distance(const NSrpForest::Point<NumericType>& first, const NSrpForest::Point<NumericType>& second) {
        if (second.Dimension() != first.Dimension()) {
            throw PointException("cant find distance cause not equal dimensions");
        }

        NumericType sums = 0;
        for (int i = 0; i < first.Dimension(); ++i) {
            sums += (first.at(i) - second.at(i)) * (first.at(i) - second.at(i));
        }

        return sums;
    }

};

#ifndef RPFOREST_POINTFORRPTREE_H
#define RPFOREST_POINTFORRPTREE_H

#endif //RPFOREST_POINTFORRPTREE_H
