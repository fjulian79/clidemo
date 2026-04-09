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

#include <Arduino.h>
#include <cli/cli.hpp>

#include "unit-test.hpp"

#include <stdio.h>
#include <stdint.h>

/**
 * @brief Tests the CliHistory ring-buffer implementation.
 */
UNITTEST_DECL(history) {
#if CLI_HISTORYSIZ == 0
     ioStream.printf("\n[SKIPPED] History unit tests - CLI_HISTORYSIZ is 0 (history disabled)\n");
     return;
#else
     CliHistory history;

     char buf[CLI_COMMANDSIZ];
     /* needed for entries > CLI_COMMANDSIZ */
     char lbuf[CLI_HISTORYSIZ];       
     /* Two 97-char strings fill 196 of 200 bytes — used for eviction tests. */
     char big[98];

     ioStream.printf("\n[1] Initial state\n");
     TEST_ASSERT("get_free_space == CLI_HISTORYSIZ",
          history.get_free_space() == CLI_HISTORYSIZ);
     TEST_ASSERT("is_used == false",
          history.is_used == false);
     TEST_ASSERT("seek_backward -> false",
          history.seek_backward() == false);
     TEST_ASSERT("seek_forward -> false",
          history.seek_forward()  == false);
     TEST_ASSERT("read -> 0",
          history.read(buf, sizeof(buf)) == 0);

     ioStream.printf("[2] Invalid append arguments\n");
     TEST_ASSERT("append(nullptr, 5) -> false",
          history.append(nullptr, 5) == false);
     TEST_ASSERT("append(str, 0) -> false",
          history.append("hi", 0) == false);
     /* str[4] == 'o', not '\0' */
     TEST_ASSERT("append with len not at NUL-term -> false",
          history.append("hello", 4) == false);
     /* len > sizeof(Buffer)-1 guard; short-circuits before str[200] access */
     TEST_ASSERT("append len == CLI_HISTORYSIZ -> false",
          history.append("x", CLI_HISTORYSIZ) == false);
     TEST_ASSERT("state unchanged after all invalid appends",
          history.get_free_space() == CLI_HISTORYSIZ &&
          history.read(buf, sizeof(buf)) == 0);

     ioStream.printf("[3] Single entry\n");
     TEST_ASSERT("append \"hello\" -> true",
          history.append("hello", 5) == true);
     TEST_ASSERT("get_free_space == size - 6",
          history.get_free_space() == CLI_HISTORYSIZ - 6);
     TEST_ASSERT("read -> \"hello\", returns 5",
          history.read(buf, sizeof(buf)) == 5 && strcmp(buf, "hello") == 0);
     TEST_ASSERT("seek_backward at only entry -> false",
          history.seek_backward() == false);
     TEST_ASSERT("seek_forward  at only entry -> false",
          history.seek_forward()  == false);

     ioStream.printf("[4] read() edge cases\n");
     /* "hello" is 5 chars; need buffer >= 6 (5 + NUL) */
     TEST_ASSERT("read(buf, 5) -> 0  (buffer too small)", 
          history.read(buf, 5) == 0);
     TEST_ASSERT("read(buf, 6) -> 5  (exact fit)",
          history.read(buf, 6) == 5 && strcmp(buf, "hello") == 0);
     TEST_ASSERT("read(nullptr, 10) -> 0",               
          history.read(nullptr, 10) == 0);
     TEST_ASSERT("read(buf, 0) -> 0",               
          history.read(buf, 0) == 0);

     ioStream.printf("[5] Two entries — navigation\n");
     history.clear();
     history.append("first",  5); /* older  */
     history.append("second", 6); /* newer, pRead here after append */
     TEST_ASSERT("read newest -> \"second\"",
          history.read(buf, sizeof(buf)) == 6 && strcmp(buf, "second") == 0);
     TEST_ASSERT("seek_backward          -> true",       
          history.seek_backward() == true);
     TEST_ASSERT("read older  -> \"first\"",
          history.read(buf, sizeof(buf)) == 5 && strcmp(buf, "first") == 0);
     TEST_ASSERT("seek_backward at oldest -> false",
          history.seek_backward() == false);
     TEST_ASSERT("read still  -> \"first\" (no change)",
          history.read(buf, sizeof(buf)) == 5 && strcmp(buf, "first") == 0);
     TEST_ASSERT("seek_forward           -> true",       
          history.seek_forward() == true);
     TEST_ASSERT("read newer  -> \"second\"",
          history.read(buf, sizeof(buf)) == 6 && strcmp(buf, "second") == 0);
     TEST_ASSERT("seek_forward at newest -> false",
          history.seek_forward() == false);

     ioStream.printf("[6] Three entries — full navigation cycle\n");
     history.clear();
     history.append("alpha", 5); /* oldest */
     history.append("beta",  4);
     history.append("gamma", 5); /* newest */
     TEST_ASSERT("read newest \"gamma\"",
          history.read(buf, sizeof(buf)) == 5 && strcmp(buf, "gamma") == 0);
     TEST_ASSERT("bwd -> true",
          history.seek_backward() == true);
     TEST_ASSERT("read \"beta\"",
          history.read(buf, sizeof(buf)) == 4 && strcmp(buf, "beta") == 0);
     TEST_ASSERT("bwd -> true",
          history.seek_backward() == true);
     TEST_ASSERT("read \"alpha\"",
          history.read(buf, sizeof(buf)) == 5 && strcmp(buf, "alpha") == 0);
     TEST_ASSERT("bwd at oldest -> false",
          history.seek_backward() == false);
     TEST_ASSERT("fwd -> true",
          history.seek_forward()  == true);
     TEST_ASSERT("read \"beta\"",
          history.read(buf, sizeof(buf)) == 4 && strcmp(buf, "beta") == 0);
     TEST_ASSERT("fwd -> true",
          history.seek_forward()  == true);
     TEST_ASSERT("read \"gamma\"",
          history.read(buf, sizeof(buf)) == 5 && strcmp(buf, "gamma") == 0);
     TEST_ASSERT("fwd at newest -> false",
          history.seek_forward()  == false);

     ioStream.printf("[7] New append always resets pRead to newest entry\n");
     history.seek_backward();  /* move pRead off the newest entry */
     history.append("delta", 5);
     TEST_ASSERT("read after append -> \"delta\"",
          history.read(buf, sizeof(buf)) == 5 && strcmp(buf, "delta") == 0);
     TEST_ASSERT("seek_forward at newest -> false",
          history.seek_forward() == false);

     ioStream.printf("[8] clear() resets to initial state\n");
     history.clear();
     TEST_ASSERT("get_free_space == CLI_HISTORYSIZ",
          history.get_free_space() == CLI_HISTORYSIZ);
     TEST_ASSERT("seek_backward -> false",
          history.seek_backward() == false);
     TEST_ASSERT("seek_forward  -> false",
          history.seek_forward()  == false);
     TEST_ASSERT("read          -> 0",
          history.read(buf, sizeof(buf)) == 0);

     ioStream.printf("[9] Eviction — oldest entry removed if space needed\n");
     /* CLI_HISTORYSIZ = 200.
     * A(97) uses 98 bytes -> pHead = 98.
     * B(97) uses 98 bytes -> pHead = 196, free = (200-196) + 0 = 4.
     * "evict"(5) needs 6 bytes -> A-entry evicted, pTail advances to 98. */
     history.clear();
     memset(big, 'A', 97); big[97] = '\0';
     history.append(big, 97);
     memset(big, 'B', 97);
     history.append(big, 97);
     TEST_ASSERT("free space == 4 before eviction",      
          history.get_free_space() == 4);
     history.append("evict", 5);
     TEST_ASSERT("read after eviction -> \"evict\"",
          history.read(buf, sizeof(buf)) == 5 && strcmp(buf, "evict") == 0);
     TEST_ASSERT("bwd -> B-string still present",        
          history.seek_backward() == true);
     TEST_ASSERT("read B-string (97 chars, all 'B')",
          history.read(buf, sizeof(buf)) == 97 && 
          buf[0] == 'B' && buf[96] == 'B');
     TEST_ASSERT("bwd -> A-string evicted, no older entry", 
          history.seek_backward() == false);

     ioStream.printf("[10] Wrap-around — entry spanning buffer boundary\n");
     /* After test [9]: pHead=2, pTail=98.
     * "evict" was written at 196..199 ('e','v','i','c') then 0..1 ('t','\0').
     * Verify read() correctly assembles the string across the wrap. */
     TEST_ASSERT("fwd from B-string back to \"evict\" -> true", 
          history.seek_forward() == true);
     TEST_ASSERT("read wrapped entry -> \"evict\"",
          history.read(buf, sizeof(buf)) == 5 && strcmp(buf, "evict") == 0);

     ioStream.printf("[11] is_used flag\n");
     history.is_used = true;
     TEST_ASSERT("is_used can be set true",  
          history.is_used == true);
     history.is_used = false;
     TEST_ASSERT("is_used can be set false", 
          history.is_used == false);

     ioStream.printf("[12] Single-character entry (minimum valid length)\n");
     history.clear();
     TEST_ASSERT("append \"x\" (len=1)           -> true",
          history.append("x", 1) == true);
     TEST_ASSERT("get_free_space == size - 2",
          history.get_free_space() == CLI_HISTORYSIZ - 2);
     TEST_ASSERT("read -> \"x\", returns 1",
          history.read(buf, sizeof(buf)) == 1 && strcmp(buf, "x") == 0);

     ioStream.printf("[13] Maximum-length entry (CLI_HISTORYSIZ-1 chars) + full-buffer state\n");
     /* A single entry of CLI_HISTORYSIZ-1 chars occupies all CLI_HISTORYSIZ bytes
     * (chars + NUL), leaving get_free_space() == 0. This exercises the
     * pHead==pTail branch of get_free_space() and the evict-all / clear()
     * path inside append() when the next write is requested. */
     history.clear();
     memset(lbuf, 'M', CLI_HISTORYSIZ - 1);
     lbuf[CLI_HISTORYSIZ - 1] = '\0';
     TEST_ASSERT("append max-len string (len=CLI_HISTORYSIZ-1) -> true",
          history.append(lbuf, CLI_HISTORYSIZ - 1) == true);
     TEST_ASSERT("get_free_space == 0  (pHead==pTail branch)",
          history.get_free_space() == 0);
     TEST_ASSERT("seek_backward on full single-entry buffer -> false",
          history.seek_backward() == false);
     TEST_ASSERT("seek_forward  on full single-entry buffer -> false",
          history.seek_forward()  == false);
     /* Appending anything must evict the max-len entry; pTail reaches pHead
     * inside the eviction loop -> clear() is called, then new entry written. */
     TEST_ASSERT("append \"new\" after full buffer -> true",
          history.append("new", 3) == true);
     TEST_ASSERT("read after evict-all -> \"new\"",
          history.read(buf, sizeof(buf)) == 3 && strcmp(buf, "new") == 0);
     TEST_ASSERT("no older entry after evict-all: seek_backward -> false",
          history.seek_backward() == false);

     ioStream.printf("[14] Two evictions needed for one append\n");
     /* Three 9-char entries (10 bytes each) = 30 bytes, free = 170.
     * A 180-char entry needs 181 bytes: oldest two entries (20 bytes) must be
     * evicted before there is enough space; the third entry survives. */
     history.clear();
     history.append("aaaaaaaaa", 9); /* entry 1 — oldest */
     history.append("bbbbbbbbb", 9); /* entry 2           */
     history.append("ccccccccc", 9); /* entry 3 — newest  */
     TEST_ASSERT("free space == CLI_HISTORYSIZ-30 before large append",
          history.get_free_space() == CLI_HISTORYSIZ - 30);
     memset(lbuf, 'D', 180); lbuf[180] = '\0';
     TEST_ASSERT("append 180-char entry (requires 2 evictions) -> true",
          history.append(lbuf, 180) == true);
     TEST_ASSERT("read D-entry -> 180 chars, all 'D'",
          history.read(lbuf, sizeof(lbuf)) == 180 &&
          lbuf[0] == 'D' && lbuf[179] == 'D');
     TEST_ASSERT("bwd -> entry 3 (ccccccccc) survived eviction",
          history.seek_backward() == true);
     TEST_ASSERT("read entry 3 -> \"ccccccccc\"",
          history.read(buf, sizeof(buf)) == 9 && strcmp(buf, "ccccccccc") == 0);
     TEST_ASSERT("bwd -> entries 1+2 are gone, no further entry",
          history.seek_backward() == false);

     ioStream.printf("[15] Complete eviction — clear() path inside append()\n");
     /* Three 2-char entries (3 bytes each) = 9 bytes total, free = 191.
     * A 198-char entry needs 199 bytes: all three entries must be evicted;
     * on the third eviction pTail reaches pHead -> clear() is triggered
     * internally, then the new entry is written from position 0.
     * This also exercises the wrapped seek_forward because the D-entry from
     * test [14] left pHead at an offset, so after clear() and this write the
     * new entry starts at the physical beginning of the ring buffer. */
     history.clear();
     history.append("ab", 2);
     history.append("cd", 2);
     history.append("ef", 2);
     memset(lbuf, 'G', 198); lbuf[198] = '\0';
     TEST_ASSERT("append 198-char entry after 3 small entries -> true",
          history.append(lbuf, 198) == true);
     TEST_ASSERT("read G-entry -> 198 chars, all 'G'",
          history.read(lbuf, sizeof(lbuf)) == 198 &&
          lbuf[0] == 'G' && lbuf[197] == 'G');
     TEST_ASSERT("no older entry after complete eviction",
          history.seek_backward() == false);
     TEST_ASSERT("no newer entry (only 1 entry remains)",
          history.seek_forward()  == false);

     ioStream.printf("[16] seek scan crossing physical buffer boundary\n");
     /* Tests [1]-[15] never have the *internal scan loop* of seek_forward or
     * seek_backward cross the buffer end. [10] only verifies that read()
     * assembles a wrapped entry, but seek_forward in [10] scans through the
     * B-string (pos 98..195=NUL) — no wrap in the scan.
     *
     * Here we recreate the same physical layout as [9]/[10] and then append
     * two more entries so that:
     *   seek_forward("evict"->196): 
     *         scan 196,197,198,199,->0,1=NUL  wraps 199->0
     *   seek_backward("next"->2): 
     *         scan 0,'t', ->199,'c',198..195=NUL  wraps 0->199
     *
     * Buffer layout after setup (CLI_HISTORYSIZ=200):
     *   [0]='t'  [1]='\0'         <- tail of "evict" (wrapped)
     *   [2..6]   = "next\0"
     *   [7..11]  = "over\0"
     *   [98..195]= 97*'B' + '\0'  <- B-string (pTail=98)
     *   [196..199]= "evic"        <- head of "evict"
     *   pHead=12, pTail=98, pRead=12 (newest="over")
     */
     history.clear();
     memset(big, 'A', 97); big[97] = '\0';
     /* [0..97],  pHead=98 */
     history.append(big, 97);               
     memset(big, 'B', 97);
     /* [98..195], pHead=196, free=4 */
     history.append(big, 97);             
     /* evicts A; "evict" wraps [196..1], pHead=2, pTail=98 */
     history.append("evict", 5);            
     /* [2..6],   pHead=7  */
     history.append("next",  4);            
     /* [7..11],  pHead=12 */
     history.append("over",  4);            

     /* --- full backward walk --- */
     TEST_ASSERT("read newest -> \"over\"",
          history.read(buf, sizeof(buf)) == 4 && strcmp(buf, "over") == 0);
     TEST_ASSERT("bwd over->next: scan finds NUL at pos 1 (no scan wrap)",
          history.seek_backward() == true);
     TEST_ASSERT("read -> \"next\"",
          history.read(buf, sizeof(buf)) == 4 && strcmp(buf, "next") == 0);
     /* seek_backward from pos 2: initial decrement-2 lands at 0 ('t'), then
      * the while loop decrements 0 -> 199 (WRAP), 
      * scans 199,198,197,196,195=NUL */
     TEST_ASSERT("bwd next->evict: backward scan wraps 0->199",
          history.seek_backward() == true);
     TEST_ASSERT("read -> \"evict\"",
          history.read(buf, sizeof(buf)) == 5 && strcmp(buf, "evict") == 0);
     TEST_ASSERT("bwd evict->B: scan hits pTail",
          history.seek_backward() == true);
     TEST_ASSERT("read -> B-string (97 'B's)",
          history.read(buf, sizeof(buf)) == 97 && 
          buf[0] == 'B' && buf[96] == 'B');
     TEST_ASSERT("bwd at oldest -> false",
          history.seek_backward() == false);

     /* --- full forward walk --- */
     TEST_ASSERT("fwd B->evict: scan through B-string, no scan wrap",
          history.seek_forward() == true);
     TEST_ASSERT("read -> \"evict\"",
          history.read(buf, sizeof(buf)) == 5 && strcmp(buf, "evict") == 0);
     /* seek_forward from pos 196: the while loop increments 196,197,198,199
     * then wraps 199->0 (WRAP), 0='t', 1=NUL -> next entry at pos 2 */
     TEST_ASSERT("fwd evict->next: forward scan wraps 199->0",
          history.seek_forward() == true);
     TEST_ASSERT("read -> \"next\"",
          history.read(buf, sizeof(buf)) == 4 && strcmp(buf, "next") == 0);
     TEST_ASSERT("fwd next->over: scan stays within linear region",
          history.seek_forward() == true);
     TEST_ASSERT("read -> \"over\"",
          history.read(buf, sizeof(buf)) == 4 && strcmp(buf, "over") == 0);
     TEST_ASSERT("fwd at newest -> false",
          history.seek_forward() == false);

     ioStream.printf("[17] Duplicate prevention — identical consecutive entries\n");
     /* When appending a string identical to the last entry (pLast), append()
      * should return true but not write anything. */
     history.clear();
     history.append("test", 4);
     size_t free_before = history.get_free_space();
     TEST_ASSERT("append identical \"test\" -> true (no write)",
          history.append("test", 4) == true);
     TEST_ASSERT("free space unchanged (duplicate not written)",
          history.get_free_space() == free_before);
     TEST_ASSERT("read still returns \"test\"",
          history.read(buf, sizeof(buf)) == 4 && strcmp(buf, "test") == 0);
     TEST_ASSERT("no older entry exists",
          history.seek_backward() == false);
     
     /* Different entry should be appended normally */
     TEST_ASSERT("append different \"other\" -> true",
          history.append("other", 5) == true);
     TEST_ASSERT("free space decreased",
          history.get_free_space() < free_before);
     TEST_ASSERT("read -> \"other\"",
          history.read(buf, sizeof(buf)) == 5 && strcmp(buf, "other") == 0);
     TEST_ASSERT("bwd -> previous entry \"test\" exists",
          history.seek_backward() == true);
     TEST_ASSERT("read -> \"test\"",
          history.read(buf, sizeof(buf)) == 4 && strcmp(buf, "test") == 0);

     ioStream.printf("[18] Duplicate after navigation — pRead reset\n");
     /* User navigates in history (pRead != pLast), then enters the same
      * command as the newest entry. The duplicate should not be written,
      * but pRead must be reset to pLast. */
     history.clear();
     history.append("first", 5);
     history.append("second", 6);
     history.append("third", 5);
     
     /* Navigate backward */
     history.seek_backward(); /* now at "second" */
     history.seek_backward(); /* now at "first"  */
     TEST_ASSERT("read after navigation -> \"first\"",
          history.read(buf, sizeof(buf)) == 5 && strcmp(buf, "first") == 0);
     
     /* Append same as newest entry (pLast="third") */
     free_before = history.get_free_space();
     TEST_ASSERT("append \"third\" (identical to pLast) -> true",
          history.append("third", 5) == true);
     TEST_ASSERT("free space unchanged (duplicate not written)",
          history.get_free_space() == free_before);
     TEST_ASSERT("pRead reset: read -> \"third\" (newest)",
          history.read(buf, sizeof(buf)) == 5 && strcmp(buf, "third") == 0);
     TEST_ASSERT("at newest entry: seek_forward -> false",
          history.seek_forward() == false);

     ioStream.printf("[19] is_used flag reset after append\n");
     /* After any append() — whether writing a new entry or detecting a
      * duplicate — is_used should be set to false and pRead should point
      * to the newest entry. */
     history.clear();
     history.append("alpha", 5);
     history.append("beta", 4);
     
     history.is_used = true;
     history.seek_backward(); /* navigate to "alpha" */
     TEST_ASSERT("is_used was true before append", 
          history.is_used == true);
     
     history.append("gamma", 5);
     TEST_ASSERT("is_used reset to false after append",
          history.is_used == false);
     TEST_ASSERT("pRead at newest: read -> \"gamma\"",
          history.read(buf, sizeof(buf)) == 5 && strcmp(buf, "gamma") == 0);
     
     /* Same test with duplicate detection path */
     history.is_used = true;
     history.seek_backward(); /* navigate away */
     history.append("gamma", 5); /* identical to pLast */
     TEST_ASSERT("is_used reset to false even on duplicate",
          history.is_used == false);
     TEST_ASSERT("pRead at newest: read -> \"gamma\"",
          history.read(buf, sizeof(buf)) == 5 && strcmp(buf, "gamma") == 0);

     ioStream.printf("[20] Duplicate detection with wrapped entry\n");
     /* Verify duplicate detection works when the pLast entry is wrapped
      * across the buffer boundary. */
     history.clear();
     memset(big, 'X', 97); big[97] = '\0';
     history.append(big, 97);    /* [0..97], pHead=98  */
     memset(big, 'Y', 97);
     history.append(big, 97);    /* [98..195], pHead=196, pTail=0 */
     history.append("wrap", 4);   /* evicts X; [196..199,0]="wrap\0", pHead=1 */
     
     free_before = history.get_free_space();
     TEST_ASSERT("append wrapped \"wrap\" duplicate -> true",
          history.append("wrap", 4) == true);
     TEST_ASSERT("free space unchanged (wrapped duplicate detected)",
          history.get_free_space() == free_before);
     TEST_ASSERT("read -> \"wrap\"",
          history.read(buf, sizeof(buf)) == 4 && strcmp(buf, "wrap") == 0);

     ioStream.printf("[21] Duplicate detection — case sensitivity\n");
     /* Strings differing only in case should NOT be treated as duplicates */
     history.clear();
     history.append("Test", 4);
     free_before = history.get_free_space();
     TEST_ASSERT("append \"test\" (different case) -> true (new entry)",
          history.append("test", 4) == true);
     TEST_ASSERT("free space decreased (not a duplicate)",
          history.get_free_space() < free_before);
     TEST_ASSERT("read newest -> \"test\"",
          history.read(buf, sizeof(buf)) == 4 && strcmp(buf, "test") == 0);
     TEST_ASSERT("bwd -> \"Test\" (previous entry)",
          history.seek_backward() == true);
     TEST_ASSERT("read -> \"Test\"",
          history.read(buf, sizeof(buf)) == 4 && strcmp(buf, "Test") == 0);

     ioStream.printf("[22] Duplicate detection — empty history\n");
     /* Calling append() on an empty history (pLast==0) should always write */
     history.clear();
     TEST_ASSERT("append \"first\" on empty history -> true",
          history.append("first", 5) == true);
     TEST_ASSERT("entry was written",
          history.get_free_space() == CLI_HISTORYSIZ - 6);
     TEST_ASSERT("read -> \"first\"",
          history.read(buf, sizeof(buf)) == 5 && strcmp(buf, "first") == 0);

     ioStream.printf("[23] Multiple different entries then duplicate\n");
     /* Build up several entries, then try to duplicate the most recent */
     history.clear();
     history.append("cmd1", 4);
     history.append("cmd2", 4);
     history.append("cmd3", 4);
     history.append("cmd4", 4);
     
     free_before = history.get_free_space();
     TEST_ASSERT("append duplicate \"cmd4\" -> true",
          history.append("cmd4", 4) == true);
     TEST_ASSERT("free space unchanged",
          history.get_free_space() == free_before);
     
     /* Verify full history is intact */
     TEST_ASSERT("read newest -> \"cmd4\"",
          history.read(buf, sizeof(buf)) == 4 && strcmp(buf, "cmd4") == 0);
     history.seek_backward();
     TEST_ASSERT("read -> \"cmd3\"",
          history.read(buf, sizeof(buf)) == 4 && strcmp(buf, "cmd3") == 0);
     history.seek_backward();
     TEST_ASSERT("read -> \"cmd2\"",
          history.read(buf, sizeof(buf)) == 4 && strcmp(buf, "cmd2") == 0);
     history.seek_backward();
     TEST_ASSERT("read -> \"cmd1\"",
          history.read(buf, sizeof(buf)) == 4 && strcmp(buf, "cmd1") == 0);
     TEST_ASSERT("no older entry",
          history.seek_backward() == false);
#endif
}
