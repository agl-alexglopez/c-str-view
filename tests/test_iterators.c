#include "str_view.h"
#include "test.h"

#include <stdio.h>
#include <string.h>

static enum test_result test_iter(void);
static enum test_result test_iter2(void);
static enum test_result test_riter(void);
static enum test_result test_riter2(void);
static enum test_result test_riter_multi(void);
static enum test_result test_min_delim(void);
static enum test_result test_min_delim_two_byte(void);
static enum test_result test_min_delim_three_byte(void);
static enum test_result test_min_delim_four_byte(void);
static enum test_result test_min_delim_five_byte(void);
static enum test_result test_rmin_delim(void);
static enum test_result test_rmin_delim_two_byte(void);
static enum test_result test_rmin_delim_three_byte(void);
static enum test_result test_rmin_delim_four_byte(void);
static enum test_result test_rmin_delim_five_byte(void);
static enum test_result test_simple_delim(void);
static enum test_result test_rsimple_delim(void);
static enum test_result test_tail_delim(void);
static enum test_result test_rtail_delim(void);
static enum test_result test_rtriple_delim(void);
static enum test_result test_rquad_delim(void);
static enum test_result test_iter_repeating_delim(void);
static enum test_result test_iter_multichar_delim(void);
static enum test_result test_riter_multichar_delim(void);
static enum test_result test_iter_multichar_delim_short(void);
static enum test_result test_riter_multichar_delim_short(void);
static enum test_result test_iter_delim_larger_than_str(void);
static enum test_result test_riter_delim_larger_than_str(void);
static enum test_result test_tokenize_not_terminated(void);
static enum test_result test_tokenize_three_views(void);
static enum test_result test_rtokenize_three_views(void);

#define NUM_TESTS (size_t)31
const struct fn_name all_tests[NUM_TESTS] = {
    {test_iter, "test_iter"},
    {test_iter2, "test_iter2"},
    {test_riter, "test_riter"},
    {test_riter_multi, "test_riter_multi"},
    {test_riter2, "test_riter2"},
    {test_min_delim, "test_min_delim"},
    {test_min_delim_two_byte, "test_min_delim_two_byte"},
    {test_min_delim_three_byte, "test_min_delim_three_byte"},
    {test_min_delim_four_byte, "test_min_delim_four_byte"},
    {test_min_delim_five_byte, "test_min_delim_five_byte"},
    {test_rmin_delim, "test_rmin_delim"},
    {test_rmin_delim_two_byte, "test_rmin_delim_two_byte"},
    {test_rmin_delim_three_byte, "test_rmin_delim_three_byte"},
    {test_rmin_delim_four_byte, "test_rmin_delim_four_byte"},
    {test_rmin_delim_five_byte, "test_rmin_delim_five_byte"},
    {test_simple_delim, "test_simple_delim"},
    {test_rsimple_delim, "test_rsimple_delim"},
    {test_tail_delim, "test_tail_delim"},
    {test_rtail_delim, "test_rtail_delim"},
    {test_rtriple_delim, "test_rtriple_delim"},
    {test_rquad_delim, "test_rquad_delim"},
    {test_iter_repeating_delim, "test_iter_repeating_delim"},
    {test_iter_multichar_delim, "test_iter_multichar_delim"},
    {test_riter_multichar_delim, "test_riter_multichar_delim"},
    {test_iter_multichar_delim_short, "test_iter_multichar_delim_short"},
    {test_riter_multichar_delim_short, "test_riter_multichar_delim_short"},
    {test_iter_delim_larger_than_str, "test_iter_delim_larger_than_str"},
    {test_riter_delim_larger_than_str, "test_riter_delim_larger_than_str"},
    {test_tokenize_not_terminated, "test_tokenize_not_terminated"},
    {test_tokenize_three_views, "test_tokenize_three_view"},
    {test_rtokenize_three_views, "test_rtokenize_three_view"},
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
            (void)fprintf(stderr,
                          RED "test_iterators.c test failed: " CYAN "%s\n" NONE,
                          all_tests[i].name);
            res = FAIL;
        }
    }
    return res;
}

static enum test_result
test_iter(void)
{
    const char *const reference = "A B C D E G H I J K L M N O P";
    str_view chars = sv(reference);
    size_t i = 0;
    for (const char *cur = sv_begin(chars); cur != sv_end(chars);
         cur = sv_next(cur))
    {
        CHECK(*cur, reference[i], char, "%c");
        ++i;
    }
    i = 0;
    str_view cur = sv_begin_tok(chars, sv(" "));
    for (; !sv_end_tok(chars, cur); cur = sv_next_tok(chars, cur, sv(" ")))
    {
        CHECK(sv_front(cur), reference[i], char, "%c");
        i += 2;
    }
    CHECK(sv_front(cur), '\0', char, "%c");
    str_view cur2 = sv_begin_tok(chars, sv(","));
    for (; !sv_end_tok(chars, cur2); cur2 = sv_next_tok(chars, cur2, sv(",")))
    {
        CHECK(sv_strcmp(cur2, reference), EQL, sv_threeway_cmp, "%d");
    }
    CHECK(sv_front(cur2), '\0', char, "%c");
    return PASS;
}

