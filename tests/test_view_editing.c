#include "str_view.h"
#include "test.h"

#include <limits.h>
#include <stdbool.h>
#include <string.h>

static str_view const dirslash = SV("/");

static enum test_result test_prefix_suffix(void);
static enum test_result test_substr(void);
static enum test_result test_dir_entries(void);
static enum test_result test_progressive_search(void);

#define NUM_TESTS (size_t)4
static test_fn const all_tests[NUM_TESTS] = {
    test_prefix_suffix,
    test_substr,
    test_dir_entries,
    test_progressive_search,
};

int
main()
{
    enum test_result res = PASS;
    for (size_t i = 0; i < NUM_TESTS; ++i)
    {
        enum test_result const t_res = all_tests[i]();
        if (t_res == FAIL)
        {
            res = FAIL;
        }
    }
    return res;
}

static enum test_result
test_prefix_suffix(void)
{
    char const *const reference = "Remove the suffix! No, remove the prefix!";
    char const *const ref_prefix = "Remove the suffix!";
    char const *const ref_suffix = "No, remove the prefix!";
    str_view entire_string = sv(reference);
    str_view prefix = sv_remove_suffix(entire_string, 23);
    size_t i = 0;
    for (char const *c = sv_begin(prefix); c != sv_end(prefix); c = sv_next(c))
    {
        CHECK(*c, ref_prefix[i], char, "%c");
        ++i;
    }
    i = 0;
    str_view const suffix = sv_remove_prefix(entire_string, 19);
    for (char const *c = sv_begin(suffix); c != sv_end(suffix); c = sv_next(c))
    {
        CHECK(*c, ref_suffix[i], char, "%c");
        ++i;
    }
    CHECK(sv_empty(sv_remove_prefix(entire_string, ULLONG_MAX)), true, bool,
          "%d");
    CHECK(sv_empty(sv_remove_suffix(entire_string, ULLONG_MAX)), true, bool,
          "%d");
    return PASS;
}

static enum test_result
test_substr(void)
{
    char const ref[27] = {
        [0] = 'A',  [1] = ' ',  [2] = 's',   [3] = 'u',  [4] = 'b',  [5] = 's',
        [6] = 't',  [7] = 'r',  [8] = 'i',   [9] = 'n',  [10] = 'g', [11] = '!',
        [12] = ' ', [13] = 'H', [14] = 'a',  [15] = 'v', [16] = 'e', [17] = ' ',
        [18] = 'a', [19] = 'n', [20] = 'o',  [21] = 't', [22] = 'h', [23] = 'e',
        [24] = 'r', [25] = '!', [26] = '\0',
    };
    char const *const substr1 = "A substring!";
    char const *const substr2 = "Have another!";
    str_view const substr1_view = sv(substr1);
    str_view const substr2_view = sv(substr2);
    CHECK(sv_strcmp(sv_substr(sv(ref), 0, strlen(substr1)), substr1), SV_EQL,
          sv_threeway_cmp, "%d");
    CHECK(sv_strcmp(sv_substr(sv(ref), strlen(substr1) + 1, strlen(substr2)),
                    substr2),
          SV_EQL, sv_threeway_cmp, "%d");
    CHECK(sv_strcmp(sv_substr(sv(ref), 0, ULLONG_MAX), ref), SV_EQL,
          sv_threeway_cmp, "%d");
    /* Make sure the fill function adds null terminator */
    char dump_substr1[27] = {[13] = '@'};
    (void)sv_fill(27, dump_substr1, sv_substr(sv(ref), 0, strlen(substr1)));
    CHECK(sv_strcmp(substr1_view, dump_substr1), SV_EQL, sv_threeway_cmp, "%d");
    /* Make sure the fill function adds null terminator */
    char dump_substr2[27] = {[14] = '@'};
    (void)sv_fill(27, dump_substr2,
                  sv_substr(sv(ref), strlen(substr1) + 1, strlen(substr2)));
    CHECK(sv_strcmp(substr2_view, dump_substr2), SV_EQL, sv_threeway_cmp, "%d");
    return PASS;
}

static enum test_result
test_dir_entries(void)
{
    CHECK(sv_empty(sv_substr(dirslash, 0, sv_rfind(dirslash, 0, dirslash))),
          true, bool, "%d");
    str_view const root_single_entry = SV("/usr");
    str_view const root_single_entry_slash = SV("/usr/");
    str_view const without_last_slash
        = sv_substr(root_single_entry_slash, 0,
                    sv_rfind(root_single_entry_slash,
                             sv_len(root_single_entry_slash), dirslash));
    CHECK(sv_cmp(without_last_slash, root_single_entry), SV_EQL,
          sv_threeway_cmp, "%d");
    str_view const special_file = SV("/this/is/a/very/special/file");
    char const *const toks[6] = {"this", "is", "a", "very", "special", "file"};
    size_t i = 0;
    for (str_view tok = sv_begin_tok(special_file, dirslash);
         !sv_end_tok(special_file, tok);
         tok = sv_next_tok(special_file, tok, dirslash), ++i)
    {
        CHECK(sv_strcmp(tok, toks[i]), SV_EQL, sv_threeway_cmp, "%d");
    }
    CHECK(i, sizeof(toks) / sizeof(toks[0]), size_t, "%zu");
    return PASS;
}

static enum test_result
test_progressive_search(void)
{
    str_view const starting_path
        = SV("/this/is/not/the/file/you/are/looking/for");
    char const *const sub_paths[10] = {
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
        CHECK(sv_strcmp(path, sub_paths[i]), SV_EQL, sv_threeway_cmp, "%d");
        ++i;
    }
    CHECK(i, sizeof(sub_paths) / sizeof(sub_paths[0]), size_t, "%zu");
    char const *const sub_paths_rev[9] = {
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
         path = sv_remove_suffix(path, sv_len(path)
                                           - sv_find_last_of(path, dirslash)))
    {
        CHECK(sv_strcmp(path, sub_paths_rev[i]), SV_EQL, sv_threeway_cmp, "%d");
        ++i;
    }
    CHECK(i, sizeof(sub_paths_rev) / sizeof(sub_paths_rev[0]), size_t, "%zu");
    return PASS;
}
