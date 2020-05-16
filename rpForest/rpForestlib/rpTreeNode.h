#pragma once
#include <cstdlib>
#include <set>
#include <algorithm>
#include <random>
#include <iterator>

#include "pointForRpTree.h"

namespace NSrpForest {

    class RpTreeNodeExpression {
    public:
        RpTreeNodeExpression(const std::string& error_m)
            : message(error_m)
        {}

        std::string GetError() { return message; }

    private:
        std::string message{""};
    };

    template <typename NumericType>
    using pIter = typename std::set<Point<NumericType>>::iterator;

    template <typename NumericType>
    class RpTreeNode {
    public:
        RpTreeNode() = default;
        ~RpTreeNode() {
            DeleteNode(left);
            DeleteNode(right);
        };

        RpTreeNode(const RpTreeNode& from) {
            node_points = from.node_points;
            mid_for_node = from.mid_for_node;
            Ns = from.Ns;
            projection_for_node = from.projection_for_node;

            if (from.left != nullptr) {
                left = new RpTreeNode;
                CopyNodeFromNode(*left, *from.left);
            }
            if (from.right != nullptr) {
                right = new RpTreeNode;
                CopyNodeFromNode(*right, *from.right);
            }
        }

        RpTreeNode& operator=(const RpTreeNode& second) {
            if (this != &second) {
                DeleteNode(left);
                DeleteNode(right);
                CopyNodeFromNode(*this, second);
            }

            return *this;
        }

        explicit RpTreeNode(const std::set<Point<NumericType>>& U, int min_W_size)
                : node_points(U)
                , Ns(min_W_size)
        {
            if (min_W_size <= 0) {
                throw RpTreeNodeExpression("min leaf size must be >= 1");
            }

            if (U.size() > Ns) {
                std::set<Point<NumericType>> W;

                if (U.size() >= 2 * Ns) {
                    while (true) {
                        for (const auto dot : U) {
                            if (rand() % 2) {
                                W.insert(dot);
                            }
                        }
                        if (W.size() >= Ns) {
                            break;
                        }
                        W.clear();
                    }
                } else {
                    W = U;
                }

                projection_for_node = whichProjection(W, U.begin()->Dimension() / 2);

                std::vector<int> W_projection;
                W_projection.reserve(W.size());
                for (const auto dot : W) {
                    W_projection.push_back(dot.at(projection_for_node));
                }
                std::sort(W_projection.begin(), W_projection.end());

                mid_for_node = W_projection[W_projection.size() / 2];

                std::set<Point<NumericType>> WL, WR;
                for (const auto& point : W) {
                    if (point.at(projection_for_node) < mid_for_node) {
                        WL.insert(point);
                    } else {
                        WR.insert(point);
                    }
                }

                if (!WL.empty() && !WR.empty()) {
                    left = new RpTreeNode(WL, Ns);
                    right = new RpTreeNode(WR, Ns);
                }
            }
        }

        std::set<Point<NumericType>>& TreeDownhill(const Point<NumericType>& point) {
            if (point.at(projection_for_node) < mid_for_node) {
                if (left == nullptr) {
                    return node_points;
                }
                return left->TreeDownhill(point);
            }

            if (right == nullptr) {
                return node_points;
            }
            return right->TreeDownhill(point);
        }

        void WriteNodeTo(std::ofstream& file) const {
            int node_points_size = node_points.size();
            file.write(reinterpret_cast<const char*>(&node_points_size), sizeof(node_points_size));
            int mid_for_node_copy = mid_for_node;
            file.write(reinterpret_cast<const char*>(&mid_for_node_copy), sizeof(mid_for_node_copy));
            int projection_for_node_copy = projection_for_node;
            file.write(reinterpret_cast<const char*>(&projection_for_node_copy), sizeof(projection_for_node_copy));
            int Ns_copy = Ns;
            file.write(reinterpret_cast<const char*>(&Ns_copy), sizeof(Ns_copy));
            for (auto& point : node_points) {
                point.WritePointTo(file);
            }

            bool hasLeft = left != nullptr;
            file.write(reinterpret_cast<const char*>(&hasLeft), sizeof(hasLeft));
            if (hasLeft) {
                left->WriteNodeTo(file);
            }

            bool hasRight = right != nullptr;
            file.write(reinterpret_cast<const char*>(&hasRight), sizeof(hasRight));
            if (hasRight) {
                right->WriteNodeTo(file);
            }
        }

