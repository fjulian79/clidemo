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

#include "unit-test.hpp"
#include <cli/cli.hpp>

/** 
 * Use UNITTEST_DECL(_name_) to declare all test functions, then add them to the 
 * unittestTab with UNITTEST(_name_).
 */
UNITTEST_DECL(history);

/**
 * A table is used to store the test name and the corresponding function pointer 
 * for easy lookup and execution of tests based on user input.
 */
unittest_t unittestTab[] = {
    UNITTEST(history),
    {0, 0}
};

/**
 * The main CLI command for running unit tests.
 * You will not need to touch this, add new tests to the table above and
 * implement them in separate files like src/test/test-history.cpp
 */
CLI_COMMAND(test){
    TestRun testRun;

    CliCommand::exec(ioStream, "info", nullptr, 0);

    ioStream.printf("Running unit tests ...\n\n");
    if (argc != 1) {
        ioStream.printf("Usage: test [testname]\n");
        ioStream.printf("Available tests:\n");
        for (size_t i = 0; unittestTab[i].name != nullptr; i++) {
            ioStream.printf("  %s\n", unittestTab[i].name);
        }
        return -1;
    }

    if (strcmp(argv[0], "all") == 0) {
        for (size_t i = 0; unittestTab[i].name != nullptr; i++) {
            ioStream.printf("=== Running test: %s ===\n", unittestTab[i].name);
            unittestTab[i].pfunc(ioStream, testRun);
        }
    } else {
        bool found = false;
        for (size_t i = 0; unittestTab[i].name != nullptr; i++) {
            if (strcmp(argv[0], unittestTab[i].name) == 0) {
                ioStream.printf("=== Running test: %s ===\n", unittestTab[i].name);
                unittestTab[i].pfunc(ioStream, testRun);
                found = true;
                break;
            }
        }
        if (!found) {
            ioStream.printf("Test '%s' not found. Use 'test all' to run all tests.\n", argv[0]);
            return -1;
        }
    }

    testRun.summary(ioStream);

    return 0;
}
