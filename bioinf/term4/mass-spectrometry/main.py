from itertools import accumulate, takewhile

ALPHABET = {
    'A': 71, 'C': 103, 'D': 115, 'E': 129, 'F': 147, 'G': 57, 'H': 137, 'I': 113, 'K': 128, 'L': 113,
    'M': 131, 'N': 114, 'P': 97, 'Q': 128, 'R': 156, 'S': 87, 'T': 101, 'V': 99, 'W': 186, 'Y': 163
}

ALPHABET_TEST = {
    'X': 4, 'Z': 5
}


def get_spectrum(s: str) -> list[int]:
    prefixes = list(accumulate((ALPHABET[c] for c in s), initial=0))
    total = prefixes[-1]
    return sorted(set(prefixes) | {total - p for p in prefixes})


def get_peptide(spectrum: set[int]) -> str | None:
    target = max(spectrum)

    def dfs(mass: int, path: str) -> str | None:
        if mass == target:
            if set(get_spectrum(path)) == spectrum:
                return path

            return None

        for aa, m in ALPHABET.items():
            nm = mass + m
            if {nm, target - nm} <= spectrum:
                if (r := dfs(nm, path + aa)) is not None:
                    return r

        return None

    return dfs(0, "")


def get_protein(
        proteome: str,
        spectral_vector: tuple[int, ...],
        alphabet: dict[str, int],
) -> str | None:
    sv = (0,) + spectral_vector
    m = len(spectral_vector)

    best_score, best_peptide = float('-inf'), None
    for i in range(len(proteome)):
        masses = accumulate((alphabet[c] for c in proteome[i:]), initial=0)
        prefixes = list(takewhile(lambda p: p <= m, masses))

        if prefixes[-1] == m:
            score = sum(sv[p] for p in prefixes)
            if score > best_score:
                best_score = score
                best_peptide = proteome[i:i + len(prefixes) - 1]

    return best_peptide


def main():
    spectral_vector = tuple(map(int, input().split()))
    proteome = input().strip()
    alphabet = ALPHABET if len(spectral_vector) > 30 else ALPHABET_TEST

    print(get_protein(proteome, spectral_vector, alphabet))


if __name__ == '__main__':
    main()
