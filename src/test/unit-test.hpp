/*
 * clidemo, a example and test bench for my command line library libcli.
 *
 * Copyright (C) 2026 Julian Friedrich
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <Arduino.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

/**
 * @brief Use UNITTEST_DECL(_name_) to declare test functions, then add them to 
 * the test suite by adding them to the test tablr using UNITTEST(_name_).
 */
#define UNITTEST_DECL(_name)     void test_##_name(Stream& ioStream, TestRun& testRun)   

/**
 * @brief Use UNITTEST(_name_) when adding test functions to the test table.
 */
#define UNITTEST(_name)     {#_name, test_##_name}

/**
 * @brief The TestRun class is used to track the number of passed and failed 
 * assertions during a test run, and to provide a summary at the end.
 */
class TestRun {
    public:
        TestRun() : passed(0), failed(0) {}

        void pass() { 
            passed++; 
        }
        
        void fail() { 
            failed++; 
        }

        void reset() { 
            passed = 0;
            failed = 0; 
        }

        void do_assert(Stream& ioStream,const char* name, bool condition) {
            if (condition) {
                ioStream.printf("  PASS: %s\n", name);
                pass();
            } else {
                ioStream.printf("  FAIL: %s\n", name);
                fail();
            }
        }

        uint32_t getPassed() const { 
            return passed; 
        }
        
        uint32_t getFailed() const { 
            return failed; 
        }

        void summary(Stream& ioStream) const {
            uint32_t total = passed + failed;
            ioStream.printf("\nResult: %d/%d passed%s\n\n", passed, total,
                failed == 0 ? " -- all good!" : " -- FAILURES detected!");
        }

    private:
        uint32_t passed;
        uint32_t failed;
};

/**
 * @brief Test function pointer type to use in the unittest table and when
 * defining test functions.
 */
typedef void (*TestFuncPtr)(Stream& ioStream, TestRun& testRun);

/**
 * @brief The unittest_t structure is used to store the test name and the
 * corresponding function pointer for easy lookup and execution of tests based 
 * on user input.
 */
typedef struct {

    const char *name;
    TestFuncPtr pfunc;

} unittest_t;

// ---------------------------------------------------------------------------
// Test macros
// ---------------------------------------------------------------------------

/**
 * @brief Print a section header to visually group related assertions.
 */
#define TEST_SECTION(name)  ioStream.printf("\n[" name "]\n")

// ---------------------------------------------------------------------------
// Core — named free-form assertion
// ---------------------------------------------------------------------------

/**
 * @brief Assert a free-form condition with an explicit description.
 * @param name  String printed as the test description.
 * @param cond  Expression that must evaluate to true.
 */
#define TEST_ASSERT(name, cond)                                             \
    do{                                                                     \
        testRun.do_assert(ioStream, name, cond);                            \
    } while (0)

// ---------------------------------------------------------------------------
// Boolean
// ---------------------------------------------------------------------------

/** 
 * @brief Assert that @p cond is true. 
 */
#define TEST_ASSERT_TRUE(cond)                                              \
    do {                                                                    \
        testRun.do_assert(ioStream, #cond, cond);                           \
    } while (0)

/** 
 * @brief Assert that @p cond is false. 
 */
#define TEST_ASSERT_FALSE(cond)                                             \
    do {                                                                    \
        testRun.do_assert(ioStream, "!" #cond, !(cond));                    \
    } while (0)

// ---------------------------------------------------------------------------
// Integer equality — prints expected/actual on failure
// ---------------------------------------------------------------------------

/** 
 * @brief Assert that integer @p act equals @p exp. 
 */
#define TEST_ASSERT_EQUAL_INT(exp, act)                                     \
    do {                                                                    \
        testRun.do_assert(ioStream,                                         \
            #act " == " #exp, (int)(act) == (int)(exp));                    \
    } while (0)

/** 
 * @brief Assert that integer @p act does not equal @p exp. 
 */
#define TEST_ASSERT_NOT_EQUAL_INT(exp, act)                                 \
    do {                                                                    \
        testRun.do_assert(ioStream,                                         \
        #act " != " #exp, (int)(act) != (int)(exp));                        \
    } while (0) 

// ---------------------------------------------------------------------------
// String equality — prints expected/actual on failure
// ---------------------------------------------------------------------------

/** 
 * @brief Assert that C-string @p act equals @p exp. 
 */
#define TEST_ASSERT_EQUAL_STRING(exp, act)                                  \
    do {                                                                    \
        testRun.do_assert(ioStream,                                         \
            #act " == " #exp, strcmp((act), (exp)) == 0);                   \
    } while (0)

// ---------------------------------------------------------------------------
// Pointer / null checks
// ---------------------------------------------------------------------------

/** 
 * @brief Assert that @p ptr is nullptr. 
 */
#define TEST_ASSERT_NULL(ptr)                                               \
    do {                                                                    \
        testRun.do_assert(ioStream, #ptr " == nullptr", (ptr) == nullptr);  \
    } while (0)

/** 
 * @brief Assert that @p ptr is not nullptr. 
 */
#define TEST_ASSERT_NOT_NULL(ptr)                                           \
    do {                                                                    \
        testRun.do_assert(ioStream, #ptr " != nullptr", (ptr) != nullptr);  \
    } while (0)
