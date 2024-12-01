from hypothesis import given, strategies as st, note
from utils import *


@given(
    st.text("abcdefghijklmnopqrstuvwxyz", min_size=1, max_size=20),
    st.integers(min_value=0, max_value=255),
)
def test_simple(cmd_name, exit_code):
    p = TMP_DIR.joinpath(cmd_name)

    with open(p, 'w') as f:
        f.write(f'''\
#!/usr/bin/env sh

exit {exit_code}
''')

    p.chmod(0o744)

    try:
        res, outp, err = run_command([f'{SRC_DIR}/assist.sh', p])

        note(str((res, outp, err)))

        assert res == 0
        assert str(exit_code) in outp

    finally:
        p.unlink(missing_ok=True)


@given(
    st.text("abcdefghijklmnopqrstuvwxyz", min_size=1, max_size=20),
    st.text("abcdefghijklmnopqrstuvwxyz", min_size=15),
    st.integers(min_value=0, max_value=255),
)
def test_stdout(cmd_name, content, exit_code):
    p = TMP_DIR.joinpath(cmd_name)

    with open(p, 'w') as f:
        f.write(f'''\
#!/usr/bin/env sh

cat <<EOF
{content}
EOF

exit {exit_code}
''')

    p.chmod(0o744)

    try:
        res, outp, err = run_command([f'{SRC_DIR}/assist.sh', p], f'o\n')

        note(str((res, outp, err)))

        assert res == 0
        assert content in outp
        assert str(exit_code) in outp

    finally:
        p.unlink(missing_ok=True)


@given(
    st.text("abcdefghijklmnopqrstuvwxyz", min_size=1, max_size=20),
    st.text("abcdefghijklmnopqrstuvwxyz", min_size=15),
    st.integers(min_value=0, max_value=255),
)
def test_nothing(cmd_name, content, exit_code):
    p = TMP_DIR.joinpath(cmd_name)

    with open(p, 'w') as f:
        f.write(f'''\
#!/usr/bin/env sh

cat <<EOF
{content}
EOF

exit {exit_code}
''')

    p.chmod(0o744)

    try:
        res, outp, err = run_command([f'{SRC_DIR}/assist.sh', p], f'n\n')

        note(str((res, outp, err)))

        assert res == 0
        assert content not in outp
        assert str(exit_code) in outp

    finally:
        p.unlink(missing_ok=True)


@given(
    st.text("abcdefghijklmnopqrstuvwxyz", min_size=1, max_size=20),
    st.text("abcdefghijklmnopqrstuvwxyz", min_size=15),
)
def test_stdin(cmd_name, content):
    p = TMP_DIR.joinpath(cmd_name)

    with open(p, 'w') as f:
        f.write(content)

    try:
        res, outp, err = run_command([f'{SRC_DIR}/assist.sh', '/usr/bin/env', 'cat', p, '-'], f'o\n')

        note(str((res, outp, err)))

        assert res == 0
        assert content in outp
        assert '0' in outp

    finally:
        p.unlink(missing_ok=True)


@given(
    st.text("abcdefghijklmnopqrstuvwxyz", min_size=1, max_size=20),
    st.text("abcdefghijklmnopqrstuvwxyz", min_size=15),
    st.integers(min_value=0, max_value=255),
)
def test_pager(cmd_name, content, exit_code):
    p = TMP_DIR.joinpath(cmd_name)

    with open(p, 'w') as f:
        f.write(f'''\
#!/usr/bin/env sh

cat <<EOF
{content}
EOF

exit {exit_code}
''')

    p.chmod(0o744)

    try:
        res, outp, err = run_command([f'{SRC_DIR}/assist.sh', p], f'p\n', env={'PAGER': 'cat'})

        note(str((res, outp, err)))

        assert res == 0
        assert content in outp
        assert str(exit_code) in outp

    finally:
        p.unlink(missing_ok=True)


@given(
    st.text("abcdefghijklmnopqrstuvwxyz", min_size=1, max_size=20),
    st.text("abcdefghijklmnopqrstuvwxyz", min_size=15),
    st.integers(min_value=0, max_value=255),
)
def test_editor(cmd_name, content, exit_code):
    p = TMP_DIR.joinpath(cmd_name)

    with open(p, 'w') as f:
        f.write(f'''\
#!/usr/bin/env sh

cat <<EOF
{content}
EOF

exit {exit_code}
''')

    p.chmod(0o744)

    try:
        res, outp, err = run_command([f'{SRC_DIR}/assist.sh', p], f'e\n', env={'EDITOR': 'cat'})

        note(str((res, outp, err)))

        assert res == 0
        assert content in outp
        assert str(exit_code) in outp

    finally:
        p.unlink(missing_ok=True)
