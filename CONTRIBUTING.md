# Contribution Guidelines

## Developer Certificate of Origin (DCO)

All contributions to The FINCH CubeSat Project Flight Software require agreement to
the Developer Certificate of Origin (DCO).
The full text of the DCO is provided below and can also be found at
https://developercertificate.org:
```
Developer Certificate of Origin
Version 1.1

Copyright (C) 2004, 2006 The Linux Foundation and its contributors.

Everyone is permitted to copy and distribute verbatim copies of this
license document, but changing it is not allowed.


Developer's Certificate of Origin 1.1

By making a contribution to this project, I certify that:

(a) The contribution was created in whole or in part by me and I
    have the right to submit it under the open source license
    indicated in the file; or

(b) The contribution is based upon previous work that, to the best
    of my knowledge, is covered under an appropriate open source
    license and I have the right under that license to submit that
    work with modifications, whether created in whole or in part
    by me, under the same open source license (unless I am
    permitted to submit under a different license), as indicated
    in the file; or

(c) The contribution was provided directly to me by some other
    person who certified (a), (b) or (c) and I have not modified
    it.

(d) I understand and agree that this project and the contribution
    are public and that a record of the contribution (including all
    personal information I submit with it, including my sign-off) is
    maintained indefinitely and may be redistributed consistent with
    this project or the open source license(s) involved.
```
All contributors must agree to the DCO when submitting patches, as indicated by including
a `Signed-off-by:` line in every patch.
```
Signed-off-by: Your Full Name <example@email.domain>
```

## Git Commit Guidelines

Multiple well-structured commits are preferred over a single squashed commit.

This approach is preferred because:

1. Each commit should represent a single, logical change, making it easier to review and understand
2. Well-structured commits can be reverted individually if necessary,
without breaking working areas of code
3. Cherry-picking specific commits into other branches is much easier and
safer when commits are atomic and meaningful
4. Tools like `git blame` and `git bisect` are more effective
when each commit has a clear purpose and detailed message
5. Helps maintain a Git project that will scale over many years with many contributors

### Required PR Commit Structure

When submitting a pull request, organize your commits as follows:

1. Enable Kconfig controlled options or devicetree related changes
2. Code commits for adding or updating source code, such as drivers or libraries
3. Test code commits for adding or updating tests related to the new or modified code
4. Continuous Integration (CI) changes to update or add workflows or scripts supporting the changes
5. Test image updates
6. Flight software updates

## Git Commit Message Requirements

All contributors must follow the format below:
```
[area]: [subject line]

[commit message body (minimum one line)]

Signed-off-by: Your Full Name <example@email.domain>
```
In the commit message body, all contributors are required to provide detailed explanations
of the changes, including the reasoning and justification behind them.
Well-written commit messages are important for project history and tools like `git blame`,
which help contributors and maintainers understand the origin and reasoning behind specific changes.
Also, note that each line must not exceed 72 characters, and the Git author name and email address
must match those used in the DCO signoff.
