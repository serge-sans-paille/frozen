/*
 * Frozen
 * Copyright 2016 QuarksLab
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

module;

#include <cassert>

export module frozen;

#ifdef FROZEN_USE_STD_MODULE
import std;
#endif 

extern "C++" {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winclude-angled-in-module-purview"
#include <frozen/algorithm.h>
#include <frozen/map.h>
#include <frozen/random.h>
#include <frozen/set.h>
#include <frozen/string.h>
#include <frozen/unordered_set.h>
#include <frozen/unordered_map.h>
#pragma clang diagnostic pop
}

export namespace frozen {
    /* algorithm */ 
    using frozen::search;
    using frozen::knuth_morris_pratt_searcher;
    using frozen::make_knuth_morris_pratt_searcher;
    using frozen::boyer_moore_searcher;
    using frozen::make_boyer_moore_searcher;

    /* map */ 
    using frozen::map;
    using frozen::make_map;
    
    /* random */
    using frozen::linear_congruential_engine;
    using frozen::minstd_rand0;
    using frozen::minstd_rand;
    using frozen::default_prg_t;

    /* set */
    using frozen::set;
    using frozen::make_set;

    /* string */
    using frozen::basic_string;
    using frozen::string;
    using frozen::wstring;
    using frozen::u8string;
    using frozen::u16string;
    using frozen::u32string;

    namespace string_literals {
      using frozen::string_literals::operator"" _s;
    }

    /* unordered_map */
    using frozen::unordered_map;
    using frozen::make_unordered_map;

    /* unordered_set */
    using frozen::unordered_set;
    using frozen::make_unordered_set;
}

