#ifndef RANK_IN_RANGE_RANK_IN_RANGE_H
#define RANK_IN_RANGE_RANK_IN_RANGE_H

#include <algorithm>
#include <functional>
#include <vector>
#include <unordered_map>

namespace rank_in_range {
    
    // helpers
    // https://stackoverflow.com/questions/355967/how-to-use-msvc-intrinsics-to-get-the-equivalent-of-this-gcc-code
    static uint32_t popcnt(uint32_t x)
    {
        x -= ((x >> 1) & 0x55555555);
        x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
        x = (((x >> 4) + x) & 0x0f0f0f0f);
        x += (x >> 8);
        x += (x >> 16);
        return x & 0x0000003f;
    }
    
    static uint32_t clz(uint32_t x) {
        x |= (x >> 1);
        x |= (x >> 2);
        x |= (x >> 4);
        x |= (x >> 8);
        x |= (x >> 16);
        return 32 - popcnt(x);
    }
    
    static uint32_t int_log2_pure(uint32_t x) {
        return 32 - clz(x) - 1;
    }
    
    static uint32_t int_log2(uint32_t x) {
#ifdef __GNUC__
        return 32 - __builtin_clz(x) - 1;
#else
        return int_log2_pure(x);
#endif
    }
    
    struct RankResult {
        RankResult(int bg, int ed): rank_bg(bg), rank_ed(ed) {}
        int rank_bg;
        int rank_ed;
    };
    
    typedef std::pair<int, int> RankCacheKey;
    
    struct RankCacheKeyHash {
        size_t operator ()(const RankCacheKey &key) const {
            return std::hash<int>()(key.first) ^ std::hash<int>()(key.second);
        }
    };
    
    template <class T>
    struct RankCache {
        template <class Iter>
        RankCache(const Iter &bg, const Iter &ed): sorted_values(bg, ed) {
            std::sort(sorted_values.begin(), sorted_values.end());
        }
        
        RankCache(const RankCache<T> &left, const RankCache<T> &right): sorted_values(left.sorted_values.size() + right.sorted_values.size()) {
            std::merge(
                       left.sorted_values.begin(),
                       left.sorted_values.end(),
                       right.sorted_values.begin(),
                       right.sorted_values.end(),
                       sorted_values.begin());
        }
        
        RankResult rank(const T &x) const {
            const auto bg = sorted_values.begin();
            const auto ed = sorted_values.end();
            return RankResult(std::lower_bound(bg, ed, x) - bg, std::upper_bound(bg, ed, x) - bg);
        }
        
        std::vector<T> sorted_values;
    };
    
    template <class T, class Iter>
    class Ranker {
    public:
        Ranker(const Iter &x): x_(x) {
            
        }
        
        int remove_cache_before(int i) {
            //
        }
        
        // amortized complexity: O(log(window size))
        RankResult rank_in_range(const T &x, int bg, int ed, bool reference_impl = false) {
            const int count = ed - bg;
            
            // direct calc for small range
            if (reference_impl || count < minimum_cache_count()) {
                int less = 0;
                int same = 0;
                for (int i = bg; i < ed; i++) {
                    const auto v = x_[i];
                    if (v < x) {
                        less++;
                    } else if (v == x) {
                        same++;
                    }
                }
                return RankResult(less, less + same);
            }
            
            const int level = int_log2(count);
            const int level_count = 1 << level;
            const int mask = ~(level_count - 1);
            const int center = ed & mask;
            
            // calc by merge sort for power of 2 aligned range
            if (level_count == count  && bg % count == 0) {
                const int idx = center / level_count;
                const auto cache = get_rank_cache(level, idx);
            }
            
            // split
            const auto left_rank = rank_in_range(x, bg, center);
            const auto right_rank = rank_in_range(x, center, ed);
            
            return RankResult(left_rank.rank_bg + right_rank.rank_bg, left_rank.rank_ed + right_rank.rank_ed);
        }
    private:
        static int minimum_cache_count() {
            return 16;
        }
        
        const RankCache<T> *get_rank_cache(int level, int idx) {
            const RankCacheKey key(level, idx);
            const auto found = cache_.find(key);
            if (found != cache_.end()) {
                return &found->second;
            }
            
            const int count = 1 << level;

            if (count <= minimum_cache_count()) {
                // sort directly
                const RankCache<T> cache(x_ + count * idx, x_ + count * (idx + 1));
                const auto res = cache_.emplace(key, std::move(cache));
                return &res.first->second;
            } else {
                // merge sort using lower level cache
                const auto left = get_rank_cache(level - 1, 2 * idx);
                const auto right = get_rank_cache(level - 1, 2 * idx + 1);

                const RankCache<T> cache(*left, *right);
                const auto res = cache_.emplace(key, std::move(cache));
                return &res.first->second;
            }
        }
        
        const Iter x_;
        std::unordered_map<RankCacheKey, RankCache<T>, RankCacheKeyHash> cache_;
    };
    
}

#endif // RANK_IN_RANGE_RANK_IN_RANGE_H

