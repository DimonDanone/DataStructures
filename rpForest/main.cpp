#include <iostream>
#include <algorithm>
#include <map>

#include <rpForest.h>
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

Pint GeneratePint() {
    Pint res({rand() % 500, rand() % 600});
    return res;
}

void TestForest(std::set<Pint>& train, int nn_count, int how_much_points) {

    std::vector<std::vector<Pint>> forest_answers; //Knn для леса
    std::vector<Pint> points; //Случайные точки
    std::map<Pint, std::vector<Pint>> new_points_type; //
    std::vector<int> points_res;

    {
        LOG_DURATION("build + knn finding")
        int how_much_trees = 200; // Кол-во деревьев
        NSrpForest::RpForest<int> async_forest(train, how_much_trees, 2); // Сохранение леса

        {
            LOG_DURATION("only KNN finding")

            Pint p_test;
            for (int i = 0; i < how_much_points; ++i) {
                p_test = GeneratePint();
                //auto ans = async_forest.KnnForPoint(p_test, nn_count);
                auto ans = async_forest.KnnForPoint(p_test, nn_count);

                ans.resize(nn_count);
                forest_answers.push_back(ans);
                points.push_back(p_test);
            }
        }
    }

    std::vector<Pint> for_test;
    for (auto& now : train) {
        for_test.push_back(now);
    }


    int ok_cout = 0, not_ok_count = 0;

    std::vector<int> right_type;
    int now_point_pos = 0;
    for (auto& point : points) {
        std::vector<std::pair<double, Pint>> right_ans;
        for (int j = 0; j < train.size(); ++j) {
            right_ans.push_back(std::make_pair(NSrpForest::Distance(point, for_test[j]), for_test[j]));
        }

        std::sort(right_ans.begin(), right_ans.end());
        right_ans.resize(nn_count);

        bool is_ok_points = true;
        std::vector<Pint> right_ans_points;
        for (int i = 0; i < nn_count; ++i) {
            right_ans_points.push_back(right_ans[i].second);
        }
        std::sort(right_ans_points.begin(), right_ans_points.end());
        std::sort(forest_answers[now_point_pos].begin(), forest_answers[now_point_pos].end());

        for (int i = 0; i < nn_count; ++i) {
            if (right_ans_points[i] != forest_answers[now_point_pos][i]) {
                is_ok_points = false;
            }
        }

        if (is_ok_points) {
            //std::cout << "Ok. (POINTS)";
            ok_cout++;
        } else {
            std::cout << "Not Ok. (POINTS)";
            for (int i = 0; i < nn_count; i++) {
                std::cout << "(" << right_ans_points[i] << "??" << forest_answers[now_point_pos][i] << "),   ";
            }
            std::cout << "for point: " << point << std::endl;
            not_ok_count++;
        }

        now_point_pos++;
    }

    cout << ok_cout << '\n' << not_ok_count << endl;
}

int main() {

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

//Тестирование поиска knn для 100 случайных точек

    int nn_count = 5; //кол-во соседей

    std::set<Pint> train;
    std::map<Pint, std::string> train_type;
    std::ifstream file("test.txt");
    if (!file) {
        std::cout << "NO FILE!\n"; 
    }

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

        TestForest(train, nn_count, 100);
    }
    train.clear();

    {
        LOG_DURATION("Big test (1e4 points)")

        for (int i = 0; i < 1e4; ++i) {
            train.insert(GeneratePint());
        }

        TestForest(train, nn_count, 1000);

    }

    return 0;
}