static enum test_result
test_iter2(void)
{
    const char *const reference = " A B C D E G H I J K L M N O P ";
    const size_t size = 15;
    const char *const toks[15] = {
        "A", "B", "C", "D", "E", "G", "H", "I",
        "J", "K", "L", "M", "N", "O", "P",
    };
    str_view chars = sv(reference);
    size_t i = 0;
    for (const char *cur = sv_begin(chars);
         cur != sv_end(chars) && i < sv_len(chars); cur = sv_next(cur))
    {
        CHECK(*cur, reference[i], char, "%c");
        ++i;
    }
    i = 0;
    str_view cur = sv_begin_tok(chars, sv(" "));
    for (; !sv_end_tok(chars, cur) && i < size;
         cur = sv_next_tok(chars, cur, sv(" ")))
    {
        CHECK(sv_front(cur), *toks[i], char, "%c");
        CHECK(sv_len(cur), strlen(toks[i]), size_t, "%zu");
        ++i;
    }
    CHECK(sv_front(cur), '\0', char, "%c");
    i = 0;
    str_view cur2 = sv_begin_tok(chars, sv(","));
    for (; !sv_end_tok(chars, cur2) && i < 1;
         cur2 = sv_next_tok(chars, cur2, sv(",")))
    {
        CHECK(sv_strcmp(cur2, reference), EQL, sv_threeway_cmp, "%d");
        ++i;
    }
    CHECK(sv_front(cur2), '\0', char, "%c");
    return PASS;
}

static enum test_result
test_riter(void)
{
    const str_view ref = sv("A B C D E G H I J K L M N O P");
    size_t i = sv_len(ref) - 1;
    str_view cur = sv_rbegin_tok(ref, sv(" "));
    for (; !sv_rend_tok(ref, cur); cur = sv_rnext_tok(ref, cur, sv(" ")))
    {
        CHECK(sv_front(cur), *sv_pos(ref, i), char, "%c");
        i -= 2;
    }
    CHECK(sv_begin(cur), sv_begin(ref), char *const, "%s");
    str_view cur2 = sv_rbegin_tok(ref, sv(","));
    for (; !sv_rend_tok(ref, cur2); cur2 = sv_rnext_tok(ref, cur2, sv(",")))
    {
        CHECK(sv_cmp(cur2, ref), EQL, sv_threeway_cmp, "%d");
    }
    CHECK(sv_begin(cur2), sv_begin(ref), char *const, "%s");
    return PASS;
}

static enum test_result
test_riter2(void)
{
    const str_view ref = sv(" A B C D E G H I J K L M N O P ");
    const size_t size = 15;
    const char *const toks[15] = {
        "A", "B", "C", "D", "E", "G", "H", "I",
        "J", "K", "L", "M", "N", "O", "P",
    };
    size_t character = sv_len(ref);
    for (const char *c = sv_rbegin(ref); character && c != sv_rend(ref);
         c = sv_rnext(c))
    {
        --character;
        CHECK(c, sv_pos(ref, character), char *const, "%s");
        CHECK(*c, sv_at(ref, character), char, "%c");
    }
    CHECK(character, 0UL, size_t, "%zu");
    size_t i = size;
    str_view cur = sv_rbegin_tok(ref, sv(" "));
    for (; !sv_rend_tok(ref, cur) && i; cur = sv_rnext_tok(ref, cur, sv(" ")))
    {
        --i;
        CHECK(sv_front(cur), *toks[i], char, "%c");
        CHECK(sv_len(cur), strlen(toks[i]), size_t, "%zu");
    }
    CHECK(sv_begin(cur), sv_begin(ref), char *const, "%s");
    i = 1;
    str_view cur2 = sv_rbegin_tok(ref, sv(","));
    for (; !sv_rend_tok(ref, cur2) && i;
         cur2 = sv_rnext_tok(ref, cur2, sv(",")))
    {
        --i;
        CHECK(sv_cmp(cur2, ref), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(cur2), sv_len(ref), size_t, "%zu");
    }
    CHECK(sv_begin(cur2), sv_begin(ref), char *const, "%s");
    return PASS;
}

