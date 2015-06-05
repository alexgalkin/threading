/*
    Copyright 2015, Felspar Co Ltd. http://www.kirit.com/f5
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#pragma once


#include <mutex>
#include <vector>


namespace f5 {


    inline namespace threading {


        /// Thread safe associative array (map) implemented on a std::vector
        template<typename K, typename V>
        class tsmap {
            /// Mutex used to control access to the vector
            mutable std::mutex mutex;
            /// Vector which stores the data
            std::vector<std::pair<K, V>> map;

            /// Return the lower bound for the key
            auto lower_bound(const K &k) const {
                return std::lower_bound(map.begin(), map.end(), k,
                    [](const auto &l, auto r) {
                        return l.first < r;
                    });
            }
            auto lower_bound(const K &k) {
                return std::lower_bound(map.begin(), map.end(), k,
                    [](const auto &l, auto r) {
                        return l.first < r;
                    });
            }
        public:
            /// Return a pointer to the value if found. If not found then
            /// return nullptr
            V const *find(const K &k) const {
                std::unique_lock<std::mutex> lock(mutex);
                auto found = lower_bound(k);
                if ( found == map.end() ) {
                    return nullptr;
                } else {
                    return &found->second;
                }
            }

            /// Adds a value at the key if there isn't one there already.
            /// Returns a reference to the item
            template<typename... Args>
            V &emplace_if_not_found(const K &k, Args&&... args) {
                std::unique_lock<std::mutex> lock(mutex);
                auto bound = lower_bound(k);
                if ( bound != map.end() && bound->first == k ) {
                    return bound->second;
                }
                return map.emplace(bound, std::piecewise_construct,
                    std::forward_as_tuple(k),
                    std::forward_as_tuple(args...))->second;
            }
            /// Adds a value at the key if there isn't one there already.
            /// Returns a reference to the newly constructed item
            template<typename F>
            V &add_if_not_found(const K &k, F lambda) {
                std::unique_lock<std::mutex> lock(mutex);
                auto bound = lower_bound(k);
                if ( bound != map.end() && bound->first == k ) {
                    return bound->second;
                }
                return map.emplace(bound, std::piecewise_construct,
                    std::forward_as_tuple(k),
                    std::forward_as_tuple(lambda()))->second;
            }
        };


    }


}

