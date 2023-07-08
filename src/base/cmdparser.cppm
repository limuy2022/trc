/****************************************************************************

getopt.c - Read command line options

AUTHOR: Gregory Pietsch
CREATED Fri Jan 10 21:13:05 1997

DESCRIPTION:

The getopt() function parses the command line arguments.  Its arguments argc
and argv are the argument count and array as passed to the main() function
on program invocation.  The argument optstring is a list of available option
characters.  If such a character is followed by a colon (`:'), the option
takes an argument, which is placed in optarg.  If such a character is
followed by two colons, the option takes an optional argument, which is
placed in optarg.  If the option does not take an argument, optarg is NULL.

The external variable optind is the index of the next array element of argv
to be processed; it communicates from one call to the next which element to
process.

The getopt_long() function works like getopt() except that it also accepts
long options started by two dashes `--'.  If these take values, it is either
in the form

--arg=value

 or

--arg value

It takes the additional arguments longopts which is a pointer to the first
element of an array of type GETOPT_LONG_OPTION_T.  The last element of the
array has to be filled with NULL for the name field.

The longind pointer points to the index of the current long option relative
to longopts if it is non-NULL.

The getopt() function returns the option character if the option was found
successfully, `:' if there was a missing parameter for one of the options,
`?' for an unknown option character, and EOF for the end of the option list.

The getopt_long() function's return value is described in the header file.

The function getopt_long_only() is identical to getopt_long(), except that a
plus sign `+' can introduce long options as well as `--'.

The following describes how to deal with options that follow non-option
argv-elements.

If the caller did not specify anything, the default is REQUIRE_ORDER if the
environment variable POSIXLY_CORRECT is defined, PERMUTE otherwise.

REQUIRE_ORDER means don't recognize them as options; stop option processing
when the first non-option is seen.  This is what Unix does.  This mode of
operation is selected by either setting the environment variable
POSIXLY_CORRECT, or using `+' as the first character of the optstring
parameter.

PERMUTE is the default.  We permute the contents of ARGV as we scan, so that
eventually all the non-options are at the end.  This allows options to be
given in any order, even with programs that were not written to expect this.

RETURN_IN_ORDER is an option available to programs that were written to
expect options and other argv-elements in any order and that care about the
ordering of the two.  We describe each non-option argv-element as if it were
the argument of an option with character code 1.  Using `-' as the first
character of the optstring parameter selects this mode of operation.

The special argument `--' forces an end of option-scanning regardless of the
value of ordering.  In the case of RETURN_IN_ORDER, only `--' can cause
getopt() and friends to return EOF with optind != argc.

COPYRIGHT NOTICE AND DISCLAIMER:

Copyright (C) 1997 Gregory Pietsch

This file and the accompanying getopt.h header file are hereby placed in the
public domain without restrictions.  Just give the author credit, don't
claim you wrote it or prevent anyone else from using it.

Gregory Pietsch's current e-mail address:
gpietsch@comcast.net
****************************************************************************/

module;
#include <cstdio>
#include <cstdlib>
#include <cstring>
export module cmdparser;