static enum test_result
test_riter_multi(void)
{
    const str_view ref = sv("//A//B//C//D//E//G//H//I//J//K//L//M//N//O//P//");
    const str_view delim = sv("//");
    const size_t size = 15;
    const char *const toks[15] = {
        "A", "B", "C", "D", "E", "G", "H", "I",
        "J", "K", "L", "M", "N", "O", "P",
    };
    size_t i = size;
    const size_t last_delim_pos = sv_rfind(ref, sv_len(ref), delim);
    CHECK(last_delim_pos, sv_len(ref) - 2, size_t, "%zu");
    str_view cur = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, cur) && i; cur = sv_rnext_tok(ref, cur, delim))
    {
        --i;
        CHECK(sv_front(cur), *toks[i], char, "%c");
        CHECK(sv_len(cur), strlen(toks[i]), size_t, "%zu");
    }
    CHECK(sv_begin(cur), sv_begin(ref), char *const, "%s");
    i = 1;
    str_view cur2 = sv_rbegin_tok(ref, SV(","));
    for (; !sv_rend_tok(ref, cur2) && i;
         cur2 = sv_rnext_tok(ref, cur2, SV(",")))
    {
        --i;
        CHECK(sv_cmp(cur2, ref), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(cur2), sv_len(ref), size_t, "%zu");
    }
    CHECK(sv_begin(cur2), sv_begin(ref), char *const, "%s");
    return PASS;
}

static enum test_result
test_min_delim(void)
{
    str_view ref = SV("/0");
    const str_view delim = SV("/");
    const str_view tok = SV("0");
    str_view i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i); i = sv_next_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    ref = SV("0/");
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i); i = sv_next_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    ref = SV("/0/");
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i); i = sv_next_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    ref = SV("0/0");
    size_t sz = 2;
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i) && sz; i = sv_next_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("/0/0");
    sz = 2;
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i) && sz; i = sv_next_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("0/0/");
    sz = 2;
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i) && sz; i = sv_next_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("/0/0/");
    sz = 2;
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i) && sz; i = sv_next_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    return PASS;
}

static enum test_result
test_min_delim_two_byte(void)
{
    str_view ref = SV("//0");
    const str_view delim = SV("//");
    const str_view tok = SV("0");
    str_view i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i); i = sv_next_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    ref = SV("0//");
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i); i = sv_next_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    ref = SV("//0//");
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i); i = sv_next_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    ref = SV("0//0");
    size_t sz = 2;
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i) && sz; i = sv_next_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("//0//0");
    sz = 2;
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i) && sz; i = sv_next_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("0//0//");
    sz = 2;
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i) && sz; i = sv_next_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("//0//0//");
    sz = 2;
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i) && sz; i = sv_next_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    return PASS;
}

static enum test_result
test_min_delim_three_byte(void)
{
    str_view ref = SV("///0");
    const str_view delim = SV("///");
    const str_view tok = SV("0");
    str_view i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i); i = sv_next_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    ref = SV("0///");
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i); i = sv_next_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    ref = SV("///0///");
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i); i = sv_next_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    ref = SV("0///0");
    size_t sz = 2;
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i) && sz; i = sv_next_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("///0///0");
    sz = 2;
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i) && sz; i = sv_next_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("0///0///");
    sz = 2;
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i) && sz; i = sv_next_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("///0///0///");
    sz = 2;
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i) && sz; i = sv_next_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    return PASS;
}

static enum test_result
test_min_delim_four_byte(void)
{
    str_view ref = SV("////0");
    const str_view delim = SV("////");
    const str_view tok = SV("0");
    str_view i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i); i = sv_next_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    ref = SV("0////");
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i); i = sv_next_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    ref = SV("////0////");
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i); i = sv_next_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    ref = SV("0////0");
    size_t sz = 2;
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i) && sz; i = sv_next_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("////0////0");
    sz = 2;
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i) && sz; i = sv_next_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("0////0////");
    sz = 2;
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i) && sz; i = sv_next_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("////0////0////");
    sz = 2;
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i) && sz; i = sv_next_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    return PASS;
}