        void ReadNodeFrom(std::ifstream& file) {
            if (!file.eof()) {
                node_points.clear();

                int node_points_size;
                file.read(reinterpret_cast<char *>(&node_points_size), sizeof(node_points_size));
                file.read(reinterpret_cast<char *>(&mid_for_node), sizeof(mid_for_node));
                file.read(reinterpret_cast<char *>(&projection_for_node), sizeof(projection_for_node));
                file.read(reinterpret_cast<char *>(&Ns), sizeof(Ns));
                for (int i = 0; i < node_points_size; ++i) {
                    Point<NumericType> point;
                    point.ReadPointFrom(file);
                    node_points.insert(point);
                }

                delete left;
                delete right;

                bool hasLeft, hasRight;
                file.read(reinterpret_cast<char*>(&hasLeft), sizeof(hasLeft));
                if (hasLeft) {
                    left = new RpTreeNode<NumericType>;
                    left->ReadNodeFrom(file);
                }

                file.read(reinterpret_cast<char*>(&hasRight), sizeof(hasRight));
                if (hasRight) {
                    right = new RpTreeNode<NumericType>;
                    right->ReadNodeFrom(file);
                }
            }
        }

    private:
        std::set<Point<NumericType>> node_points;

        RpTreeNode* left = nullptr;
        RpTreeNode* right = nullptr;

        NumericType mid_for_node = 0;
        NumericType projection_for_node = 0;

        int Ns = 1;

        NumericType whichProjection(const std::set<Point<NumericType>>& W, int nTry) {
            NumericType res_disp = 0;
            int res_pr = 0;

            for (int i = 0; i < nTry; ++i) {
                std::random_device rd;
                std::mt19937_64 mersenne_random(rd());

                int num = mersenne_random() % W.begin()->Dimension();
                NumericType all_sum = 0;
                for (const auto& dot : W) {
                    all_sum += dot.at(num);
                }

                NumericType avg = all_sum / W.size();

                NumericType sum_disp = 0;
                for (const auto &dot : W) {
                    sum_disp += (avg - dot.at(num)) * (avg - dot.at(num));
                }

                NumericType iDisp = sum_disp / W.size();

                if (iDisp > res_disp) {
                    res_disp = iDisp;
                    res_pr = num;
                }
            }

            return res_pr;
        }

        void DeleteNode(RpTreeNode*& now_node) {
            if (now_node == nullptr) {
                return;
            }

            if (now_node->left != nullptr) {
                DeleteNode(now_node->left);
            }

            if (now_node->right != nullptr) {
                DeleteNode(now_node->right);
            }

            delete now_node;
            now_node = nullptr;
        }

        void CopyNodeFromNode(RpTreeNode& to, const RpTreeNode& from) {
            to.node_points = from.node_points;
            to.mid_for_node = from.mid_for_node;
            to.Ns = from.Ns;
            to.projection_for_node = from.projection_for_node;

            if (from.left != nullptr) {
                to.left = new RpTreeNode;
                CopyNodeFromNode(*to.left, *from.left);
            }
            if (from.right != nullptr) {
                to.right = new RpTreeNode;
                CopyNodeFromNode(*to.right, *from.right);
            }
        }

    };

};



#ifndef RPFOREST_RPTREENODE_H
#define RPFOREST_RPTREENODE_H

#endif //RPFOREST_RPTREENODE_H
