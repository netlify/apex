# Contributing to Apex

First, thanks for the interest in contributing! Before contributing be sure to read out [Code of Conduct](CODE_OF_CONDUCT.md)

## License

By contributing to Apex, you agree to license your commits under the [Apache 2.0 License](LICENSE.md).

## Adding New Features

When adding new features a few things have to be taken into account:

 * Your code must work with Clang in C++20 mode, and work on Linux.
    * At some point in the future, it will have to work with libc++, instead of libstdc++
 * Your code must pass all sanitizer checks
 * Your code must be documented.
 * If new code, you must supply at least 2 unit test cases.

## Style Guides

### C++ Style Guide

Currently, this is enforced with a mix of both clang-tidy and clang-format. That said, clang-format cannot (currently) enforce several style decisions found in our code base:

 * east `const` -> `auto const name`, `type const&`, etc.
 * `everything_is_snake_case`, `template <class ExceptForTemplateParameters>`
 * `typename` may only be used when using a dependent type, or for a 'valid type check' in a `concept`.
 * 2 spaces per indent
 
(NOTE: More will be added above as time goes on, however clang-tidy and clang-format cannot always be used, so following them is less important than other checks)

### Git Commit Messages

Git commit messages should utilize [gitm😍ji](https://gitmoji.carloscuesta.me/) for each line item. If a line item requires more information, place it in a paragraph below the line item with a 2 space indent (unless the description is for the subject line of the commit). e.g.,

```gitcommit
⚡ Improve the speed with fooing the bar

  Fooing a bar has been proven to be slow in certain cases due to a lack of
  constexpr-ing all the things. This change improves this by marking some
  foo overloads constexpr when operating on a bar.
🐛 Resolve outstanding std::launder bug
  After consulting with 13 oracles, 12 wizards, and the editor of the C++ standard
  we've finally figured out when to use std::launder: never, unless told to otherwise
♻ Refactor several functions to use concepts
```
