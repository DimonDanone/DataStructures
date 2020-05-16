#include <iostream>
#include <algorithm>
#include <map>

#include "rpForest.h"
#include "log_duration.h"

using Pint = NSrpForest::Point<int>;

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec) {
    for (size_t  i = 0; i < vec.size(); ++i) {
        out << vec[i];
        if (i != vec.size() - 1) {
            out << ", ";
        }
    }

    return out;
}

Pint GeneratePint(int size) {
    vector<int> a(size);
    for (int i = 0; i < size; ++i) {
        a[i] = rand() % 500;
    }
    Pint res(a);
    return res;
}

vector<int> TestForest(std::set<Pint>& train, int nn_count, int how_much_points, int trees_count, int pt_size) {

    std::vector<std::vector<Pint>> forest_answers; //Knn для леса
    std::vector<Pint> points; //Случайные точки
    std::map<Pint, std::vector<Pint>> new_points_type; //
    std::vector<int> points_res;

    int how_much_trees = trees_count; // Кол-во деревьев

    {
        LOG_DURATION("build forest")

        NSrpForest::RpForest<int> test(train, how_much_trees, 4); // Сохранение леса
    }

    NSrpForest::RpForest<int> async_forest(train, how_much_trees, 4); // Сохранение леса

    {
        LOG_DURATION("KNN finding")

        Pint p_test;
        for (int i = 0; i < how_much_points; ++i) {
            p_test = GeneratePint(pt_size);
            auto ans = async_forest.KnnForPoint(p_test, nn_count);

            vector<Pint> res_;
            for (int j = 0; j < nn_count; ++j) {
                res_.push_back(ans[j]);
            }

            forest_answers.push_back(res_);
            points.push_back(p_test);
        }
    }

    std::vector<Pint> for_test;
    for (auto& now : train) {
        for_test.push_back(now);
    }

    vector<int> how_much_wrong(nn_count + 1);

    int ok_cout = 0, not_ok_count = 0;
    {
        LOG_DURATION("finding answer with full search")

        int now_point_pos = 0;
        vector<Pint> all_train;

        for (auto p : train) {
            all_train.push_back(p);
        }

        for (auto &point : points) {

            std::sort(all_train.begin(), all_train.end(),
                    [point](const Pint& first, const Pint& second) {
                        long long d1 = Distance(first, point);
                        long long d2 = Distance(second, point);
                        if (d1 != d2) {
                            return d1 < d2;
                        }
                        return first < second;
            });

            bool is_ok_points = true;

            vector<Pint> res_;
            std::set_intersection(all_train.begin(), all_train.end(), forest_answers[now_point_pos].begin(), forest_answers[now_point_pos].end(), back_inserter(res_));

            how_much_wrong[nn_count - res_.size()]++;

            now_point_pos++;
        }
        for (int i = 0; i < how_much_wrong.size(); ++i) {
            cout << i << " mistakes: " << how_much_wrong[i] << endl;
        }
    }

    return how_much_wrong;
}

int main() {

    while(true) {
        cout << "Input test number:\n 1 - tiny test (train - 400, test - 100);\n 2 - big test (train - 1e6, test - 1e4);\n "
                "3 - binary write/read test;\n 4 - correctness test (train - 1e4, test - 1e3);\n 0 - exit;" << endl;
        int x;
        cin >> x;

        int nn_count = 5; //кол-во соседей
        std::set<Pint> train;

        if (x == 1) {
            std::map<Pint, std::string> train_type;
            std::ifstream file("test.txt");
            {
                LOG_DURATION("Correct work of forest based on 400 points")
                int k = 0;

                while (!file.eof()) {
                    int x, y;
                    std::string s;
                    file >> x >> y >> s;
                    Pint p({x, y});
                    train.insert(p);
                    train_type[p] = s;
                    if (k == 400) {
                        break;
                    }
                    k++;
                }

                TestForest(train, nn_count, 100, 50, 2);
            }

        } else if (x == 2) {

            cout << "This test needs too much time. Are you want to run it? [Y/n]" << endl;
            string s;
            cin >> s;
            if (s == "Y") {
                //Тестирование скорости на 1e6 точках
                for (int i = 0; i < 1e6; ++i) {
                    train.insert(GeneratePint(2));
                }

                TestForest(train, nn_count, 1000, 20, 2);
            }
        } else if (x == 3) {
            Pint p1({0, 0});
            Pint p2({1, 3});
            Pint p3({1, 1});
            Pint p4({3, 1});
            Pint p5({3, 12});
            Pint p6({6, 7});
            Pint p7({0, 9});

            std::set<Pint> train1 = {p1, p2, p3, p4, p5, p6, p7};

            {
                LOG_DURATION("test write and read")
                NSrpForest::RpForest<int> async_forest(train1, 3);


                Pint p_test({12, 32});
                auto ans1 = async_forest.KnnForPoint(p_test, 2);

                std::ofstream test_f("test_bin", ios_base::binary);
                async_forest.WriteForestTo(test_f);
                test_f.close();

                std::ifstream test_read("test_bin", ios_base::binary);
                NSrpForest::RpForest<int> readable_forest;
                readable_forest.ReadForestFrom(test_read);
                test_read.close();

                auto ans2 = readable_forest.KnnForPoint(p_test, 2);
                std::cout << "standart forest: " << ans1 << "\n" << "read forest: " << ans2 << std::endl;
            }
        } else if (x == 4) {
            cout << "Write trees count: " << endl;
            int trees_count;
            cin >> trees_count;
            cout << "Write k for kNN: " << endl;
            cin >> nn_count;
            int pt_size;
            cout << "Write point dimesion: " << endl;
            cin >> pt_size;

            for (int i = 0; i < 1e4; ++i) {
                train.insert(GeneratePint(pt_size));
            }

            TestForest(train, nn_count, 1000, trees_count, pt_size);
        } else if (x == 0) {
            break;
        }
    }

    return 0;
}