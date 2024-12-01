from hypothesis import given, strategies as st, note
from utils import *


# https://stackoverflow.com/a/66701672
def remainder(n, d):
    return (-1 if n < 0 else 1) * (abs(n) % abs(d))


@st.composite
def expr(draw, min_depth=1, max_depth=6, literal=st.integers(min_value=-128, max_value=127)):
    def _e(depth):
        if depth <= 0:
            val = draw(literal)
            return str(val), val

        depth -= 1
        lhs, lhs_v = _e(depth)
        rhs, rhs_v = _e(depth)

        ops = [
            (f'({lhs} + {rhs})', lhs_v + rhs_v),
            (f'({lhs} - {rhs})', lhs_v - rhs_v),
            (f'({lhs} * {rhs})', lhs_v * rhs_v),
        ]

        if rhs_v != 0:
            ops.append((f'({lhs} / {rhs})', int(lhs_v / rhs_v)))
            ops.append((f'({lhs} % {rhs})', int(remainder(lhs_v, rhs_v))))

        return draw(st.sampled_from(ops))

    return _e(draw(st.integers(min_value=min_depth, max_value=max_depth)))


@given(expr())
def test_stdin(expr):
    expr, value = expr
    res, outp, err = run_command([f'{SRC_DIR}/calc.sh'], expr)

    note(str((res, outp, err)))

    assert res == 0
    assert err == ''
    assert int(outp.strip()) == value


@given(expr())
def test_args(expr):
    expr, value = expr
    res, outp, err = run_command([f'{SRC_DIR}/calc.sh', *expr.split()])

    note(str((res, outp, err)))

    assert res == 0
    assert err == ''
    assert int(outp.strip()) == value
