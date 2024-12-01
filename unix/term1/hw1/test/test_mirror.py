from hypothesis import given, strategies as st, note
from utils import *


@given(st.lists(st.text("abcdefghijklmnopqrstuvwxyz")), st.lists(st.sampled_from("VH")))
def test_stdin(content, ops):
    expected = content

    for op in ops:
        if op == 'V':
            expected = list(reversed(expected))
        elif op == 'H':
            expected = [''.join(reversed(x)) for x in expected]

    res, outp, err = run_command([f'{SRC_DIR}/mirror.sh', *ops], '\n'.join(content) + '\n')

    note(str((res, outp, err)))

    assert res == 0
    assert err == ''
    assert outp == '\n'.join(expected) + '\n'
