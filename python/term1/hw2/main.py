from tex_gen import generate_document, generate_table, generate_image


def main():
    print(
        generate_document(
            generate_table([
                ["Name", "Age"],
                ["Alice", "25"],
                ["Bob", "30"],
            ]),
            generate_image(
                "valk.png",
                caption="A valkyrie."
            ),
            packages=["graphicx"],
        )
    )


if __name__ == "__main__":
    main()
