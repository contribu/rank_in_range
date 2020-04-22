#include <gtest/gtest.h>

#include <random>
#include "rank_in_range/rank_in_range.h"

TEST(Helpers, int_log2) {
    using namespace rank_in_range;
    EXPECT_EQ(int_log2(1), 0);
    EXPECT_EQ(int_log2(2), 1);
    EXPECT_EQ(int_log2(3), 1);
    EXPECT_EQ(int_log2(4), 2);
    EXPECT_EQ(int_log2(5), 2);
    EXPECT_EQ(int_log2(7), 2);
    EXPECT_EQ(int_log2(8), 3);
    EXPECT_EQ(int_log2(65536), 16);
}

TEST(Helpers, int_log2_pure) {
    using namespace rank_in_range;
    EXPECT_EQ(int_log2_pure(1), 0);
    EXPECT_EQ(int_log2_pure(2), 1);
    EXPECT_EQ(int_log2_pure(3), 1);
    EXPECT_EQ(int_log2_pure(4), 2);
    EXPECT_EQ(int_log2_pure(5), 2);
    EXPECT_EQ(int_log2_pure(7), 2);
    EXPECT_EQ(int_log2_pure(8), 3);
    EXPECT_EQ(int_log2_pure(65536), 16);
}

TEST(Helpers, ranker_split_center) {
    using namespace rank_in_range;
    EXPECT_EQ(ranker_split_center(0, 3), 2);
    EXPECT_EQ(ranker_split_center(0, 5), 4);
    EXPECT_EQ(ranker_split_center(0, 6), 4);
    EXPECT_EQ(ranker_split_center(0, 9), 8);
    
    EXPECT_EQ(ranker_split_center(1, 3), 2);
    EXPECT_EQ(ranker_split_center(1, 4), 2);
    EXPECT_EQ(ranker_split_center(1, 5), 4);
    EXPECT_EQ(ranker_split_center(1, 6), 4);
    EXPECT_EQ(ranker_split_center(1, 7), 4);
    EXPECT_EQ(ranker_split_center(1, 8), 4);
}

TEST(RankCache, from_array) {
    using namespace rank_in_range;
    std::vector<int> ar({ 3, 2, 1 });
    RankCache<int> cache(ar.begin(), ar.end());
    
    ASSERT_EQ(cache.sorted_values.size(), 3);
    EXPECT_EQ(cache.sorted_values[0], 1);
    EXPECT_EQ(cache.sorted_values[1], 2);
    EXPECT_EQ(cache.sorted_values[2], 3);
}

TEST(RankCache, from_rank_cache) {
    using namespace rank_in_range;
    std::vector<int> left_ar({ 1, 2, 4 });
    RankCache<int> left(left_ar.begin(), left_ar.end());
    std::vector<int> right_ar({ 3, 5, 6 });
    RankCache<int> right(right_ar.begin(), right_ar.end());
    
    RankCache<int> cache(left, right);
    
    ASSERT_EQ(cache.sorted_values.size(), 6);
    EXPECT_EQ(cache.sorted_values[0], 1);
    EXPECT_EQ(cache.sorted_values[1], 2);
    EXPECT_EQ(cache.sorted_values[2], 3);
    EXPECT_EQ(cache.sorted_values[3], 4);
    EXPECT_EQ(cache.sorted_values[4], 5);
    EXPECT_EQ(cache.sorted_values[5], 6);
}

TEST(RankCache, rank) {
    using namespace rank_in_range;
    std::vector<int> ar({ 1, 2, 2, 3 });
    RankCache<int> cache(ar.begin(), ar.end());
    
    EXPECT_EQ(cache.rank(0).rank_bg, 0);
    EXPECT_EQ(cache.rank(0).rank_ed, 0);
    EXPECT_EQ(cache.rank(1).rank_bg, 0);
    EXPECT_EQ(cache.rank(1).rank_ed, 1);
    EXPECT_EQ(cache.rank(2).rank_bg, 1);
    EXPECT_EQ(cache.rank(2).rank_ed, 3);
    EXPECT_EQ(cache.rank(3).rank_bg, 3);
    EXPECT_EQ(cache.rank(3).rank_ed, 4);
    EXPECT_EQ(cache.rank(4).rank_bg, 4);
    EXPECT_EQ(cache.rank(4).rank_ed, 4);
}

