#include "str_view.h"
#include "test.h"

#include <limits.h>
#include <stdio.h>

static const str_view dirslash = {.s = "/", .sz = SVLEN("/")};

static enum test_result test_prefix_suffix(void);
static enum test_result test_substr(void);
static enum test_result test_dir_entries(void);
static enum test_result test_progressive_search(void);

#define NUM_TESTS (size_t)4
const struct fn_name all_tests[NUM_TESTS] = {
    {test_prefix_suffix, "test_prefix_suffix"},
    {test_substr, "test_substr"},
    {test_dir_entries, "test_dir_entries"},
    {test_progressive_search, "test_dir_dfs"},
};

int
main()
{
    enum test_result res = PASS;
    for (size_t i = 0; i < NUM_TESTS; ++i)
    {
        const enum test_result t_res = all_tests[i].fn();
        if (t_res == FAIL)
        {
            printf("\n");
            printf("test_view_editing test failed: %s\n", all_tests[i].name);
            res = FAIL;
        }
    }
    return res;
}

static enum test_result
test_prefix_suffix(void)
{
    const char *const reference = "Remove the suffix! No, remove the prefix!";
    const char *const ref_prefix = "Remove the suffix!";
    const char *const ref_suffix = "No, remove the prefix!";
    str_view entire_string = sv(reference);
    str_view prefix = sv_remove_suffix(entire_string, 23);
    size_t i = 0;
    for (const char *c = sv_begin(prefix); c != sv_end(prefix); c = sv_next(c))
    {
        CHECK(*c, ref_prefix[i]);
        ++i;
    }
    i = 0;
    const str_view suffix = sv_remove_prefix(entire_string, 19);
    for (const char *c = sv_begin(suffix); c != sv_end(suffix); c = sv_next(c))
    {
        CHECK(*c, ref_suffix[i]);
        ++i;
    }
    CHECK(sv_empty(sv_remove_prefix(entire_string, ULLONG_MAX)), true);
    CHECK(sv_empty(sv_remove_suffix(entire_string, ULLONG_MAX)), true);
    return PASS;
}

static enum test_result
test_substr(void)
{
    const char ref[27] = {
        [0] = 'A',  [1] = ' ',  [2] = 's',   [3] = 'u',  [4] = 'b',  [5] = 's',
        [6] = 't',  [7] = 'r',  [8] = 'i',   [9] = 'n',  [10] = 'g', [11] = '!',
        [12] = ' ', [13] = 'H', [14] = 'a',  [15] = 'v', [16] = 'e', [17] = ' ',
        [18] = 'a', [19] = 'n', [20] = 'o',  [21] = 't', [22] = 'h', [23] = 'e',
        [24] = 'r', [25] = '!', [26] = '\0',
    };
    const char *const substr1 = "A substring!";
    const char *const substr2 = "Have another!";
    const str_view substr1_view = sv(substr1);
    const str_view substr2_view = sv(substr2);
    CHECK(sv_strcmp(sv_substr(sv(ref), 0, sv_strlen(substr1)), substr1), EQL);
    CHECK(sv_strcmp(
              sv_substr(sv(ref), sv_strlen(substr1) + 1, sv_strlen(substr2)),
              substr2),
          EQL);
    CHECK(sv_strcmp(sv_substr(sv(ref), 0, ULLONG_MAX), ref), EQL);
    /* Make sure the fill function adds null terminator */
    char dump_substr1[27] = {[13] = '@'};
    (void)sv_fill(dump_substr1, 27, sv_substr(sv(ref), 0, sv_strlen(substr1)));
    CHECK(sv_strcmp(substr1_view, dump_substr1), EQL);
    /* Make sure the fill function adds null terminator */
    char dump_substr2[27] = {[14] = '@'};
    (void)sv_fill(
        dump_substr2, 27,
        sv_substr(sv(ref), sv_strlen(substr1) + 1, sv_strlen(substr2)));
    CHECK(sv_strcmp(substr2_view, dump_substr2), EQL);
    return PASS;
}

static enum test_result
test_dir_entries(void)
{
    const str_view root_single_entry = {"/usr", SVLEN("/usr")};
    const str_view root_single_entry_slash = {"/usr/", SVLEN("/usr/")};
    CHECK(sv_empty(sv_substr(dirslash, 0, sv_rfind(dirslash, 0, dirslash))),
          true);
    const str_view without_last_slash
        = sv_substr(root_single_entry_slash, 0,
                    sv_rfind(root_single_entry_slash,
                             sv_svlen(root_single_entry_slash), dirslash));
    CHECK(sv_svcmp(without_last_slash, root_single_entry), EQL);
    const str_view special_file = {"/this/is/a/very/special/file",
                                   SVLEN("/this/is/a/very/special/file")};
    const char *const toks[6] = {"this", "is", "a", "very", "special", "file"};
    size_t i = 0;
    for (str_view tok = sv_begin_tok(special_file, dirslash);
         !sv_end_tok(special_file, tok);
         tok = sv_next_tok(special_file, tok, dirslash), ++i)
    {
        CHECK(sv_strcmp(tok, toks[i]), EQL);
    }
    CHECK(i, sizeof(toks) / sizeof(toks[0]));
    return PASS;
}

static enum test_result
test_progressive_search(void)
{
    const str_view starting_path
        = {"/this/is/not/the/file/you/are/looking/for",
           SVLEN("/this/is/not/the/file/you/are/looking/for")};
    const char *const sub_paths[10] = {
        "/this/is/not/the/file/you/are/looking/for",
        "this/is/not/the/file/you/are/looking/for",
        "is/not/the/file/you/are/looking/for",
        "not/the/file/you/are/looking/for",
        "the/file/you/are/looking/for",
        "file/you/are/looking/for",
        "you/are/looking/for",
        "are/looking/for",
        "looking/for",
        "for",
    };
    size_t i = 0;
    for (str_view path = starting_path; !sv_empty(path);
         path = sv_remove_prefix(path, sv_find_first_of(path, dirslash) + 1))
    {
        CHECK(sv_strcmp(path, sub_paths[i]), EQL);
        ++i;
    }
    CHECK(i, sizeof(sub_paths) / sizeof(sub_paths[0]));
    const char *const sub_paths_rev[9] = {
        "/this/is/not/the/file/you/are/looking/for",
        "/this/is/not/the/file/you/are/looking",
        "/this/is/not/the/file/you/are",
        "/this/is/not/the/file/you",
        "/this/is/not/the/file",
        "/this/is/not/the",
        "/this/is/not",
        "/this/is",
        "/this",
    };
    i = 0;
    for (str_view path = starting_path; !sv_empty(path);
         path = sv_remove_suffix(path, sv_svlen(path)
                                           - sv_find_last_of(path, dirslash)))
    {
        CHECK(sv_strcmp(path, sub_paths_rev[i]), EQL);
        ++i;
    }
    CHECK(i, sizeof(sub_paths_rev) / sizeof(sub_paths_rev[0]));
    return PASS;
}