static enum test_result
test_min_delim_five_byte(void)
{
    str_view ref = SV("/////0");
    const str_view delim = SV("/////");
    const str_view tok = SV("0");
    str_view i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i); i = sv_next_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    ref = SV("0/////");
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i); i = sv_next_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    ref = SV("/////0/////");
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i); i = sv_next_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    ref = SV("0/////0");
    size_t sz = 2;
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i) && sz; i = sv_next_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("/////0/////0");
    sz = 2;
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i) && sz; i = sv_next_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("0/////0/////");
    sz = 2;
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i) && sz; i = sv_next_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("/////0/////0/////");
    sz = 2;
    i = sv_begin_tok(ref, delim);
    for (; !sv_end_tok(ref, i) && sz; i = sv_next_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_end(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    return PASS;
}

static enum test_result
test_rmin_delim(void)
{
    str_view ref = SV("/0");
    const str_view delim = SV("/");
    const str_view tok = SV("0");
    str_view i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i); i = sv_rnext_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    ref = SV("0/");
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i); i = sv_rnext_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    ref = SV("/0/");
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i); i = sv_rnext_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    ref = SV("0/0");
    size_t sz = 2;
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i) && sz; i = sv_rnext_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("/0/0");
    sz = 2;
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i) && sz; i = sv_rnext_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("0/0/");
    sz = 2;
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i) && sz; i = sv_rnext_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("/0/0/");
    sz = 2;
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i) && sz; i = sv_rnext_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    return PASS;
}

static enum test_result
test_rmin_delim_two_byte(void)
{
    str_view ref = SV("//0");
    const str_view delim = SV("//");
    const str_view tok = SV("0");
    str_view i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i); i = sv_rnext_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    ref = SV("0//");
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i); i = sv_rnext_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    ref = SV("//0//");
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i); i = sv_rnext_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    ref = SV("0//0");
    size_t sz = 2;
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i) && sz; i = sv_rnext_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("//0//0");
    sz = 2;
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i) && sz; i = sv_rnext_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("0//0//");
    sz = 2;
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i) && sz; i = sv_rnext_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("//0//0//");
    sz = 2;
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i) && sz; i = sv_rnext_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    return PASS;
}

static enum test_result
test_rmin_delim_three_byte(void)
{
    str_view ref = SV("///0");
    const str_view delim = SV("///");
    const str_view tok = SV("0");
    str_view i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i); i = sv_rnext_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    ref = SV("0///");
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i); i = sv_rnext_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    ref = SV("///0///");
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i); i = sv_rnext_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    ref = SV("0///0");
    size_t sz = 2;
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i) && sz; i = sv_rnext_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("///0///0");
    sz = 2;
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i) && sz; i = sv_rnext_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("0///0///");
    sz = 2;
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i) && sz; i = sv_rnext_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("///0///0///");
    sz = 2;
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i) && sz; i = sv_rnext_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    return PASS;
}

static enum test_result
test_rmin_delim_four_byte(void)
{
    str_view ref = SV("////0");
    const str_view delim = SV("////");
    const str_view tok = SV("0");
    str_view i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i); i = sv_rnext_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    ref = SV("0////");
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i); i = sv_rnext_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    ref = SV("////0////");
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i); i = sv_rnext_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    ref = SV("0////0");
    size_t sz = 2;
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i) && sz; i = sv_rnext_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("////0////0");
    sz = 2;
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i) && sz; i = sv_rnext_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("0////0////");
    sz = 2;
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i) && sz; i = sv_rnext_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("////0////0////");
    sz = 2;
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i) && sz; i = sv_rnext_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    return PASS;
}

static enum test_result
test_rmin_delim_five_byte(void)
{
    str_view ref = SV("/////0");
    const str_view delim = SV("/////");
    const str_view tok = SV("0");
    str_view i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i); i = sv_rnext_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    ref = SV("0/////");
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i); i = sv_rnext_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    ref = SV("/////0/////");
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i); i = sv_rnext_tok(ref, i, delim))
    {
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    ref = SV("0/////0");
    size_t sz = 2;
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i) && sz; i = sv_rnext_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("/////0/////0");
    sz = 2;
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i) && sz; i = sv_rnext_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("0/////0/////");
    sz = 2;
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i) && sz; i = sv_rnext_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    ref = SV("/////0/////0/////");
    sz = 2;
    i = sv_rbegin_tok(ref, delim);
    for (; !sv_rend_tok(ref, i) && sz; i = sv_rnext_tok(ref, i, delim))
    {
        --sz;
        CHECK(sv_cmp(i, tok), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(i), sv_len(tok), size_t, "%zu");
    }
    CHECK(sv_begin(i), sv_begin(ref), char *const, "%s");
    CHECK(sz, 0UL, size_t, "%zu");
    return PASS;
}

static enum test_result
test_simple_delim(void)
{
    const char *const reference = "0/1/2/2/3//3////3/4/4/4/////4";
    const char *const toks[11] = {
        "0", "1", "2", "2", "3", "3", "3", "4", "4", "4", "4",
    };
    const str_view ref_view = sv(reference);
    const str_view delim = sv("/");
    size_t i = 0;
    for (str_view tok = sv_begin_tok(ref_view, delim);
         !sv_end_tok(ref_view, tok) && i < sizeof(toks) / sizeof(toks[0]);
         tok = sv_next_tok(ref_view, tok, delim))
    {
        CHECK(sv_strcmp(tok, toks[i]), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(tok), strlen(toks[i]), size_t, "%zu");
        ++i;
    }
    CHECK(i, sizeof(toks) / sizeof(toks[0]), size_t, "%zu");
    return PASS;
}

