from utils import gen_uniq_seq


def main():
    gen_uniq_seq("uniq_seq_1.txt", 2_000_000, 0)
    gen_uniq_seq("uniq_seq_2.txt", 1_000, 0)


if __name__ == "__main__":
    main()