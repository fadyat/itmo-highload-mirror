from hypothesis import given, strategies as st, note
from utils import *


@given(st.lists(st.from_regex("[\t 0-z]*", fullmatch=True)))
def test_stdin(content):
    expected = [l.rstrip() for l in content]

    res, outp, err = run_command([f'{SRC_DIR}/trim.sh'], '\n'.join(content) + '\n')

    note(str((res, outp, err)))

    assert res == 0
    assert err == ''
    assert outp == '\n'.join(expected) + '\n'


@given(
    st.text("abcdefghijklmnopqrstuvwxyz", min_size=1, max_size=20),
    st.lists(st.from_regex("[\t 0-z]*", fullmatch=True)),
)
def test_file(filename, content):
    expected = [l.rstrip() for l in content]

    p = TMP_DIR.joinpath(filename)

    with open(p, 'w') as f:
        f.write('\n'.join(content))

    try:
        res, outp, err = run_command([f'{SRC_DIR}/trim.sh', p])

        note(str((res, outp, err)))

        assert res == 0
        assert err == ''
        assert outp == ''

        with open(p, 'r') as f:
            assert f.read() == '\n'.join(expected)

    finally:
        p.unlink(missing_ok=True)