namespace trc::cmdparser {
export const int no_argument = 0;
export const int required_argument = 1;
export const int OPTIONAL_ARG = 2;

/* GETOPT_LONG_OPTION_T: The type of long option */
export typedef struct GETOPT_LONG_OPTION_T {
    const char* name; /* the name of the long option */
    int has_arg; /* one of the above macros */
    int* flag; /* determines if getopt_long() returns a
                * value for a long option; if it is
                * non-NULL, 0 is returned as a function
                * value and the value of val is stored in
                * the area pointed to by flag.  Otherwise,
                * val is returned. */
    int val; /* determines the value to return if flag is
              * NULL. */
} GETOPT_LONG_OPTION_T;

export typedef GETOPT_LONG_OPTION_T option;

/* globally-defined variables */
export char* optarg = nullptr;
export int optind = 0;
export int opterr = 1;
export int optopt = '?';

/* types */
typedef enum GETOPT_ORDERING_T {
    PERMUTE,
    RETURN_IN_ORDER,
    REQUIRE_ORDER
} GETOPT_ORDERING_T;

/* reverse_argv_elements:  reverses num elements starting at argv */
static void reverse_argv_elements(char** argv, int num) {
    int i;
    char* tmp;

    for (i = 0; i < (num >> 1); i++) {
        tmp = argv[i];
        argv[i] = argv[num - i - 1];
        argv[num - i - 1] = tmp;
    }
}

/* permute: swap two blocks of argv-elements given their lengths */
static void permute(char** argv, int len1, int len2) {
    reverse_argv_elements(argv, len1);
    reverse_argv_elements(argv, len1 + len2);
    reverse_argv_elements(argv, len2);
}

/* is_option: is this argv-element an option or the end of the option list? */
static int is_option(char* argv_element, int only) {
    return ((argv_element == nullptr) || (argv_element[0] == '-')
        || (only && argv_element[0] == '+'));
}

/* getopt_internal:  the function that does all the dirty work */
static int getopt_internal(int argc, char** argv, char* shortopts,
    GETOPT_LONG_OPTION_T* longopts, int* longind, int only) {
    GETOPT_ORDERING_T ordering = PERMUTE;
    static size_t optwhere = 0;
    size_t permute_from = 0;
    int num_nonopts = 0;
    int optindex = 0;
    size_t match_chars = 0;
    char* possible_arg = nullptr;
    int longopt_match = -1;
    int has_arg = -1;
    char* cp = nullptr;
    int arg_next = 0;

    /* first, deal with silly parameters and easy stuff */
    if (argc == 0 || argv == nullptr
        || (shortopts == nullptr && longopts == nullptr))
        return (optopt = '?');
    if (optind >= argc || argv[optind] == nullptr)
        return EOF;
    if (strcmp(argv[optind], "--") == 0) {
        optind++;
        return EOF;
    }
    /* if this is our first time through */
    if (optind == 0)
        optind = optwhere = 1;

    /* define ordering */
    if (shortopts != nullptr && (*shortopts == '-' || *shortopts == '+')) {
        ordering = (*shortopts == '-') ? RETURN_IN_ORDER : REQUIRE_ORDER;
        shortopts++;
    } else
        ordering
            = (getenv("POSIXLY_CORRECT") != nullptr) ? REQUIRE_ORDER : PERMUTE;

    /*
     * based on ordering, find our next option, if we're at the beginning of
     * one
     */
    if (optwhere == 1) {
        switch (ordering) {
        case PERMUTE:
            permute_from = optind;
            num_nonopts = 0;
            while (!is_option(argv[optind], only)) {
                optind++;
                num_nonopts++;
            }
            if (argv[optind] == nullptr) {
                /* no more options */
                optind = permute_from;
                return EOF;
            } else if (strcmp(argv[optind], "--") == 0) {
                /* no more options, but have to get `--' out of the way */
                permute(argv + permute_from, num_nonopts, 1);
                optind = permute_from + 1;
                return EOF;
            }
            break;
        case RETURN_IN_ORDER:
            if (!is_option(argv[optind], only)) {
                optarg = argv[optind++];
                return (optopt = 1);
            }
            break;
        case REQUIRE_ORDER:
            if (!is_option(argv[optind], only))
                return EOF;
            break;
        }
    }
    /* we've got an option, so parse it */

    /* first, is it a long option? */
    if (longopts != nullptr
        && (memcmp(argv[optind], "--", 2) == 0
            || (only && argv[optind][0] == '+'))
        && optwhere == 1) {
        /* handle long options */
        if (memcmp(argv[optind], "--", 2) == 0)
            optwhere = 2;
        longopt_match = -1;
        possible_arg = strchr(argv[optind] + optwhere, '=');
        if (possible_arg == nullptr) {
            /* no =, so next argv might be arg */
            match_chars = strlen(argv[optind]);
            possible_arg = argv[optind] + match_chars;
            match_chars = match_chars - optwhere;
        } else
            match_chars = (possible_arg - argv[optind]) - optwhere;
        for (optindex = 0; longopts[optindex].name != nullptr; optindex++) {
            if (memcmp(argv[optind] + optwhere, longopts[optindex].name,
                    match_chars)
                == 0) {
                /* do we have an exact match? */
                if (match_chars == strlen(longopts[optindex].name)) {
                    longopt_match = optindex;
                    break;
                }
                /* do any characters match? */
                else {
                    if (longopt_match < 0)
                        longopt_match = optindex;
                    else {
                        /* we have ambiguous options */
                        if (opterr)
                            fprintf(stderr,
                                "%s: option `%s' is ambiguous "
                                "(could be `--%s' or `--%s')\n",
                                argv[0], argv[optind],
                                longopts[longopt_match].name,
                                longopts[optindex].name);
                        return (optopt = '?');
                    }
                }
            }
        }
        if (longopt_match >= 0)
            has_arg = longopts[longopt_match].has_arg;
    }
    /* if we didn't find a long option, is it a short option? */
    if (longopt_match < 0 && shortopts != nullptr) {
        cp = strchr(shortopts, argv[optind][optwhere]);
        if (cp == nullptr) {
            /* couldn't find option in shortopts */
            if (opterr)
                fprintf(stderr, "%s: invalid option -- `-%c'\n", argv[0],
                    argv[optind][optwhere]);
            optwhere++;
            if (argv[optind][optwhere] == '\0') {
                optind++;
                optwhere = 1;
            }
            return (optopt = '?');
        }
        has_arg = ((cp[1] == ':')
                ? ((cp[2] == ':') ? OPTIONAL_ARG : required_argument)
                : no_argument);
        possible_arg = argv[optind] + optwhere + 1;
        optopt = *cp;
    }
    /* get argument and reset optwhere */
    arg_next = 0;
    switch (has_arg) {
    case OPTIONAL_ARG:
        if (*possible_arg == '=')
            possible_arg++;
        if (*possible_arg != '\0') {
            optarg = possible_arg;
            optwhere = 1;
        } else
            optarg = nullptr;
        break;
    case required_argument:
        if (*possible_arg == '=')
            possible_arg++;
        if (*possible_arg != '\0') {
            optarg = possible_arg;
            optwhere = 1;
        } else if (optind + 1 >= argc) {
            if (opterr) {
                fprintf(stderr, "%s: argument required for option `", argv[0]);
                if (longopt_match >= 0)
                    fprintf(stderr, "--%s'\n", longopts[longopt_match].name);
                else
                    fprintf(stderr, "-%c'\n", *cp);
            }
            optind++;
            return (optopt = ':');
        } else {
            optarg = argv[optind + 1];
            arg_next = 1;
            optwhere = 1;
        }
        break;
    case no_argument:
        if (longopt_match < 0) {
            optwhere++;
            if (argv[optind][optwhere] == '\0')
                optwhere = 1;
        } else
            optwhere = 1;
        optarg = nullptr;
        break;
    }

    /* do we have to permute or otherwise modify optind? */
    if (ordering == PERMUTE && optwhere == 1 && num_nonopts != 0) {
        permute(argv + permute_from, num_nonopts, 1 + arg_next);
        optind = permute_from + 1 + arg_next;
    } else if (optwhere == 1)
        optind = optind + 1 + arg_next;

    /* finally return */
    if (longopt_match >= 0) {
        if (longind != nullptr)
            *longind = longopt_match;
        if (longopts[longopt_match].flag != nullptr) {
            *(longopts[longopt_match].flag) = longopts[longopt_match].val;
            return 0;
        } else
            return longopts[longopt_match].val;
    } else
        return optopt;
}

/* function prototypes */
export int getopt(int argc, char** argv, char* optstring) {
    return getopt_internal(argc, argv, optstring, nullptr, nullptr, 0);
}

export int getopt_long(int argc, char** argv, const char* shortopts,
    const GETOPT_LONG_OPTION_T* longopts, int* longind) {
    return getopt_internal(argc, argv, (char*)shortopts,
        (GETOPT_LONG_OPTION_T*)longopts, longind, 0);
}

export int getopt_long_only(int argc, char** argv, const char* shortopts,
    const GETOPT_LONG_OPTION_T* longopts, int* longind) {
    return getopt_internal(argc, argv, (char*)shortopts,
        (GETOPT_LONG_OPTION_T*)longopts, longind, 1);
}
}
