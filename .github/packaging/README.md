# Release binaries

The `Build release binaries` workflow runs when any tag is pushed (including a
new tag created through GitHub). Both builds must succeed before the workflow
creates a release or attaches assets to an existing release for that tag.
Rerunning a tag build replaces the assets with the same names; existing release
notes are preserved. New releases remain drafts until the uploaded archives and
checksums have been downloaded and verified. A failed upload can be retried;
its draft will not be published until verification succeeds. Updates to already
published releases replace assets in place and are not atomic.
No additional secrets are required.

- `QTester104-linux-x86_64.tar.gz`: extract and run `QTester104/AppRun`.
  Qt libraries and plugins are bundled. Built on Ubuntu 24.04; requires a
  compatible x86_64 Linux system with glibc 2.39 or newer and a graphical desktop
  (X11 or XWayland). Edit `QTester104/usr/bin/qtester104.ini` to configure it.
- `QTester104-windows-x64.zip`: extract the entire archive and run
  the included `vc_redist.x64.exe` first to install the MSVC runtime, then run
  `QTester104.exe` on Windows 10/11 x64. See `README-FIRST.txt` in the ZIP.
  Qt libraries and plugins are included. Edit the adjacent `qtester104.ini`
  to configure it.
- `SHA256SUMS.txt`: checksums of both release archives.

Both packages include the license and application readme. Linux uses the Qt
OpenSSL TLS backend and the system OpenSSL 3 runtime; Windows includes Qt's
native Schannel TLS backend.

To release, push a tag pointing to a commit containing this workflow, for example:

```sh
git tag V3.2.1
git push origin V3.2.1
```

Use Actions → Build release binaries → Run workflow to test builds without
publishing a release. Download the two build artifacts from that run. Existing
tags are not rebuilt automatically when this workflow is added. Tags pushed by
another workflow using `GITHUB_TOKEN` do not trigger a new workflow run.

Qt is pinned in `QT_VERSION` in the workflow. Windows uses MSVC 2022; Linux uses
the matching Qt GCC desktop distribution. Linux runtime packaging uses
[linuxdeploy](https://github.com/linuxdeploy/linuxdeploy) and its
[Qt plugin](https://github.com/linuxdeploy/linuxdeploy-plugin-qt).

The deployment tools use fixed release URLs and SHA-256 checksums in
`linuxdeploy.sha256`. When upgrading a tool, update its URL and verified checksum
together. Download failures or checksum mismatches fail the build.

Both jobs extract the finished archive and launch its application with the
build-time Qt paths removed. Linux uses Xvfb to exercise the X11 platform plugin;
Windows runs the bundled runtime installer before checking startup. The check
fails if the process exits within five seconds. This catches startup and plugin
loading failures, but is not a protocol or TLS handshake test. The hosted Windows
runner already has development runtimes installed, so this is not a substitute
for testing on a clean Windows machine.