static enum test_result
test_rsimple_delim(void)
{
    const char *const reference = "0/1/2/2/3//3////3/4/4/4/////4";
    const char *const toks[11] = {
        "0", "1", "2", "2", "3", "3", "3", "4", "4", "4", "4",
    };
    const size_t size = sizeof(toks) / sizeof(toks[0]);
    const str_view ref_view = sv(reference);
    const str_view delim = sv("/");
    size_t i = size;
    for (str_view tok = sv_rbegin_tok(ref_view, delim);
         !sv_rend_tok(ref_view, tok) && i;
         tok = sv_rnext_tok(ref_view, tok, delim))
    {
        --i;
        CHECK(sv_strcmp(tok, toks[i]), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(tok), strlen(toks[i]), size_t, "%zu");
    }
    CHECK(i, 0UL, size_t, "%zu");
    return PASS;
}

static enum test_result
test_tail_delim(void)
{
    const char *const reference = "0/1//2//2//3//3////3//4//4//4///////4578";
    const char *const toks[10] = {
        "0/1", "2", "2", "3", "3", "3", "4", "4", "4", "/4578",
    };
    const str_view ref_view = sv(reference);
    const str_view delim = sv("//");
    size_t i = 0;
    for (str_view tok = sv_begin_tok(ref_view, delim);
         !sv_end_tok(ref_view, tok); tok = sv_next_tok(ref_view, tok, delim))
    {
        CHECK(sv_strcmp(tok, toks[i]), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(tok), strlen(toks[i]), size_t, "%zu");
        ++i;
    }
    CHECK(i, sizeof(toks) / sizeof(toks[0]), size_t, "%zu");
    return PASS;
}

static enum test_result
test_rtail_delim(void)
{
    const char *const reference = "0/1//2//2//3//3////3//4//4//4///4578";
    const char *const toks[10] = {
        "0/1", "2", "2", "3", "3", "3", "4", "4", "4/", "4578",
    };
    const size_t size = sizeof(toks) / sizeof(toks[0]);
    const str_view ref_view = sv(reference);
    const str_view delim = sv("//");
    size_t i = size;
    for (str_view tok = sv_rbegin_tok(ref_view, delim);
         !sv_rend_tok(ref_view, tok); tok = sv_rnext_tok(ref_view, tok, delim))
    {
        --i;
        CHECK(sv_strcmp(tok, toks[i]), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(tok), strlen(toks[i]), size_t, "%zu");
    }
    CHECK(i, 0UL, size_t, "%zu");
    return PASS;
}

static enum test_result
test_rtriple_delim(void)
{
    const char *const reference
        = "!!0/1!!!2!!!2!!!3!3!!!!!!3!!!4!!!4!!4!!!4578";
    const char *const toks[8] = {
        "!!0/1", "2", "2", "3!3", "3", "4", "4!!4", "4578",
    };
    const size_t size = sizeof(toks) / sizeof(toks[0]);
    const str_view ref_view = sv(reference);
    const str_view delim = sv("!!!");
    size_t i = size;
    for (str_view tok = sv_rbegin_tok(ref_view, delim);
         !sv_rend_tok(ref_view, tok); tok = sv_rnext_tok(ref_view, tok, delim))
    {
        --i;
        CHECK(sv_strcmp(tok, toks[i]), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(tok), strlen(toks[i]), size_t, "%zu");
    }
    CHECK(i, 0UL, size_t, "%zu");
    return PASS;
}

static enum test_result
test_rquad_delim(void)
{
    const char *const reference
        = "!!!0/1!!!!2!!!!2!!!!3!!3!!!!!!!!3!!!!4!!!!4!!4!!!!4578";
    const char *const toks[8] = {
        "!!!0/1", "2", "2", "3!!3", "3", "4", "4!!4", "4578",
    };
    const size_t size = sizeof(toks) / sizeof(toks[0]);
    const str_view ref_view = sv(reference);
    const str_view delim = sv("!!!!");
    size_t i = size;
    for (str_view tok = sv_rbegin_tok(ref_view, delim);
         !sv_rend_tok(ref_view, tok); tok = sv_rnext_tok(ref_view, tok, delim))
    {
        --i;
        CHECK(sv_strcmp(tok, toks[i]), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(tok), strlen(toks[i]), size_t, "%zu");
    }
    CHECK(i, 0UL, size_t, "%zu");
    return PASS;
}

