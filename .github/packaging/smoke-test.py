"""Check that an extracted GUI package stays running without build-time Qt paths."""

import os
from pathlib import Path
import subprocess
import sys
import tempfile


def main():
    executable = Path(sys.argv[1]).absolute()
    if not executable.is_file():
        raise SystemExit(f"Missing executable: {executable}")

    env = {
        key: value for key, value in os.environ.items()
        if not key.upper().startswith(("QT_", "QML"))
        and key.upper() not in {"LD_LIBRARY_PATH", "LD_PRELOAD"}
    }
    # Do not resolve AppRun's symlink: exercise the package's actual entry point.
    if os.name == "nt":
        system_root = Path(os.environ["SystemRoot"])
        env["PATH"] = os.pathsep.join(map(str, [executable.parent, system_root / "System32", system_root]))
    else:
        env["PATH"] = "/usr/bin:/bin"

    with tempfile.TemporaryDirectory() as directory:
        # Explicit config prevents the distributed sample from contacting real devices.
        config = Path(directory) / "smoke.ini"
        config.write_text("[RTU1]\nIP_ADDRESS=\nALLOW_COMMANDS=0\n")
        with tempfile.TemporaryFile() as log:
            process = subprocess.Popen(
                [str(executable), str(config)], cwd=directory, env=env,
                stdout=log, stderr=subprocess.STDOUT,
            )
            try:
                code = process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                print("PASS: packaged application remained running for 5 seconds")
            else:
                log.seek(0)
                raise SystemExit(f"Application exited early ({code}):\n{log.read().decode(errors='replace')}")
            finally:
                if process.poll() is None:
                    process.terminate()
                    try:
                        process.wait(timeout=5)
                    except subprocess.TimeoutExpired:
                        process.kill()
                        process.wait()


if __name__ == "__main__":
    main()