TEST(Ranker, rank_in_one_range) {
    using namespace rank_in_range;
    typedef std::vector<int> Array;
    Array ar({ 1, 2, 2, 3 });
    Ranker<int, Array::iterator> ranker(ar.begin());
    
    EXPECT_EQ(ranker.rank_in_range(0, 0, 1).rank_bg, 0);
    EXPECT_EQ(ranker.rank_in_range(0, 0, 1).rank_ed, 0);
    EXPECT_EQ(ranker.rank_in_range(1, 0, 1).rank_bg, 0);
    EXPECT_EQ(ranker.rank_in_range(1, 0, 1).rank_ed, 1);
    EXPECT_EQ(ranker.rank_in_range(2, 0, 1).rank_bg, 1);
    EXPECT_EQ(ranker.rank_in_range(2, 0, 1).rank_ed, 1);
    
    EXPECT_EQ(ranker.rank_in_range(1, 1, 2).rank_bg, 0);
    EXPECT_EQ(ranker.rank_in_range(1, 1, 2).rank_ed, 0);
    EXPECT_EQ(ranker.rank_in_range(2, 1, 2).rank_bg, 0);
    EXPECT_EQ(ranker.rank_in_range(2, 1, 2).rank_ed, 1);
    EXPECT_EQ(ranker.rank_in_range(3, 1, 2).rank_bg, 1);
    EXPECT_EQ(ranker.rank_in_range(3, 1, 2).rank_ed, 1);
}

TEST(Ranker, rank_in_range) {
    using namespace rank_in_range;
    typedef std::vector<int> Array;
    Array ar({ 1, 2, 2, 3 });
    Ranker<int, Array::iterator> ranker(ar.begin());

    EXPECT_EQ(ranker.rank_in_range(0, 0, 3).rank_bg, 0);
    EXPECT_EQ(ranker.rank_in_range(0, 0, 3).rank_ed, 0);
    EXPECT_EQ(ranker.rank_in_range(1, 0, 3).rank_bg, 0);
    EXPECT_EQ(ranker.rank_in_range(1, 0, 3).rank_ed, 1);
    EXPECT_EQ(ranker.rank_in_range(2, 0, 3).rank_bg, 1);
    EXPECT_EQ(ranker.rank_in_range(2, 0, 3).rank_ed, 3);
    EXPECT_EQ(ranker.rank_in_range(3, 0, 3).rank_bg, 3);
    EXPECT_EQ(ranker.rank_in_range(3, 0, 3).rank_ed, 3);

    EXPECT_EQ(ranker.rank_in_range(1, 1, 4).rank_bg, 0);
    EXPECT_EQ(ranker.rank_in_range(1, 1, 4).rank_ed, 0);
    EXPECT_EQ(ranker.rank_in_range(2, 1, 4).rank_bg, 0);
    EXPECT_EQ(ranker.rank_in_range(2, 1, 4).rank_ed, 2);
    EXPECT_EQ(ranker.rank_in_range(3, 1, 4).rank_bg, 2);
    EXPECT_EQ(ranker.rank_in_range(3, 1, 4).rank_ed, 3);
    EXPECT_EQ(ranker.rank_in_range(4, 1, 4).rank_bg, 3);
    EXPECT_EQ(ranker.rank_in_range(4, 1, 4).rank_ed, 3);
}

TEST(Ranker, random_test) {
    using namespace rank_in_range;
    typedef std::vector<int> Array;
    Array ar;
    std::mt19937 engine(0);
    std::uniform_int_distribution<> dist(0, 100);
    for (int i = 0; i < 10000; i++) {
        ar.push_back(dist(engine));
    }
    Ranker<int, Array::iterator> ranker(ar.begin());

    for (int w = 0; w < 1000; w++) {
        for (int i = 0; i < ar.size() - w; i++) {
            const auto result = ranker.rank_in_range(ar[i], i, i + w);
            const auto reference_result = ranker.rank_in_range(ar[i], i, i + w, true);
            EXPECT_EQ(result.rank_bg, reference_result.rank_bg);
            EXPECT_EQ(result.rank_ed, reference_result.rank_ed);
        }
    }
}

TEST(Ranker, remove_cache_before) {
    using namespace rank_in_range;
    typedef std::vector<int> Array;
    Array ar(256);
    Ranker<int, Array::iterator> ranker(ar.begin());
    
    // create cache
    ranker.rank_in_range(0, 0, 256);
    const auto original_cache_count = ranker.cache_count();
    EXPECT_EQ(ranker.cache_count(), 1 + 2 + 4 + 8 + 16);
    
    ranker.remove_cache_before(32);
    const auto removed_count = 1 + 2;
    
    EXPECT_EQ(ranker.cache_count(), original_cache_count - removed_count);
}