static enum test_result
test_iter_repeating_delim(void)
{
    const char *toks[14] = {
        "A",  "B", "C", "D",   "E", "F",  "G",
        "HI", "J", "K", "LMN", "O", "Pi", "\\(*.*)/",
    };
    const char *const reference
        = " A   B  C     D  E F G HI J   K LMN O   Pi  \\(*.*)/  ";
    const str_view ref_view = sv(reference);
    size_t i = 0;
    str_view cur = sv_begin_tok(ref_view, sv(" "));
    for (; !sv_end_tok(ref_view, cur);
         cur = sv_next_tok(ref_view, cur, sv(" ")))
    {
        CHECK(sv_strcmp(cur, toks[i]), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(cur), strlen(toks[i]), size_t, "%zu");
        ++i;
    }
    CHECK(sv_front(cur), '\0', char, "%c");
    str_view cur2 = sv_begin_tok(ref_view, sv(","));
    for (; !sv_end_tok(ref_view, cur2);
         cur2 = sv_next_tok(ref_view, cur2, sv(",")))
    {
        CHECK(sv_strcmp(cur2, reference), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(cur2), strlen(reference), size_t, "%zu");
    }
    CHECK(sv_front(cur2), '\0', char, "%c");
    return PASS;
}

static enum test_result
test_iter_multichar_delim(void)
{
    const char *toks[14] = {
        "A",     "B", "C", "D",      "E", "F",  "G",
        "HacbI", "J", "K", "LcbaMN", "O", "Pi", "\\(*.*)/",
    };
    const char *const reference
        = "abcAabcBabcCabcabcabcDabcEabcFabcGabcHacbIabcJabcabcabcabcKabcLcbaMN"
          "abcOabcabcPiabcabc\\(*.*)/abc";
    size_t i = 0;
    const str_view delim = SV("abc");
    const str_view ref_view = sv(reference);
    str_view cur = sv_begin_tok(ref_view, delim);
    for (; !sv_end_tok(ref_view, cur); cur = sv_next_tok(ref_view, cur, delim))
    {
        CHECK(sv_strcmp(cur, toks[i]), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(cur), strlen(toks[i]), size_t, "%zu");
        ++i;
    }
    CHECK(sv_front(cur), '\0', char, "%c");
    str_view cur2 = sv_begin_tok(ref_view, sv(" "));
    for (; !sv_end_tok(ref_view, cur2);
         cur2 = sv_next_tok(ref_view, cur2, sv(" ")))
    {
        CHECK(sv_strcmp(cur2, reference), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(cur2), strlen(reference), size_t, "%zu");
    }
    CHECK(sv_front(cur2), '\0', char, "%c");
    return PASS;
}

static enum test_result
test_riter_multichar_delim(void)
{
    const char *toks[14] = {
        "A",     "B", "C", "D",      "E", "F",  "G",
        "HacbI", "J", "K", "LcbaMN", "O", "Pi", "\\(*.*)/",
    };
    const size_t size = sizeof(toks) / sizeof(toks[0]);
    const char *const reference
        = "abcAabcBabcCabcabcabcDabcEabcFabcGabcHacbIabcJabcabcabcabcKabcLcbaMN"
          "abcOabcabcPiabcabc\\(*.*)/abc";
    const str_view delim = sv("abc");
    const str_view ref_view = sv(reference);
    str_view cur = sv_rbegin_tok(ref_view, delim);
    size_t i = size;
    for (; !sv_rend_tok(ref_view, cur) && i;
         cur = sv_rnext_tok(ref_view, cur, delim))
    {
        --i;
        CHECK(sv_strcmp(cur, toks[i]), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(cur), strlen(toks[i]), size_t, "%zu");
    }
    CHECK(i, 0UL, size_t, "%zu");
    CHECK(sv_begin(cur), reference, char *const, "%s");
    str_view cur2 = sv_rbegin_tok(ref_view, sv(" "));
    for (; !sv_rend_tok(ref_view, cur2);
         cur2 = sv_rnext_tok(ref_view, cur2, sv(" ")))
    {
        CHECK(sv_strcmp(cur2, reference), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(cur2), strlen(reference), size_t, "%zu");
    }
    CHECK(sv_begin(cur2), reference, char *const, "%s");
    return PASS;
}

