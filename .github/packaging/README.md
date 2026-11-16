# Release binaries

The `Build release binaries` workflow runs when any tag is pushed (including a
new tag created through GitHub). Both builds must succeed before the workflow
creates a release or attaches assets to an existing release for that tag.
Rerunning a tag build replaces the assets with the same names; existing release
notes are preserved. No additional secrets are required.

- `QTester104-linux-x86_64.tar.gz`: extract and run `QTester104/AppRun`.
  Qt libraries and plugins are bundled. Built on Ubuntu 24.04; requires a
  compatible x86_64 Linux system with glibc 2.39 or newer and a graphical desktop
  (X11 or XWayland). Edit `QTester104/usr/bin/qtester104.ini` to configure it.
- `QTester104-windows-x64.zip`: extract the entire archive and run
  `QTester104.exe` on Windows 10/11 x64. Qt plugins and the MSVC runtime are
  included. Edit the adjacent `qtester104.ini` to configure it.
- `SHA256SUMS.txt`: checksums of both release archives.

Both packages include the license and application readme. Linux uses the Qt
OpenSSL TLS backend and the system OpenSSL 3 runtime; Windows includes Qt's
native Schannel TLS backend.

To release, push a tag pointing to a commit containing this workflow, for example:

```sh
git tag V3.1.2
git push origin V3.1.2
```

Use Actions → Build release binaries → Run workflow to test builds without
publishing a release. Download the two build artifacts from that run. Existing
tags are not rebuilt automatically when this workflow is added. Tags pushed by
another workflow using `GITHUB_TOKEN` do not trigger a new workflow run.

Qt is pinned in `QT_VERSION` in the workflow. Windows uses MSVC 2022; Linux uses
the matching Qt GCC desktop distribution. Linux runtime packaging uses
[linuxdeploy](https://github.com/linuxdeploy/linuxdeploy) and its
[Qt plugin](https://github.com/linuxdeploy/linuxdeploy-plugin-qt).
