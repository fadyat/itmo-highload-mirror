from utils import *


def collect_passwd():
    expected = []

    with open('/etc/passwd', 'r') as f:
        for l in f:
            l = l.split(':')

            if l[5] == '/dev/null':
                expected.append((int(l[2]), l[0]))

    return expected


def test_simple():
    res, outp, err = run_command([f'{SRC_DIR}/homeless.sh'])

    assert res == 0
    assert err == ''

    outp = outp.strip().split('\n')
    outp = [tuple(l.strip().split('\t')) for l in outp]
    outp = set(outp)

    expected = collect_passwd()
    expected = [(str(i), u) for i, u in expected]
    expected.append(('Total:', str(len(expected))))
    expected = set(expected)

    assert outp == expected


def test_sorted():
    res, outp, err = run_command([f'{SRC_DIR}/homeless.sh'])

    assert res == 0
    assert err == ''

    expected = collect_passwd()
    expected.sort(key=lambda x: x[0])
    cnt = len(expected)

    expected = ''.join([f'{i}\t{u}\n' for i, u in expected])
    expected += f'Total:\t{cnt}\n'

    assert outp == expected
