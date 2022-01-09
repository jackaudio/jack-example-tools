# Contributing Guidelines

These are the contributing guidelines for jack-example-tools.
All contributions, unless noted otherwise, are licensed under the terms of the
GPL-2.0-or-later (see [LICENSE](LICENSE)).
Contributions to the [tools/zalsa/](tools/zalsa/) are licensed under the terms
of the GPL-3.0-or-later (see [tools/zalsa/LICENSE](tools/zalsa/LICENSE)).

Problems with and feature requests for the project may be reported in the
[issue tracker](https://github.com/jackaudio/jack-example-tools/issues).

Changes to the code-base can be provided via [pull
requests](https://github.com/jackaudio/jack-example-tools/pulls).

## Continuous Integration

The project is built in a [continuous integration
(CI)](https://github.com/jackaudio/jack-example-tools/actions) pipeline handled
by github actions upon pull request and push to the default branch. The
integration aims at covering all jack implementations on as many operating
systems as possible.

Changes to the project may only be merged if the CI finishes successfully.

## Testing

Contributors are expected to test their changes to the project on as many
operating systems as available to them *before* opening a pull request.