static enum test_result
test_iter_multichar_delim_short(void)
{
    const char *toks[14] = {
        "A",     "B", "C", "D",      "E",   "F",  "G",
        "H---I", "J", "K", "L-M--N", "--O", "Pi", "\\(*.*)/",
    };
    const char *const reference = "-----A-----B-----C-----D-----E-----F-----G--"
                                  "---H---I-----J-----K-----L-M--N"
                                  "-------O-----Pi-----\\(*.*)/-----";
    size_t i = 0;
    const str_view delim = SV("-----");
    const str_view ref_view = sv(reference);
    str_view cur = sv_begin_tok(ref_view, delim);
    for (; !sv_end_tok(ref_view, cur); cur = sv_next_tok(ref_view, cur, delim))
    {
        CHECK(sv_strcmp(cur, toks[i]), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(cur), strlen(toks[i]), size_t, "%zu");
        ++i;
    }
    CHECK(sv_front(cur), '\0', char, "%c");
    str_view cur2 = sv_begin_tok(ref_view, sv(" "));
    for (; !sv_end_tok(ref_view, cur2);
         cur2 = sv_next_tok(ref_view, cur2, sv(" ")))
    {
        CHECK(sv_strcmp(cur2, reference), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(cur2), strlen(reference), size_t, "%zu");
    }
    CHECK(sv_front(cur2), '\0', char, "%c");
    return PASS;
}

static enum test_result
test_riter_multichar_delim_short(void)
{
    const char *toks[14] = {
        "A",     "B", "C", "D",        "E", "F",  "G",
        "H---I", "J", "K", "L-M--N--", "O", "Pi", "\\(*.*)/",
    };
    const size_t size = sizeof(toks) / sizeof(toks[0]);
    const char *const reference = "-----A-----B-----C-----D-----E-----F-----G--"
                                  "---H---I-----J-----K-----L-M--N"
                                  "-------O-----Pi-----\\(*.*)/-----";
    size_t i = size;
    const str_view delim = sv("-----");
    const str_view ref_view = sv(reference);
    str_view cur = sv_rbegin_tok(ref_view, delim);
    for (; !sv_rend_tok(ref_view, cur);
         cur = sv_rnext_tok(ref_view, cur, delim))
    {
        --i;
        CHECK(sv_strcmp(cur, toks[i]), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(cur), strlen(toks[i]), size_t, "%zu");
    }
    CHECK(sv_begin(cur), reference, char *const, "%s");
    CHECK(i, 0UL, size_t, "%zu");
    str_view cur2 = sv_rbegin_tok(ref_view, sv(" "));
    for (; !sv_rend_tok(ref_view, cur2);
         cur2 = sv_rnext_tok(ref_view, cur2, sv(" ")))
    {
        CHECK(sv_strcmp(cur2, reference), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(cur2), strlen(reference), size_t, "%zu");
    }
    CHECK(sv_begin(cur2), reference, char *const, "%s");
    return PASS;
}

static enum test_result
test_iter_delim_larger_than_str(void)
{
    const char *const ref = "A-B";
    const char *const delim = "-----";
    const str_view delim_view = sv(delim);
    const str_view ref_view = sv(ref);
    str_view constructed = sv_delim(ref, delim);
    str_view cur = sv_begin_tok(ref_view, delim_view);
    CHECK(sv_cmp(constructed, cur), EQL, sv_threeway_cmp, "%d");
    CHECK(sv_strcmp(constructed, ref), EQL, sv_threeway_cmp, "%d");
    CHECK(sv_strcmp(cur, ref), EQL, sv_threeway_cmp, "%d");

    for (; !sv_end_tok(ref_view, cur);
         cur = sv_next_tok(ref_view, cur, delim_view))
    {
        CHECK(sv_strcmp(cur, ref), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(cur), strlen(ref), size_t, "%zu");
    }
    CHECK(sv_front(cur), '\0', char, "%c");
    return PASS;
}

static enum test_result
test_riter_delim_larger_than_str(void)
{
    const char *const ref = "A-B";
    const char *const delim = "-----";
    const str_view delim_view = sv(delim);
    const str_view ref_view = sv(ref);
    str_view constructed = sv_delim(ref, delim);
    str_view cur = sv_rbegin_tok(ref_view, delim_view);
    CHECK(sv_cmp(constructed, cur), EQL, sv_threeway_cmp, "%d");
    CHECK(sv_strcmp(constructed, ref), EQL, sv_threeway_cmp, "%d");
    CHECK(sv_strcmp(cur, ref), EQL, sv_threeway_cmp, "%d");

    for (; !sv_rend_tok(ref_view, cur);
         cur = sv_rnext_tok(ref_view, cur, delim_view))
    {
        CHECK(sv_cmp(cur, ref_view), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(cur), sv_len(ref_view), size_t, "%zu");
    }
    CHECK(*sv_begin(cur), *ref, char, "%c");
    return PASS;
}

static enum test_result
test_tokenize_not_terminated(void)
{
    const char *const path_str = "this/path/will/be/missing/its/child";
    const char *const toks[6] = {
        "this", "path", "will", "be", "missing", "its",
    };
    const str_view path = sv(path_str);
    const str_view delim = sv("/");
    const str_view childless_path
        = sv_remove_suffix(path, sv_len(path) - sv_find_last_of(path, delim));
    size_t i = 0;
    for (str_view tok = sv_begin_tok(childless_path, delim);
         !sv_end_tok(childless_path, tok);
         tok = sv_next_tok(childless_path, tok, delim))
    {
        CHECK(sv_strcmp(tok, toks[i]), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(tok), strlen(toks[i]), size_t, "%zu");
        ++i;
    }
    CHECK(i, sizeof(toks) / sizeof(toks[0]), size_t, "%zu");
    return PASS;
}

static enum test_result
test_tokenize_three_views(void)
{
    const char *const path_str = "all/of/these/paths/are/unique/and/split/up";
    const char *const toks[3][3] = {
        {"all", "of", "these"},
        {"paths", "are", "unique"},
        {"and", "split", "up"},
    };
    const size_t size = sizeof(toks) / sizeof(toks[0]);
    const str_view path = sv(path_str);
    const str_view delim = sv("/");
    const str_view first = sv_substr(path, 0, sv_find(path, 0, sv("/paths/")));
    const str_view second = sv_substr(path, sv_find(path, 0, sv("/paths/")),
                                      sv_find(path, 0, sv("/and/"))
                                          - sv_find(path, 0, sv("/paths/")));
    const str_view third
        = sv_substr(path, sv_find(path, 0, sv("/and/")),
                    sv_len(path) - sv_find(path, 0, sv("/and/")));
    size_t i = 0;
    for (str_view tok1 = sv_begin_tok(first, delim),
                  tok2 = sv_begin_tok(second, delim),
                  tok3 = sv_begin_tok(third, delim);
         !sv_end_tok(first, tok1) && !sv_end_tok(second, tok2)
         && !sv_end_tok(third, tok3) && i < size;
         tok1 = sv_next_tok(first, tok1, delim),
                  tok2 = sv_next_tok(second, tok2, delim),
                  tok3 = sv_next_tok(third, tok3, delim))
    {
        CHECK(sv_strcmp(tok1, toks[0][i]), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(tok1), strlen(toks[0][i]), size_t, "%zu");
        CHECK(sv_strcmp(tok2, toks[1][i]), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(tok2), strlen(toks[1][i]), size_t, "%zu");
        CHECK(sv_strcmp(tok3, toks[2][i]), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(tok3), strlen(toks[2][i]), size_t, "%zu");
        ++i;
    }
    CHECK(i, sizeof(toks) / sizeof(toks[0]), size_t, "%zu");
    return PASS;
}

static enum test_result
test_rtokenize_three_views(void)
{
    const char *const path_str = "all/of/these/paths/are/unique/and/split/up";
    const char *const toks[3][3] = {
        {"all", "of", "these"},
        {"paths", "are", "unique"},
        {"and", "split", "up"},
    };
    const size_t size = sizeof(toks) / sizeof(toks[0]);
    const str_view path = sv(path_str);
    const str_view delim = sv("/");
    const str_view first = sv_substr(path, 0, sv_find(path, 0, sv("/paths/")));
    const str_view second = sv_substr(path, sv_find(path, 0, sv("/paths/")),
                                      sv_find(path, 0, sv("/and/"))
                                          - sv_find(path, 0, sv("/paths/")));
    const str_view third
        = sv_substr(path, sv_find(path, 0, sv("/and/")),
                    sv_len(path) - sv_find(path, 0, sv("/and/")));
    size_t i = size;
    for (str_view tok1 = sv_rbegin_tok(first, delim),
                  tok2 = sv_rbegin_tok(second, delim),
                  tok3 = sv_rbegin_tok(third, delim);
         !sv_rend_tok(first, tok1) && !sv_rend_tok(second, tok2)
         && !sv_rend_tok(third, tok3) && i;
         tok1 = sv_rnext_tok(first, tok1, delim),
                  tok2 = sv_rnext_tok(second, tok2, delim),
                  tok3 = sv_rnext_tok(third, tok3, delim))
    {
        --i;
        CHECK(sv_strcmp(tok1, toks[0][i]), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(tok1), strlen(toks[0][i]), size_t, "%zu");
        CHECK(sv_strcmp(tok2, toks[1][i]), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(tok2), strlen(toks[1][i]), size_t, "%zu");
        CHECK(sv_strcmp(tok3, toks[2][i]), EQL, sv_threeway_cmp, "%d");
        CHECK(sv_len(tok3), strlen(toks[2][i]), size_t, "%zu");
    }
    CHECK(i, 0UL, size_t, "%zu");
    return PASS;
}
